#include "game_model_func.h"
#include "thread"
int curX, curY;//��ǰѡ�е�ֲ�����ƶ������е�λ��
int curPlant;//0��δѡ�У�1��ѡ��һ��ֲ��
int curShvoel = 0;
//static int sunCost = 0;
static int status = 0;
static int chomper_eat = 0;//ʳ�˻�ʳ�˱�־


vector<int> ZwCost = { 100,50,25,150,50,150,75,125};


void ZombiePool::creatZombie()
{
	if (zmCount == ZM_MAX)
		return;

	//��ʬ����Ƶ��
	static int zmFre = 50;
	static int ZMcount = 0;
	ZMcount++;
	if (ZMcount > zmFre)
	{
		ZMcount = 0;
		zmFre = 200 + rand() % 400;
		//ˢ�½�ʬ
		int i, zmMax = sizeof(zm) / sizeof(zm[0]);
		
		for (i = 0; i < zmMax && zm[i].used; i++);
		if (i < zmMax)
		{
			//memset(&zm[i], 0, sizeof(zm[i]));
			zm[i].used = true;
			zm[i].x = WIN_WIDTH;
			zm[i].row = rand() % 5; //�����������
			zm[i].y = (172 - 102) + (1 + zm[i].row) * 100;
			zm[i].speed = 1;
			zm[i].blood = 100;
			zm[i].dead = false;
			zm[i].dead_type = 0;
			zm[i].eating = false;
			zm[i].frozen_timer = 0;
			zm[i].frozen = false;
			zmCount++;
		}
	}

}


void ZombiePool::updateZombie()
{
	int zmMax = sizeof(zm) / sizeof(zm[0]);

	//ˢ�½�ʬ����
	static int upZMcount = 0;
	upZMcount++;
	if (upZMcount > 4)
	{
		upZMcount = 0;
		//���½�ʬλ��
		for (int i = 0; i < zmMax; i++)
		{
			if (zm[i].used)
			{
				zm[i].x -= zm[i].speed;
				if (zm[i].x < 150)//��ʬͨ����ƺ
				{
					GameStatus = FAIL;
				}
			}
		}
	}

	static int reZMcount = 0;
	reZMcount++;
	if (reZMcount > 3)
	{
		reZMcount = 0;//��ʬͼƬˢ��Ƶ��
		for (int i = 0; i < zmMax; i++)
		{
			if (zm[i].used)
			{
				if (zm[i].dead)//������
				{
					zm[i].frameIndex++;//����֡++
					if (zm[i].frameIndex >= 20)//�������
					{
						killCount++;
						zm[i].used = false;//��ʼ��
					}
				}
				else if (zm[i].eating)
				{
					if(zm[i].frameIndex%7 == 0)
						PlaySound("res/usingAudio/chomp.wav", NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
					zm[i].frameIndex = (zm[i].frameIndex + 1) % 21;
				}
				else if (zm[i].frozen)
				{
					continue;
				}
				else
				{
					zm[i].frameIndex = (zm[i].frameIndex + 1) % 22;
				}
			}
		}
	}
}
ZombieImg::ZombieImg()
{
	char name[64];
	for (int i = 0; i < 22; i++)//�������뽩ʬ�н�ͼƬ
	{
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&imgZM[i], name);
			ofs.close();
		}
	}

	for (int i = 0; i < 20; i++)//��ʬ����ͼƬ
	{
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&imgZMDead[i], name);
			ofs.close();
		}
	}
	for (int i = 0; i < 10; i++)//�Ǳ�ը����ͼƬ����
	{
		sprintf_s(name, sizeof(name), "res/zm_dead2/%d.png", i + 1);
		ifstream ofs_2(name, ifstream::in);
		if (ofs_2.is_open())
		{
			loadimage(&imgZMDead2[i], name);
			ofs_2.close();
		}
	}
	for (int i = 0; i < 12; i++)//ͷ
	{
		sprintf_s(name, sizeof(name), "res/zm_dead2_2/%d.png", i + 1);
		ifstream ofs_2_2(name, ifstream::in);
		if (ofs_2_2.is_open())
		{
			loadimage(&imgZMDead2_2[i], name);
			ofs_2_2.close();
		}
	}

	for (int i = 0; i < 21; i++)//��ʬ��ҧͼƬ
	{
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&imgZMEat[i], name);
			ofs.close();
		}
	}
	loadimage(&imgZMFROZEN, "res/zhiwu/6/icetrap.png");
}


void ZombiePool::drawZM()
{
	int zmMax = sizeof(zm) / sizeof(zm[0]);
	for (int i = 0; i < zmMax; i++)
	{
		if (zm[i].used)
		{
			//IMAGE* Img = &zImg.imgZM[zm[i].frameIndex];
			IMAGE* Img = NULL;
			IMAGE* Img_2 = NULL;
			IMAGE* Img_3 = &zImg.imgZMFROZEN;//����ͼƬ
			
			if (zm[i].dead && zm[i].dead_type == 1)
				Img = zImg.imgZMDead;
			else if (zm[i].dead && zm[i].dead_type == 2)
			{
				Img = zImg.imgZMDead2;
				Img_2 = zImg.imgZMDead2_2;
			}
			
			else if (zm[i].eating)
				Img = zImg.imgZMEat;//ѡȡʹ�õ�ͼƬ��
			else
				Img = zImg.imgZM;

			Img += zm[i].frameIndex;
			if (zm[i].dead_type == 2)
			{
				putimagePNG(
					zm[i].x+45,
					zm[i].y - Img->getheight(),
					Img);
				putimagePNG(
					zm[i].x + 45,
					zm[i].y - Img_2->getheight(),
					Img_2);
			}
			else if (zm[i].frozen)
			{
				if (zm[i].frozen_timer < 10000)
				{
					putimagePNG(zm[i].x, zm[i].y - Img->getheight(), Img);
					putimagePNG(zm[i].x+90, zm[i].y - Img_3->getheight(), Img_3);
					zm[i].frozen_timer++;
				}
				else
				{
					zm[i].frozen_timer = 0;
					zm[i].frozen = false;
					zm[i].speed = 1;
				}
				
			}
			else 
			{
					putimagePNG(
						zm[i].x,
						zm[i].y - Img->getheight(),
						Img);
			}
		}
	}
}

void GameInit::CollisionCheck()
{


	//�����ӵ�����ײ���
	BulletCheck();

	//��������ײ����
	PotatoBoomCheck();
	//������ը����
	PepperBoomCheck();
	//ӣ�ұ�ը����
	CherryBoomCheck();
	//ʳ�˻����˼���
	ChomperEatCheck();
	//����������ʬ����
	IceShroomCheck();
	//��ʬ��ֲ�����ײ���
	ZMEatCheck();

}

void GameInit::BulletCheck()
{
	int bCount = sizeof(pea.bullets) / sizeof(pea.bullets[0]);
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);

	for (int i = 0; i < bCount; i++)
	{
		if (pea.bullets[i].used == false || pea.bullets[i].blast)
			continue;
		if (pea.bullets[i].used == true && pea.bullets[i].x > WIN_WIDTH)
		{
			pea.bullets[i].used = false;
		}
		for (int k = 0; k < zCount; k++)
		{
			if (zm.zm[k].used == false)
				continue;
			int x1 = zm.zm[k].x + 80;
			int x2 = zm.zm[k].x + 110;
			int x = pea.bullets[i].x;
			if (!zm.zm[k].dead && pea.bullets[i].row == zm.zm[k].row && x > x1 && x < x2)//�ӵ�����
			{
				PlaySound("res/usingAudio/firepea.wav", NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
				zm.zm[k].blood -= 15;
				pea.bullets[i].blast = true;
				pea.bullets[i].speed = 0;

				//�жϽ�ʬ�Ƿ�����
				if (zm.zm[k].blood <= 0)
				{
					zm.zm[k].dead = true;
					zm.zm[k].dead_type = 2;
					zm.zm[k].speed = 0;
					zm.zm[k].frameIndex = 0;//�����к�ָ������ͼƬ֡
				}
				break;
			}
		}
	}
}

void GameInit::ZMEatCheck()
{
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].dead)
			continue;

		int row = zm.zm[i].row;
		for (int k = 0; k < 9; k++)
		{
			int zhiWuX = 256 - 112 + k * 81;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zm.zm[i].x + 80;
			//��ֲ�ﲻ����
			if (plC.map[row][k].type == 0)
			{
				if (x3 > x1 && x3 < x2 && zm.zm[i].eating)//�����ʬ�ڿ�ҧ
				{
					zm.zm[i].eating = false;
					zm.zm[i].frameIndex = 0;
					zm.zm[i].speed = 1;
				}
				continue;
			}
			//int zhiWuX = 256 - 112 + k * 81;
			//int x1 = zhiWuX + 10;
			//int x2 = zhiWuX + 60;
			//int x3 = zm.zm[i].x + 80;
			else if (x3 > x1 && x3 < x2)
			{
				if (plC.map[row][k].catched)//�ж�ֲ���Ƿ񱻲���
				{
					if (!zm.zm[i].eating)//�ж��Ƿ�ʼ��ʳ
					{
						zm.zm[i].eating = true;
						zm.zm[i].speed = 0;
						zm.zm[i].frameIndex = 0;
					}
					plC.map[row][k].blood--;
					if (plC.map[row][k].blood <= 0)//�ж�ֲ���Ƿ�����
					{
						//���ǣ���������
						plC.map[row][k].type = 0;
						plC.map[row][k].catched = false;
						if (!zm.zm[i].frozen)
						{
							zm.zm[i].eating = false;
							zm.zm[i].frameIndex = 0;
							zm.zm[i].speed = 1;
						}
						else if(zm.zm[i].frozen)
						{
							zm.zm[i].eating = false;
							zm.zm[i].frameIndex = 0;
						}
					}
				}
				else//���򣬽����ҧ״̬
				{
					plC.map[row][k].catched = true;				
					zm.zm[i].eating = true;
					zm.zm[i].speed = 0;
					zm.zm[i].frameIndex = 0;
				}
			}
		}
	}

}

//��ըͼƬ
void PotatoBoom(int x, int y)
{
	PlaySound("res/usingAudio/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	static IMAGE img1, img2;
	loadimage(&img1, "res/zhiwu/2/boom.png");
	loadimage(&img2, "res/zhiwu/2/mashed.png");
	for (int temp = 0; temp < 500; temp++)
	{
		putimagePNG(x, y, &img1);
		putimagePNG(x, y, &img2);
	}
}
void IceShroom(int x, int y)
{
	PlaySound("res/usingAudio/frozen.wav", NULL, SND_FILENAME  | SND_ASYNC);
	static IMAGE img1;
	loadimage(&img1, "res/zhiwu/6/Snow.png");
	for (int i = 0; i < 500; i++)
	{
		putimagePNG(x, y, &img1);
	}
}
void ChomperEat(int x, int y)
{
	static IMAGE img[8];
	char name[64];
	for (int i = 0; i < 8; i++)
	{
		sprintf_s(name, sizeof(name), "res/zhiwu/5/%d.png", i + 14);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&img[i], name);
			ofs.close();
		}
	}
	chomper_eat = 1;
	for (int temp = 0; temp < 8; temp++)
	{
		for (int k = 0; k < 100; k++)
		{
			putimagePNG(x, y - 20, &img[temp]);
		}
	}
	chomper_eat = 0;
}


void CherryBoom(int x, int y)
{
	PlaySound("res/usingAudio/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	static IMAGE ch_img[15];
	char name[64];
	for (int i = 0; i < 15; i++)
	{
		sprintf_s(name, sizeof(name), "res/zhiwu/3/%d.png", i + 4);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&ch_img[i], name);
			ofs.close();
		}
	}
	for (int i=0;i<15;i++)
	{
		for(int temp=0;temp<100;temp++)
		putimagePNG(x, y, &ch_img[i]);
	}
}


void Pepper(int x, int y)
{
	PlaySound("res/usingAudio/fire.wav", NULL, SND_FILENAME | SND_ASYNC);
	static IMAGE pe_img[16];
	char name[64];
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(name, sizeof(name), "res/zhiwu/7/%d.png", i + 1);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&pe_img[i], name);
			ofs.close();
		}
	}
	for (int i = 0; i < 8; i++)
	{
		for (int temp = 0; temp < 100; temp++)
			putimagePNG(x, y, &pe_img[i]);
	}
	for (int j = 8; j < 16; j++)
	{
		for (int temp = 0; temp < 100; temp++)
		{
			putimagePNG(x, y-60, &pe_img[j]);
		}
	}
}


void GameInit::PotatoBoomCheck()
{
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].dead)
			continue;

		int row = zm.zm[i].row;
		for (int k = 0; k < 9; k++)
		{
			//������������
			if (plC.map[row][k].type != TU_DOU_LEI + 1) continue;

			int zhiWuX = 256 - 112 + k * 81;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zm.zm[i].x + 80;
			if (x3 > x1 && x3 < x2)
			{
				if (plC.map[row][k].timer >= 100)//�ж��Ƿ���
				{
					thread t(&PotatoBoom,x1,plC.map[row][k].y
					);
					t.detach();
					//��ʬ����

							zm.zm[i].dead = true;
							zm.zm[i].dead_type = 1;
							zm.zm[i].speed = 0;
							zm.zm[i].frameIndex = 0;

					//����������
					
					plC.map[row][k].type = 0;
				}
			}
		}
	}

}


void GameInit::PepperBoomCheck()
{
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	int zhiWuX = 0, x1 = 0, y1 = 0, x, y;
	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (plC.map[row][col].type == HUO_BAO_LA_JIAO + 1 )
			{
				zhiWuX = 256 - 112 + col * 81;
				x1 = zhiWuX + 10;
				y1 = plC.map[row][col].y;
				x = row;
				y = col;
			}
		}
	}
	if (x1 == 0)
	{
		return;
	}
	thread pep(&Pepper, x1, y1);
	pep.detach();

	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].used&&zm.zm[i].row==x)
		{
			zm.zm[i].dead = true;
			zm.zm[i].dead_type = 1;
			zm.zm[i].speed = 0;
			zm.zm[i].frameIndex = 0;
		}
	}

	plC.map[x][y].blood = 60;
	plC.map[x][y].type = 0;
}
void GameInit::CherryBoomCheck()
{
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].dead || !zm.zm[i].used)
			continue;
		int row = zm.zm[i].row;
		for (int k = 0; k < 9; k++)
		{
	
			if (plC.map[row][k].type != YING_TAO_ZHA_DAN + 1) continue;

			int zhiWuX = 256 - 112 + k * 81;
			int x1 = zhiWuX -60;
			int x2 = zhiWuX + 60;
			int x3 = zm.zm[i].x + 80;
			if (x3 > x1 && x3 < x2)
			{
					thread cherry(&CherryBoom, x1, plC.map[row][k].y);
					cherry.detach();
					//��ʬ����
					for (int d_count = 0; d_count < zCount; d_count++)
					{
						int z_x = zm.zm[d_count].x + 80;
						if (z_x > x1 - 60 && z_x < x2 + 120 && (zm.zm[d_count].row == row || zm.zm[d_count].row == row + 1 || zm.zm[d_count].row == row - 1))
						{
							zm.zm[d_count].dead = true;
							zm.zm[d_count].dead_type = 1;
							zm.zm[d_count].speed = 0;
							zm.zm[d_count].frameIndex = 0;
						}
					}

					plC.map[row][k].blood = 60;
					plC.map[row][k].type = 0;
			}
		}
	}

}


void GameInit::IceShroomCheck()
{
	int ice_flag = 0;
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	int zhiWuX = 0, x1 = 0, y1 = 0,x,y;
	for (int row = 0; row < 5; row++)
	{
		for (int col = 0; col < 9; col++)
		{
			if (plC.map[row][col].type == HAN_BING_GU + 1&&plC.map[row][col].timer>=100)
			{
				 zhiWuX = 256 - 112 + col * 81;
				 x1 = zhiWuX + 10;
				 y1 = plC.map[row][col].y;
				
				 x = row;
				 y = col;
			}
		}
	}
	if (x1 == 0)
	{
		return;
	}
	
	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].used)
		{
			ice_flag = 1;
			zm.zm[i].frozen = true;
			zm.zm[i].speed = 0;
		}
	}
	if (ice_flag == 1)
	{
		thread iceshroom(&IceShroom, x1, y1);
		iceshroom.detach();
	}
	plC.map[x][y].blood = 60;
	plC.map[x][y].type = 0;
}
void GameInit::ChomperEatCheck()
{
	int zCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zm.zm[i].dead || !zm.zm[i].used)
			continue;
		int row = zm.zm[i].row;
		for (int k = 0; k < 9; k++)
		{

			if (plC.map[row][k].type != SHI_REN_HUA + 1) continue;

			int zhiWuX = 256 - 112 + k * 81;
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zm.zm[i].x + 80;
			if (x3 > x1-60 && x3 < x2+100&&plC.map[row][k].timer==0)
			{
				thread chomper(&ChomperEat, x1, plC.map[row][k].y-20 );
				chomper.detach();
					zm.zm[i].dead = true;
					zm.zm[i].dead_type = 3;
					zm.zm[i].used = false;
					zm.zm[i].speed = 0;
					zm.zm[i].frameIndex = 0;
					zm.killCount++;
					//ʳ�˻�״̬�л�
					plC.map[row][k].timer ++;
			}
		}
	}
}

bulletPool::bulletPool()
{
		//��ʼ���㶹�ӵ�ͼƬ
		loadimage(&imgBulletNormal, "res/bullets/PeaNormal/PeaNormal_0.png");
		memset(bullets, 0, sizeof(bullets[0]));

		//��ʼ���㶹�ӵ���ըͼƬ֡
		loadimage(&imgBulletBlast[3], "res/bullets/PeaNormalExplode/PeaNormalExplode_0.png");
		for (int i = 0; i < 3; i++)
		{
			float k = (i + 1) * 0.2f;
			loadimage(&imgBulletBlast[i], "res/bullets/PeaNormalExplode/PeaNormalExplode_0.png",
				imgBulletBlast[3].getwidth() * k,
				imgBulletBlast[3].getheight() * k, true);

		}

}

void bulletPool::ShootCheck(ZombiePool& zm,PlantCard &plC)
{
	int Bulletlines[5] = { 0 };
	int zmCount = sizeof(zm.zm) / sizeof(zm.zm[0]);
	int dangerX = WIN_WIDTH - zm.zImg.imgZM[0].getwidth() + 112;
	for (int i = 0; i < zmCount; i++)
	{
		if (zm.zm[i].used && zm.zm[i].x < dangerX)
		{
			Bulletlines[zm.zm[i].row] = 1;
		}
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plC.map[i][j].type == WAN_DOU + 1 && Bulletlines[i])
			{
				plC.map[i][j].timer++;
				if(plC.map[i][j].timer > 70)
				{
					plC.map[i][j].timer = 0;
					//int wid = plC.imgPlant[plC.map[i][j].type - 1][0]->getwidth() - 10;
					shoot(i, j, plC.imgPlant[plC.map[i][j].type - 1][0]->getwidth() - 10);//�����ӵ�
				}
			}
			else if(plC.map[i][j].type == WAN_DOU + 1 && !Bulletlines[i] && plC.map[i][j].timer < 70)//�㶹��ȴ����
			{
				plC.map[i][j].timer = 70;
			}
		}
	}
}

void bulletPool::shoot(int i,int j,int wid)
{
	static int count = 0;
	if (++count < 2) return;
	count = 0;

	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int k;
	for (k = 0; k < bulletMax && bullets[k].used; k++);//�ҵ�����ʹ�õ��ӵ�
	if (k < bulletMax)
	{
		bullets[k].used = true;
		bullets[k].row = i;
		bullets[k].speed = 10;

		bullets[k].blast = false;
		bullets[k].frameIndex = 0;

		int zwX = 256 - 112 + j * 81;
		int zwY = (179 - 102) + i * 102 + 14;
		bullets[k].x = zwX + wid;
		bullets[k].y = zwY + 5;
	}
}


void bulletPool::updateBullet()
{
	static int count = 0;
	if (++count < 2) return;
	count = 0;
	int countMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < countMax; i++)
	{
		if (bullets[i].used)
		{
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH)
			{
				bullets[i].used = false;
			}

			//��ʵ���ӵ�����ײ���
			if (bullets[i].blast == true)
			{
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex >= 4)
				{
					bullets[i].frameIndex = 0;
					bullets[i].used = false;
				}
			}
		}
	}
}

void bulletPool::drawBullet()
{
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++)
	{

		if (bullets[i].used)
		{
			//�������
			if (bullets[i].blast)
			{
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];
				putimagePNG(bullets[i].x, bullets[i].y, img);
			}
			else
			{
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}
	}
}

void Sunshineball::updateSunshine()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if (balls[i].used)
		{
			balls[i].frameindex = (balls[i].frameindex + 1) % 29;//����ͼƬ+1
			if (balls[i].status == SUNSHINE_DOWN)//����½�
			{
				Sunshine* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1)//�Ƿ񵽴��յ�
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND)//���
			{
				balls[i].timer++;
				if (balls[i].timer >= 75)//�Ƿ�ʱ
				{
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT)//�ռ�
			{
				Sunshine* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);//�ƶ�
				if (sun->t >= 1)//�Ƿ񵽴��յ�
				{
					//sunCost += balls[i].val;//����ֵ++
					sunCost += 50 + 25 * (rand() % 3);
					sun->used = false;//����
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT)//����״̬
			{
				Sunshine* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);//���������߼��㵱ǰλ��
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;
					sun->timer = 0;
				}
			}
			else
			{
				continue;
			}
		}
	}
}

void Sunshineball::updateSunImg()
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if(balls[i].used)
		{
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, &sImg.sunImg[balls[i].frameindex]);
		}
	}
}

//��ʼ��
void Sunshineball::creatSunshine(PlantCard &plC)
{
	static int fre = 100;//Ƶ��
	static int sunCount = 0;//֡��
	sunCount++;
	if (sunCount >= fre)
	{
		//����Ƶ��
		fre = 100 + rand() % 400;
		sunCount = 0;

		//�����ȡ��ʹ�õ�����
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
			if (i >= ballMax) return;
		balls[i].used = true;
		balls[i].frameindex = 0;
		balls[i].timer = 0;
		
		balls[i].status = SUNSHINE_DOWN;//��ֱ����
		balls[i].t = 0;//��ʼʱ��Ϊ0
		balls[i].p1 = vector2(260 - 112 + rand() % (850 - (260 - 112)), 60);//���
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);//�յ�
		int off = 2;//�ƶ����ؾ���
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0f / (distance / off);//ÿ���ƶ����ٶ�
	}

	//���տ���������
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 5; i++)//�������տ�
	{
		for (int j = 0; j < 9; j++)
		{
			if (plC.map[i][j].type == XIANG_RI_KUI + 1)//�ҵ�
			{
				plC.map[i][j].timer++;
				if (plC.map[i][j].timer > 700)//�жϲ�������
				{
					plC.map[i][j].timer = 0;

					int k = 0;
					for (k; k < ballMax && balls[k].used; k++);
						if (k >= ballMax) return;//����ʹ��

					balls[k].used = true;
					balls[k].p1 = vector2(plC.map[i][j].x,plC.map[i][j].y);//���
					int w = (100 + rand() % 50) * (rand()%2 ? 1 : -1);//�趨ƫ��λ��
					balls[k].p4 = vector2(plC.map[i][j].x+w,
						plC.map[i][j].y + plC.imgPlant[XIANG_RI_KUI][0]->getheight()
					-sImg.sunImg[0].getheight());//�յ�
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);//·���ڵ�
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 100);//·���ڵ�
					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.05f;
					balls[k].t = 0;
				}
			}
		}
	}
}


void Sunshineball::collectSunshine(ExMessage* msg)
{
	int count = sizeof(balls) / sizeof(balls[0]);
	int w = sImg.sunImg[0].getwidth();
	int h = sImg.sunImg[0].getheight();
	for (int i = 0; i < count; i++)
	{
		if (balls[i].used)
		{
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;

			if (msg->x > x && msg->x < x + w && msg->y > y && msg->y < y + h)
			{
				balls[i].status = SUNSHINE_COLLECT;
				
				PlaySound("res/sunshine.wav", NULL,SND_FILENAME | SND_ASYNC);
				
				////���������Ծ·��

					balls[i].p1 = balls[i].pCur;//���
					balls[i].p4 = vector2(262, 0);//�յ�
					balls[i].t = 0;
					float distance = dis(balls[i].p1 - balls[i].p4);//��Ծ����
					float off = 14;//ÿ���ƶ������ؾ���
					balls[i].speed = 1.0f/(distance / off);//�ٶ�
					break;
			}
		}
	}
}


SunshineImg::SunshineImg()
{
	//��ʼ������ͼƬ
	char name[64];
	for (int i = 0; i < 29; i++)//��������ͼƬ
	{
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		ifstream ofs(name, ifstream::in);
		if (ofs.is_open())
		{
			loadimage(&sunImg[i], name);
			ofs.close();
		}
	}
}


void PlantCard::LoadPlantCard()
{
	memset(imgPlant, 0, sizeof(imgPlant[0]));
	memset(map, 0, sizeof(map));

	//��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < ZHI_WU_ZHONGLEI; i++)
	{
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 42; j++)//ֲ��仯ͼƬ����
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//�ж��ļ��Ƿ����
			ifstream ofs(name,ifstream::in);
			if (ofs.is_open())
			{
				imgPlant[i][j] = new IMAGE;
				loadimage(imgPlant[i][j], name);
				ofs.close();
			}
			else
			{
				break;
			}
		}
	}

}

void PlantCard::UpdatePlantCard()
{
	for (int i = 0; i < ZHI_WU_ZHONGLEI; i++)
	{
		int x = (338 - 112) + i * 65;
		int y = 6;
		putimage(x, y, &imgCards[i]);
	}
}

void PlantCard::updatePlant()
{
	void ChomperEat(int x, int y);
	static int count = 0;//��������
	 static int count_c = 0;
	 static int flag = 0;
	if (++count < 5) return;
	count = 0;
	for (int i = 0; i < 5; i++)//ˢ��ֲ��
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				if (map[i][j].type == HAN_BING_GU + 1 && map[i][j].timer < 100)
				{
					map[i][j].timer++;
					map[i][j].frameindex = (map[i][j].frameindex + 1) % 11;
				}
				else if (map[i][j].type == HAN_BING_GU + 1)
				{
					map[i][j].frameindex = (map[i][j].frameindex + 1) % 11 + 11;
				}
				if (map[i][j].type == TU_DOU_LEI + 1 && map[i][j].timer < 100)//�����׻���ʱ��
				{
					map[i][j].timer++;
					map[i][j].frameindex = 2 + (map[i][j].frameindex + 1) % 2;
				}
				else if (map[i][j].type == TU_DOU_LEI + 1)
				{
					map[i][j].frameindex = (map[i][j].frameindex + 1) % 2;
				}
				else if (map[i][j].type == JIAN_GUO + 1)
				{
					if (count_c++ > 2)
					{
						count_c = 0;
						if (map[i][j].blood > 600)
						{
							map[i][j].frameindex = (map[i][j].frameindex + 1) % 16;
						}
						else if (map[i][j].blood > 300)
						{
							map[i][j].frameindex = (map[i][j].frameindex + 1) % 11 + 16;
						}
						else
						{
							map[i][j].frameindex = (map[i][j].frameindex + 1) % 15 + 27;
						}
					}
					
				
				}
				else if (map[i][j].type == SHI_REN_HUA + 1)//ʳ�˻���ʼ
				{
					if (map[i][j].timer==0)
					{
						map[i][j].frameindex = (map[i][j].frameindex + 1) % 12;
					}
					else if (map[i][j].timer != 0)
					{
						map[i][j].timer++;
						map[i][j].frameindex = (map[i][j].frameindex + 1) % 6 + 21;
					}
			
				}
				else if (map[i][j].type == YING_TAO_ZHA_DAN + 1)//ӣ��ը����ʼ
				{
					if (count_c < 6&&flag==0)
					{
						count_c++;
					}
					else
					{
						
						if (count_c > 0)
						{
							count_c--;
						}
						else
						{
							flag = 0;
						}
					}
						map[i][j].frameindex = (map[i][j].frameindex + count_c) % 6;
				}
				

				else
					map[i][j].frameindex++;

				int PlantType = map[i][j].type - 1;
				int index = map[i][j].frameindex;
			
				
			 if (imgPlant[PlantType][index] == NULL)
				{
					map[i][j].frameindex = 0;
				}
			}
		}
	}
}

void PlantCard::drawPlant()
{
	//������ֲ��ֲ��
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (map[i][j].type > 0)
			{
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 14;
				int PlantType = map[i][j].type - 1;
				int index = map[i][j].frameindex;
				//putimagePNG(x, y, plC.imgPlant[PlantType][index]);
		
				
				 if (PlantType == SHI_REN_HUA &&chomper_eat==0)
				{
					 if (index >= 21)
					 {
						 PlaySound("res/usingAudio/chomper.wav", NULL, SND_FILENAME | SND_NOSTOP | SND_ASYNC);
					 }
					putimagePNG(map[i][j].x, map[i][j].y - 20, imgPlant[PlantType][index]);
					if (map[i][j].timer == 100)
					{
						map[i][j].timer = 0;
					}
				}
				 else if (PlantType == HUO_BAO_LA_JIAO)
				 {
					 
				 }

				else
				{
					putimagePNG(map[i][j].x, map[i][j].y, imgPlant[PlantType][index]);
				}
			}
		}
	}
}

IMAGE* PlantCard::getImg(int i)
{
	return &imgCards[i];
}


void GameInit::UpdateGame()
{
	if (zm.killCount >= ZM_MAX)
	{
		//EndBatchDraw();
		zm.GameStatus = WIN;
		return;
	}
	plC.updatePlant();

	ba.creatSunshine(plC);//��������
	ba.updateSunshine();

	zm.creatZombie();//������ʬ
	zm.updateZombie();


	//�ж��ӵ��Ƿ���Ҫ����
	pea.ShootCheck(zm,plC);

	//�����ӵ�
	pea.updateBullet();

	//ʵ���㶹�ӵ��ͽ�ʬ����ײ
	CollisionCheck();
}


void GameInit::UpdateImg()
{
	//��ʼ����
	BeginBatchDraw();

	//��Ⱦ����ͼƬ
	win.UpdateWindow(-112, 0);
	winbar.UpdateWindowPNG(250-112, 0);
	putimagePNG(250 - 112 + 612, 0, &ShovelImg[1]);
	putimagePNG(250 - 112 + 612 + 20, 17, &ShovelImg[0]);
	//��Ⱦֲ�￨��
	//plC.LoadPlantCard();
	plC.UpdatePlantCard();


	//������ֲ��ֲ��
	plC.drawPlant();

	//�����϶������е�ֲ��
	if (curPlant > 0)
	{
		IMAGE* img = plC.imgPlant[curPlant - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}
	//�����϶��еĲ���
	else if (curShvoel > 0)
	{
		putimagePNG(curX - ShovelImg[0].getwidth() / 2, curY - ShovelImg[0].getheight() / 2, &ShovelImg[0]);
	}

	//��ʾ����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunCost);
	outtextxy(276-112, 67, scoreText);

	//���ƽ�ʬ
	zm.drawZM();


	//�����ӵ�����
	pea.drawBullet();

	//�����������
	ba.updateSunImg();

	FlushBatchDraw();
	//����˫����
	EndBatchDraw();
}

//�û�����
void GameInit::userClick()
{
	ExMessage msg;
	static int index = -1;
	//static int status = 0;
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (msg.x > (338 - 112) && msg.x < (338 - 112) + 65 * ZHI_WU_ZHONGLEI && msg.y < 96 && sunCost >= ZwCost[(msg.x - (338 - 112))/65])
			{
				index = (msg.x - (338 - 112)) / 65;
				curX = msg.x;
				curY = msg.y;
				status = 1;
				curPlant = index + 1;
			}
			else if (msg.x > 770 && msg.x < 832 && msg.y > 17 && msg.y < 79.5)
			{
				curX = msg.x;
				curY = msg.y;
				curShvoel = 1;
				status = 2;
			}
			else if (msg.x > (338 - 112) && msg.x < (338 - 112) + 65 * ZHI_WU_ZHONGLEI && msg.y < 96)
			{
				index = -1;
				PlaySound("res/usingAudio/wrong.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			else
			{
				index = -1;
				ba.collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status)
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP && status == 2)
		{
			if (msg.x > 256 - 112 && msg.y > (179 - 102) && msg.y < (489 + 102))
			{
				int row = (msg.y - (179 - 102)) / 102;//��ֲλ��
				int col = (msg.x - (256 - 112)) / 81;

				if (plC.map[row][col].type > 0)//�Ƿ���Բ���
				{
					PlaySound("res/usingAudio/delete.wav", NULL, SND_FILENAME | SND_ASYNC);
					plC.map[row][col].type = 0;
					plC.map[row][col].catched = 0;
				}
			}

			status = 0;
			curShvoel = 0;
		}
		else if (msg.message == WM_LBUTTONUP && status == 1)
		{
			if (msg.x > 256 - 112 && msg.y > (179 - 102) && msg.y < (489 + 102))
			{
				int row = (msg.y - (179 - 102)) / 102;//��ֲλ��
				int col = (msg.x - (256 - 112)) / 81;
			
				if (plC.map[row][col].type == 0)//�Ƿ������ֲ
				{
					PlaySound("res/usingAudio/plant1.wav", NULL, SND_FILENAME | SND_ASYNC);
					sunCost -= ZwCost[index];
					index = -1;
					plC.map[row][col].type = curPlant;
					plC.map[row][col].frameindex = 0;
					
					if (plC.map[row][col].type == JIAN_GUO + 1)
						plC.map[row][col].blood = 1000;
					else
						plC.map[row][col].blood = 60;

					if (curPlant == WAN_DOU + 1)
						plC.map[row][col].timer = 70;
					else
						plC.map[row][col].timer = 0;

					plC.map[row][col].x = 256 - 112 + col * 81;
					plC.map[row][col].y = (179 - 102) + row * 102 + 14;
					plC.map[row][col].catched = false;
					plC.map[row][col].eated = false;
				}
			}
			
			status = 0;
			curPlant = 0;
		}
	}
}

bool GameInit::checkOver()
{
	if (zm.GameStatus == WIN)
	{
		loadimage(0, "res/EndPng/win2.png");
		mciSendString("play res/usingAudio/win.mp3", NULL, 0, NULL);
		Sleep(4000);
		return true;
	}
	else if (zm.GameStatus == FAIL)
	{
		loadimage(0, "res/EndPng/fail2.png");
		mciSendString("play res/usingAudio/lose.mp3", NULL, 0, NULL);
		Sleep(6000);
		return true;
	}
	else
		return false;
}


//��Ϸ��ʼ
void GameInit::gameInit1()
{
	win.LoadWimage("res/Map/map0.jpg");
	winbar.LoadWimage("res/bar5.png");
	loadimage(&ShovelImg[0], "res/Shovel.png", 58, 62.5);
	loadimage(&ShovelImg[1], "res/ShovelBank.png",99,102);
//	plC.LoadPlantCard();

	//�����������
	srand((unsigned int)time(NULL));

	//������Ϸ����
	curPlant = 0;
	win.MakeWindow(WIN_WIDTH,WIN_HEIGHT);

	//��ʼ��Ѱ��Ƭͷ��ʬפ������
	char name[64];
	for (int i = 0; i < 11; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		win.LoadZSimage(i, name);
	}

	//ƬͷѲ��
	win.viewSence();
	barsDown();

	//��������
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy_s(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//�����
	settextstyle(&f);
	setbkmode(TRANSPARENT);//�������屳��ɫ
	setcolor(BLACK);

	//ѭ�������ʱ��
	int timer = 0;

	//��������
	mciSendString("play res/usingAudio/Mountains.mp3 repeat", NULL, 0, NULL);

	while (1)
	{
		userClick();
		UpdateImg();

		timer += getDelay();//������ε���ѭ����ʱ���
		if (timer <= 15)//����ʱ�����٣���ˢ��ֲ��ͼƬ
		{
			continue;
		}

		timer = 0;
		UpdateGame();
		if (checkOver())
			break;
	}


}

void GameInit::barsDown()
{
	int height = winbar.getheight();
	BeginBatchDraw();
	for (int y = -height; y <= 0; y++)
	{
		
		win.UpdateWindow(-112,0);
		winbar.UpdateWindowPNG(250-112, y);
		putimagePNG(250 - 112 + 612, y, &ShovelImg[1]);
		putimagePNG(250 - 112 + 612 + 20, 17 + y, &ShovelImg[0]);

		for (int i = 0; i < ZHI_WU_ZHONGLEI; i++)
		{
			int x = (338 - 112) + i * 65;
			putimagePNG(x, 6 + y, plC.getImg(i));
		}

		FlushBatchDraw();
		Sleep(10);
	}
	EndBatchDraw();
}

void GameInit::startUI(int *type)
{
	Window bg, menu1, menu2,menu3,menu4,menu5;
	bg.LoadWimage("res/menu.png");
	menu1.LoadWimage("res/menu1.png");
	menu2.LoadWimage("res/menu2.png");
	menu3.LoadWimage("wait");
	menu4.LoadWimage("wait");
	menu5.LoadWimage("res/quit.png");


	bg.MakeWindow(WIN_WIDTH, WIN_HEIGHT);
	
	//ѡ����
	int flag = 0;

	//ˢ��ѭ����ʾͼƬ
	while (true)
	{
		BeginBatchDraw();
		bg.UpdateWindow();
		
		if (flag == 1)
			menu2.UpdateWindowPNG(474,75);
		else
			menu1.UpdateWindowPNG(474,75);

		if (flag == 2)
			menu4.UpdateWindowPNG(474,75+200);
		else
			menu3.UpdateWindowPNG(474,75+200);
		menu5.UpdateWindowPNG(800,480);
		

		ExMessage msg;
		if (peekmessage(&msg))
		{
			//�жϿ�ʼ����
			if (msg.message == WM_LBUTTONDOWN && msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 140)
			{
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP && flag == 1)
			{
				*type = 1;
				EndBatchDraw();
				return;
			}
			//�ж��˳�ϵͳ����
			else if (msg.message == WM_LBUTTONDOWN && msg.x > 800 && msg.x < 860 && msg.y > 480 && msg.y < 540)
			{
				flag = 2;
			}
			else if (msg.message == WM_LBUTTONUP && flag == 2)
			{
				*type = 2;
				EndBatchDraw();
				return;
			}
		}
		EndBatchDraw();
	}

}
