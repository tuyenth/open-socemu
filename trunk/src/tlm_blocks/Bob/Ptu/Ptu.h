#ifndef PTU_H_
#define PTU_H_

#include "Generic/Peripheral/Peripheral.h"

/// Registers definition
enum
{
    REG_PTU_DC_PTU_ADDR,
    REG_PTU_DP_EP2I_HCI_PKGLEN,
    REG_PTU_DC_PTU_RW,
    REG_PTU_DC_PTU_STATUS,
    REG_PTU_DC_PTU_HCI_SEL,
    REG_PTU_DC_PTU_HC_SEL,
    REG_PTU_DC_PTU_SEL_CTL,
    REG_PTU_DP_UART_DATA,
    REG_PTU_DP_EP0_DATA,
    REG_PTU_DP_EP1_DATA,
    REG_PTU_DP_EP2_DATA,
    REG_PTU_DP_EP3_DATA,
    REG_PTU_DP_EP4_DATA,
    REG_PTU_DP_EP_TRG,
    REG_PTU_DP_EP0_PKLEN,
    REG_PTU_DP_EP2_PKLEN,
    REG_PTU_DP_EP3_PKLEN,
    REG_PTU_DP_EP4_PKLEN,
    REG_PTU_DP_EP2_DP,
    REG_PTU_DP2_EP0_DATA,
    REG_PTU_DP2_EP1_DATA,
    REG_PTU_DP2_EP_TRG,
    REG_PTU_DP2_EP0_PKLEN,
    REG_PTU_DP3_EP0_DATA,
    REG_PTU_DP3_EP1_DATA,
    REG_PTU_DP3_EP_TRG,
    REG_PTU_DP3_EP0_PKLEN,
    REG_PTU_DC_PTU_HUBCUR,
    REG_PTU_DP_ESPI_DATA,
    REG_PTU_SR_PTU_STATUS0,
    REG_PTU_SR_PTU_STATUS1,
    REG_PTU_SR_PTU_STATUS2,
    REG_PTU_SR_PTU_STATUS3,
    REG_PTU_SR_PTU_STATUS4,
    REG_PTU_SR_PTU_STATUS5,
    REG_PTU_SR_PTU_STATUS6,
    REG_PTU_SR_PTU_STATUS7,
    REG_PTU_SR_PTU_STATUS8,
    REG_PTU_SR_PTU_EN0,
    REG_PTU_SR_PTU_EN1,
    REG_PTU_SR_PTU_EN2,
    REG_PTU_SR_PTU_EN3,
    REG_PTU_SR_PTU_EN4,
    REG_PTU_SR_PTU_EN5,
    REG_PTU_SR_PTU_EN6,
    REG_PTU_SR_PTU_EN7,
    REG_PTU_SR_PTU_EN8,
    REG_PTU_SR_PTU_DIS0,
    REG_PTU_SR_PTU_DIS1,
    REG_PTU_SR_PTU_DIS2,
    REG_PTU_SR_PTU_DIS3,
    REG_PTU_SR_PTU_DIS4,
    REG_PTU_SR_PTU_TEST0,
    REG_PTU_SR_PTU_TEST1,
    REG_PTU_SR_PTU_TEST2,
    REG_PTU_SR_PTU_TEST3,
    REG_PTU_SR_PTU_ESET4,
    REG_PTU_SR_PTU_ECLR4,
    REG_PTU_DC_PTU_PWRON2GOOD,
    REG_PTU_SR_PTU_STATUS9,
    REG_PTU_SR_PTU_EN9,
    REG_PTU_SR_PTU_DIS9,
    REG_PTU_SR_PTU_TEST9,
    REG_PTU_DP_EP2O_HCI_PKGLEN,
    REG_PTU_USB0_DC_USB_CFG,
    REG_PTU_USB0_DC_INTERFACEVAL,
    REG_PTU_USB0_DC_MAXPOWER,
    REG_PTU_USB0_DC_CFGNUM,
    REG_PTU_USB0_DC_BMRT,
    REG_PTU_USB0_DC_BREQUEST,
    REG_PTU_USB0_DC_WVALUEL,
    REG_PTU_USB0_DC_WVALUEH,
    REG_PTU_USB0_DC_WINDEXL,
    REG_PTU_USB0_DC_WINDEXH,
    REG_PTU_USB0_DC_WLENGTHL,
    REG_PTU_USB0_DC_WLENGTHH,
    REG_PTU_USB1_DC_USB_CFG = 0x200/4,
    REG_PTU_USB1_DC_INTERFACEVAL,
    REG_PTU_USB1_DC_MAXPOWER,
    REG_PTU_USB1_DC_CFGNUM,
    REG_PTU_USB1_DC_BMRT,
    REG_PTU_USB1_DC_BREQUEST,
    REG_PTU_USB1_DC_WVALUEL,
    REG_PTU_USB1_DC_WVALUEH,
    REG_PTU_USB1_DC_WINDEXL,
    REG_PTU_USB1_DC_WINDEXH,
    REG_PTU_USB1_DC_WLENGTHL,
    REG_PTU_USB1_DC_WLENGTHH,
    REG_PTU_USB2_DC_USB_CFG = 0x300/4,
    REG_PTU_USB2_DC_INTERFACEVAL,
    REG_PTU_USB2_DC_MAXPOWER,
    REG_PTU_USB2_DC_CFGNUM,
    REG_PTU_USB2_DC_BMRT,
    REG_PTU_USB2_DC_BREQUEST,
    REG_PTU_USB2_DC_WVALUEL,
    REG_PTU_USB2_DC_WVALUEH,
    REG_PTU_USB2_DC_WINDEXL,
    REG_PTU_USB2_DC_WINDEXH,
    REG_PTU_USB2_DC_WLENGTHL,
    REG_PTU_USB2_DC_WLENGTHH,
    REG_PTU_DC_PTU_UART_DHBR = 0x400/4,
    REG_PTU_DC_PTU_UART_DLBR,
    REG_PTU_DC_PTU_UART_AB0,
    REG_PTU_DC_PTU_UART_FCR = 0x410/4,
    REG_PTU_DC_PTU_UART_AB1,
    REG_PTU_DC_PTU_UART_LCR = 0x41c/4,
    REG_PTU_DC_PTU_UART_MCR,
    REG_PTU_DC_PTU_UART_LSR,
    REG_PTU_DC_PTU_UART_MSR,
    REG_PTU_DC_PTU_UART_RFL,
    REG_PTU_DC_PTU_UART_TFL,
    REG_PTU_DC_PTU_UART_RFC,
    REG_PTU_DC_PTU_UART_ESC,
    REG_PTU_DC_PTU_UART_ABR,
    REG_PTU_DC_AVAILABLE02,
    REG_PTU_DC_AVAILABLE03,
    REG_PTU_UART_HO_PKLEN,
    REG_PTU_UART_HI_PKLEN,
    REG_PTU_UART_HO_DMA_CTL,
    REG_PTU_UART_HI_DMA_CTL,
    REG_PTU_UART_HO_BSIZE,
    REG_PTU_UART_HI_BSIZE,
    REG_PTU_FPGA_ID,
    REG_PTU_IIC_CADDR_WCTL = 0x500/4,
    REG_PTU_IIC_WDATA0,
    REG_PTU_IIC_WDATA1,
    REG_PTU_IIC_WDATA2,
    REG_PTU_IIC_WDATA3,
    REG_PTU_IIC_WDATA4,
    REG_PTU_IIC_WDATA5,
    REG_PTU_IIC_WDATA6,
    REG_PTU_IIC_WDATA7,
    REG_PTU_IIC_BYTE_CNT,
    REG_PTU_IIC_CTL,
    REG_PTU_IIC_RD_EN_INT,
    REG_PTU_IIC_RDATA0,
    REG_PTU_IIC_RDATA1,
    REG_PTU_IIC_RDATA2,
    REG_PTU_IIC_RDATA3,
    REG_PTU_IIC_RDATA4,
    REG_PTU_IIC_RDATA5,
    REG_PTU_IIC_RDATA6,
    REG_PTU_IIC_RDATA7,
    REG_PTU_ESPI_CTRL = 0x600/4,
    REG_PTU_ESPI_SS,
    REG_PTU_ESPI_MS,
    REG_PTU_ESPI_H_CTRL,
    REG_PTU_ESPI_H_SA,
    REG_PTU_ESPI_H_MS,
    REG_PTU_ESPI_RX_AF_THR,
    REG_PTU_ESPI_TX_AE_THR,
    REG_PTU_ESPI_H_SS,
    REG_PTU_ESPI_PKT_INFO0,
    REG_PTU_ESPI_PKT_INFO1,
    REG_PTU_ESPI_PKT_INFO2,
    REG_PTU_ESPI_PKT_INFO3,
    REG_PTU_ESPI_PKT_INFO4,
    REG_PTU_ESPI_FW_RST_HO_FIFO,
    REG_PTU_ESPI_FW_RST_HI_FIFO,
    REG_PTU_ESPI_FW_RST_HO_DMAC_SM,
    REG_PTU_SR_PTU_STATUS10,
    REG_PTU_SR_PTU_EN10,
    REG_PTU_ESPI_FW_RST_HI_DMAC_SM,
    REG_PTU_AA_CH0_STATUS = 0x700/4,
    REG_PTU_AA_CH0_USER = 0x720/4,
    REG_PTU_AA_CH1_STATUS = 0x740/4,
    REG_PTU_AA_CH1_USER = 0x760/4,
    REG_PTU_AA_LENGTH_A = 0x780/4,
    REG_PTU_AA_LENGTH_B,
    REG_PTU_AA_LENGTH_C,
    REG_PTU_AA_SILENCE,
    REG_PTU_AA_N,
    REG_PTU_AA_X,
    REG_PTU_AA_STATUS,
    REG_PTU_AA_CONFIG,
    REG_PTU_AA_RELAXBUF,
    REG_PTU_AA_DG_CRD_CONFIG,
    REG_PTU_DB_UART_DHBR = 0x800/4,
    REG_PTU_DB_UART_DLBR,
    REG_PTU_DB_UART_FCR,
    REG_PTU_DB_UART_LCR,
    REG_PTU_DB_UART_MCR,
    REG_PTU_DB_UART_LSR,
    REG_PTU_DB_UART_MSR,
    REG_PTU_DB_UART_DATA,
    REG_PTU_DB_UART_SNAP,
    REG_PTU_DB_UART_TXW,
    REG_PTU_DB_UART_RXW,
    REG_PTU_RTC_UCODEM_START = 0x900/4,
    REG_PTU_RTC_UCODEM_STOP = 0xaff/4,
    REG_PTU_RTC_TAPM_START = 0xb00/4,
    REG_PTU_RTC_TAPM_STOP = 0xbff/4,
    REG_PTU_RTC_COEFFM_START = 0xc00/4,
    REG_PTU_RTC_COEFFM_STOP = 0xcff/4,
    REG_PTU_RTC_INTEGERD = 0xd00/4,
    REG_PTU_RTC_FRACTIOND,
    REG_PTU_RTC_INITIAL_PC,
    REG_PTU_RTC_CONFIG,
    REG_PTU_PCM_CONTROL = 0xe00/4,
    REG_PTU_PCM_CH0_DIN,
    REG_PTU_PCM_CH0_DOUT,
    REG_PTU_PCM_CH1_DIN,
    REG_PTU_PCM_CH1_DOUT,
    REG_PTU_PCM_CH2_DIN,
    REG_PTU_PCM_CH2_DOUT,
    REG_PTU_PCM_WATERLINE,
    REG_PTU_PCM_FIFO_RST,
    REG_PTU_PCM_FIFO_STAT,
    REG_PTU_FPGA_I2S_TEST,
    REG_PTU_PCM_CONTROL_2,
    REG_PTU_SDIO_SERVICE_ID = 0xf00/4,
    REG_PTU_SDIO_STATUS,
    REG_PTU_SDIO_CONFIG,
    REG_PTU_SDIO_RLENGTH,
    REG_PTU_SDIO_WLENGTH,
    REG_PTU_SDIO_RBUF,
    REG_PTU_SDIO_WBUF,
    REG_PTU_SDIO_INTR_STATUS,
    REG_PTU_SDIO_INTR_MSK,
    REG_PTU_SDIO_RTDAT,
    REG_PTU_SDIO_INTRD,
    REG_PTU_SDIO_INTR_STATUS2,
    REG_PTU_SDIO_INTR_MSK2,
    REG_PTU_SDIO_INTR_SET,
    REG_PTU_SDIO_INTR_SET2,
    REG_PTU_SDIO_DEBUG,
    REG_PTU_SDIO_DEBUG_A1,
    REG_PTU_SDIO_DEBUG_D1,
    REG_PTU_SDIO_DEBUG_A2,
    REG_PTU_SDIO_DEBUG_D2,
    REG_PTU_HUB_DEV_DESC = 0x2000/4,
    REG_PTU_HUB_STRINGBUF2 = 0x2808/4,
    REG_PTU_HUB_CFGRAM_TEST,
    REG_PTU_COUNT
};

struct Ptu : Peripheral<REG_PTU_COUNT>
{
    /// Constructor
    Ptu(sc_core::sc_module_name name)
    : Peripheral<REG_PTU_COUNT>(name)
    {
        // initialize the registers content
    }

protected:
    /// Specific blocking transport method
    virtual void
    slave_b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        uint32_t length = trans.get_data_length();
        sc_dt::uint64 addr = trans.get_address();
        sc_dt::uint64 index = addr/4;

        // sanity check
        TLM_TRANS_SANITY(trans);

        // some specific registers can be accessed in byte mode
        if ((length != 4) &&
            ((index == REG_PTU_DC_PTU_HC_SEL) ||
             (index == REG_PTU_DC_PTU_SEL_CTL)))
        {
            uint32_t* ptr = reinterpret_cast<uint32_t*>(trans.get_data_ptr());

            // only aligned access in the 8 lower bytes
            assert((addr & 3) == 0);
            assert(length == 1);
            assert((*ptr & 0xFFFFFF00) == 0);

            this->m_reg[index] &= ~0xFF;
            this->m_reg[index] |= *ptr;

            // there was no error in the processing
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
        }
        else
        {
            // regular access
            Peripheral<REG_PTU_COUNT>::slave_b_transport(trans, delay);
        }
        return;
    }

private:
    /** Register read function
     * @param[in] offset Offset of the register to read
     * @return The value read
     */
    uint32_t
    reg_rd(uint32_t offset);

    /** Register write function
     * @param[in] offset Offset of the register to read
     * @param[in] offset Value to write in the register
     */
    void
    reg_wr(uint32_t offset, uint32_t value);

};

#endif /*PTU_H_*/
