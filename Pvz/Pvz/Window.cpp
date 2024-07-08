#include "Window.h"

void Window::LoadWimage(LPCTSTR path)
{
	loadimage(&img, path);
}

void Window::LoadZSimage(int i,LPCTSTR path)
{
		loadimage(&imgZMStand[i], path);
}

void Window::MakeWindow(int x,int y)
{
	initgraph(x, y);
}

void Window::viewSence()
{
	int xMin = WIN_WIDTH - img.getwidth();
	//僵尸位置节点
	vector2 points[9] = {
		{550,80},{530,160},{630,170},{530,200},{515,270},
		{560,370},{605,340},{705,280},{690,340}
	};

	//僵尸矗立位置
	int index[9];
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;
	}

	//推进镜头
	int count = 0;
	BeginBatchDraw();
	for (int x = 0; x >= xMin; x -= 2)
	{
		putimage(x, 0, &img);

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
			if (count >= 10)
				index[k] = (index[k] + 1) % 11;
		}
		if (count >= 10)
			count = 0;
		FlushBatchDraw();
		Sleep(5);
	}
	EndBatchDraw();

	//停留时间
	BeginBatchDraw();
	for (int i = 0; i < 100; i++)
	{
		putimage(xMin, 0, &img);

		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x, points[k].y, &imgZMStand[index[k]]);
			if (count >= 3)
			{
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 3)
			count = 0;

		FlushBatchDraw();
		Sleep(20);
	}
	EndBatchDraw();

	//回拉镜头
	BeginBatchDraw();
	for (int x = xMin; x <= -112; x += 2)
	{

		putimage(x, 0, &img);
		
		count++;
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)
			count = 0;
		FlushBatchDraw();
		Sleep(5);
	}
	EndBatchDraw();

}

void Window::UpdateWindow(int x,int y)
{
	putimage(x, y, &img);
}

void Window::UpdateWindowPNG(int x, int y)
{
	putimagePNG(x, y, &img);
}

int Window::getwidth()
{
	return img.getwidth();
}

int Window::getheight()
{
	return img.getheight();
}

