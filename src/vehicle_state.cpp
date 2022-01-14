
#include <vehicle_state.h>

VehicleState::VehicleState()
{}

VehicleState::~VehicleState()
{}


void VehicleState::update_eng0A0(const CAN_message_t & msg)
{
  memcpy(&eng0A0, msg.buf, sizeof(ENG_0A0));
}

void VehicleState::update_ems260(const CAN_message_t & msg)
{
  EMS_260 newMsg;
  memcpy(&newMsg, msg.buf, sizeof(EMS_260));

  memcpy(&ems260, msg.buf, sizeof(EMS_260));
  /*
  if(state changed)
  {
    do somthing
    ims control...
    cruise led on/off
  }
  */
}

void VehicleState::update_ems329(const CAN_message_t & msg)
{
  memcpy(&ems329, msg.buf, sizeof(EMS_329));
}

void VehicleState::update_tcu43F(const CAN_message_t & msg)
{
  memcpy(&tcu43F, msg.buf, sizeof(TCU_43F));
}