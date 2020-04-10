/*
 * hw_def.h
 *
 *  Created on: 2020. 3. 20.
 *      Author: Baram
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"



#define _HW_DEF_RTOS_MEM_SIZE(x)              ((x)/1)

#define _HW_DEF_RTOS_THREAD_PRI_MAIN          osPriorityNormal
#define _HW_DEF_RTOS_THREAD_PRI_USB           osPriorityNormal
#define _HW_DEF_RTOS_THREAD_PRI_LED           osPriorityNormal
#define _HW_DEF_RTOS_THREAD_PRI_LCD           osPriorityNormal

#define _HW_DEF_RTOS_THREAD_MEM_MAIN          _HW_DEF_RTOS_MEM_SIZE( 6*1024 )
#define _HW_DEF_RTOS_THREAD_MEM_USB           _HW_DEF_RTOS_MEM_SIZE( 2*1024 )
#define _HW_DEF_RTOS_THREAD_MEM_LED           _HW_DEF_RTOS_MEM_SIZE( 1*1024 )
#define _HW_DEF_RTOS_THREAD_MEM_LCD           _HW_DEF_RTOS_MEM_SIZE( 8*1024 )



#define _USE_HW_RTOS


#define _USE_HW_LED
#define      HW_LED_MAX_CH          4






#endif /* SRC_HW_HW_DEF_H_ */
