#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <linux/input.h>
#include <stdlib.h>
#include "touch.h"

int judgeFunction(struct point p) 
{
    if (p.x >= 857 && p.x <= 985 && p.y >= 37 && p.y <= 163) {
        return 1;
    } else if (p.x >= 857 && p.x <= 985 && p.y >= 237 && p.y <= 363) {
        return 2;
    } else if (p.x >= 857 && p.x <= 985 && p.y >= 437 && p.y <= 563) {
        return 3;
    } else {
        return 0;
    }
}

struct point get_point()
{   
    struct point p;
    int fd = open("/dev/input/event0",O_RDONLY);
    if(-1 == fd)
    {
        perror("打开触摸屏失败");
        p.x = p.y = -1;
        return p;
    }

    struct input_event ev;
    
    while(1)//点击/滑动屏幕时，并不是只有一个输入事件发生，更加不是只有一个 结构体数据，所以我们需要循环读取
    {
        read(fd,&ev,sizeof(ev));
        //printf("type:%d,code:%d,value:%d\n",ev.type,ev.code,ev.value);
        if(ev.type==EV_ABS&&ev.code==REL_X)
            p.x = ev.value;
        if(ev.type==EV_ABS&&ev.code==REL_Y)
            p.y = ev.value;
        if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0)//手指松开，跳出循环
            break;
    }

    close(fd);
    return p;
}

enum DIREC get_direction()
{
    int fd = open("/dev/input/event0",O_RDONLY);
    if(-1 == fd)
    {
        perror("");
        return ERROR;
    }

    struct input_event ev;
    struct point start,end;
    start.x = -1;
    start.y = -1;
    //获取起点和终端坐标
    while(1)
    {
        read(fd,&ev,sizeof(ev));
        printf("type:%d,code:%d,value:%d\n",ev.type,ev.code,ev.value);
        if(ev.type==EV_ABS&&ev.code==REL_X)
        {
            end.x = ev.value;//一直覆盖，最后一个坐标就是终点坐标
            if(start.x == -1)//只把第一个点的坐标赋值给 start.x，赋值之后就不会等于-1，后面就不会赋值了
                start.x = ev.value;
        }
        if(ev.type==EV_ABS&&ev.code==REL_Y)
        {
            end.y = ev.value;
            if(start.y == -1)
                start.y = ev.value;
        }
        if(ev.type==EV_KEY&&ev.code==BTN_TOUCH&&ev.value==0)//手指松开，跳出循环
            break;
    }

    //根据起点和终点坐标计算滑动方向
    int _x = end.x-start.x;
    int _y = end.y-start.y;
    if(abs(_x) > abs(_y))
    {
        //水平方向大于垂直方向: 左或者右
        if(_x > 10)//10个单位是为了防误触
        {
            //右
            return RIGHT;
        }
        else if(_x < -10)
        {
            //左
            return LEFT;
        }
    }
    else
    {
        //水平方向小于垂直方向: 上或者下
        if(_y > 10)
        {
            //下
            return DOWN;
        }
        else if(_y < -10)
        {
            //上
            return UP;
        }
    }

    close(fd);
    return OTHER;
}