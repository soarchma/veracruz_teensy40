
#include <can_bus.h>

#define NUM_C_CAN_RX_MAILBOXES 10
#define NUM_C_CAN_TX_MAILBOXES 0

#define NUM_B_CAN_RX_MAILBOXES 8
#define NUM_B_CAN_TX_MAILBOXES 2

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> CAN_C;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> CAN_B;

VehicleState VS;

void canSniff(const CAN_message_t & msg);
void canSniff_eng0A0(const CAN_message_t & msg);
void canSniff_ems260(const CAN_message_t & msg);
void canSniff_ems329(const CAN_message_t & msg);
void canSniff_tcu43F(const CAN_message_t & msg);

void can_init()
{
  // C-CAN 설정
  CAN_C.begin();
  CAN_C.setBaudRate(500000);  // 500Kbps
  CAN_C.setMaxMB(NUM_C_CAN_RX_MAILBOXES + NUM_C_CAN_TX_MAILBOXES);
  for (int i = 0; i < NUM_C_CAN_RX_MAILBOXES; i++){
    CAN_C.setMB(FLEXCAN_MAILBOX(i), RX, STD);
  }
  CAN_C.setMBFilter(REJECT_ALL);
  CAN_C.enableMBInterrupts();
  // CAN_C.enableFIFOInterrupt();
  CAN_C.onReceive(MB0, canSniff_eng0A0);
  CAN_C.onReceive(MB1, canSniff_ems260);
  CAN_C.onReceive(MB2, canSniff_ems329);
  CAN_C.onReceive(MB3, canSniff_tcu43F);
  CAN_C.setMBFilter(MB0, 0x0A0);
  CAN_C.setMBFilter(MB1, 0x260);
  CAN_C.setMBFilter(MB2, 0x329);
  CAN_C.setMBFilter(MB3, 0x43F);
  CAN_C.mailboxStatus();

  // B-CAN 설정
  CAN_B.begin();
  CAN_B.setBaudRate(50000);  // 50Kbps
  CAN_B.setMaxMB(NUM_B_CAN_RX_MAILBOXES + NUM_B_CAN_TX_MAILBOXES);
  for (int i = 0; i < NUM_B_CAN_RX_MAILBOXES; i++){
    CAN_B.setMB(FLEXCAN_MAILBOX(i), RX, STD);
  }
  for (int i = NUM_B_CAN_RX_MAILBOXES; i < (NUM_B_CAN_RX_MAILBOXES + NUM_B_CAN_TX_MAILBOXES); i++){
    CAN_B.setMB(FLEXCAN_MAILBOX(i), TX, STD);
  }
  CAN_B.setMBFilter(REJECT_ALL);
  CAN_B.enableMBInterrupts();
  CAN_B.onReceive(MB0, canSniff);
  CAN_B.onReceive(MB1, canSniff);
  CAN_B.onReceive(MB2, canSniff);
  CAN_B.setMBFilter(MB0, 0x0A0);
  CAN_B.setMBFilter(MB1, 0x260);
  CAN_B.setMBFilter(MB2, 0x329);
  CAN_B.setMBFilter(MB3, 0x43F);
  CAN_B.mailboxStatus();
}

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> & get_handle_c_can()
{
  return CAN_C;
}

FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> & get_handle_b_can()
{
  return CAN_B;
}

VehicleState & get_vehicle_state()
{
  return VS;
}


// #define CAN_ID_ENG    0x0A0
// #define CAN_ID_EMS16  0x260

#define BIT_MASK_1_GLOW_STAT        0x01
#define BIT_MASK_1_CRUISE_LAMP_MAIN 0x02
#define BIT_MASK_1_CRUISE_LAMP_SET  0x04
#define BIT_MASK_1_PRE_FUEL_CUT_IN  0x08
#define BIT_MASK_3_ENG_STAT         0x70


void canSniff(const CAN_message_t & msg)
{
    Serial.print("MB "); Serial.print(msg.mb);
    Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" EXT: "); Serial.print(msg.flags.extended);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" ID: "); Serial.print(msg.id, HEX);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < msg.len; i++ ) {
      Serial.print(msg.buf[i], HEX); Serial.print(" ");
    } Serial.println();
}

void canSniff_eng0A0(const CAN_message_t & msg)
{
  // uint16_t temp1 = msg.buf[2] | (msg.buf[3] << 8);
  // double rpm1 = temp1 * 0.25;

  ENG_0A0 temp;
  memcpy(&temp, msg.buf, sizeof(ENG_0A0));

  VS.update_eng0A0(msg);

  Serial.print(",   ");
  Serial.print(msg.buf[4], DEC);
  Serial.print(" Km/h ");
  Serial.print(temp.u16Rpm * 0.25);
  Serial.println(" RPM");

}

void canSniff_ems260(const CAN_message_t & msg)
{
  EMS_260 temp;
  memcpy(&temp, msg.buf, sizeof(EMS_260));

  VS.update_ems260(msg);

  if(msg.buf[3] & BIT_MASK_1_CRUISE_LAMP_MAIN)
  {
    Serial.print("CRUISE ■ ");
  }
  else
  {
    Serial.print("CRUISE □ ");
  }

  if(msg.buf[3] & BIT_MASK_1_CRUISE_LAMP_SET)
  {
    Serial.print("SET ■ ");
  }
  else
  {
    Serial.print("SET □ ");
  }

  if(msg.buf[3] & BIT_MASK_1_PRE_FUEL_CUT_IN)
  {
    Serial.print("F-CUT ■ ");
  }
  else
  {
    Serial.print("F-CUT □ ");
  }

  // Serial.print(" bCruiseLampMain-");
  // Serial.print(temp.bCruiseLampMain);
  // Serial.print(" bCruiseLampSet-");
  // Serial.print(temp.bCruiseLampSet);
  Serial.print(" bEngStat-");
  Serial.print(temp.u8EngStat);
  // Serial.print(" bGlowStat-");
  // Serial.print(temp.bGlowStat);
  // Serial.print(" bPreFuelCutIn-");
  // Serial.print(temp.bPreFuelCutIn);
}

void canSniff_ems329(const CAN_message_t & msg)
{
  VS.update_ems329(msg);
}

void canSniff_tcu43F(const CAN_message_t & msg)
{
  VS.update_tcu43F(msg);
}