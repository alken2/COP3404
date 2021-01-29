#include <stdio.h>

int main(void) {
  int timer = 0x0000E10; //this will be received from TIMER
  int hour = 0;
  int minute = 0;
  char mode = 0;
  int bitmsk = 0b0111111111111111111111111 & timer;
  //printf("%06X\n",timer);
  //printf("%06X\n",bitmsk);
  if (timer != bitmsk) {
    //24 hour clock
    timer = bitmsk;
    while (timer > 3600) {
      hour = hour + 1;
      if (hour == 24) {
        hour = 0;
      }
      timer = timer - 3600;
    }
  }
  else {
    //12 hour clock
    mode = 'A';
    while (timer > 3600) {
      hour = hour + 1;
      if (hour == 12) {
        hour = 0;
        if (mode == 'A') {
          mode = 'P';
        }
        else {
          mode = 'A';
        }
      }
      timer = timer - 3600;
    }
    if (hour == 0) {
      hour = 12;
    }
  }
  while (timer > 60) {
    minute = minute + 1;
    timer = timer - 60;
  }
  printf("%02d%02d%c\n",hour,minute,mode);
  return 0;
}

