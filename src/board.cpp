
#include <board.h>


void board_init () {
  // Analog Input
  // 자동차 전압
  pinMode(PIN_AN_IN_REF_VOLTAGE, INPUT);
  // 크루즈 컨트롤 리모트 스위치 전압 샌싱
  pinMode(PIN_AN_IN_CRUISE_REMOTE, INPUT);
  // AHLS 리모트 스위치 전압 샌싱
  pinMode(PIN_AN_IN_AHLS_REMOTE, INPUT);
  // AHLS Sensor Signal 전압 샌싱
  pinMode(PIN_AN_IN_AHLS_SIGNAL, INPUT);
  // AHLS Signal 출력 전압 체크
  pinMode(PIN_AN_IN_AHLS_CHECK, INPUT);

  // Digital Output
  // Cruise Remote
  pinMode(PIN_DI_OUT_CRUISE_MAIN, OUTPUT);
  digitalWrite (PIN_DI_OUT_CRUISE_MAIN, LOW);
  pinMode(PIN_DI_OUT_CRUISE_RESUME, OUTPUT);
  digitalWrite (PIN_DI_OUT_CRUISE_RESUME, LOW);
  pinMode(PIN_DI_OUT_CRUISE_SET, OUTPUT);
  digitalWrite (PIN_DI_OUT_CRUISE_SET, LOW);
  pinMode(PIN_DI_OUT_CRUISE_CANCEL, OUTPUT);
  digitalWrite (PIN_DI_OUT_CRUISE_CANCEL, LOW);

  // Cruise Lamp
  pinMode(PIN_DI_OUT_LED_CRUISE_MAIN, OUTPUT);
  digitalWrite (PIN_DI_OUT_LED_CRUISE_MAIN, LOW);
  pinMode(PIN_DI_OUT_LED_CRUISE_SET, OUTPUT);
  digitalWrite (PIN_DI_OUT_LED_CRUISE_SET, LOW);

#if 1
  // C-CAN Mode - Normal Mode
  pinMode(PIN_DI_OUT_CAN_C_STB, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_C_STB, LOW);
#else
  // C-CAN Mode - Listen-Only Standby Mode
  pinMode(PIN_DI_OUT_CAN_C_STB, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_C_STB, High);
#endif

#if 1
  // B-CAN Mode - Normal Mode
  pinMode(PIN_DI_OUT_CAN_B_STB, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_B_STB, HIGH);
  pinMode(PIN_DI_OUT_CAN_B_EN, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_B_EN, HIGH);
#else
  // B-CAN Mode - Power On Standby Mode
  pinMode(PIN_DI_OUT_CAN_B_STB, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_B_STB, LOW);
  pinMode(PIN_DI_OUT_CAN_B_EN, OUTPUT);
  digitalWrite (PIN_DI_OUT_CAN_B_EN, LOW);
#endif
}

