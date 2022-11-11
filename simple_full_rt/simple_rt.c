#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>
#include <alchemy/timer.h>
#include <math.h>

#include "Output_data.h"

#define CLOCK_RES 1e-9
#define LOOP_PERIOD 1e6

RT_TASK loop_task;
struct Output_data;

//输出的方式可以时间戳写在文件当中，也可以通过usb连GPIO的方式进行输出
int output_csv_single_pulse(FILE *file, Output_data *output)
{
  fprintf(file, "%d,", output->ctr);
  //fprintf(file, "%.5f,", output->timestamp);
  fprintf(file, "%f,", output->pulse_increment);
  fprintf(file, "%d,", output->x_position);
  fprintf(file, "%d,", output->y_position);
  fprintf(file, "%d,", output->x_step);
  fprintf(file, "%d,", output->y_step);
  fprintf(file, "%.5f,", output->real_cycle_time);
  fprintf(file, "%.5f,", output->real_intepolation_time);
  fprintf(file, "%f", output->intrp_prop_on_cycle);
}
// Use DDA to intepolate the path
void dda_line_intepolation(void *arg)
{
  RT_TASK *curtask;

  RT_TASK_INFO curtaskinfo;
  int iret = 0;

  RTIME tstart, now, tcycle,tintepolation, tlast, tcurrent;

  float tproportion;

  //先使用硬编码的方式直接输入路径点
  int start_x = 0;
  int start_y = 0;
  int end_x = 154;
  int end_y = 123;

  //累加m次以后到达终点
  int m = 256;
  int n = 8;

  //设定步长距离与初始值
  int j_vx = end_x - start_x;
  int j_vy = end_y - start_y;
  int j_RX = 0;
  int j_RY = 0;

  //设定初始位置
  int current_x = start_x;
  int current_y = start_y;

  int step_x = 0;
  int step_y = 0;

  Output_data * output;
  output = (Output_data*)malloc(sizeof(Output_data));
  

  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  tstart = rt_timer_read();

  //创建文件指针，准备将带有时间戳的文件写入CSV文件当中去
  FILE *file = fopen("output.csv", "w");
  //写csv文件的标题
  fprintf(file, "ID,PulseSize,X,Y,x_step,y_step,Cycle Time,Intepolation Time,Intepolation Porprotion");

  curtask = rt_task_self();
  //获取此时时钟的指针
  rt_task_inquire(curtask, &curtaskinfo);
  //设置计数器
  int ctr = 1;

  while (1)
  {
    tcurrent = rt_timer_read() - tstart;
    tcycle = tcurrent - tlast;
    printf("Loop count: %d, Loop time: %.5f ms\n", ctr,  tcycle/ 1000000.0);
    
    ctr++;

    step_x = 0;
    step_y = 0;

    j_RX += j_vx;
    j_RY += j_vy;
    // printf("ctr: %d, m: %d\n", ctr, m);
    if (ctr <= m)
    {
      if (j_RX >= m)
      {
        current_x += 1;
        j_RX -= m;
        step_x = 1;
      }
      if (j_RY >= m)
      {
        current_y += 1;
        j_RY -= m;
        step_y = 1;
      }
      printf("current_x: %d, current_y: %d, (%d,%d)\n", current_x, current_y, step_x, step_y);
    }
    else
    {
      printf("Reach the destination. \n");
      fclose(file);
      free(output);
      break;
    }

    tlast = rt_timer_read() - tstart;
    tintepolation = tlast-tcurrent;
    tproportion = (tintepolation/1000000.0)/(tcycle/1000000.0)*100;
    printf("Intepolation_Time: %.5f ms \n",tintepolation/1000000.0);
    printf("Intepolation_Proportion: %.5f %% \n", (tintepolation/1000000.0)/(tcycle/1000000.0)*100);

    output->ctr = ctr;
    output->pulse_increment = 1;
    output->x_position = current_x;
    output->y_position = current_y;
    output->x_step = step_x;
    output->y_step = step_y;
    output->real_cycle_time = tcycle;
    output->real_intepolation_time = tintepolation;
    output->intrp_prop_on_cycle = tproportion;

    output_csv_single_pulse(file,output);
    rt_task_wait_period(NULL);
    
    //输出当前处于第几个周期，并且输出周期的时间
    
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

  // Print the info
  printf("Starting task %s with period of 10 ms ....\n", curtaskinfo.name);

  // Make the task periodic with a specified loop period
  rt_task_set_periodic(NULL, TM_NOW, LOOP_PERIOD);

  tstart = rt_timer_read();

  // Start the task loop
  while (1)
  {
    printf("Loop count: %d, Loop time: %.5f ms\n", ctr, (rt_timer_read() - tstart) / 1000000.0);
    ctr++;
    rt_task_wait_period(NULL);
  }
}

int main(int argc, char **argv)
{
  char str[20];

  //锁定内存相关内容，以防止Page交换引起大量延时
  mlockall(MCL_CURRENT | MCL_FUTURE);

  printf("Starting cyclic task...\n");

  

  sprintf(str, "cyclic_task");
  //创建实时任务
  rt_task_create(&loop_task, str, 0, 50, 0);

  //将需要使用的代码进行注释，尽量保证内外不影响
  rt_task_start(&loop_task, &dda_line_intepolation, 0);

  // rt_task_start(&loop_task, &loop_task_proc, 0);

  getchar();

  rt_task_delete(&loop_task);

  
  return 0;
}
