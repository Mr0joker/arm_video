#ifndef LCD_H
#define LCD_H

/**
 * 打开 LCD 设备
 * @return 成功打开返回文件描述符，失败返回 -1
 */
int open_lcd_device();

/**
 * 获取 LCD 的信息，如宽度和高度
 * @param width 用于存储 LCD 宽度的指针
 * @param height 用于存储 LCD 高度的指针
 */
void get_lcd_info(int *width, int *height);

/**
 * 映射 LCD 设备
 * @param lcd_ptr 用于存储映射后的指针的指针
 */
void mmap_lcd_device(unsigned int **lcd_ptr);

/**
 * 解除映射 LCD 设备
 * @param lcd_ptr 用于存储映射后的指针的指针
 */
void unmapping_lcd_device(unsigned int **lcd_ptr);
#endif