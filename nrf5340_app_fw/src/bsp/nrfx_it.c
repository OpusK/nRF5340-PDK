/*
 * nrfx_it.c
 *
 *  Created on: 2020. 4. 12.
 *      Author: Baram
 */



__attribute__((weak)) void UARTE0_IRQHandler(void)
{

}


void SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQHandler(void)
{
  UARTE0_IRQHandler();
}
