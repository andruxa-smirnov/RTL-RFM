#ifndef _FSK_H_GUARD
#define _FSK_H_GUARD

void fsk_init();
void fsk_cleanup();
int8_t fsk_decode(int16_t sample, int16_t magnitude);

#endif