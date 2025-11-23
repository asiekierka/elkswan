/*
 * WonderSwan bank switching
 */
#include <linuxmt/config.h>
#include <linuxmt/types.h>
#include <arch/io.h>
#include <arch/ports.h>
#include <arch/segment.h>
#include <arch/swan.h>

#ifdef SETUP_MEM_BANKS
unsigned char bank_get_maximum(void)
{
    return SETUP_MEM_BANKS - 1;
}

unsigned char bank_get_current(void)
{
    return inb(BANK_RAM_PORT) & bank_get_maximum();
}

void bank_set_current(unsigned char bank)
{
    outb(bank & bank_get_maximum(), BANK_RAM_PORT);
}
#endif