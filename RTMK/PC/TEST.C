#include "RTMK.H"
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <signal.h>

#define IT 0x1C
#define VIDEO_RAM 0xB8000000

PROCESS p1,p2;

int i,j;

char far* p=(char far*)VIDEO_RAM+1;


void interrupt (*old_vector)();

void interrupt clock_it()
{
 outp(0x20,0x20);
  i+=1;
  if(i==200){
    i=0;
    send(p2,1);
  }
  else send(p1,1);
}

far process1()
{
  while(1) {

    p++;
    *p++=0x31;
    if(p>(char far *)VIDEO_RAM+25*80*2) p=(char far* )VIDEO_RAM+1;
    wait(ANY_SIGNAL);
    reset(ALL_SIGNALS);
  }
}
far process2()
{static long n;
  enable();
  while(1) {
    printf("process 2:waiting\t");
    wait(1);
    printf("process 2 :reseting signals\t");
    reset(1);
    printf("process 2:calculating");
    for(j=0;j<60;j+=1){
     for(n=0;n<100000;n+=1);
     printf(".");
    }
    printf("calculation terminated ");
  }
}

jmp_buf sys_context;
void terminate()
{
  longjmp(sys_context,1);
}


void main() {

  clrscr();
  create_process(&p1,process1);
  create_process(&p2,process2);

  old_vector=getvect(IT);
  disable();
  signal(SIGINT,terminate);
  setvect(IT,clock_it);
  if(!setjmp(sys_context)){
  run_kernel();
  }
  setvect(IT,old_vector);
}
