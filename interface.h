#ifndef __INTERFACE_H__
#define __INTERFACE_H__




/**
 * @brief 在指定的 LCD 上显示 BMP 图片，并支持偏移
 * @param bmp_path BMP 图片路径
 * @param x_ofset x 方向偏移量
 * @param y_ofset y 方向偏移量
 * @return 0 表示成功，-1 表示失败
 */




void bmp_display(int *lcd_point,const char *pathname,int posx,int posy);

//主页面初始化
void face_main_init(void);
//主页面结束
void face_main_end(void);
//监控界面初始化
void face_video_init(void);
//视频播放初始化
void face_avi_init(void);
//图片界面初始化
void face_photo_init(void);

#endif