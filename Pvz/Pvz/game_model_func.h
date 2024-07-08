#pragma once
#include "head.h"
#include "Window.h"
using namespace std;

#define ZM_MAX 10

//植物类型与总数
enum {WAN_DOU, XIANG_RI_KUI,TU_DOU_LEI,YING_TAO_ZHA_DAN,JIAN_GUO,SHI_REN_HUA, HAN_BING_GU,HUO_BAO_LA_JIAO,ZHI_WU_ZHONGLEI};

//游戏状态
enum {GOING, WIN, FAIL};


extern int curX, curY;//当前选中的植物在移动过程中的位置
extern int curPlant;//0：未选中；1：选中一种植物
static int sunCost = 0;//阳光值


class Zombie
{
public:
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;
	int blood;//血量
	bool dead;//判断是否死亡
	int dead_type;
	bool eating;//判断是否进行啃咬
	bool frozen;
	int frozen_timer;
};


class ZombieImg
{
public:
	IMAGE imgZM[22];
	IMAGE imgZMDead[20];
	IMAGE imgZMDead2[10];
	IMAGE imgZMDead2_2[12];
	IMAGE imgZMEat[21];
	IMAGE imgZMFROZEN;
public:
	ZombieImg();
};


struct bullet
{
	int x, y;
	int row;
	bool used;
	int speed;
	bool blast;//判断是否产生碰撞
	int frameIndex;//子弹碰撞后的图片帧
};


class Plant
{
public:
	int type;//植物类型（0为无植物）
	int frameindex;//序列帧序号
	bool catched;//是否被啃咬
	int blood;//植物生命值
	bool eated;
	int timer;//计时器
	int x, y;//植物坐标
public:
};

class PlantCard
{
public:
	PlantCard()
	{
		LoadPlantCard();
	}
	//加载植物卡牌图片
	void LoadPlantCard();

	//刷新植物卡牌贴图
	void UpdatePlantCard();

	void updatePlant();

	void drawPlant();

	//获取图片
	IMAGE* getImg(int i);


public:
	IMAGE* imgPlant[ZHI_WU_ZHONGLEI][42];
	Plant map[5][9];
private:
	IMAGE imgCards[ZHI_WU_ZHONGLEI];
};


enum {SUNSHINE_DOWN, SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};


class SunshineImg
{
public:
	IMAGE sunImg[29];

public:
	SunshineImg();
};

class Sunshine
{
public:
	int frameindex;//帧序列号
	bool used;//是否使用标记
	int timer;//着陆停留时间

	float t;//贝塞尔曲线的时间点0 . . 1

	vector2 p1, p2, p3, p4;//曲线路径点
	vector2 pCur;//当前时刻位置
	float speed;//运动速度
	int status;//光球状态
};

class Sunshineball
{
public:
	Sunshine balls[10];
	SunshineImg sImg;

public:
	Sunshineball()
	{
		memset(balls, 0, sizeof(balls[0]));
	}
	void updateSunImg();

	void creatSunshine(PlantCard &plC);

	void updateSunshine();

	void collectSunshine(ExMessage *msg);
};

class ZombiePool
{
public:
	Zombie zm[10];
	ZombieImg zImg;
	int killCount;//击杀数
	int zmCount;//僵尸总数
	int GameStatus;//游戏状态
	
public:
	ZombiePool()
	{
		memset(zm, 0, sizeof(zm));
		killCount = 0;
		zmCount = 0;
		GameStatus = GOING;
	}

	void creatZombie();

	void updateZombie();

	void drawZM();
};

class bulletPool
{
public:
	bullet bullets[50];
	IMAGE  imgBulletNormal;
	IMAGE  imgBulletBlast[4];
public:
	bulletPool();

	void ShootCheck(ZombiePool& zm, PlantCard& plC);

	void shoot(int i, int j, int wid);

	void updateBullet();

	void drawBullet();
};

class GameInit
{
public:
	GameInit() { };

	//主菜单
	void startUI(int *type);

	//刷新游戏内植物数据（动态化处理）
	void UpdateGame();

	//更新游戏图片
	void UpdateImg();

	//游戏开始函数
	void gameInit1();

	//植物条框下拉
	void barsDown();

	//碰撞检测
	void CollisionCheck();

	//子弹碰撞检测
	void BulletCheck();

	//僵尸啃咬检测
	void ZMEatCheck();

	//土豆雷爆炸检测
	void PotatoBoomCheck();
	//辣椒爆炸检测
	void PepperBoomCheck();
	//樱桃炸弹爆炸检测
	void CherryBoomCheck();
	//食人花吃食人检测
	void ChomperEatCheck();
	//寒冰菇冻僵尸检测
	void IceShroomCheck();
	//用户操作
	void userClick();

	//检测游戏状态
	bool checkOver();

private:
	Window win;
	IMAGE ShovelImg[2];
	Window winbar;
	PlantCard plC;
	Sunshineball ba;
	ZombiePool zm;
	bulletPool pea;
};
