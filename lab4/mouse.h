#ifndef MOUSE_H
#define MOUSE_H
#include <lcom/lcf.h>

void mouse_delay();

void enableMouseStreaming();

void disableMouseStreaming();

int mouse_subscribe_int(int *hook_id);

int mouse_unsubscribe_int();

int check_input_buffer();

uint32_t mouse_read();

struct packet mouse_int_handler();

struct packet readDataPacket();

#endif
