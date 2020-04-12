/*
 * mx25r6435f.h
 *
 *  Created on: 2020. 4. 2.
 *      Author: HanCheol Cho
 */

#ifndef SRC_COMMON_HW_INCLUDE_MX25R6435F_H_
#define SRC_COMMON_HW_INCLUDE_MX25R6435F_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


#if defined(_USE_HW_QSPI) && HW_QSPI_DRIVER == MX25R6435F

#include "qspi.h"


bool mx25r6435fInitDriver(qspi_driver_t *p_driver);


#endif

#ifdef __cplusplus
}
#endif




#endif /* SRC_COMMON_HW_INCLUDE_MX25R6435F_H_ */
