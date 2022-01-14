
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
  pinMode(PIN_DI_OUT_CRUISE_MAIN, OUTPUT);    // 2
  digitalWrite (PIN_DI_OUT_CRUISE_MAIN, LOW);
  pinMode(PIN_DI_OUT_CRUISE_RESUME, OUTPUT);  // 3
  digitalWrite (PIN_DI_OUT_CRUISE_RESUME, LOW);
  pinMode(PIN_DI_OUT_CRUISE_SET, OUTPUT);     // 4
  digitalWrite (PIN_DI_OUT_CRUISE_SET, LOW);
  pinMode(PIN_DI_OUT_CRUISE_CANCEL, OUTPUT);  // 5
  digitalWrite (PIN_DI_OUT_CRUISE_CANCEL, LOW);
}

