#include "mouse.h"
#include "i8042.h"
#include <lcom/lcf.h>
static int mouse_hook_id;

uint8_t dataPackets[3];
struct packet dataPacket;
int current_data_packet = 0;

void mouse_delay(){
  tickdelay(micros_to_tick(20000));
}

void enableMouseStreaming(){

}

void disableMouseStreaming(){

}

int mouse_subscribe_int(int *hook_id){
  //IRQ_EXCLUSIVE
  mouse_hook_id = *hook_id;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_EXCLUSIVE, &mouse_hook_id);
}

int mouse_unsubscribe_int(){
  return sys_irqrmpolicy(&mouse_hook_id);
}

int check_input_buffer(){
  uint32_t status;
  if(sys_inb(STATUS_REG, &status) == OK){
    if(status & BIT(1)){
      //Input buffer is full, can't write commands or arguments
      return 1;
    } else {
      return 0;
    }

  } else {
    printf("Error reading from status register");
    return 1;
  }
}

uint32_t mouse_read(){
  int timeout = 3;
  uint32_t readData;
  while(timeout){
    if(check_input_buffer() == OK){
      if(sys_inb(KBD_OUT_BUF, &readData)){
        if(current_data_packet != 0 || (current_data_packet == 0 && (readData & BIT(3)) == 1)){
          // BIT 3 of first packet is always 1
          //read of a packet success
          dataPackets[current_data_packet++] = readData;
          if(current_data_packet == 3) {
            dataPacket = readDataPacket();
            current_data_packet = 0;
          }
        }
        break;
      }
    }
    timeout--;
    mouse_delay();
  }
}

struct packet readDataPacket(){
  struct packet p;
  for(unsigned i=0; i < 3; i++){
    p.bytes[i] = dataPackets[i];
  }
  
  p.lb = p.bytes[0] & BIT(0);
  p.rb = p.bytes[0] & BIT(1);
  p.mb = p.bytes[0] & BIT(2);
  p.x_ov = p.bytes[0] & BIT(6);
  p.y_ov = p.bytes[0] & BIT(7);
  p.delta_x = p.bytes[1];
  p.delta_y = p.bytes[2];
  return p;
}

struct packet mouse_int_handler(){
  mouse_read();
  if(current_data_packet == 3){
    current_data_packet = 0;
    return dataPacket;
  }
  struct packet empty;
  empty.bytes[0] = 0;
  return empty;
}
