/*
 * ap.cpp
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */




#include "ap.h"



static void threadLED(void const *argument);
static void threadLCD(void const *argument);


void apInit(void)
{
  hwInit();

  cmdifOpen(_DEF_UART1, 57600);

  osThreadDef(threadLED, threadLED, _HW_DEF_RTOS_THREAD_PRI_LED, 0, _HW_DEF_RTOS_THREAD_MEM_LED);
  osThreadCreate(osThread(threadLED), NULL);

  osThreadDef(threadLCD, threadLCD, _HW_DEF_RTOS_THREAD_PRI_LCD, 0, _HW_DEF_RTOS_THREAD_MEM_LCD);
  osThreadCreate(osThread(threadLCD), NULL);
}

void apMain(void)
{
  uint32_t pre_time;

  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();

      ledToggle(_DEF_LED2);
    }

    cmdifMain();
    osThreadYield();
  }
}

static void threadLED(void const *argument)
{
  while(1)
  {
    ledToggle(_DEF_LED1);
    delay(500);
  }
}


static void threadLCD(void const *argument)
{
  while(1)
  {
    ledToggle(_DEF_LED3);
    delay(1000);
  }
}
