#include <linuxmt/types.h>
#include <linuxmt/config.h>
#include <linuxmt/memory.h>
#include "arch/io.h"
#include "spi.h"

#define NILE_SPI_CNT_PORT 0xE0
#define NILE_SPI_MODE_WRITE         0x0000
#define NILE_SPI_MODE_READ          0x0200
#define NILE_SPI_MODE_EXCH          0x0400
#define NILE_SPI_MODE_WAIT_READ     0x0600
#define NILE_SPI_MODE_MASK          0x0600
#define NILE_SPI_CLOCK_CART         0x0800
#define NILE_SPI_CLOCK_FAST         0x0000
#define NILE_SPI_CLOCK_MASK         0x0800
#define NILE_SPI_DEV_NONE           0x0000
#define NILE_SPI_DEV_TF             0x1000
#define NILE_SPI_DEV_FLASH          0x2000
#define NILE_SPI_DEV_MCU            0x3000
#define NILE_SPI_DEV_MASK           0x3000
#define NILE_SPI_BUFFER_IDX         0x4000
#define NILE_SPI_START              0x8000
#define NILE_SPI_BUSY               0x8000
#define NILE_SPI_CFG_MASK           (NILE_SPI_BUFFER_IDX | NILE_SPI_DEV_MASK | NILE_SPI_CLOCK_MASK)

#define NILE_BANK_RAM_SPI_TX 15
#define NILE_BANK_ROM_SPI_RX 510

#define NILE_BANK_RAM_PORT  0xD0
#define NILE_BANK_ROM1_PORT 0xD4
#define RAM_SEG  0x1000
#define ROM1_SEG 0x3000

static void spi_wait_busy(void) {
    // TODO: Implement timeouts
    while (inb(NILE_SPI_CNT_PORT + 1) & (NILE_SPI_BUSY >> 8));
}

void spi_cs_0(void) {
    outw(NILE_SPI_CLOCK_FAST | NILE_SPI_DEV_TF, NILE_SPI_CNT_PORT);
}

void spi_cs_1(void) {
    outw(NILE_SPI_CLOCK_CART | NILE_SPI_DEV_NONE, NILE_SPI_CNT_PORT);
}

void spi_init_ll(void) {
    spi_cs_1();
}

void spi_transmit(uint8_t data) {
    uint16_t ram_bank = inw(NILE_BANK_RAM_PORT);
    outw(NILE_BANK_RAM_SPI_TX, NILE_BANK_RAM_PORT);
    pokeb(0, RAM_SEG, data);
    outw(ram_bank, NILE_BANK_RAM_PORT);

    spi_wait_busy();
    outw((inw(NILE_SPI_CNT_PORT) & NILE_SPI_CFG_MASK) ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_WRITE), NILE_SPI_CNT_PORT);
}

uint8_t spi_receive(void) {
    spi_wait_busy();
    outw((inw(NILE_SPI_CNT_PORT) & NILE_SPI_CFG_MASK) ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ), NILE_SPI_CNT_PORT);

    uint16_t rom_bank = inw(NILE_BANK_ROM1_PORT);
    outw(NILE_BANK_ROM_SPI_RX, NILE_BANK_ROM1_PORT);
    spi_wait_busy();
    outw(inw(NILE_SPI_CNT_PORT) ^ NILE_SPI_BUFFER_IDX, NILE_SPI_CNT_PORT);
    uint8_t data = peekb(0, ROM1_SEG);
    outw(rom_bank, NILE_BANK_ROM1_PORT);
    return data;
}

void spi_send_ffs(uint16_t bytes) {
    spi_wait_busy();
    outw((inw(NILE_SPI_CNT_PORT) & NILE_SPI_CFG_MASK) ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ | (bytes - 1)), NILE_SPI_CNT_PORT);
}

#ifdef CONFIG_HW_SPI
void spi_read_block(char *buf, ramdesc_t seg, uint16_t count) {
    spi_wait_busy();
    outw((inw(NILE_SPI_CNT_PORT) & NILE_SPI_CFG_MASK) ^ (NILE_SPI_BUFFER_IDX | NILE_SPI_START | NILE_SPI_MODE_READ | (count - 1)), NILE_SPI_CNT_PORT);

    uint16_t rom_bank = inw(NILE_BANK_ROM1_PORT);
    outw(NILE_BANK_ROM_SPI_RX, NILE_BANK_ROM1_PORT);
    spi_wait_busy();
    outw(inw(NILE_SPI_CNT_PORT) ^ NILE_SPI_BUFFER_IDX, NILE_SPI_CNT_PORT);
    fmemcpyb(buf, seg, 0, ROM1_SEG, count);
    outw(rom_bank, NILE_BANK_ROM1_PORT);
}

/**
 * Write one block from buffer to SD card via hardware SPI
 * 
 * buf: offset address of write buffer
 * seg: segment adress of write buffer
 * count: number of bytes to write from buffer (has to be 512)
 */
void spi_write_block(char *buf, ramdesc_t seg, uint16_t count) {
    for (uint16_t i = 0; i < count; i++) {
        spi_transmit(peekb((uint16_t) (buf + i), seg));
    }
}
#endif