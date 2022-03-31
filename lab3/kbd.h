#ifndef KBD_H
#define KBD_H
#define DELAY_US 20000
#include <stdint.h>

static int kbd_hook_id;

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

uint32_t kbd_read();

uint32_t kbd_int_handler();


int read_cmd_byte(uint32_t *cmd);

int write_cmd_byte(uint32_t *cmd);
#endif




