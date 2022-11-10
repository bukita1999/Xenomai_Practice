#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>

#include "Output_data.h"

#define CLOCK_RES 1e-9
#define LOOP_PERIOD 1e7

RT_TASK loop_task;

//输出的方式可以时间戳写在文件当中，也可以通过usb连GPIO的方式进行输出
int output_csv_single_pulse(FILE *file) 
{

}

int dda_line_intepolation(float start_x, float start_y, float end_x, float end_y, float single_step) // dda开环控制
{
  RT_TASK *curtask;
  RTIME tstart, now;
  RT_TASK_INFO curtaskinfo;
  int iret = 0;

  RTIME tstart, now;

  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  curtask = rt_task_self();
  //获取此时时钟的指针
  rt_task_inquire(curtask, &curtaskinfo);
  int ctr = 0;

  while (1)
  {
    printf("Loop count: %d, Loop time: %.5f ms\n", ctr, (rt_timer_read() - tstart) / 1000000.0);
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
  int ctr = 0;

  //Print the info
  printf("Starting task %s with period of 10 ms ....\n", curtaskinfo.name);

  //Make the task periodic with a specified loop period
  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  tstart = rt_timer_read();

  //Start the task loop
  while(1){
    printf("Loop count: %d, Loop time: %.5f ms\n", ctr, (rt_timer_read() - tstart)/1000000.0);
    ctr++;
    rt_task_wait_period(NULL);
  }
}

int main(int argc, char **argv)
{
  char str[20];

  mlockall(MCL_CURRENT | MCL_FUTURE); //锁定内存相关内容，以防止Page交换引起大量延时

  printf("Starting cyclic task...\n");

  sprintf(str, "cyclic_task");
  rt_task_create(&loop_task, str, 0, 50, 0);

  rt_task_start(&loop_task, &dda_line_intepolation, 0);

  pause();

  return 0;
}
