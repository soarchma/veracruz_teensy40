#pragma once

#include <Arduino.h>

typedef void (*timerCallback)  (void);

void timer_init();
void setTimeout(unsigned long d, timerCallback f);