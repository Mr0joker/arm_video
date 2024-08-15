#ifndef VIDEO_DISPLAY_H
#define VIDEO_DISPLAY_H

#include <linux/videodev2.h>
struct v4l2_buffer readbuffer;
unsigned char *mptr[4];

//摄像头显示
void video_display(int fd,unsigned int *lcdptr);
/**
 * 解析 JPEG 文件数据并将其转换为 RGB 数据
 * @param jpegData 指向 JPEG 数据的指针
 * @param rgbdata 存储转换后的 RGB 数据的缓冲区
 * @param size JPEG 数据的大小
 * @return 1 表示成功，0 表示失败
 */
int read_JPEG_file(const char *jpegData, char *rgbdata, int size);

/**
 * 在 LCD 上显示 RGB 数据
 * @param rgbdata 要显示的 RGB 数据
 * @param w 图像宽度
 * @param h 图像高度
 */
void lcd_show_rgb(unsigned char *rgbdata, int w, int h,unsigned int *lcdptr);

/**
 * 打开视频设备
 * @return 成功打开返回文件描述符，失败返回 -1
 */
int open_device();

/**
 * 设置视频采集格式
 * @param fd 视频设备文件描述符
 */
void set_capture_format(int fd);

/**
 * 向视频设备申请内核缓冲区队列
 * @param fd 视频设备文件描述符
 */
void request_buffer_space(int fd);

/**
 * 映射视频设备的缓冲区
 * @param fd 视频设备文件描述符
 */
void map_buffers(int fd);

/**
 * 开始视频采集
 * @param fd 视频设备文件描述符
 */
void start_capture(int fd);

/**
 * 从视频设备提取一帧数据
 * @param fd 视频设备文件描述符
 * @param rgbdata 存储提取的帧数据的缓冲区
 */
void extract_frame_data(int fd, unsigned char rgbdata[]);



/**
 * 通知内核已处理完一帧数据
 * @param fd 视频设备文件描述符
 */
void notify_kernel(int fd);

/**
 * 停止视频采集
 * @param fd 视频设备文件描述符
 */
void stop_capture(int fd);

/**
 * 解除视频设备缓冲区的映射
 * @param fd 视频设备文件描述符
 */
void unmap_buffers(int fd);

/**
 * 关闭视频设备
 * @param fd 视频设备文件描述符
 */
void close_device(int fd);

#endif