#pragma once
#include "head.h"
using namespace std;


#define WIN_WIDTH 900
#define WIN_HEIGHT 600

class Window
{
public:
	
	//加载图片
	void LoadWimage(LPCTSTR path);
	void LoadZSimage(int i,LPCTSTR path);
	
	//绘制窗口
	void MakeWindow(int x = 0,int y = 0);

	//片头巡场
	void viewSence();

	//渲染窗口
	void UpdateWindow(int x = 0,int y = 0);
	void UpdateWindowPNG(int x = 0, int y = 0);

	//获取数据
	int getwidth();
	int getheight();
private:
	IMAGE img;
	IMAGE imgZMStand[11];
};