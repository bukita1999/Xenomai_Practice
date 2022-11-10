#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>



#define CLOCK_RES 1e-9
#define LOOP_PERIOD 1e7

RT_TASK loop_task;

enum OUTPUT_MODE
{
    GPIO_MODE,FILE_MODE,
};



int output_file_single_pulse(char * file_str) //输出的方式可以时间戳写在文件当中，也可以通过usb连GPIO的方式进行输出
{

}

int dda_line_intepolation(float start_x,float start_y,float end_x, float end_y,float single_step) //dda开环控制
{
  RTIME tstart, now;
  int ctr = 0;

  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  tstart = rt_timer_read();

  while(1){
    printf("Loop count: %d, Loop time: %.5f ms\n", ctr, (rt_timer_read() - tstart)/1000000.0);
    ctr++;
    rt_task_wait_period(NULL);
  }
}

void loop_task_proc(void *arg)
{
  RT_TASK *curtask;
  RT_TASK_INFO curtaskinfo;
  int iret = 0;

  RTIME tstart, now;

  curtask = rt_task_self();
  rt_task_inquire(curtask, &curtaskinfo);
  


  printf("Starting task %s with period of 10 ms ....\n", curtaskinfo.name);


  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  tstart = rt_timer_read();


  
}

int main(int argc, char **argv)
{
  char str[20];


  mlockall(MCL_CURRENT | MCL_FUTURE);

  printf("Starting cyclic task...\n");


  sprintf(str, "cyclic_task");
  rt_task_create(&loop_task, str, 0, 50, 0);


  rt_task_start(&loop_task, &loop_task_proc, 0);


  pause();

  return 0;
}

