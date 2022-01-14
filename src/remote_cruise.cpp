
#include <pin_map.h>
#include <isr_timer.h>
#include <remote_cruise.h>
#include <ext_cruise.h>


#define TIMER_INTERVAL_10MS             10L

/*
# 기본 기능
  크루즈 컨트롤 핸들 리모컨 입력을 분석하여 아래의 출력 핀 중 하나를 High로 일정 시간 유지 후 Low 로 변경 한다.
  PIN_DI_OUT_CRUISE_MAIN   2
  PIN_DI_OUT_CRUISE_RESUME 3
  PIN_DI_OUT_CRUISE_SET    4
  PIN_DI_OUT_CRUISE_CANCEL 5

# 고급 기능

*/

void cruise_main_low()
{
  Serial.println("cruise_main_low");
  digitalWrite(PIN_DI_OUT_CRUISE_MAIN, LOW);
}
void cruise_resume_low()
{
  Serial.println("cruise_resume_low");
  digitalWrite(PIN_DI_OUT_CRUISE_RESUME, LOW);
}
void cruise_set_low()
{
  Serial.println("cruise_set_low");
  digitalWrite(PIN_DI_OUT_CRUISE_SET, LOW);
}
void cruise_cancel_low()
{
  Serial.println("cruise_cancel_low");
  digitalWrite(PIN_DI_OUT_CRUISE_CANCEL, LOW);
}

void cruise_remote_handler(uint8_t btnEvent, uint8_t state)
{
  // static uint32_t prevMillis = 0;
  // uint32_t curMillis = millis();

  switch(btnEvent)
  {
    case 0: // Cruise Control Main Switch
    {
      digitalWrite(PIN_DI_OUT_CRUISE_MAIN, HIGH);
      setTimeout(TIMER_INTERVAL_10MS, cruise_main_low);
    }
    break;

    case 1: // Cruise Control Resume/Accel Switch
    {
      if(2 == state)
      { // Long Key
      }
      else
      {
        digitalWrite(PIN_DI_OUT_CRUISE_RESUME, HIGH);
        setTimeout(TIMER_INTERVAL_10MS, cruise_resume_low);
      }
    }
    break;

    case 2: // Cruise Control Set/Coast Switch
    {
      if(2 == state)
      { // Long Key
      }
      else
      {
        digitalWrite(PIN_DI_OUT_CRUISE_SET, HIGH);
        setTimeout(TIMER_INTERVAL_10MS, cruise_set_low);
      }
    }
    break;

    case 3: // Cruise Control Cancel Switch
    {
      digitalWrite(PIN_DI_OUT_CRUISE_CANCEL, HIGH);
      setTimeout(TIMER_INTERVAL_10MS, cruise_cancel_low);
    }
    break;

    default:
    { // do nothing...
    }
    break;
  }
}



ButtonState * init_cruise_remote(uint8_t pinNum)
{
  uint16_t adcRefs[5] = {280, 500, 191, 95, 15};
  
  static ButtonState btnCruise(pinNum, adcRefs, 500, 15, true, cruise_remote_handler);

  return & btnCruise;
}

