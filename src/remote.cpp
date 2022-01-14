
#include <pin_map.h>

#include <remote.h>

static ButtonState * btnCruise;
static ButtonState * btnAhls;

void remote_init()
{
  btnCruise = init_cruise_remote(PIN_AN_IN_CRUISE_REMOTE);
  btnAhls = init_ahls_remote(PIN_AN_IN_AHLS_REMOTE);
}

void remote_update()
{
  btnCruise->update();
  btnAhls->update();
}