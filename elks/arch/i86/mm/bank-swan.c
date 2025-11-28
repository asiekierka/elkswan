/*
 * WonderSwan bank switching
 */
#include <linuxmt/types.h>
#include <linuxmt/config.h>
#include <linuxmt/memory.h>
#include <arch/io.h>
#include <arch/ports.h>
#include <arch/segment.h>
#include <arch/swan.h>

bank_t bank_get_maximum(void)
{
    int count = SETUP_MEM_BANKS;
    if (count > MAX_BANKS)
        return MAX_BANKS - 1;
    else
        return count - 1;
}

bank_t bank_get_current(void)
{
    return inb(BANK_RAM_PORT);
}

void bank_set_current(bank_t bank)
{
    outb(bank, BANK_RAM_PORT);
}

int bank_seg_is_rom(seg_t seg)
{
    return seg >= 0x2000;
}

static void __bank_seg_copy(seg_t dstseg, bank_t dstb, seg_t srcseg, bank_t srcb, size_t paras)
{
    if (srcseg >= 0x2000 || srcseg < 0x1000) {
        outb(dstb, BANK_RAM_PORT);
        fmemcpyw(0, dstseg, 0, srcseg, paras << 3);
        return;
    } else if (SETUP_ARCH_TYPE == ARCH_TYPE_SWAN_NILE) {
        srcseg += 0x2000;
        outb(dstb, BANK_RAM_PORT);
        outw(srcb, 0xD4);
        fmemcpyw(0, dstseg, 0, srcseg, paras << 3);
        return;
    }

    const unsigned int __far *srcp = _MK_FP(srcseg, 0);
    unsigned int __far *destp = _MK_FP(dstseg, 0);

    // TODO: Optimize.
    for (size_t i = 0; i < (paras << 3); i++) {
        asm volatile ("" ::: "memory");
        outb(srcb, BANK_RAM_PORT);
        asm volatile ("" ::: "memory");
        unsigned int c = srcp[i];
        asm volatile ("" ::: "memory");
        outb(dstb, BANK_RAM_PORT);
        asm volatile ("" ::: "memory");
        destp[i] = c;
    }
}

void bank_seg_copy(seg_t dstseg, bank_t dstb, seg_t srcseg, bank_t srcb, size_t paras)
{
    // printk("bank_seg_copy %04x %02x %04x %02x %d\n", dstseg, dstb, srcseg, srcb, paras);
    
    bank_t cur_bank = bank_get_current();

    if (SETUP_ARCH_TYPE == ARCH_TYPE_SWAN_NILE) {
        if (dstseg >= 0x2000) {
            dstb = NILE_BANK_PSRAM_LINEAR_OFFSET + (dstseg >> 12);
            dstseg = (dstseg & 0xFFF) | 0x1000;

            // Handle cross-RAM-bank copies
            while (dstseg + paras > 0x2000) {
                size_t paras_part = 0x2000 - dstseg;
                __bank_seg_copy(dstseg, dstb, srcseg, srcb, paras_part);
                paras -= paras_part;
                srcseg += paras_part;
                dstseg = 0x1000;
                dstb++;
            }
        }
    }

    __bank_seg_copy(dstseg, dstb, srcseg, srcb, paras);

    bank_set_current(cur_bank);
}

void bank_memcpyb(byte_t *dstoff, seg_t dstseg, bank_t dstb, byte_t *srcoff, seg_t srcseg, bank_t srcb, size_t count) {
    bank_t cur_bank = bank_get_current();

    const unsigned char __far *srcp = _MK_FP(srcseg, (word_t) srcoff);
    unsigned char __far *destp = _MK_FP(dstseg, (word_t) dstoff);

    // TODO: Optimize.
    for (size_t i = 0; i < count; i++) {
        asm volatile ("" ::: "memory");
        outb(srcb, BANK_RAM_PORT);
        asm volatile ("" ::: "memory");
        unsigned char c = srcp[i];
        asm volatile ("" ::: "memory");
        outb(dstb, BANK_RAM_PORT);
        asm volatile ("" ::: "memory");
        destp[i] = c;
    }

    bank_set_current(cur_bank);
}

#define BANK_WRAP_START \
    bank_t old_bank = inb(BANK_RAM_PORT); \
    outb(bank, BANK_RAM_PORT); \
    asm volatile("" ::: "memory")
#define BANK_WRAP_END \
    asm volatile("" ::: "memory"); \
    outb(old_bank, BANK_RAM_PORT)

byte_t bank_peekb (bank_t bank, word_t off, seg_t seg) {
    BANK_WRAP_START;
    byte_t result = peekb(off, seg);
    BANK_WRAP_END;
    return result;
}

word_t bank_peekw (bank_t bank, word_t off, seg_t seg) {
    BANK_WRAP_START;
    word_t result = peekw(off, seg);
    BANK_WRAP_END;
    return result;
}

long_t bank_peekl (bank_t bank, word_t off, seg_t seg) {
    BANK_WRAP_START;
    long_t result = peekw(off, seg);
    BANK_WRAP_END;
    return result;
}

void bank_pokeb (bank_t bank, word_t off, seg_t seg, byte_t val) {
    if (seg >= 0x2000) {
        seg = seg + (off >> 4);
        off &= 15;

        bank = NILE_BANK_PSRAM_LINEAR_OFFSET + (seg >> 12);
        seg = (seg & 0xFFF) | 0x1000;
    }
    BANK_WRAP_START;
    pokeb(off, seg, val);
    BANK_WRAP_END;
}

void bank_pokew (bank_t bank, word_t off, seg_t seg, word_t val) {
    if (seg >= 0x2000) {
        bank_pokeb(bank, off, seg, val);
        bank_pokeb(bank, off + 1, seg, val >> 8);
        return;
    }
    BANK_WRAP_START;
    pokew(off, seg, val);
    BANK_WRAP_END;
}

void bank_pokel (bank_t bank, word_t off, seg_t seg, long_t val) {
    if (seg >= 0x2000) {
        bank_pokew(bank, off, seg, val);
        bank_pokew(bank, off + 2, seg, val >> 16);
        return;
    }
    BANK_WRAP_START;
    pokel(off, seg, val);
    BANK_WRAP_END;
}
