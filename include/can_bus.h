#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <vehicle_state.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> & get_handle_c_can(); // C-CAN
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> & get_handle_b_can(); // B-CAN

VehicleState & get_vehicle_state();

#define can_c get_handle_c_can()
#define can_b get_handle_b_can()
#define vs get_vehicle_state()

void can_init();
