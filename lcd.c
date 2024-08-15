#include "lcd.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>

int lcdfd = 0;
extern unsigned int *lcdptr;
int lcd_w = 800, lcd_h = 480;

// 打开 lcd 设备
int open_lcd_device()
{
    lcdfd = open("/dev/fb0", O_RDWR);
    if (lcdfd < 0)
    {
        perror("打开 lcd 失败");
        return -1;
    }
    printf("打开 lcd 成功\n");
    return lcdfd;
}

// 获取 lcd 信息
void get_lcd_info(int *width, int *height)
{
    struct fb_var_screeninfo info;
    int lret = ioctl(lcdfd, FBIOGET_VSCREENINFO, &info);
    if (lret < 0)
    {
        perror("获取 lcd 数据失败");
        return;
    }
    printf("获取 lcd 数据成功\n");
    *width = info.xres;
    *height = info.yres;
}

// 映射 lcd 设备
void mmap_lcd_device(unsigned int **lcd_ptr)
{
    *lcd_ptr = (unsigned int *)mmap(NULL, lcd_w * lcd_h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
    if (*lcd_ptr == MAP_FAILED)
    {
        perror("mmap 失败");
        return;
    }
    printf("mmap成功\n");
}

//解除映射
void unmapping_lcd_device(unsigned int **lcd_ptr) {
    if (munmap(*lcd_ptr, lcd_w * lcd_h * 4) == -1) {
        perror("munmap 失败");
        return;
    }
    printf("munmap 成功\n");
}