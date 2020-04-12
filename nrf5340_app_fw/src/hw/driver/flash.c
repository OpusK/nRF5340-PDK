/*
 * flash.c
 *
 *  Created on: 2020. 3. 13.
 *      Author: Baram
 */






#include "flash.h"
#include "cmdif.h"
#include "nrfx_nvmc.h"
#ifdef _USE_HW_QSPI
#include "qspi.h"
#endif



#define FLASH_ADDR_OFFSET         0x00000000
#define FLASH_MAX_SIZE            (1*1024*1024)
#define FLASH_SECTOR_SIZE         (4*1024)
#define FLASH_PAGE_SIZE           (256)
#define FLASH_MAX_SECTOR          (FLASH_MAX_SIZE / FLASH_SECTOR_SIZE)


static bool is_init = false;


static bool _flashInit(void);
static bool _flashEraseSector(uint32_t start_sector,  uint32_t sector_cnt);
static bool _flashWritePage(uint32_t addr, uint32_t buf_addr);



#ifdef _USE_HW_CMDIF
void flashCmdifInit(void);
void flashCmdif(void);
#endif




bool flashInit(void)
{

  if (_flashInit() == false)
  {
    logPrintf("False                \t : Fail\r\n");
  }

#ifdef _USE_HW_QSPI
  qspiInit();
#endif

#ifdef _USE_HW_CMDIF
  flashCmdifInit();
#endif

  is_init = true;

  return true;
}

bool flashErase(uint32_t addr, uint32_t length)
{
  bool ret = false;

  int32_t start_sector = -1;
  int32_t end_sector = -1;


#ifdef _USE_HW_QSPI
  if (addr >= qspiGetAddr() && addr < (qspiGetAddr() + qspiGetLength()))
  {
    ret = qspiErase(addr - qspiGetAddr(), length);
    return ret;
  }
#endif


  if (addr < FLASH_ADDR_OFFSET) return false;
  if (addr >= (FLASH_ADDR_OFFSET + FLASH_MAX_SIZE)) return false;
  if ((addr+length) > (FLASH_ADDR_OFFSET + FLASH_MAX_SIZE)) return false;

  start_sector = -1;
  end_sector = -1;


  for (int i=0; i<FLASH_MAX_SECTOR; i++)
  {
    bool update = false;
    uint32_t start_addr;
    uint32_t end_addr;


    start_addr = FLASH_ADDR_OFFSET + i * FLASH_SECTOR_SIZE;
    end_addr   = start_addr + FLASH_SECTOR_SIZE - 1;

    if (start_addr >= addr && start_addr < (addr+length))
    {
      update = true;
    }
    if (end_addr >= addr && end_addr < (addr+length))
    {
      update = true;
    }

    if (addr >= start_addr && addr <= end_addr)
    {
      update = true;
    }
    if ((addr+length-1) >= start_addr && (addr+length-1) <= end_addr)
    {
      update = true;
    }


    if (update == true)
    {
      if (start_sector < 0)
      {
        start_sector = i;
      }
      end_sector = i;
    }
  }

  if (start_sector >= 0)
  {
    ret = _flashEraseSector(start_sector,  (end_sector - start_sector) + 1);
  }


  return ret;
}

bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t index;
  uint32_t write_length;
  uint32_t write_addr;
  uint8_t buf[FLASH_PAGE_SIZE];
  uint32_t offset;



#ifdef _USE_HW_QSPI
  if (addr >= qspiGetAddr() && addr < (qspiGetAddr() + qspiGetLength()))
  {
    ret = qspiWrite(addr - qspiGetAddr(), p_data, length);
    return ret;
  }
#endif


  index = 0;
  offset = addr%FLASH_PAGE_SIZE;

  if (offset != 0 || length < FLASH_PAGE_SIZE)
  {
    write_addr = addr - offset;
    memcpy(&buf[0], (void *)write_addr, FLASH_PAGE_SIZE);
    memcpy(&buf[offset], &p_data[0], constrain(FLASH_PAGE_SIZE-offset, 0, length));

    ret = _flashWritePage(write_addr, (uint32_t)buf);
    if (ret != true)
    {
      return false;
    }

    if (length < FLASH_PAGE_SIZE)
    {
      index += length;
    }
    else
    {
      index += (FLASH_PAGE_SIZE - offset);
    }
  }


  while(index < length)
  {
    write_length = constrain(length - index, 0, FLASH_PAGE_SIZE);

    ret = _flashWritePage(addr + index, (uint32_t)&p_data[index]);
    if (ret != true)
    {
      ret = false;
      break;
    }

    index += write_length;

    if ((length - index) > 0 && (length - index) < FLASH_PAGE_SIZE)
    {
      offset = length - index;
      write_addr = addr + index;
      memcpy(&buf[0], (void *)write_addr, FLASH_PAGE_SIZE);
      memcpy(&buf[0], &p_data[index], offset);

      ret = _flashWritePage(write_addr, (uint32_t)buf);
      if (ret != true)
      {
        return false;
      }
      break;
    }
  }

  return ret;
}

bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint8_t *p_byte = (uint8_t *)addr;



  for (int i=0; i<length; i++)
  {
    p_data[i] = p_byte[i];
  }

  return ret;
}







#ifdef _USE_HW_CMDIF
void flashCmdifInit(void)
{
  cmdifAdd("flash", flashCmdif);
}

void flashCmdif(void)
{
  bool ret = true;
  uint32_t i;
  uint32_t addr;
  uint32_t length;
  uint8_t  data;
  uint32_t pre_time;
  bool flash_ret;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("info", 0) == true)
    {
      cmdifPrintf("flash init  : %d\n", is_init);
      cmdifPrintf("flash addr  : 0x%X\n", 0x00000000);
      #ifdef _USE_HW_QSPI
      cmdifPrintf("qspi init   : %d\n", (int)qspiIsInit());
      cmdifPrintf("qspi addr   : 0x%X\n", qspiGetAddr());
      #endif
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 3)
  {
    if(cmdifHasString("read", 0) == true)
    {
      addr   = (uint32_t)cmdifGetParam(1);
      length = (uint32_t)cmdifGetParam(2);

      for (i=0; i<length; i++)
      {
        flash_ret = flashRead(addr+i, &data, 1);

        if (flash_ret == true)
        {
          cmdifPrintf( "addr : 0x%X\t 0x%02X\n", addr+i, data);
        }
        else
        {
          cmdifPrintf( "addr : 0x%X\t Fail\n", addr+i);
        }
      }
    }
    else if(cmdifHasString("erase", 0) == true)
    {
      addr   = (uint32_t)cmdifGetParam(1);
      length = (uint32_t)cmdifGetParam(2);

      pre_time = millis();
      flash_ret = flashErase(addr, length);

      cmdifPrintf( "addr : 0x%X\t len : %d %d ms\n", addr, length, (millis()-pre_time));
      if (flash_ret)
      {
        cmdifPrintf("OK\n");
      }
      else
      {
        cmdifPrintf("FAIL\n");
      }
    }
    else if(cmdifHasString("write", 0) == true)
    {
      addr = (uint32_t)cmdifGetParam(1);
      data = (uint8_t )cmdifGetParam(2);

      pre_time = millis();
      flash_ret = flashWrite(addr, &data, 1);

      cmdifPrintf( "addr : 0x%X\t 0x%02X %d ms\n", addr, data, millis()-pre_time);
      if (flash_ret)
      {
        cmdifPrintf("OK\n");
      }
      else
      {
        cmdifPrintf("FAIL\n");
      }
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "flash info\n");
    cmdifPrintf( "flash read  [addr] [length]\n");
    cmdifPrintf( "flash erase [addr] [length]\n");
    cmdifPrintf( "flash write [addr] [data]\n");
  }
}
#endif









bool _flashInit(void)
{

  return true;
}

bool _flashEraseSector(uint32_t start_sector,  uint32_t sector_cnt)
{
  bool ret = true;
  uint32_t start_addr;


  start_addr = start_sector * FLASH_SECTOR_SIZE;


  for (int i=0; i<sector_cnt; i++)
  {
    nrfx_nvmc_page_erase(start_addr + (i*FLASH_SECTOR_SIZE));
  }

  return ret;
}

bool _flashWritePage(uint32_t addr, uint32_t buf_addr)
{
  uint32_t buf[FLASH_PAGE_SIZE/4];
  uint8_t *p_dst;
  uint8_t *p_src;
  bool ret = true;

  p_dst = (uint8_t *)buf;
  p_src = (uint8_t *)buf_addr;

  for (int i=0; i<FLASH_PAGE_SIZE; i++)
  {
    p_dst[i] = p_src[i];
  }

  nrfx_nvmc_words_write(addr, buf, FLASH_PAGE_SIZE/4);

  return ret;
}





