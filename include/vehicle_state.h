#include <Arduino.h>
#include <FlexCAN_T4.h>

/*
Size: 8 Bytes
ID:   0x0A0
*/
struct ENG_0A0
{
  uint8_t   u8Pid_04h;  // Scale: 0.392157, Bias: 0, Min: 0, Max: 100, Unit: %

  uint8_t   u8Pid_05h;  // Scale: 1, Bias: -40, Min: -40, Max: 215, Unit: ???

  uint16_t  u16Rpm;     // PID_0Ch -> Scale: 0.25, Bias: 0, Min: 0, Max: 16383.8, Unit: rpm

  uint8_t   u8Speed;    // PID_0Dh -> Scale: 1, Bias: 0, Min: 0, Max: 255, Unit: km/h

  uint8_t   u8Pid_11h;  // Scale: 0.392157, Bias: 0, Min: 0, Max: 100, Unit: %

  uint16_t  u16Pid_03h; // Scale: 1, Bias: 0, Min: 0, Max: 65535, Unit: ???
};

/*
Size: 8 Bytes
ID:   0x260
*/
struct EMS_260
{
  uint8_t  u8TqiMin;       // Scale: 0.390625, Bias: 0, Min: 0, Max: 99.6094, Unit: %

  uint8_t  u8Tqi;          // Scale: 0.390625, Bias: 0, Min: 0, Max: 99.6094, Unit: %

  uint8_t  u8TqiTarget;    // Scale: 0.390625, Bias: 0, Min: 0, Max: 99.6094, Unit: %

  uint8_t  u8GlowStat        : 1;
  uint8_t  u8CruiseLampMain  : 1;
  uint8_t  u8CruiseLampSet   : 1;
  uint8_t  u8PreFuelCutIn    : 1;
  uint8_t  u8EngStat         : 3;
  uint8_t  u8SoakTimeErr     : 1;

  uint8_t  u8SoakTime;     // Scale: 1, Bias: 0, Min: 0, Max: 255, Unit: Min

  uint8_t  u8TqiMax;       // Scale: 0.390625, Bias: 0, Min: 0, Max: 99.6094, Unit: %

  uint8_t  u8SoakTimeCur;  // Scale: 0.375, Bias: -35.625, Min: -35.625, Max: 60, Unit: ???

  uint8_t  u8CheckSum        : 4;
  uint8_t  u8AliveCounter    : 2;
  uint8_t  u8EmsAclAct       : 2;
};

/*
Size: 8 Bytes
ID:   0x329
*/
struct EMS_329
{
  uint8_t   u8CONF_TCU        : 6;  // CONF_TCU-> scale: 1, TQ_STND -> scale: 10 Unit: Nm, CAN_VERS -> Scale: 1, Max: 7.7
  uint8_t   u8MUL_CODE        : 2;

  uint8_t   u8TEMP_ENG;         // Scale: 0.75, Bias: -48, Min: -48, Max: 143.25, Unit: ???

  uint8_t   u8MAF_FAC_ALTI_MMV; // Scale: 0.00781, Bias: 0, Min: 0, Max: 1.99155, Unit: ???

  uint8_t   u8VB_OFF_ACT      : 1;
  uint8_t   u8ACK_ES          : 1;
  uint8_t   u8CONF_MIL_FMY    : 3;
  uint8_t   u8OD_OFF_REQ      : 1;
  uint8_t   u8ACC_ACT         : 1;
  uint8_t   u8CLU_ACK         : 1;

  uint8_t   u8BRAKE_ACT       : 2;
  uint8_t   u8ENG_CHR         : 4;
  uint8_t   u8GP_CTL          : 2;

  uint8_t   u8TPS;              // Scale: 0.469484, Bias: -15.0235, Min: -15.0235, Max: 104.695, Unit: %

  uint8_t   u8PV_AV_CAN;        // Scale: 0.3906, Bias: 0, Min: 0, Max: 99.603, Unit: %

  uint8_t   u8ENG_VOL;          // Scale: 0.1, Bias: 0, Min: 0, Max: 25.5, Unit: liter
};

/*
Size: 8 Bytes
ID:   0x43F
*/
struct TCU_43F
{
  uint8_t   u8ETL_TCU;   // Scale: 2, Bias: 0, Min: 0, Max: 508, Unit: Nm 현재 기어 상태

  uint8_t   u8CUR_GR        : 4;  // 현재 기어 레버 위치
  uint8_t   u8CF_Tcu_Alive  : 2;
  uint8_t   u8CF_Tcu_ChkSum : 2;

  uint8_t   u8VS_TCU;   // Scale: 1, Bias: 0, Min: 0, Max: 254, Unit: km/h

  uint8_t   u8FAN_CTRL_TCU  : 2;
  uint8_t   u8BRAKE_ACT_TCU : 2;
  uint8_t   u8FUEL_CUT_TCU  : 1;
  uint8_t   u8INH_FUEL_CUT  : 1;
  uint8_t   u8IDLE_UP_TCU   : 1;
  uint8_t   u8N_INC_TCU     : 1;
  
  uint8_t   u8SPK_RTD_TCU;  // Scale: 0.375, Bias: -23.625, Min: -15, Max: 15, Unit: ???
  
  uint16_t  u16N_TC_RAW;    // Scale: 0.25, Bias: 0, Min: 0, Max: 16383.5, Unit: rpm

  uint8_t   u8VS_TCU_DECIMAL; // Scale: 0.0078125, Bias: -23.625, Min: -15, Max: 0.992188, Unit: km/h
};


class VehicleState
{
  public:
  ENG_0A0 eng0A0;
  EMS_260 ems260;
  EMS_329 ems329;
  TCU_43F tcu43F;

  VehicleState();
  ~VehicleState();

  void update_eng0A0(const CAN_message_t & msg);
  void update_ems260(const CAN_message_t & msg);
  void update_ems329(const CAN_message_t & msg);
  void update_tcu43F(const CAN_message_t & msg);
};