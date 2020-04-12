/*
 * qspi.c
 *
 *  Created on: 2020. 4. 2.
 *      Author: HanCheol Cho
 */





#include "qspi.h"
#include "qspi/mx25r6435f.h"



static bool is_init = false;


static qspi_driver_t qspi;



bool qspiInit(void)
{
  bool ret = true;
  qspi_info_t info;


#if HW_QSPI_DRIVER == MX25R6435F
  mx25r6435fInitDriver(&qspi);
#else
  return false;
#endif


  if (qspi.init() == true)
  {
    ret = true;
  }
  else
  {
    ret = false;
  }


  if (qspi.getID(&info) == true)
  {
    logPrintf("QSPI               \t: OK\r\n");
  }
  else
  {
    logPrintf("QSPI               \t: Fail\r\n");
    ret = false;
  }


  is_init = ret;

  return ret;
}

bool qspiIsInit(void)
{
  return is_init;
}

bool qspiReset(void)
{
  bool ret = false;



  if (is_init == true)
  {
    if (qspi.reset() == true)
    {
      ret = true;
    }
  }

  return ret;
}

bool qspiRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret;

  if (addr >= qspiGetLength())
  {
    return false;
  }

  ret = qspi.read(p_data, addr, length);

  return ret;
}

bool qspiWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret;

  if (addr >= qspiGetLength())
  {
    return false;
  }

  ret = qspi.write(p_data, addr, length);

  return ret;
}

bool qspiEraseBlock(uint32_t block_addr)
{
  bool ret;

  ret = qspi.eraseBlock(block_addr);

  return ret;
}

bool qspiEraseSector(uint32_t block_addr)
{
  bool ret;

  ret = qspi.eraseSector(block_addr);

  return ret;
}

bool qspiErase(uint32_t addr, uint32_t length)
{
  bool ret;
  uint32_t flash_length;
  uint32_t block_size;
  uint32_t block_begin;
  uint32_t block_end;
  uint32_t i;


  flash_length = qspi.getFlashSize();
  block_size   = qspi.getSectorSize();


  if ((addr > flash_length) || ((addr+length) > flash_length))
  {
    return false;
  }
  if (length == 0)
  {
    return false;
  }


  block_begin = addr / block_size;
  block_end   = (addr + length - 1) / block_size;


  for (i=block_begin; i<=block_end; i++)
  {
    ret = qspiEraseSector(block_size*i);
    if (ret == false)
    {
      break;
    }
  }

  return ret;
}

bool qspiEraseChip(void)
{
  bool ret;

  ret = qspi.eraseChip();

  return ret;
}

bool qspiGetStatus(void)
{
  bool ret;

  ret = qspi.getStatus();

  return ret;
}

bool qspiGetInfo(qspi_info_t* p_info)
{
  bool ret;

  ret = qspi.getInfo(p_info);

  return ret;
}

bool qspiEnableMemoryMappedMode(void)
{
  bool ret;

  ret = qspi.enableMemoryMappedMode();

  return ret;
}

uint32_t qspiGetAddr(void)
{
  return QSPI_BASE_ADDR;
}

uint32_t qspiGetLength(void)
{
  return qspi.getFlashSize();
}

