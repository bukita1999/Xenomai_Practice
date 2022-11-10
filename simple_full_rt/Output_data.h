#include <time.h>


typedef struct Output_data{
    int ctr;
    char timestamp[40]; //时间戳字符化后的效果
    float pulse_increment; //脉冲增量实际对比，此处是真实一个脉冲增量对应的距离
    int x_position; //目前所走到的位置x坐标
    int y_position;  //目前所走到的位置y坐标
    int x_step; //x轴是否前进，前进为1，未前进则为0
    int y_step; //y轴是否前进，前进为1，未前进则为0
    float real_cycle_time; //实际周期时间是运行平台可能实时性并不足够好，可能会有相应的波动
    float real_intepolation_time; //由于实施周期平台不一定足够好
    float intrp_prop_on_cycle; //差补时间占周期时间的比例
}Output_data;

extern struct Output_data;