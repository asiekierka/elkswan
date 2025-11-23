#ifndef __ARCH_8086_SEGMENT_H
#define __ARCH_8086_SEGMENT_H

#include <linuxmt/types.h>
#include <linuxmt/config.h>

extern seg_t kernel_cs, kernel_ds;
extern short *_endtext, *_endftext, *_enddata, *_endbss;
extern short endistack[];
extern short endtstack[];
extern unsigned int heapsize;

#ifdef SETUP_MEM_BANKS
/* bank.c */
unsigned char bank_get_maximum(void);
unsigned char bank_get_current(void);
void bank_set_current(unsigned char);
#endif

#endif
