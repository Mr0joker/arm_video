// 标准输入输出库
#include <stdio.h>
#include <math.h>
#include<stdlib.h> 

// 系统IO库
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// 内存映射
#include <sys/mman.h>
extern unsigned int *lcdptr;


void Draw_Pixel(int x,int y,int color,int *lcd_point)
{
    if(x>=0&&x<800&&y>=0&&y<480)
    *(lcd_point+(y*800+x)) = color;
	
}

void bmp_display(int *lcd_point,const char *pathname,int posx,int posy)
{
	int picture_id = open(pathname,O_RDONLY);
    if(picture_id == -1)
    {
        perror("picture open failed");
        return ;
    }

    // 读取BMP图片的一些属性信息
    // 读取文件魔数，用于描述该文件是什么类型的文件
    unsigned char data[4]={0};
    read(picture_id,data,2);
    if(data[0]!='B'||data[1]!='M') // if(data[0] != 0x42||data[1] != 0x4d)
    {
        printf("this picture not bmp\n");
        close(picture_id);
        return ;
    }

    // 读取像素数组的偏移量
    lseek(picture_id,0x0a,SEEK_SET); // 偏移到像素数组偏移量数据的起始位置
    read(picture_id,data,4);

    // 1000 0000 | 0100 0000 | 0010 0000 | 0001 0000
    /*
                                      0001 0000 data[0]
                            0010 0000 0000 0000 data[1]
                  0100 0000 0000 0000 0000 0000 data[2]
        1000 0000 0000 0000 0000 0000 0000 0000 data[3]
    */
    int offset = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // 读取宽高
    lseek(picture_id,0x12,SEEK_SET);
    read(picture_id,data,4);
    int width = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    read(picture_id,data,4);
    int height = data[3] << 24 | data[2] << 16  | data[1] << 8 | data[0];

    // 读取色深，只有知道色深才知道每个图片的像素点所占字节数
    lseek(picture_id,0x1c,SEEK_SET);
    read(picture_id,data,2);
    int depth = data[1] << 8 | data[0];

    // 计算图片的填充字节数
    int fills = 0; // 填充字节默认为零

    if((width*(depth/8))%4)
    {
        fills = 4 - ((width*(depth/8))%4);
    }

    // 求出每一行真实字节数
    int real_bytes =  (width*(depth/8)) + fills;

    // 搞一个动态数据用来保存像素数组的数据
    unsigned char *color_array = (unsigned char*)malloc(real_bytes*abs(height));
    unsigned char *color_point = color_array;

    lseek(picture_id,offset,SEEK_SET);
    read(picture_id,color_array,real_bytes*abs(height));
    // 循环变量图片的像素点
    for(int h = 0;h < abs(height);h++)
    {
        for(int w = 0;w < width;w++)
        {
            unsigned char a,r,g,b;
            b = *color_point++;
            g = *color_point++;
            r = *color_point++;
            a = (depth == 24)?0:*color_point++;

            int color = a << 24 | r << 16 | g << 8 | b;
            // 将颜色画到屏幕上
            // int new_w = (int)(w*4) + posx;
            // int new_h = (int)(((height > 0) ? height - 1 - h : abs(height))*4) + posy;

            Draw_Pixel(w+posx,((height>0)?height-1-h:abs(height))+posy,color,lcd_point);
        }
        // 跳过填充字节
        color_point+=fills;
    }
    
    free(color_array);
    close(picture_id);
}


void face_main_init(void)
{
    bmp_display(lcdptr,"./BMP/interface.bmp",0,0);
    bmp_display(lcdptr,"./BMP/capphoto.bmp",30,30);
    bmp_display(lcdptr,"./BMP/photo.bmp",230,30);
    bmp_display(lcdptr,"./BMP/avi.bmp",430,30);
    bmp_display(lcdptr,"./BMP/exit.bmp",30,350);
}

void face_main_end(void)
{
    bmp_display(lcdptr,"./BMP/end.bmp",0,0);
}


void face_video_init(void)
{
    bmp_display(lcdptr,"./BMP/backdrop.bmp",640,0);
    bmp_display(lcdptr,"./BMP/capphoto.bmp",670,30);
    bmp_display(lcdptr,"./BMP/video.bmp",670,190);
    bmp_display(lcdptr,"./BMP/exit.bmp",670,350);
}


void face_photo_init(void)
{
    bmp_display(lcdptr,"./BMP/backdrop.bmp",640,0);
    bmp_display(lcdptr,"./BMP/prev.bmp",670,30);
    bmp_display(lcdptr,"./BMP/next.bmp",670,190);
    bmp_display(lcdptr,"./BMP/send.bmp",670,350);
}


void face_avi_init(void)
{
    bmp_display(lcdptr,"./BMP/backdrop.bmp",640,0);
    bmp_display(lcdptr,"./BMP/send.bmp",670,30);
    bmp_display(lcdptr,"./BMP/exit.bmp",670,350);
}