
#include <remote_ahls.h>
#include <ext_ahls.h>
#include <EEPROM.h>

/*
# 기본 기능
*/

void ahls_remote_handler(uint8_t btnEvent, uint8_t state)
{
  switch(btnEvent)
  {
    case 0: // 전조등 올림 / 전압 MAX 설정
    {
      if(2 == state)
      { // Long Key
        // 현재 poti 전압 값을 저장 한다.
        ext_ahsl_save_poti_max();
      }
      else
      {
        ext_ahls_offset_inc();
      }
    }
    break;

    case 1: // 전조등 내림 / 전압 MIN 설정
    {
      if(2 == state)
      { // Long Key
        // 현재 poti 전압 값을 저장 한다.
        ext_ahsl_save_poti_min();
      }
      else
      {
        ext_ahls_offset_dec();
      }
    }
    break;

    case 2: // 전조등 자동 제어
    {
      if(2 == state)
      { // Long Key
      }
      else
      {
        ext_ahls_auto_control();
      }
    }
    break;

    case 3: // 전조등 기본 설정
    {
      if(2 == state)
      { // Long Key
        // poti 전압 값을 초기값으로 저장 한다.
        ext_ahsl_save_poti_def();
      }
      else
      {
        ext_ahls_set_default();
      }
    }
    break;

    default:
    {
    }
    break;
  }
}

ButtonState * init_ahls_remote(uint8_t pinNum)
{
  uint16_t adcRefs[5] = {1023, 693, 480, 274, 101};
  static ButtonState btnAhls(pinNum, adcRefs, 2000, 70, false, ahls_remote_handler);

  return & btnAhls;
}
