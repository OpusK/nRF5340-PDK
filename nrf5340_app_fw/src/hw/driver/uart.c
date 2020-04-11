/*
 * uart.c
 *
 *  Created on: 2020. 4. 12.
 *      Author: Baram
 */




#include "uart.h"
#include "qbuffer.h"
#ifdef _USE_HW_VCP
#include "vcp.h"
#endif





#define UART_MODE_POLLING       0
#define UART_MODE_INTERRUPT     1
#define UART_MODE_DMA           2
#define UART_MODE_VCP           3

#define UART_HW_NONE            0
#define UART_HW_VCP             1
#define UART_HW_UART            2



#define UART_RX_BUF_LENGTH      1024





typedef struct
{
  bool     is_open;
  uint8_t  ch;
  uint32_t baud;
  uint8_t  tx_mode;
  uint8_t  rx_mode;
  uint8_t  hw_driver;

  uint32_t rx_i;
  uint8_t  rx_dma_buf[2][1];
  uint8_t  rx_buf[UART_RX_BUF_LENGTH];

  uint8_t  q_buf[UART_RX_BUF_LENGTH];

  qbuffer_t qbuffer_rx;

  NRF_UARTE_Type *handle;
} uart_t;



static uart_t uart_tbl[UART_MAX_CH];

static char print_buffer[UART_MAX_CH][256];


static void uartStartRx(uint8_t channel);
static uint32_t uartGetBaudData(uint32_t baud);


bool uartInit(void)
{
  uint8_t i;


  for (i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open  = false;
    uart_tbl[i].rx_mode  = UART_MODE_POLLING;
    uart_tbl[i].tx_mode  = UART_MODE_POLLING;
    uart_tbl[i].hw_driver = UART_HW_NONE;
    uart_tbl[i].rx_i     = 0;
  }

  return true;
}

bool uartOpen(uint8_t channel, uint32_t baud)
{
  bool ret = false;
  uart_t *p_uart;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }

  switch(channel)
  {
    case _DEF_UART1:
      p_uart = &uart_tbl[channel];

      p_uart->ch        = channel;
      p_uart->baud      = baud;
      p_uart->hw_driver = UART_HW_UART;
      p_uart->rx_mode   = UART_MODE_INTERRUPT;
      p_uart->tx_mode   = UART_MODE_POLLING;

      p_uart->handle =  NRF_UARTE0;

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);

      p_uart->is_open  = true;


      p_uart->handle->PSEL.RXD = NRF_GPIO_PIN_MAP(0,22);
      p_uart->handle->PSEL.TXD = NRF_GPIO_PIN_MAP(0,20);

      p_uart->handle->BAUDRATE = uartGetBaudData(baud);
      p_uart->handle->ENABLE = (UARTE_ENABLE_ENABLE_Enabled<<UARTE_ENABLE_ENABLE_Pos);
      p_uart->handle->SHORTS = UARTE_SHORTS_ENDRX_STARTRX_Msk;

      p_uart->handle->INTENSET =  UARTE_INTENSET_ENDRX_Msk
                                | UARTE_INTENSET_RXSTARTED_Msk
                                | UARTE_INTENSET_ERROR_Msk;

      NVIC_SetPriority(SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQn, 7);
      NVIC_EnableIRQ(SPIM0_SPIS0_TWIM0_TWIS0_UARTE0_IRQn);

      uartStartRx(channel);
      ret = true;
      break;

    case _DEF_UART2:
      p_uart = &uart_tbl[channel];

      p_uart->baud      = baud;
      p_uart->hw_driver = UART_HW_VCP;
      p_uart->rx_mode   = UART_MODE_VCP;
      p_uart->tx_mode   = UART_MODE_VCP;
      p_uart->is_open   = true;
      ret = true;
      break;
  }

  return ret;
}

bool uartClose(uint8_t channel)
{
  bool ret = false;


  if (channel >= UART_MAX_CH)
  {
    return false;
  }


  if (uart_tbl[channel].is_open == true && uart_tbl[channel].hw_driver == UART_HW_UART)
  {
  }

  return ret;
}

void uartStartRx(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];

  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    p_uart->handle->EVENTS_ENDRX = 0;
    p_uart->handle->RXD.PTR = (uint32_t)p_uart->rx_dma_buf[p_uart->rx_i];
    p_uart->rx_i ^= 1;
    p_uart->handle->RXD.MAXCNT = 1;

    p_uart->handle->TASKS_STARTRX = 1;
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    //HAL_UART_Receive_DMA(p_uart->handle, (uint8_t *)p_uart->qbuffer_rx.p_buf, p_uart->qbuffer_rx.length);
  }
}

uint32_t uartGetBaud(uint8_t channel)
{
  if (channel >= UART_MAX_CH)
  {
    return 0;
  }

#ifdef _USE_HW_VCP
  if (uart_tbl[channel].hw_driver == UART_HW_VCP)
  {
    return vcpGetBaud();
  }
#endif

  return uart_tbl[channel].baud;
}

uint32_t uartGetBaudData(uint32_t baud)
{
  uint32_t ret;

  switch(baud)
  {
    case 1200:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud1200;
      break;
    case 2400:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud2400;
      break;
    case 4800:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud4800;
      break;
    case 9600:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud9600;
      break;
    case 14400:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud14400;
      break;
    case 19200:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud19200;
      break;
    case 28800:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud28800;
      break;
    case 31250:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud31250;
      break;
    case 38400:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud38400;
      break;
    case 56000:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud56000;
      break;
    case 57600:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud57600;
      break;
    case 76800:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud76800;
      break;
    case 115200:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud115200;
      break;
    case 230400:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud230400;
      break;
    case 250000:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud250000;
      break;
    case 460800:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud460800;
      break;
    case 921600:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud921600;
      break;
    case 1000000:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud1M;
      break;

    default:
      ret = UARTE_BAUDRATE_BAUDRATE_Baud57600;
      break;
  }

  return ret;
}

uint32_t uartAvailable(uint8_t channel)
{
  uint32_t ret = true;
  uart_t *p_uart = &uart_tbl[channel];


  if (channel >= UART_MAX_CH)
  {
    return 0;
  }
#ifdef _USE_HW_VCP
  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpAvailable();
  }
#endif
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    ret = qbufferAvailable(&uart_tbl[channel].qbuffer_rx);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    //p_uart->qbuffer_rx.ptr_in = p_uart->qbuffer_rx.length - ((DMA_Stream_TypeDef *)p_uart->hdma_rx->Instance)->NDTR;
    //ret = qbufferAvailable(&p_uart->qbuffer_rx);
  }

  return ret;
}

void uartFlush(uint8_t channel)
{
#ifdef _USE_HW_VCP
  if (uart_tbl[channel].rx_mode == UART_MODE_VCP)
  {
    vcpFlush();
  }
#endif

  if (uart_tbl[channel].rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferFlush(&uart_tbl[channel].qbuffer_rx);
  }
  if (uart_tbl[channel].rx_mode == UART_MODE_DMA)
  {
    //uart_tbl[channel].qbuffer_rx.ptr_in  = uart_tbl[channel].qbuffer_rx.length - ((DMA_Stream_TypeDef *)uart_tbl[channel].hdma_rx->Instance)->NDTR;
    //uart_tbl[channel].qbuffer_rx.ptr_out = uart_tbl[channel].qbuffer_rx.ptr_in;
  }
}

void uartPutch(uint8_t channel, uint8_t ch)
{
  uartWrite(channel, &ch, 1 );
}

uint8_t uartGetch(uint8_t channel)
{
  uint8_t ret = 0;


  while(1)
  {
    if (uartAvailable(channel) > 0)
    {
      ret = uartRead(channel);
      break;
    }
  }

  return ret;
}

int32_t uartWrite(uint8_t channel, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];
  uint32_t pre_time;

  if (p_uart->is_open != true)
  {
    return 0;
  }

#ifdef _USE_HW_VCP
  if (p_uart->tx_mode == UART_MODE_VCP)
  {
    vcpWrite(p_data, length);
  }
#endif
  if (p_uart->tx_mode == UART_MODE_POLLING)
  {
    p_uart->handle->EVENTS_TXDRDY = 0;
    p_uart->handle->EVENTS_ENDTX = 0;

    p_uart->handle->TXD.PTR = (uint32_t)p_data;
    p_uart->handle->TXD.MAXCNT = length;
    p_uart->handle->TASKS_STARTTX = 1;

    pre_time = millis();

    while(!p_uart->handle->EVENTS_ENDTX)
    {
      if (millis()-pre_time >= 100)
      {
        break;
      }
#ifdef _USE_HW_RTOS
      osThreadYield();
#endif
    }
    ret = length;
  }
  if (p_uart->tx_mode == UART_MODE_DMA)
  {
  }
  return ret;
}

uint8_t uartRead(uint8_t channel)
{
  uint8_t ret = 0;
  uart_t *p_uart = &uart_tbl[channel];


#ifdef _USE_HW_VCP
  if (p_uart->rx_mode == UART_MODE_VCP)
  {
    ret = vcpRead();
  }
#endif
  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  if (p_uart->rx_mode == UART_MODE_DMA)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }
  return ret;
}

int32_t uartPrintf(uint8_t channel, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;


  len = vsnprintf(print_buffer[channel], 255, fmt, arg);
  va_end (arg);

  ret = uartWrite(channel, (uint8_t *)print_buffer[channel], len);

  return ret;
}

void uartRxHandler(uint8_t channel)
{
  uart_t *p_uart = &uart_tbl[channel];


  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {

    uartStartRx(channel);
  }
}



void UARTE0_IRQHandler(void)
{
  uint8_t data;
  uart_t *p_uart = &uart_tbl[_DEF_UART1];
  uint32_t rx_cnt;


  if (p_uart->handle->EVENTS_ENDRX)
  {
    p_uart->handle->EVENTS_ENDRX = 0;
    rx_cnt = p_uart->handle->RXD.AMOUNT;

    qbufferWrite(&p_uart->qbuffer_rx, p_uart->rx_dma_buf[p_uart->rx_i], rx_cnt);
  }

  if (p_uart->handle->EVENTS_RXSTARTED)
  {
    p_uart->handle->EVENTS_RXSTARTED = 0;
    p_uart->handle->RXD.PTR = (uint32_t)p_uart->rx_dma_buf[p_uart->rx_i];
    p_uart->rx_i ^= 1;
  }

  if (p_uart->handle->EVENTS_ERROR)
  {
    p_uart->handle->EVENTS_ERROR = 0;
  }
}
