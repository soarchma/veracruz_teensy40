
// 
#define PIN_CAN_C_RX  23
#define PIN_CAN_C_TX  22
#define PIN_CAN_B_RX  0 // TJA1055T - RXD(3)
#define PIN_CAN_B_TX  1 // TJA1055T - TXD(2)

/*
 * STB: 1, EN 1:  -> Normal Mode, Normal transceiver operation
 * STB: 0, EN 1:  -> Go-to-Sleep, Switches off the INH pin after a certain time out to disable the external voltage regulator
 * STB: 0, EN 0:  -> Standby / Sleep, In Standby the INH pin is on In Sleep the INH pin is off
 * STB: 1, EN 0:  -> Power-on Standby, Allows to read back the Power-on flag
 */
#define PIN_DI_OUT_CAN_B_STB 2 // Transceiver Mode - TJA1055T - STB(5)
#define PIN_DI_OUT_CAN_B_EN  3 // Transceiver Enable - TJA1055T - EN(6)

// PIN_CAN_C_STB - HIGH: Listen-Only Standby Mode, LOW: High-Speed Mode
#define PIN_DI_OUT_CAN_C_STB 21 // Transceiver Mode - SN65HVD230 - RS(8)

#define PIN_SERIAL_RX 7 // Transceiver Mode - TJA1055T - STB(5)
#define PIN_SERIAL_TX 8 // Transceiver Enable - TJA1055T - EN(6)

// 
#define PIN_DI_OUT_CRUISE_MAIN   4
#define PIN_DI_OUT_CRUISE_RESUME 5
#define PIN_DI_OUT_CRUISE_SET    6
#define PIN_DI_OUT_CRUISE_CANCEL 9

// 
#define PIN_AN_IN_CRUISE_REMOTE  14
#define PIN_AN_IN_AHLS_CHECK     15
#define PIN_AN_IN_REF_VOLTAGE    18
#define PIN_AN_IN_AHLS_SIGNAL    19
#define PIN_AN_IN_AHLS_REMOTE    20

// 
#define PIN_DI_OUT_LED_CRUISE_MAIN 16
#define PIN_DI_OUT_LED_CRUISE_SET  17

// 
#define PIN_SPI_POTI_CS 10
// #define PIN_SPI_POTI_WLAT
// #define PIN_SPI_POTI_SHDN

