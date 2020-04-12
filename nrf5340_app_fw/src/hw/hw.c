/*
 * hw.c
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */




#include "hw.h"



extern uint32_t __isr_vector_addr;
extern uint32_t _image_start;
extern uint32_t _image_size;



__attribute__((section(".tag"))) const flash_tag_t fw_tag =
   {
    // fw info
    //
    .magic_number = 0xAAAA5555,
    .version_str  = "V200410R1",
    .board_str    = "NRF5340_PDK_B/D",
    .name_str     = "Firmware",
    .date_str     = __DATE__,
    .time_str     = __TIME__,
    .addr_tag     = (uint32_t)&fw_tag,
    .addr_fw      = (uint32_t)&__isr_vector_addr,

    .load_start   = (uint32_t)&_image_start,  // load_addr
    .load_size    = (uint32_t)&_image_size,   // load_size
   };


void hwInit(void)
{
  bspInit();

  swtimerInit();
  cmdifInit();

  ledInit();
  buttonInit();
  uartInit();
  uartOpen(_DEF_UART1, 57600);


  flashInit();
}

