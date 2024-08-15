#ifndef __TOUCH_H__
#define __TOUCH_H__

/**
 * @brief 坐标结构体，包含 x 和 y 坐标
 */
struct point
{
    int x;
    int y;
};

/**
 * @brief 定义方向枚举类型
 */
enum DIREC
{
    ERROR,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    OTHER
};

/**
 * @brief 获取触摸屏的触摸点坐标，如果获取失败则返回坐标为(-1,-1)的结构体
 * @return 触摸点的坐标结构体
 */
struct point get_point();

/**
 * @brief 获取触摸屏的滑动方向
 * @return 表示滑动方向的枚举值，包括 ERROR、UP、DOWN、LEFT、RIGHT、OTHER
 */
enum DIREC get_direction();


/**
 * @brief 判断功能
 * @return  0无需操作
 *          1拍照
 *          2录像
 *          3退出
 */
int judgeFunction(struct point p); 

#endif