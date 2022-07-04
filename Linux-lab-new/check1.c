#define __LIBRARY__
#include<unistd.h>
#include<stdlib.h>

typedef struct{
	long x;
	long y;
	long dx;
	long dy;
	long color;
} object;

_syscall0(int,init_graphics);
_syscall1(int,paint,object *, GUI);

int main(void)
{
    object GUI;
    // 初始化图形界面
    init_graphics();
    GUI.x = 50;
    GUI.y = 50;
    GUI.dx = 10;
    GUI.dy = 10;
	GUI.color = 12;
    // 绘画图形界面
    paint(&GUI);
}
