#include "video_display.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <jpeglib.h>
#include <linux/fb.h>
#include "interface.h"
#include "camera.h"
#include "avilib.h"

// 定义全局变量
int lcd_w;
extern unsigned int *lcdptr;
extern int video_exit;
extern int video_play;
unsigned int size[4];


//jpg图片解析
int read_JPEG_file(const char *jpegData, char *rgbdata, int size)
{
    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char *)jpegData, size);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);
    int row_stride = cinfo.output_width * cinfo.output_components;
    unsigned char *buffer = malloc(row_stride);
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, &buffer, 1);
        char *p = buffer;
        for (int x = 0; x < cinfo.output_width; x++)
        {
            unsigned char r, g, b, a = 0;
            if (cinfo.output_components == 3)
            {
                r = *p++;
                g = *p++;
                b = *p++;
            }
            else if (cinfo.output_components == 4)
            {
                a = *p++;
                r = *p++;
                g = *p++;
                b = *p++;
            }
            if (b > 200)
                b = b - 70;
            int color = (a << 24) | (r << 16) | (g << 8) | b;
            memcpy(rgbdata + (x + cinfo.output_width * (cinfo.output_scanline - 1)) * 3, &color, sizeof(int));
        }
    }
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return 1;
}

//画图
void lcd_show_rgb(unsigned char *rgbdata, int w, int h,unsigned int *lcdptr)
{
    unsigned int *ptr = lcdptr;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            memcpy(ptr + j, rgbdata + j * 3, 3);
        }
        ptr += lcd_w;
        rgbdata += w * 3;
    }
}

// 打开设备
int open_device()
{
    int fd = open("/dev/video7", O_RDWR);
    if (fd < 0)
    {
        perror("打开摄像头失败");
    }
    printf("打开摄像头成功\n");
    return fd;
}

// 设置采集格式
void set_capture_format(int fd)
{
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vfmt.fmt.pix.width = 640;
    vfmt.fmt.pix.height = 480;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
    if (ret < 0)
    {
        perror("设置格式失败");
    }
    printf("设置格式成功\n");
}

// 申请内核缓冲区队列
void request_buffer_space(int fd)
{
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.count = 4;
    reqbuffer.memory = V4L2_MEMORY_MMAP;
    int ret = ioctl(fd, VIDIOC_REQBUFS, &reqbuffer);
    if (ret < 0)
    {
        perror("申请队列空间失败");
    }
    printf("申请队列空间成功\n");
}

// 映射缓冲区
void map_buffers(int fd)
{
    struct v4l2_buffer mapbuffer;
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for (int i = 0; i < 4; i++)
    {
        mapbuffer.index = i;
        int ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);
        if (ret < 0)
        {
            perror("查询内核空间队列失败");
        }
        printf("查询内核空间队列成功\n");
        mptr[i] = (unsigned char *)mmap(
            NULL,
            mapbuffer.length,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            mapbuffer.m.offset);
        size[i] = mapbuffer.length;
        ret = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
        if (ret < 0)
        {
            perror("放回失败");
        }
        printf("放回成功\n");
    }
}

// 开始采集
void start_capture(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0)
    {
        perror("开启失败");
    }
    printf("开启成功\n");
}

// 提取一帧数据
void extract_frame_data(int fd, unsigned char rgbdata[])
{
    //struct v4l2_buffer readbuffer;
    readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
    if (ret < 0)
    {
        perror("提取数据失败");
    }
    // else
    // {
    // printf("提取数据成功\n");
    // }
    // printf("%p\n",mptr[readbuffer.index]);
    // printf("%d\n",readbuffer.index);
    // printf("%p\n",rgbdata);
    // printf("%d\n",readbuffer.length);
    
    read_JPEG_file(mptr[readbuffer.index], rgbdata, readbuffer.length);
}



// 通知内核
void notify_kernel(int fd)
{
    //struct v4l2_buffer readbuffer;
    int ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
    if (ret < 0)
    {
        perror("放回队列失败");
    }
    // printf("放回队列成功\n");
}

    


//监控显示
void video_display(int fd,unsigned int *lcdptr)
{
    //预先设定好录像文件路径
    avi_t *AVI = AVI_open_output_file("./output.avi");
    if (AVI == NULL) {
        perror("无法打开 AVI 输出文件");
        return;
    }
    printf("创建avi文件成功\n");
    int frame_count = 0;
    //设置avi输出格式
    int set = AVI_set_video(AVI, 640, 480, 20, "RGB24");
    if(set == -1)
    {
        printf("设置输出视频格式失败\n");
    }
    //AVI_set_audio(AVI,1,8000,16,WAVE_FORMAT_PCM,128);

    //捕获视频
    set_capture_format(fd);
    request_buffer_space(fd);
    map_buffers(fd);
    start_capture(fd);
    unsigned char rgbdata[640 * 480 * 3];
    while (1)
    {
        extract_frame_data(fd, rgbdata);
        lcd_show_rgb(rgbdata,640,480,lcdptr);
        //录像
        if(video_play)
        {
            if (AVI_write_frame(AVI, (unsigned char *)rgbdata, 640 * 480 * 3, frame_count) < 0) 
            {
                perror("写入 AVI 帧失败");
            }
            frame_count++;
        }
        notify_kernel(fd);  
        //终止
        if(video_exit)
        {
            printf("摄像终止\n");
            break;
        }
    }
    //关闭avi输出文件
    AVI_close(AVI);
    stop_capture(fd);
    unmap_buffers(fd);
}


// 停止采集
void stop_capture(int fd)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if (ret < 0)
    {
        perror("停止采集失败");
    }
    printf("停止采集成功\n");
}

// 解除映射
void unmap_buffers(int fd)
{
    for (int i = 0; i < 4; i++)
    {
        munmap(mptr[i], size[i]);
    }
    printf("解除映射\n");
}




// 关闭设备
void close_device(int fd)
{
    close(fd);
    printf("关闭设备\n");
}
