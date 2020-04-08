/*
 * led.c
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */




#include "led.h"
#include "nrf_gpio.h"


typedef struct
{
  uint32_t pin_number;
  uint8_t  on_state;
} led_port_t;


led_port_t led_port_tbl[LED_MAX_CH] =
{
    {NRF_GPIO_PIN_MAP(0,28), 0},
    {NRF_GPIO_PIN_MAP(0,29), 0},
    {NRF_GPIO_PIN_MAP(0,30), 0},
    {NRF_GPIO_PIN_MAP(0,31), 0},
};






bool ledInit(void)
{
  uint32_t i;


  for (i=0; i<LED_MAX_CH; i++)
  {
    nrf_gpio_cfg_output(led_port_tbl[i].pin_number);
    ledOff(i);
  }

  return true;
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  nrf_gpio_pin_write(led_port_tbl[ch].pin_number, led_port_tbl[ch].on_state);
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;
  nrf_gpio_pin_write(led_port_tbl[ch].pin_number, !led_port_tbl[ch].on_state);
}

void ledToggle(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;

  nrf_gpio_pin_toggle(led_port_tbl[ch].pin_number);
}
