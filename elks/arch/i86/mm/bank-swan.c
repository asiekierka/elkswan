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
    return SETUP_MEM_BANKS - 1;
}

bank_t bank_get_current(void)
{
    return inb(BANK_RAM_PORT) & bank_get_maximum();
}

void bank_set_current(bank_t bank)
{
    outb(bank & bank_get_maximum(), BANK_RAM_PORT);
}

void bank_seg_copy(seg_t dstseg, bank_t dstb, seg_t srcseg, bank_t srcb, size_t paras)
{
    bank_t cur_bank = bank_get_current();

    const unsigned int __far *srcp = _MK_FP(srcseg, 0);
    unsigned int __far *destp = _MK_FP(dstseg, 0);

    srcb &= bank_get_maximum();
    dstb &= bank_get_maximum();

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

    bank_set_current(cur_bank);
}

void bank_memcpyb(byte_t *dstoff, seg_t dstseg, bank_t dstb, byte_t *srcoff, seg_t srcseg, bank_t srcb, size_t count) {
    bank_t cur_bank = bank_get_current();

    const unsigned char __far *srcp = _MK_FP(srcseg, (word_t) srcoff);
    unsigned char __far *destp = _MK_FP(dstseg, (word_t) dstoff);

    srcb &= bank_get_maximum();
    dstb &= bank_get_maximum();

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
    BANK_WRAP_START;
    pokeb(off, seg, val);
    BANK_WRAP_END;
}

void bank_pokew (bank_t bank, word_t off, seg_t seg, word_t val) {
    BANK_WRAP_START;
    pokew(off, seg, val);
    BANK_WRAP_END;
}

void bank_pokel (bank_t bank, word_t off, seg_t seg, long_t val) {
    BANK_WRAP_START;
    pokel(off, seg, val);
    BANK_WRAP_END;
}
