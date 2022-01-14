
#include <pin_map.h>
#include <isr_timer.h>
#include <poti.h>
#include <EEPROM.h>
#include <can_bus.h>
#include <ext_ahls.h>

/*
AHLS (Auto Headlamp Leveling System)

AHLS Sensor Signal 전압 샌싱
pinMode(PIN_AN_IN_AHLS_SIGNAL, INPUT);

AHLS Signal 출력 전압 체크
pinMode(PIN_AN_IN_AHLS_CHECK, INPUT);

MCP41HV (SPI) 제어로 AHLS Signal 출력 전압 조절

시동 시 헤드램프 세레모니 기능
-> 조건: 엔진 상태, 기어레버 상태
*/

#define EEPROM_AHLS_INIT        0xEF

#define EEPROM_ADDR_AHLS_INITIALIZED  0
#define EEPROM_ADDR_AHLS_RESERVED     1
#define EEPROM_ADDR_AHLS_POTI_MAX     2
#define EEPROM_ADDR_AHLS_POTI_MIN     4
#define EEPROM_ADDR_AHLS_OFFSET_DEF   6

// #define ADC_OFFSET_MARGIN       6 // 0.1V
#define ADC_OFFSET_MARGIN       5 // 0.08V
// #define ADC_OFFSET_MARGIN       4 // 0.06V
#define ADC_READ_BUF_MAX        10

#define DEF_VAL_POTI_MAX        140   // 기준전압 - 2V (ex: 14.5V - 2V, 11.8V - 2V)
#define DEF_VAL_POTI_MIN        140   // 2.25V

// AHLS_OFFSET_STEP: 최소 작동 값
// #define AHLS_OFFSET_STEP             22  // 0.35V
// #define AHLS_OFFSET_STEP             21  // 0.33V
#define AHLS_OFFSET_STEP             19  // 0.30V
// #define AHLS_OFFSET_STEP             16  // 0.25V

#define DEF_VAL_OFFSET_DEF      0
#define DEF_VAL_OFFSET_USER     0

#define TIMER_INTERVAL_ENGINE_CHECK   1000  // 1.0 Sec
#define TIMER_INTERVAL_CEREMONY_WORK  2000  // 1.5 Sec


bool g_bInitialized = false;
int g_ahlsOffset_def = DEF_VAL_OFFSET_DEF; // 기본 옵셋
int g_ahlsOffset_usr = DEF_VAL_OFFSET_USER; // 사용자 조작 옵셋
/*
g_potiMax, g_potiMin
=> 조사각 조절 최대 범위를 제한 한다.
  기본값은 모터의 한계 값에 근접한 값.
  헤드라이트 구조나 조립 상태에 따라 모터 한계값 도달전에 기구적으로 조절이 불가능한 경우 설정 변경한다.
  g_potiMax = g_adcRefVol - g_adcAhlsChk; 기준전압과 현재 출력전압의 차.
  g_potiMin = g_adcAhlsChk; 현재 출력전압.
*/
int g_potiMax = DEF_VAL_POTI_MAX;
int g_potiMin = DEF_VAL_POTI_MIN;

int g_adcRefVol = 0;
int g_adcAhlsSig = 0;
int g_adcAhlsChk = 0;


void ext_ahls_ceremony();



void ext_ahls_eng_check()
{
  static bool worked = false;

  /*
  vs.ems260.u8EngStat
  0: 엔진 Off
  1: 시동 중
  3: 엔진 On
  */
  if(false == worked  && 3 == vs.ems260.u8EngStat)
  {
    worked = true;
    ext_ahls_ceremony();
  }
  else
  {
    setTimeout(TIMER_INTERVAL_ENGINE_CHECK, ext_ahls_eng_check);
  }
}

void ext_ahls_init()
{
  poti_init();  // MCP41HV

  // poti.writeWiperValue(125);

  if(EEPROM_AHLS_INIT != EEPROM.read(EEPROM_ADDR_AHLS_INITIALIZED))
  {
    // EEPROM 초기화 안됨.
    Serial.println("AHLS EEPROM NOT INITIALIZED!");
  }
  else
  {
    // EEPROM 초기화 됨.
    Serial.println("AHLS EEPROM INITIALIZED!");
    g_bInitialized = true;
    g_potiMax = EEPROM.read(EEPROM_ADDR_AHLS_POTI_MAX);
    g_potiMax |= EEPROM.read(EEPROM_ADDR_AHLS_POTI_MAX + 1) << 8;
    g_potiMin = EEPROM.read(EEPROM_ADDR_AHLS_POTI_MIN);
    g_potiMin |= EEPROM.read(EEPROM_ADDR_AHLS_POTI_MIN + 1) << 8;
    g_ahlsOffset_def = EEPROM.read(EEPROM_ADDR_AHLS_OFFSET_DEF);
    g_ahlsOffset_def |= EEPROM.read(EEPROM_ADDR_AHLS_OFFSET_DEF + 1) << 8;
    Serial.print("g_potiMax: ");
    Serial.print(g_potiMax);
    Serial.print(", g_potiMin: ");
    Serial.print(g_potiMin);
    Serial.print(", g_ahlsOffset_def: ");
    Serial.println(g_ahlsOffset_def);
  }

  g_ahlsOffset_usr = DEF_VAL_OFFSET_USER;

  // 세리모니 조건 검사 타이머 시작.
    setTimeout(TIMER_INTERVAL_ENGINE_CHECK, ext_ahls_eng_check);
}

void ext_ahls_update()
{
  static int adcRefVolBuf[ADC_READ_BUF_MAX] = {0,};
  static int adcAhlsSigBuf[ADC_READ_BUF_MAX] = {0,};
  static int adcAhlsChkBuf[ADC_READ_BUF_MAX] = {0,};
  static uint8_t idx = 0;
  static bool bufReady = false;
  int adcSum = 0;
  int adcOffset = 0;
  uint8_t wiperVal = 0;


  adcRefVolBuf[idx] = analogRead(PIN_AN_IN_REF_VOLTAGE);
  adcAhlsSigBuf[idx] = analogRead(PIN_AN_IN_AHLS_SIGNAL);
  adcAhlsChkBuf[idx++] = analogRead(PIN_AN_IN_AHLS_CHECK);
  if(ADC_READ_BUF_MAX <= idx || true == bufReady)
  {
    bufReady = true;
  }
  else
  {
    // 부팅 초기에 버퍼가 다 차지 않은 경우 리턴한다.
    return;
  }
  idx %= ADC_READ_BUF_MAX;

  adcSum = 0;
  for(int i = 0; i < ADC_READ_BUF_MAX; i++)
  {
    adcSum += adcRefVolBuf[i];
  }
  g_adcRefVol = (int)(adcSum / ADC_READ_BUF_MAX);

  adcSum = 0;
  for(int i = 0; i < ADC_READ_BUF_MAX; i++)
  {
    adcSum += adcAhlsSigBuf[i];
  }
  g_adcAhlsSig = (int)(adcSum / ADC_READ_BUF_MAX);

  adcSum = 0;
  for(int i = 0; i < ADC_READ_BUF_MAX; i++)
  {
    adcSum += adcAhlsChkBuf[i];
  }
  g_adcAhlsChk = (int)(adcSum / ADC_READ_BUF_MAX);


  /*
  g_adcAhlsSig:   AHLS 센서 전압
  g_adcAhlsChk:   MCP41HV에서 출력 중인 전압
  g_adcRefVol:    차량의 현재 전압 (기준 전압)
  g_ahlsOffset_def: 기본 오프셋 (비휘발성)
  g_ahlsOffset_usr: 사용자 오프셋 (휘발성)

  adcOffset 값이 0 에 가까워 질 때까지 MCP41HV 조절

  FIXME: ADC 값 안정화를 위해 평균을 내는 버퍼링 시간 만큼 g_adcAhlsChk 값이 딜레이 된다.
        따라서 전압 조절 마지막 순간에 wiperVal 값이 출렁인다.
        하지만 이러한 현상은 대부분의 경우, 모터가 해당 구간에 도달하기 전에 안정화 된다.
  */
  adcOffset = (g_adcAhlsSig - g_adcAhlsChk) + g_ahlsOffset_def + g_ahlsOffset_usr;
  wiperVal = poti.readWiperValue();

  if((0 > adcOffset)
    && (wiperVal > 0)
    && (g_adcAhlsChk >= g_potiMin))
  {
    if(-(ADC_OFFSET_MARGIN) > adcOffset)
    {
      poti.decrementWiper();

      Serial.print("DEC -> wiperVal: ");
      Serial.print(wiperVal);
      Serial.print(", g_adcRefVol: ");
      Serial.print(g_adcRefVol);
      Serial.print(", g_adcAhlsSig: ");
      Serial.print(g_adcAhlsSig);
      Serial.print(", g_adcAhlsChk: ");
      Serial.print(g_adcAhlsChk);
      Serial.print(", g_potiMin: ");
      Serial.print(g_potiMin);
      Serial.print(", g_potiMax: ");
      Serial.print(g_potiMax);
      Serial.print(", adcOffset: ");
      Serial.println(adcOffset);
    }
  }
  else if((0 < adcOffset)
  && (wiperVal < 255)
  && ((g_adcRefVol - g_adcAhlsChk) >= g_potiMax))
  {
    if(ADC_OFFSET_MARGIN < adcOffset)
    {
      poti.incrementWiper();

      Serial.print("INC -> wiperVal: ");
      Serial.print(wiperVal);
      Serial.print(", g_adcRefVol: ");
      Serial.print(g_adcRefVol);
      Serial.print(", g_adcAhlsSig: ");
      Serial.print(g_adcAhlsSig);
      Serial.print(", g_adcAhlsChk: ");
      Serial.print(g_adcAhlsChk);
      Serial.print(", g_potiMin: ");
      Serial.print(g_potiMin);
      Serial.print(", g_potiMax: ");
      Serial.print(g_potiMax);
      Serial.print(", adcOffset: ");
      Serial.println(adcOffset);
    }
  }
}

void ext_ahsl_save_poti_max()
{
  // 정지상태, 시동상태
  if(0 == vs.tcu43F.u8CUR_GR && 3 == vs.ems260.u8EngStat)
  {
    Serial.print("ext_ahsl_save_poti_max() -> g_adcRefVol - g_adcAhlsChk: ");
    Serial.println(g_adcRefVol - g_adcAhlsChk);

    g_potiMax = g_adcRefVol - g_adcAhlsChk;
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX, uint8_t(g_potiMax & 0xFF));
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX + 1, uint8_t(g_potiMax >> 8) & 0xFF);

    if(false == g_bInitialized)
    {
      g_potiMin = DEF_VAL_POTI_MIN;
      EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN, uint8_t(g_potiMin & 0xFF));
      EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN + 1, uint8_t(g_potiMin >> 8) & 0xFF);

      g_ahlsOffset_def = DEF_VAL_OFFSET_DEF;
      EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF, uint8_t(g_ahlsOffset_def & 0xFF));
      EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF + 1, uint8_t(g_ahlsOffset_def >> 8) & 0xFF);

      EEPROM.write(EEPROM_ADDR_AHLS_INITIALIZED, EEPROM_AHLS_INIT);
    }
  }
}

void ext_ahsl_save_poti_min()
{
  // 정지상태, 시동상태
  if(0 == vs.tcu43F.u8CUR_GR && 3 == vs.ems260.u8EngStat)
  {
    Serial.print("ext_ahsl_save_poti_max() -> g_adcAhlsChk: ");
    Serial.println(g_adcAhlsChk);

    g_potiMin = g_adcAhlsChk;
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN, uint8_t(g_potiMin & 0xFF));
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN + 1, uint8_t(g_potiMin >> 8) & 0xFF);

    if(false == g_bInitialized)
    {
      g_potiMax = DEF_VAL_POTI_MAX;
      EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX, uint8_t(g_potiMax & 0xFF));
      EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX + 1, uint8_t(g_potiMax >> 8) & 0xFF);
      
      g_ahlsOffset_def = DEF_VAL_OFFSET_DEF;
      EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF, uint8_t(g_ahlsOffset_def & 0xFF));
      EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF + 1, uint8_t(g_ahlsOffset_def >> 8) & 0xFF);

      EEPROM.write(EEPROM_ADDR_AHLS_INITIALIZED, EEPROM_AHLS_INIT);
    }
  }
}

void ext_ahsl_save_poti_def()
{
  // 정지상태, 시동상태
  if(0 == vs.tcu43F.u8CUR_GR && 3 == vs.ems260.u8EngStat)
  {
    Serial.println("ext_ahsl_save_poti_def()");

    g_potiMin = DEF_VAL_POTI_MIN;
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN, uint8_t(g_potiMin & 0xFF));
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MIN + 1, uint8_t(g_potiMin >> 8) & 0xFF);
    
    g_potiMax = DEF_VAL_POTI_MAX;
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX, uint8_t(g_potiMax & 0xFF));
    EEPROM.write(EEPROM_ADDR_AHLS_POTI_MAX + 1, uint8_t(g_potiMax >> 8) & 0xFF);
    
    g_ahlsOffset_def = DEF_VAL_OFFSET_DEF;
    EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF, uint8_t(g_ahlsOffset_def & 0xFF));
    EEPROM.write(EEPROM_ADDR_AHLS_OFFSET_DEF + 1, uint8_t(g_ahlsOffset_def >> 8) & 0xFF);

    EEPROM.write(EEPROM_ADDR_AHLS_INITIALIZED, EEPROM_AHLS_INIT);
  }
}

// 조사각 올림
void ext_ahls_offset_inc()
{
  // Serial.print("INC -----> g_adcRefVol: ");
  // Serial.print(g_adcRefVol);
  // Serial.print(", g_adcAhlsSig: ");
  // Serial.print(g_adcAhlsSig);
  // Serial.print(", g_adcAhlsChk: ");
  // Serial.print(g_adcAhlsChk);
  // Serial.print(", g_potiMin: ");
  // Serial.print(g_potiMin);
  // Serial.print(", g_potiMax: ");
  // Serial.println(g_potiMax);
  if((g_adcRefVol - g_adcAhlsChk) >= g_potiMax)
  {
    g_ahlsOffset_usr += AHLS_OFFSET_STEP;
    if((g_adcAhlsChk - g_potiMin) < AHLS_OFFSET_STEP)
    {
      // MIN 값에 근접한 경우 한단계 더 올린다.
      // g_ahlsOffset_usr += (g_adcAhlsChk - g_potiMin);
      for(int i = 0; i < 10; i++)
      {
        if(0 > (g_adcAhlsSig - g_adcAhlsChk) + g_ahlsOffset_def + g_ahlsOffset_usr)
        {
          g_ahlsOffset_usr += AHLS_OFFSET_STEP;
          Serial.println(" ext_ahls_offset_dec");
        }
        else
        {
          break;
        }
      }
    }
    Serial.println("ext_ahls_offset_inc");
  }
}

// 조사각 내림
void ext_ahls_offset_dec()
{
  // Serial.print("DEC -----> g_adcRefVol: ");
  // Serial.print(g_adcRefVol);
  // Serial.print(", g_adcAhlsSig: ");
  // Serial.print(g_adcAhlsSig);
  // Serial.print(", g_adcAhlsChk: ");
  // Serial.print(g_adcAhlsChk);
  // Serial.print(", g_potiMin: ");
  // Serial.print(g_potiMin);
  // Serial.print(", g_potiMax: ");
  // Serial.println(g_potiMax);
  if(g_adcAhlsChk >= g_potiMin)
  {
    g_ahlsOffset_usr -= AHLS_OFFSET_STEP;
    if(((g_adcRefVol - g_adcAhlsChk) - g_potiMax) < AHLS_OFFSET_STEP)
    {
      // MAX 값에 근접한 경우 한단계 더 내린다.
      // g_ahlsOffset_usr -= ((g_adcRefVol - g_adcAhlsChk) - g_potiMax);
      for(int i = 0; i < 10; i++)
      {
        if(0 < (g_adcAhlsSig - g_adcAhlsChk) + g_ahlsOffset_def + g_ahlsOffset_usr)
        {
          g_ahlsOffset_usr -= AHLS_OFFSET_STEP;
          Serial.println(" ext_ahls_offset_dec");
        }
        else
        {
          break;
        }
      }
    }
    Serial.println("ext_ahls_offset_dec");
  }
}

// TODO: 주행 속도에 따라 조사각을 조절한다.
void ext_ahls_auto_control()
{
  Serial.println("ext_ahls_auto_control");
  // if(onOff)
  // {}
  // else
  // {}
}

void ext_ahls_set_default()
{
  Serial.println("ahls_set_default");
  g_ahlsOffset_usr = 0;
}

/*
헤드라이트 세레모니
시동 시 한 번 작동.
전압을 설정 값 만큼 낮추고 1.5초 후에 원복한다.


헤드램프 세레모니 동작 조건
-> 엔진 On, 차량 정지 상태(기어레버 'P')
조건이 맞지 않으면 다음 시동까지 작동하지 않는다.
vs.tcu43F.u8ETL_TCU
14  => 변경 중
0   => 'P'
7   => 'R'
6   => 'N'
5   => 'D'
4
3
2
1
*/
void ext_ahls_ceremony()
{
  // g_ahlsOffset_usr = -96;  // 1.5V
  Serial.println("ext_ahls_ceremony~~~~~~~~~~~~~~");
  Serial.print("Gear 레버: ");
  Serial.print(vs.tcu43F.u8CUR_GR);
  Serial.print(", Gear 단수: ");
  Serial.println(vs.tcu43F.u8ETL_TCU);
  // 현재 기어레버 상태 'P' 또는 'N'일 경우만 동작
  // if(0 == vs.tcu43F.u8CUR_GR || 6 == vs.tcu43F.u8CUR_GR)
  // 현재 기어레버 상태 'P'일 경우만 동작
  if(0 == vs.tcu43F.u8CUR_GR)
  {
    g_ahlsOffset_usr = -80;  // 1.28V
    setTimeout(TIMER_INTERVAL_CEREMONY_WORK, ext_ahls_set_default);
  }
}

