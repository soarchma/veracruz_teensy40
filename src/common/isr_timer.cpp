
#include <isr_timer.h>
#include <TimerInterrupt_Generic.h>
#include <ISR_Timer_Generic.h>

// Init Teensy timer TEENSY_TIMER_1
TeensyTimer ITimer1(TEENSY_TIMER_1);

// Init Teensy_ISR_Timer
// Each Teensy_ISR_Timer can service 16 different ISR-based timers
ISR_Timer Teensy_ISR_Timer;

bool status = false;

#define HW_TIMER_INTERVAL_MS          50L

void TimerHandler(void)
{
  Teensy_ISR_Timer.run();
}

void timer_init()
{
  // Interval in microsecs
  if (ITimer1.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    uint32_t lastMillis = millis();
    Serial.println("Starting  ITimer1 OK, millis() = " + String(lastMillis));
    status = true;
  }
  else
  {
    Serial.println("Can't set ITimer1 correctly. Select another freq. or interval");
    status = false;
  }
}

void setTimeout(unsigned long d, timerCallback f)
{
  if(status)
    Teensy_ISR_Timer.setTimeout(d, f);
}