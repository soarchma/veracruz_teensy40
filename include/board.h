#pragma once

#include <Arduino.h>
#include <FlexCAN_T4.h>
// #include <MCP41HV.h>
#include <pin_map.h>

void board_init ();

// FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> & get_handle_c_can();
// FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> & get_handle_b_can();
// MCP41HV & get_handle_poti();

// #define can_c get_handle_c_can()
// #define can_b get_handle_b_can()
// #define poti get_handle_poti()