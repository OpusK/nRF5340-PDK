#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__




#define NRF_ACL        NRF_ACL_NS
#define NRF_AAR        NRF_AAR_NS
#define NRF_CCM        NRF_CCM_NS
#define NRF_CLOCK      NRF_CLOCK_NS
#define NRF_DCNF       NRF_DCNF_NS
#define NRF_DPPIC      NRF_DPPIC_NS
#define NRF_ECB        NRF_ECB_NS
#define NRF_EGU0       NRF_EGU0_NS
#define NRF_FICR       NRF_FICR_NS
#define NRF_GPIOTE     NRF_GPIOTE_NS
#define NRF_IPC        NRF_IPC_NS
#define NRF_NVMC       NRF_NVMC_NS
#define NRF_P0         NRF_P0_NS
#define NRF_P1         NRF_P1_NS
#define NRF_POWER      NRF_POWER_NS
#define NRF_RADIO      NRF_RADIO_NS
#define NRF_RESET      NRF_RESET_NS
#define NRF_RNG        NRF_RNG_NS
#define NRF_RTC0       NRF_RTC0_NS
#define NRF_RTC1       NRF_RTC1_NS
#define NRF_SPIM0      NRF_SPIM0_NS
#define NRF_SPIS0      NRF_SPIS0_NS
#define NRF_SWI0       NRF_SWI0_NS
#define NRF_SWI1       NRF_SWI1_NS
#define NRF_SWI2       NRF_SWI2_NS
#define NRF_SWI3       NRF_SWI3_NS
#define NRF_TEMP       NRF_TEMP_NS
#define NRF_TIMER0     NRF_TIMER0_NS
#define NRF_TIMER1     NRF_TIMER1_NS
#define NRF_TIMER2     NRF_TIMER2_NS
#define NRF_TWIM0      NRF_TWIM0_NS
#define NRF_TWIS0      NRF_TWIS0_NS
#define NRF_UARTE0     NRF_UARTE0_NS
#define NRF_UICR       NRF_UICR_NS
#define NRF_VMC        NRF_VMC_NS
#define NRF_VREQCTRL   NRF_VREQCTRL_NS
#define NRF_WDT        NRF_WDT_NS





/*
 * The following section provides the name translation for peripherals with
 * only one type of access available. For these peripherals, you cannot choose
 * between secure and non-secure mapping.
 */
/*
#define NRF_CACHE      NRF_CACHE_S
#define NRF_CACHEINFO  NRF_CACHEINFO_S
#define NRF_CACHEDATA  NRF_CACHEDATA_S
#define NRF_CRYPTOCELL NRF_CRYPTOCELL_S
#define NRF_FICR       NRF_FICR_S
#define NRF_GPIOTE0    NRF_GPIOTE0_S
#define NRF_GPIOTE1    NRF_GPIOTE1_NS
#define NRF_SPU        NRF_SPU_S
#define NRF_UICR       NRF_UICR_S
*/



#define NRFX_SYSTICK_ENABLED            1
#define NRFX_NVMC_ENABLED               1

#define NRFX_POWER_ENABLED              1
#define NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY  7

#endif // NRFX_CONFIG_H__
