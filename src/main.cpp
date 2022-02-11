/*
 * 
 */

#include <board.h>
#include <TM1637Display.h>

#include <can_bus.h>
#include <isr_timer.h>
#include <remote.h>

#include <ext_ahls.h>
#include <ext_cruise.h>
#include <ext_ims.h>


void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
  delay(500);
  /* Serial Example
  // read from port 2, send to port 0:
  if (Serial2.available()) {
    int inByte = Serial.read();
    Serial.write(inByte);
  }
  */

  board_init();

  can_init();
  timer_init();
  remote_init();

  ext_ahls_init();
  ext_cruise_init();
  ext_ims_init();
}


void loop()
{
  remote_update();
  ext_ahls_update();
  can_c.events();
  can_b.events();
}
