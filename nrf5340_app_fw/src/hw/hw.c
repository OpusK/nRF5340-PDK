/*
 * hw.c
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */




#include "hw.h"




__attribute__((aligned(2048))) __attribute__((used, section(".tag"))) const boot_tag_t boot_tag =
    {
        .boot_name    = "NRF5340_PDK_B/D",
        .boot_ver     = "B2000408R1",
        .magic_number = 0x5555AAAA,
    };


void hwInit(void)
{
  bspInit();

  ledInit();
}
