#include "RTMK.H"

PROCESS p1,p2;

int i,j;

void far process1()
{
  while(1) {
    send(p2,1);
    wait(ANY_SIGNAL);
    reset(ALL_SIGNALS);
    i+=1;
  }
}
void far process2()
{static long n;
  while(1) {
    wait(1);
    reset(1);
    for(n=0;n<10000;n+=1);
    send(p1,1);
    j+=1;
  }
}

void main() {

  create_process(&p1,process1);
  create_process(&p2,process2);
  
  run_kernel();
}
