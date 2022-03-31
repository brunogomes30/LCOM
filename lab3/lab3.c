/*
#include <minix/sysutil.h>

#define DELAY_US    20000

tickdelay(micros_to_ticks(DELAY_US));

//chamar sys_irqrmpolicy()
*/

#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "kbd.h"
#include "i8042.h"
#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  /* To be completed by the students */
  // printf("%s is not yet implemented!\n", __func__);
  //uint8_t kbd_no = KBD_HOOK_BIT;
  int ipc_status, r;
  message msg;
  extern int timer_counter;
  uint8_t kbd_irq = KBD_IRQ;
  if (kbd_subscribe_int(&kbd_irq) != OK) {
    printf("Error in subscribe int.");
    return 0;
  }
  uint32_t kbd_code = 1;
  int first = 1;
  while ( kbd_code != 0x81) { /* You may want to use a different condition */
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(kbd_irq)) { /* subscribed interrupt */
            if(first){
              kbd_code = kbd_int_handler();
            } else {
              first = 0;
            }
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  kbd_unsubscribe_int();
  //before returning, this function should print the number of sys_inb kernel calls by calling the following function that we provide you:
  //int kbd_print_no_sysinb(uint32_t cnt);
  //where: cnt is the total count of sys_inb() kernel calls by your program.
  return 1;
}

int(kbd_test_poll)() {
  uint32_t kbd_code = 0;
  while(kbd_code != 0x81){
    kbd_code = kbd_int_handler();
  }

  uint32_t cmb;
  read_cmd_byte(&cmb);
  cmb = cmb | 0x01;
  write_cmd_byte(&cmb);
  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
