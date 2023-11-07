#ifndef __CH55X_SOFTI2C_H__
#define __CH55X_SOFTI2C_H__

#include "../Lib/CH552.H"

void swi2c_init(void);
void swi2c_start(void);
void swi2c_stop(void);
void swi2c_bus_reset(void);
void swi2c_send_byte(UINT8 txd);
UINT8 swi2c_read_byte(unsigned char ack);
UINT8 swi2c_wait_ack(void);
void swi2c_ack(void);
void swi2c_nack(void);
void swi2c_single_write(UINT8 daddr, UINT8 addr, UINT8 buf);
UINT8 swi2c_single_read(UINT8 daddr, UINT8 addr);

#endif
