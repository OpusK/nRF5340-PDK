/*
 * bsp.h
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"
#include "nrf.h"


#include "cmsis_os.h"


#include "nrfx_systick.h"
#include "nrf_gpio.h"
#include "nrf_clock.h"


extern int32_t uartPrintf(uint8_t channel, const char *fmt, ...);

//#define logPrintf(...)    printf(__VA_ARGS__)
#define logPrintf(...)    uartPrintf(_DEF_UART1, __VA_ARGS__)


void bspYield(void);

void bspInit(void);
void bspDeInit(void);

extern void delay(uint32_t delay_ms);
extern uint32_t millis(void);
extern uint32_t micros(void);


#ifdef __cplusplus
}
#endif


#endif /* SRC_BSP_BSP_H_ */
