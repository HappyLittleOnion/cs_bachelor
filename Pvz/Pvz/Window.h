#pragma once
#include "head.h"
using namespace std;


#define WIN_WIDTH 900
#define WIN_HEIGHT 600

class Window
{
public:
	
	//����ͼƬ
	void LoadWimage(LPCTSTR path);
	void LoadZSimage(int i,LPCTSTR path);
	
	//���ƴ���
	void MakeWindow(int x = 0,int y = 0);

	//ƬͷѲ��
	void viewSence();

	//��Ⱦ����
	void UpdateWindow(int x = 0,int y = 0);
	void UpdateWindowPNG(int x = 0, int y = 0);

	//��ȡ����
	int getwidth();
	int getheight();
private:
	IMAGE img;
	IMAGE imgZMStand[11];
};