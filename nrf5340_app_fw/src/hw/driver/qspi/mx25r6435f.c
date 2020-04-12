/*
 * mx25r6435f.c
 *
 *  Created on: 2020. 4. 2.
 *      Author: HanCheol Cho
 */




#include "qspi/mx25r6435f.h"
#include "nrfx_qspi.h"

#if defined(_USE_HW_QSPI) && HW_QSPI_DRIVER == MX25R6435F




#define _FLASH_SIZE       0x800000    /* 64   MBits => 8MBytes */
#define _BLOCK_SIZE       0x10000     /* 128  blocks of 64KBytes */
#define _SECTOR_SIZE      0x1000      /* 2048 subsectors of 4kBytes */
#define _PAGE_SIZE        0x100       /* 65536 pages of 256 bytes */



#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99
#define QSPI_STD_CMD_RDID   0x9F



static bool init(void);
static bool getID(qspi_info_t *p_info);
static bool getInfo(qspi_info_t *p_info);
static bool reset(void);
static bool read(uint8_t *p_data, uint32_t addr, uint32_t length);
static bool write(uint8_t *p_data, uint32_t addr, uint32_t length);
static bool eraseBlock(uint32_t block_addr);
static bool eraseSector(uint32_t sector_addr);
static bool eraseChip(void);
static bool getStatus(void);
static bool enableMemoryMappedMode(void);

static uint32_t getFlashSize(void);
static uint32_t getSectorSize(void);
static uint32_t getBlockSize(void);
static uint32_t getPageSize(void);




static nrfx_qspi_config_t config;








bool mx25r6435fInitDriver(qspi_driver_t *p_driver)
{
  p_driver->init = init;
  p_driver->getID = getID;
  p_driver->getInfo = getInfo;
  p_driver->reset = reset;
  p_driver->read = read;
  p_driver->write = write;
  p_driver->eraseBlock = eraseBlock;
  p_driver->eraseSector = eraseSector;
  p_driver->eraseChip = eraseChip;
  p_driver->getStatus = getStatus;
  p_driver->enableMemoryMappedMode = enableMemoryMappedMode;
  p_driver->getFlashSize = getFlashSize;
  p_driver->getBlockSize = getBlockSize;
  p_driver->getSectorSize = getSectorSize;
  p_driver->getPageSize = getPageSize;

  return true;
}

#if 0
static void qspiHandler(nrfx_qspi_evt_t event, void * p_context)
{
  (void)event;
  (void)p_context;

  is_finished = true;
}
#endif



bool init(void)
{
  bool ret = true;

  config.xip_offset = 0;
  config.pins.sck_pin = NRF_GPIO_PIN_MAP(0,17);
  config.pins.csn_pin = NRF_GPIO_PIN_MAP(0,18);
  config.pins.io0_pin = NRF_GPIO_PIN_MAP(0,13);
  config.pins.io1_pin = NRF_GPIO_PIN_MAP(0,14);
  config.pins.io2_pin = NRF_GPIO_PIN_MAP(0,15);
  config.pins.io3_pin = NRF_GPIO_PIN_MAP(0,16);

  config.prot_if.readoc    = NRF_QSPI_READOC_FASTREAD;
  config.prot_if.writeoc   = NRF_QSPI_WRITEOC_PP;
  config.prot_if.addrmode  = NRF_QSPI_ADDRMODE_24BIT;
  config.prot_if.dpmconfig = false;

  config.phy_if.sck_delay = 0x05;
  config.phy_if.dpmen     = false;
  config.phy_if.spi_mode  = NRF_QSPI_MODE_0;
  config.phy_if.sck_freq  = NRF_QSPI_FREQ_DIV2;

  config.irq_priority  = (uint8_t)6;


  if (nrfx_qspi_init(&config, NULL, NULL) != NRFX_SUCCESS)
  {
    ret = false;
  }



  uint8_t temporary = 0x40;
  uint32_t err_code;
  nrf_qspi_cinstr_conf_t cinstr_cfg = {
      .opcode    = QSPI_STD_CMD_RSTEN,
      .length    = NRF_QSPI_CINSTR_LEN_1B,
      .io2_level = true,
      .io3_level = true,
      .wipwait   = true,
      .wren      = true
  };

  // Send reset enable
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }

  // Send reset command
  cinstr_cfg.opcode = QSPI_STD_CMD_RST;
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }

  // Switch to qspi mode
  cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
  cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }



  return ret;
}

bool getID(qspi_info_t *p_info)
{
  bool ret = true;

  if (getInfo(p_info) != true)
  {
    return false;
  }

  if (p_info->device_id[0] != 0xC2 || p_info->device_id[1] != 0x28 || p_info->device_id[2] != 0x17 )
  {
    ret = false;
  }

  return ret;
}

bool getInfo(qspi_info_t *p_info)
{
  bool ret = true;
  uint32_t err_code;

  nrf_qspi_cinstr_conf_t cinstr_cfg = {
      .opcode    = QSPI_STD_CMD_RDID,
      .length    = NRF_QSPI_CINSTR_LEN_4B,
      .io2_level = true,
      .io3_level = true,
      .wipwait   = true,
      .wren      = false
  };

  p_info->FlashSize          = _FLASH_SIZE;
  p_info->EraseSectorSize    = _SECTOR_SIZE;
  p_info->EraseSectorsNumber = (_FLASH_SIZE/_SECTOR_SIZE);
  p_info->ProgPageSize       = _PAGE_SIZE;
  p_info->ProgPagesNumber    = (_FLASH_SIZE/_PAGE_SIZE);


  // Send reset enable
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, p_info->device_id);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }

  return ret;
}

bool reset(void)
{
  bool ret = true;
  uint32_t err_code;

  nrf_qspi_cinstr_conf_t cinstr_cfg = {
      .opcode    = QSPI_STD_CMD_RSTEN,
      .length    = NRF_QSPI_CINSTR_LEN_1B,
      .io2_level = true,
      .io3_level = true,
      .wipwait   = true,
      .wren      = true
  };

  // Send reset enable
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }

  // Send reset command
  cinstr_cfg.opcode = QSPI_STD_CMD_RST;
  err_code = nrfx_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
  if (err_code != NRFX_SUCCESS)
  {
    return false;
  }

  return ret;
}

bool read(uint8_t *p_data, uint32_t addr, uint32_t length)
{
  bool ret = true;
  uint32_t err_code;

  err_code = nrfx_qspi_read(p_data, length, addr);
  if (err_code != NRFX_SUCCESS)
  {
    ret = false;
  }

  return ret;
}

bool writePage(uint32_t addr, uint32_t data_addr)
{
  bool ret = true;
  uint32_t err_code;

  err_code = nrfx_qspi_write((void *)data_addr, _PAGE_SIZE, addr);
  if (err_code != NRFX_SUCCESS)
  {
    ret = false;
  }

  return ret;
}

bool write(uint8_t *p_data, uint32_t addr, uint32_t length)
{
  bool ret = true;
  uint32_t index;
  uint32_t write_length;
  uint32_t write_addr;
  uint32_t buf_mem[_PAGE_SIZE];
  uint8_t  *buf;
  uint32_t offset;


  index = 0;
  buf = (uint8_t *)buf_mem;
  offset = addr%_PAGE_SIZE;

  if (offset != 0 || length < _PAGE_SIZE)
  {
    write_addr = addr - offset;
    //memcpy(&buf[0], (void *)write_addr, _PAGE_SIZE);
    read(&buf[0], write_addr, _PAGE_SIZE);
    memcpy(&buf[offset], &p_data[0], constrain(_PAGE_SIZE-offset, 0, length));

    ret = writePage(write_addr, (uint32_t)buf);
    if (ret != true)
    {
      return false;
    }

    if (length < _PAGE_SIZE)
    {
      index += length;
    }
    else
    {
      index += (_PAGE_SIZE - offset);
    }
  }


  while(index < length)
  {
    write_length = constrain(length - index, 0, _PAGE_SIZE);

    ret = writePage(addr + index, (uint32_t)&p_data[index]);
    if (ret != true)
    {
      ret = false;
      break;
    }

    index += write_length;

    if ((length - index) > 0 && (length - index) < _PAGE_SIZE)
    {
      offset = length - index;
      write_addr = addr + index;
      //memcpy(&buf[0], (void *)write_addr, _PAGE_SIZE);
      read(&buf[0], write_addr, _PAGE_SIZE);
      memcpy(&buf[0], &p_data[index], offset);

      ret = writePage(write_addr, (uint32_t)buf);
      if (ret != true)
      {
        return false;
      }
      break;
    }
  }

  return ret;
}

bool eraseBlock(uint32_t block_addr)
{
  bool ret = true;
  uint32_t err_code;

  err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_64KB, block_addr);
  if (err_code != NRFX_SUCCESS)
  {
    ret = false;
  }
  return ret;
}

bool eraseSector(uint32_t sector_addr)
{
  bool ret = true;
  uint32_t err_code;

  err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, sector_addr);
  if (err_code != NRFX_SUCCESS)
  {
    ret = false;
  }
  return ret;
}

bool eraseChip(void)
{
  bool ret = true;
  uint32_t err_code;

  err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_ALL, 0);
  if (err_code != NRFX_SUCCESS)
  {
    ret = false;
  }
  return ret;
}

bool getStatus(void)
{
  bool ret = true;

  return ret;
}

bool enableMemoryMappedMode(void)
{
  bool ret = true;

  return ret;
}

uint32_t getFlashSize(void)
{
  return _FLASH_SIZE;
}

uint32_t getSectorSize(void)
{
  return _SECTOR_SIZE;
}

uint32_t getBlockSize(void)
{
  return _BLOCK_SIZE;
}

uint32_t getPageSize(void)
{
  return _PAGE_SIZE;
}









#endif
