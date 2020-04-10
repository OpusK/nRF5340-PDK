/*
 * bsp.c
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */




#include "bsp.h"
#include "rtos.h"
#include "nrf_clock.h"



volatile NRF_TIMER_Type *p_timer_us = NRF_TIMER2;


static volatile uint32_t systick_counter = 0;

extern void swtimerISR(void);


//void SysTick_Handler(void)
//{
  //systick_counter++;
  //swtimerISR();
//}

extern void bspYield(void);
void bspYield(void)
{
  //tud_task();
}

void bspInit(void)
{
/*
  nrf_systick_load_set(SystemCoreClock / (1000UL / (uint32_t)1)); // 1Khz
  nrf_systick_csr_set(
      NRF_SYSTICK_CSR_CLKSOURCE_CPU |
      NRF_SYSTICK_CSR_TICKINT_ENABLE |
      NRF_SYSTICK_CSR_ENABLE);
*/

  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0));



  p_timer_us->TASKS_STOP = (TIMER_TASKS_STOP_TASKS_STOP_Trigger << TIMER_TASKS_STOP_TASKS_STOP_Pos);
  p_timer_us->PRESCALER   = 4; // 16Mhz / (2^4) = 1Mhz
  p_timer_us->BITMODE     = (TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos);
  p_timer_us->MODE        = (TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos);
  p_timer_us->TASKS_START = (TIMER_TASKS_START_TASKS_START_Trigger << TIMER_TASKS_START_TASKS_START_Pos);
}

void bspDeInit(void)
{
  // Disable Interrupts
  //
  for (int i=0; i<16; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    __DSB();
    __ISB();
  }
  SysTick->CTRL = 0;
}

int __io_putchar(int ch)
{
  //uartWrite(_DEF_UART1, (uint8_t *)&ch, 1);
  return 1;
}

void delay(uint32_t ms)
{
  uint32_t pre_time = systick_counter;

  while(systick_counter-pre_time < ms);
}


uint32_t millis(void)
{
  static uint32_t pre_us_time = 0;
  static uint32_t ms_time_h = 0;
  static uint32_t ms_time_l = 0;
  uint32_t cur_us_time;

  cur_us_time = micros();
  ms_time_l = cur_us_time / 1000;


  if ((cur_us_time ^ pre_us_time) & (1<<31))
  {
    ms_time_h += 4294967; // Overflow 처리.
  }

  pre_us_time = cur_us_time;

  //return systick_counter;
  return (ms_time_h + ms_time_l);
}

uint32_t micros(void)
{
  volatile uint32_t cur_time;

  p_timer_us->TASKS_CAPTURE[0] = (TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Trigger << TIMER_TASKS_CAPTURE_TASKS_CAPTURE_Pos);
  cur_time = (uint32_t)p_timer_us->CC[0];

  return cur_time;
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void SecureFault_Handler(void)
{
  while(1);  /* Something went wrong */
}

