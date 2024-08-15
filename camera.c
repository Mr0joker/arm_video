#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "touch.h"
#include "video_display.h"

//线程锁
pthread_mutex_t video_exit_mutex=PTHREAD_MUTEX_INITIALIZER;
//退出信号
int video_exit = 0;
int video_play = 0;

int jpg_photo_id = 1;

//拍照
int cap_photo(void)
{
    printf("拍照\n");

    // 获取当前时间
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    // 生成文件名，格式为：%d.jpg
    char filename[64];
    sprintf(filename, "./group_jpg/%d.jpg", jpg_photo_id);
    jpg_photo_id++;        


    FILE *file = fopen(filename, "w+");
    if (file == NULL) {
        perror("文件打开失败");
        return -1;
    }

    fwrite(mptr[readbuffer.index], readbuffer.length, 1, file);//采集一帧数据写入my.jpg中
    fclose(file);
    return 1;
}

//录像
int cap_camera(void)
{
    //添加录像代码
    if(video_play == 0)
    {
        video_play = 1;
        printf("开始录像\n");
    }else
    {
        video_play = 0;
        printf("结束录像\n");
    }
    return 1;
}

//退出
int quit(void)
{
    //kill -9?
    video_exit = 1;
    printf("退出相机\n");
    return 1;
}


int jug_point(int flag)
{
    switch (flag)
    {
    case 3:
        //退出程序
        quit();
        printf("退出成功\n");
        return 3;
    case 2:
        //录像
        cap_camera();
        printf("录像成功\n");
        return 2;
    case 1:
        //拍照
        cap_photo();
        printf("拍照成功\n");
        return 1;
    default:
        return 0;
    }
}
