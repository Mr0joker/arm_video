#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "video_display.h"
#include "lcd.h"
#include "touch.h"
#include <pthread.h>
#include "camera.h"
#include "interface.h"
#include "avilib.h"
#include "netsend.h"
#include "database.h"

unsigned int *lcdptr;
int video(void);
void photo(void);
void avi(void);
//退出信号
extern int video_exit;
extern int jpg_photo_id;


//监控线程
void *thread_video(void *arg) {
    printf("创建线程成功！\n");
    
    face_video_init();
    int dev_fd = open_device();
    //视频显示
    video_display(dev_fd,lcdptr);
    close_device(dev_fd);
    face_main_init();
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}


void *thread_avi(void *arg) 
{
    printf("创建线程成功！\n");
    face_avi_init();
    system("mplayer -speed 0.3 ./output.avi");
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}



//功能1摄像头
int video(void)
{
        //创建摄像头功能线程
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, thread_video, NULL)!= 0)
        {
            perror("创建线程失败");
            return -1;
        }

        video_exit = 0;
        struct point p; 
        while(1)
        {
            p = get_point();
            printf("(%0.1f,%0.1f)\n",(double)p.x*0.78125,(double)p.y*0.8);
            //判断坐标值
            int flag = judgeFunction(p);
            int e=jug_point(flag);
            if(e == 3)
            {
                 printf("退出相机功能%d\n",e);
                printf("video触摸屏关闭\n");
                break;
            }
        }
}



//功能2电子相册
void photo(void)
{
    face_photo_init();

    // 相册功能
    int i = jpg_photo_id;
    int j = 1;
    //存储名字
    char filename[64];
    struct point p; 
    while(1)
    {  
        sprintf(filename, "./jpgshow ./group_jpg/%d.jpg", j);
        int ret = system(filename);
        if (ret == -1) 
        {
            printf("执行命令失败: %s\n", filename);
            continue;
        }
        p = get_point();
        printf("(%0.1f,%0.1f)\n",(double)p.x*0.78125,(double)p.y*0.8);
        //判断坐标值
        int flag = judgeFunction(p);
        printf("%d\n",flag);
        if(flag == 1)
        {
            j--;
            continue;
        }else if(flag == 2)
        {
            j++;
            continue;
        }else if(flag == 3)
        {
            //图片发送
            sprintf(filename, "./group_jpg/%d.jpg", j);
            send_photo(filename);
        }else if(j == i)
        {
            break;
        }
    }
    printf("结束放映\n");
}

//功能3视频播放相册
void avi(void)
{
    face_avi_init();
    //bmp_display(lcdptr,"./backdrop.bmp",640,0);
    //system("mplayer -speed 0.3 ./output.avi");
    // char s[64];
    // sprintf(s,"mplayer -speed 0.5 ./%d.avi",abc);
    // system(s);

    // avi_t *AVI = AVI_open_input_file("./output.avi", 1);
    // int width = AVI_video_width(AVI);
    // int height = AVI_video_height(AVI);
    // int frames = AVI_video_frames(AVI);
    // int frame = 0;
    // char *videobuf[width*height*3];
    // for (frame = 0; frame < frames;frame++)
    // {
    //     int iskeyframe;
    //     AVI_set_video_position(AVI, frame);//将avi文件移动到第fream帧
    //     long video_bytes = AVI_frame_size(AVI,frame);//获取视频大小
    //     long actual_video_size = AVI_read_frame(AVI,(char *)videobuf,&iskeyframe);//读取视频帧数据
    // }
    // sleep(1);
    // AVI_close(AVI); 
      //创建视频播放功能线程
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, thread_avi, NULL)!= 0)
    {
        perror("创建线程失败");
        return ;
    }
    struct point p; 
    while(1)
    {
        p = get_point();
        printf("(%0.1f,%0.1f)\n",(double)p.x*0.78125,(double)p.y*0.8);
        //判断坐标值
        int flag = judgeFunction(p);
        printf("%d\n",flag);
        if(flag == 1)
        {
            send_avi();
        }
        if(flag == 3)
        {
            printf("回到主页面\n");
            face_main_init();
            break;
        }
    }

}





//argv[1]  ->  USRNAME
//argv[2]  ->  PASSWORD
int main(int argc,char *argv[])
{
    // if(argc < 3)
    // {
    //     perror("argc failed!")
    //     return -1;
    // }

    //创建一个连接对象指针
    sqlite3 *updb;

    char username[17];
    char password[17];
    printf("欢迎使用GEC6818监控设备\n");
    while(1)
    {
        printf("请输入手机号码:\n");
        scanf("%16s",username);
        int flag = 0;
        for(int i=0;i<strlen(username);i++)
        {
            //如果输入的不是数字
            if(username[i]-'0' < 0 || username[i]-'0' > 9)
            {
                printf("No Usr or Wrong password!\nLogin again\n");
                flag = 1;
                break;
            }
        }
        if(flag)
        {
            continue;9
        }
        char *PASSWORD = get_usr_passwd(updb,username);
        printf("请输入密码:\n");
        scanf("%16s",password);
        if(PASSWORD!= NULL && !strcmp(password, PASSWORD))
        {
            printf("Login successful\n");
            break;
        }else{
            printf("No Usr or Wrong password!\nLogin again\n");
        }
    }
    if (updb!= NULL) {
        sqlite3_close(updb);
    }
    
    //打开lcd
    int lcdfd = open_lcd_device();
    int lcd_w, lcd_h;
    get_lcd_info(&lcd_w, &lcd_h);
    mmap_lcd_device(&lcdptr);

    //主菜单初始化
    face_main_init();
    
    //主菜单Main_Menu
    struct point p; 
    while(1)
    {
        p = get_point();
        printf("(%0.1f,%0.1f)\n",(double)p.x*0.78125,(double)p.y*0.8);
    //判断坐标值
        if (p.x >= 45 && p.x <= 170 && p.y >= 37 && p.y <= 163) 
            {
                //摄像头功能
                printf("摄像头功能打开\n");
                video();
                //回到主页面
                printf("摄像头功能关闭\n");
                face_main_init();
            }
        else if (p.x >= 295 && p.x <= 420 && p.y >= 37 && p.y <= 163) 
            {
                //电子相册功能
                printf("电子相册打开\n");
                photo();
                //回到主页面
                printf("电子相册关闭\n");
                face_main_init();
            }
        else if (p.x >= 445 && p.x <= 670 && p.y >= 37 && p.y <= 163)
            {
                //视频播放
                printf("视频播放打开\n");
                avi();
                printf("视频播放关闭\n");
                face_main_init();
            }
        else if (p.x >= 45 && p.x <= 170 && p.y >= 437 && p.y <= 563)
            {
                //退出
                face_main_end();
                sleep(1);
                break;
            } 
        else
            {
                face_main_init();
            }
    }


    //解除映射
    unmapping_lcd_device(&lcdptr); 


    return 0;
}