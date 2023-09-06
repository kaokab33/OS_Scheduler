#include "headers.h"

int shmid2;
key_t key_sh;
int remainingtime;
bool flag;
int time = -1;
void stop();
//void setVar();
int main(int agrc, char *argv[])
{
  flag = false;
  key_sh = ftok("tempfile", 's');
  shmid2 = shmget(key_sh, 4096, IPC_CREAT | 0644);
  initClk();
  remainingtime = atoi(argv[1]);
  signal(SIGUSR2, stop);
  //signal(SIGUSR1, setVar);
  int *shared = (int *)shmat(shmid2, (void *)0, 0);
  
  while (remainingtime > 0) // remaining time should be shared memory as scheduler updates it with every clk and process should keep track of it :)
  {
    if (time != getClk() && flag == false)
    {
      time = getClk();
      remainingtime--;
      (*shared) = remainingtime;
    }
    else
      flag = false;
  }
  destroyClk(false);
  kill(getppid(), SIGUSR1);
  return 0;
}
void stop()
{
  flag=true;
}
