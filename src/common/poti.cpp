
#include <poti.h>

MCP41HV POTI(PIN_SPI_POTI_CS);

void poti_init()
{
  // MCP41HV 디지털 가변 저항 설정
  POTI.setup();
  SPI.begin();
}

MCP41HV & get_handle_poti() {
  return POTI;
}