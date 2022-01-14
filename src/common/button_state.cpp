#include <button_state.h>

ButtonState::ButtonState(uint8_t pinNum, uint16_t adcRefs[5], uint16_t longKeyTime, uint16_t offset, bool cruise, void (*handler)(uint8_t, uint8_t))
{
  this->pinNum = pinNum;
  this->state = 0;
  this->newStateCount = 0;
  this->errCount = 0;
  this->lookUpLongKey = longKeyTime;
  for(uint8_t i = 0; i < 5; i++)
  {
    this->adcRefs[i] = adcRefs[i];
  }
  for(uint8_t i = 0; i < 4; i++)
  {
    this->btnCount[i] = 0;
  }
  this->lookUpOffset = offset;
  this->cruise = cruise;
  this->prevMillis = 0;
  this->handler = handler;
  Serial.println("ButtonState::ButtonState");
}

void ButtonState::update()
  {
    uint16_t adcVal = analogRead(pinNum);
    uint16_t valMin[5] = {0,};
    uint16_t valMax[5] = {0,};
    bool err = true;
    /*
      state == 0 인 경우
      이전 상태 값과 현재 상태 값을 비교한다. 옵셋 적용.
      현재 상태 값이 옵셋 내에 들어오는 상태가 있다면 타이머 작동
      normal -> push -> normal
      normal -> long push -> normal
    */

    for(int i = 0; i < 5; i++)
    {
      if(adcVal < lookUpOffset)
      {
        valMin[i] = 0;
      }
      else
      {
        valMin[i] = adcVal - lookUpOffset;
      }

      if((adcVal + lookUpOffset) > 1024)
      {
        valMax[i] = 1024;
      }
      else
      {
        valMax[i] = adcVal + lookUpOffset;
      }

      if(true == cruise)
      {
        // Cruise Main
        valMin[1] = 500;
      }
    }

    for(uint8_t i = 0; i < 5; i++)
    {
      if((valMin[i] <= adcRefs[i]) && (valMax[i] >= adcRefs[i]))
      {
        if(i == 0)
        { // normal -> normal or push -> normal
          if(btnCount[0] || btnCount[1] || btnCount[2] || btnCount[3])
          { // push -> normal
            Serial.print("btn0: ");
            Serial.print(btnCount[0]);
            Serial.print(", btn1: ");
            Serial.print(btnCount[1]);
            Serial.print(", btn2: ");
            Serial.print(btnCount[2]);
            Serial.print(", btn3: ");
            Serial.print(btnCount[3]);

            Serial.print(", state: ");
            Serial.print(state);
            Serial.print(", newStateCount: ");
            Serial.print(newStateCount);
            Serial.print(", ErrCount: ");
            Serial.println(errCount);

            for(uint8_t j = 0; j < 4; j++)
            {
              if(1 == state)
              {
                if(btnCount[j] > 10)
                {
                  Serial.print(j + 1);
                  Serial.print(" Button pushed. State: ");
                  Serial.println(state);
                  if(handler)
                  {
                    handler(j, state);
                  }
                }
              }
              btnCount[j] = 0;
            }
            newStateCount = 0;
            errCount = 0;
            state = 0;
          }
        }
        else
        { // normal -> push or push -> push
          ++btnCount[i - 1];

          if(newStateCount == 0)
          { // normal -> push
            state = 1;
            newStateCount = 1;
            prevMillis = millis();
          }
          else
          { // push -> push
            if(1 == state)
            {
              uint32_t curtMillis = millis();

              if((curtMillis - prevMillis) >= lookUpLongKey)
              {
                state = 2;
                for(uint8_t j = 0; j < 4; j++)
                {
                  if(btnCount[j] > 10)
                  {
                    Serial.print(j + 1);
                    Serial.print(" Button Long pushed. State: ");
                    Serial.print(state);
                    Serial.print(", adcVal: ");
                    Serial.println(adcVal);
                    if(handler)
                    {
                      handler(j, state);
                    }
                  }
                }
              }
              else
              {
                ++newStateCount;
              }
            }
          }
        }
        err = false;
        break;
      }
    }
    if(err)
    {
      ++errCount;
    }
  }