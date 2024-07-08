#pragma once
#include "head.h"
#include "Window.h"
using namespace std;

#define ZM_MAX 10

//ֲ������������
enum {WAN_DOU, XIANG_RI_KUI,TU_DOU_LEI,YING_TAO_ZHA_DAN,JIAN_GUO,SHI_REN_HUA, HAN_BING_GU,HUO_BAO_LA_JIAO,ZHI_WU_ZHONGLEI};

//��Ϸ״̬
enum {GOING, WIN, FAIL};


extern int curX, curY;//��ǰѡ�е�ֲ�����ƶ������е�λ��
extern int curPlant;//0��δѡ�У�1��ѡ��һ��ֲ��
static int sunCost = 0;//����ֵ


class Zombie
{
public:
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;
	int blood;//Ѫ��
	bool dead;//�ж��Ƿ�����
	int dead_type;
	bool eating;//�ж��Ƿ���п�ҧ
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
	bool blast;//�ж��Ƿ������ײ
	int frameIndex;//�ӵ���ײ���ͼƬ֡
};


class Plant
{
public:
	int type;//ֲ�����ͣ�0Ϊ��ֲ�
	int frameindex;//����֡���
	bool catched;//�Ƿ񱻿�ҧ
	int blood;//ֲ������ֵ
	bool eated;
	int timer;//��ʱ��
	int x, y;//ֲ������
public:
};

class PlantCard
{
public:
	PlantCard()
	{
		LoadPlantCard();
	}
	//����ֲ�￨��ͼƬ
	void LoadPlantCard();

	//ˢ��ֲ�￨����ͼ
	void UpdatePlantCard();

	void updatePlant();

	void drawPlant();

	//��ȡͼƬ
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
	int frameindex;//֡���к�
	bool used;//�Ƿ�ʹ�ñ��
	int timer;//��½ͣ��ʱ��

	float t;//���������ߵ�ʱ���0 . . 1

	vector2 p1, p2, p3, p4;//����·����
	vector2 pCur;//��ǰʱ��λ��
	float speed;//�˶��ٶ�
	int status;//����״̬
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
	int killCount;//��ɱ��
	int zmCount;//��ʬ����
	int GameStatus;//��Ϸ״̬
	
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

	//���˵�
	void startUI(int *type);

	//ˢ����Ϸ��ֲ�����ݣ���̬������
	void UpdateGame();

	//������ϷͼƬ
	void UpdateImg();

	//��Ϸ��ʼ����
	void gameInit1();

	//ֲ����������
	void barsDown();

	//��ײ���
	void CollisionCheck();

	//�ӵ���ײ���
	void BulletCheck();

	//��ʬ��ҧ���
	void ZMEatCheck();

	//�����ױ�ը���
	void PotatoBoomCheck();
	//������ը���
	void PepperBoomCheck();
	//ӣ��ը����ը���
	void CherryBoomCheck();
	//ʳ�˻���ʳ�˼��
	void ChomperEatCheck();
	//����������ʬ���
	void IceShroomCheck();
	//�û�����
	void userClick();

	//�����Ϸ״̬
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
