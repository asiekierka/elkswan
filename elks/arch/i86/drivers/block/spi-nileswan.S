	.arch i186
	.code16

/**
 * nileswan SPI driver
 *
 * April 2025 Adrian Siekierka.
 */

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
#define IO_NILE_SPI_CNT    0xE0

#define NILE_POW_UNLOCK    0xDD
#define IO_NILE_POW_CNT    0xE2

#define IO_NILE_SEG_MASK   0xE4

#define NILE_SEG_RAM_SPI_TX 15
#define NILE_SEG_ROM_SPI_RX 510

	.text

.global spi_init_ll
//void spi_init_ll(void)
spi_init_ll:
	// Unlock cartridge interface
	mov	$NILE_POW_UNLOCK, %al
	out	%al, $IO_NILE_POW_CNT

	in	$IO_NILE_SEG_MASK, %ax
	and	$0xF1FF, %ax
	out	%ax, $IO_NILE_SEG_MASK

	// Pass through into spi_cs_1

.global spi_cs_1
//void spi_cs_1(void)
spi_cs_1:
	in	$IO_NILE_SPI_CNT, %ax
	and	$(~(NILE_SPI_DEV_MASK | NILE_SPI_CLOCK_MASK | NILE_SPI_BUSY)), %ax
	or	$(NILE_SPI_DEV_NONE | NILE_SPI_CLOCK_CART), %ax
	out	%ax, $IO_NILE_SPI_CNT
	ret

.global spi_cs_0
//void spi_cs_0(void)
spi_cs_0:
	in	$IO_NILE_SPI_CNT, %ax
	and	$(~(NILE_SPI_DEV_MASK | NILE_SPI_CLOCK_MASK | NILE_SPI_BUSY)), %ax
	or	$(NILE_SPI_DEV_TF | NILE_SPI_CLOCK_CART), %ax
	out	%ax, $IO_NILE_SPI_CNT
	ret

.global spi_receive
//uint8_t spi_receive(void)
spi_receive:
	// Send read command
	in	$IO_NILE_SPI_CNT, %ax
	and	$NILE_SPI_CFG_MASK, %ax
	xor	$(NILE_SPI_MODE_READ | NILE_SPI_START), %ax
	out	%ax, $IO_NILE_SPI_CNT

	call	spi_wait
	xor	$NILE_SPI_BUFFER_IDX, %ax
	out	%ax, $IO_NILE_SPI_CNT

	// Read byte
	in	$0xD2, %ax
	push	%ax
	cli
	mov	$NILE_SEG_ROM_SPI_RX, %ax
	out	%ax, $0xD2
	
	push	%ds
	push	$0x2000
	pop	%ds
	mov	0, %cl
	pop	%ds

	pop	%ax
	out	%ax, $0xD2
	sti

	mov	%cl, %al
	ret

.global spi_send_ffs
//void spi_send_ffs(uint16_t bytes)
spi_send_ffs:
	// SPI read sends FFs in return
	// TODO: Support values outside of the 1-512 range

	in	$IO_NILE_SPI_CNT, %ax
	and	$NILE_SPI_CFG_MASK, %ax
 	mov	%sp, %bx
	or	2(%bx), %ax
	dec	%ax
	or	$(NILE_SPI_MODE_READ | NILE_SPI_START), %ax
	out	%ax, $IO_NILE_SPI_CNT

	jmp	spi_wait

.global spi_transmit
//void spi_transmit(uint8_t data)
spi_transmit:
	// Write byte to transmit
	mov     %sp, %bx
	mov     2(%bx), %cx

	in	$0xC1, %al
	push	%ax
	cli
	mov	$NILE_SEG_RAM_SPI_TX, %al
	out	%al, $0xC1
	
	push	%ds
	push	$0x1000
	pop	%ds
	mov	%cl, 0
	pop	%ds

	pop	%ax
	out	%al, $0xC1
	sti

	// Send write command
	in	$IO_NILE_SPI_CNT, %ax
	and	$NILE_SPI_CFG_MASK, %ax
	xor	$(NILE_SPI_BUFFER_IDX | NILE_SPI_MODE_WRITE | NILE_SPI_START), %ax
	out	%ax, $IO_NILE_SPI_CNT

	// Fall through to spi_wait

spi_wait:
	in	$IO_NILE_SPI_CNT, %ax
	test	$NILE_SPI_BUSY, %ax
	jnz	spi_wait

	ret
