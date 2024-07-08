#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
#include"processmanager.h"
  //������ɫ
void setColor(unsigned short ForeColor = 7, unsigned short BackGroundColor = 0)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��ǰ���ھ��
	SetConsoleTextAttribute(handle, ForeColor + BackGroundColor * 0x10);//������ɫ
}
//����XY������
void setxy(int x, int y)
{
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//���ع��x
int getX()
{
	HANDLE gh_std_out;
	gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);
	return bInfo.dwCursorPosition.X;
}
//���ع��y
int getY()
{
	HANDLE gh_std_out;
	gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo(gh_std_out, &bInfo);
	return bInfo.dwCursorPosition.Y;
}
//pcb���ƣ�node���Ƶ�x
void copy_pcb(pcb* node, pcb* x)
{
	x->arrivetime = node->arrivetime;
	x->end = node->end;
	x->name = node->name;
	x->priority = node->priority;
	x->pturn = node->pturn;
	x->runtime = node->runtime;
	x->start = node->start;
	x->state = node->state;
	x->turnaroundtime = node->turnaroundtime;
	x->restime = node->restime;
	x->remain = node->remain;
	x->slice = node->slice;
	x->remain_slice = node->remain_slice;
}
//���
void push(queue* q, pcb* node)
{
	pcb* x = (pcb*)malloc(sizeof(pcb));
	copy_pcb(node, x);
	x->next = NULL;
	if (q->rear == NULL) {
		q->front = q->rear = x;
	}
	else {
		q->rear->next = x;
		q->rear = x;
	}
}
//��ʼ��pcb����
void init_pcb(pcb* p)
{
	p->state = 'f';//δִ����״̬
	p->name = (char*)malloc(sizeof(char) * 50);//Ϊ�ַ�������ռ�
	memset(p->name, '\0', sizeof(char) * 50);
}
//�ֶ����뺯��
void manually_entered(int pro_num, pcb* head)
{
	pcb* p = NULL, * tail;
	int i;
	tail = head;
	for (i = 0; i < pro_num; i++)
	{
		p = (pcb*)malloc(sizeof(pcb));
		init_pcb(p);
		printf("%d:���������:", i);
		scanf("%s", p->name);
		//����ʱ��
		printf("\t����ʱ��:");
		scanf("%d", &p->arrivetime);
		//����ʱ��
		printf("\t����ʱ��:");
		scanf("%d", &p->runtime);
		//���ȼ�
		printf("\t���ȼ�:");
		scanf("%d", &p->priority);
		p->remain = p->runtime;
		p->start = -1;
		tail->next = p;
		p->next = NULL;
		tail = p;
	}
}
//��ȡ�ļ�
void read_file(int pro_num, pcb* head)
{
	pcb* p = NULL, * tail;
	tail = head;
	int i;
	char ch;
	FILE* fp;
	if ((fp = fopen("process.txt", "r")) == NULL)
	{
		printf("OPEN FAILED!\n");
		return;
	}
	for (i = 0; i < pro_num; i++)
	{
		p = (pcb*)malloc(sizeof(pcb));
		init_pcb(p);
		//����ʱ�� ����ʱ�� ���ȼ�
		fscanf(fp, "%s %d %d %d", p->name, &p->arrivetime, &p->runtime, &p->priority);
		//���뻻�з�
		fscanf(fp, "%c", &ch);
		p->remain = p->runtime;
		p->start = -1;
		tail->next = p;
		p->next = NULL;
		tail = p;
	}
}
//�ͷ�����Ŀռ�
void free_pcb(pcb* head)
{
	pcb* tmp = NULL, * tmpnext = NULL;
	tmp = head->next;
	while (tmp)
	{
		tmpnext = tmp->next;
		free(tmp);
		tmp = tmpnext;
	}
	free(head);
}
//����������㷨�˵�����
int menu(pcb* head)
{
	int n;
	printf("****************����������㷨********************\n");
	printf("\t1->�����ȷ���\n");
	printf("\t2->����ҵ����\n");
	printf("\t3->���ʣ��ʱ������\n");
	printf("\t4->ʱ��Ƭ��ת\n");
	printf("\t5->���ھ�̬���ȼ��ĵ���(����ռʽ)\n");
	printf("\t6->���ھ�̬���ȼ��ĵ���(��ռʽ)\n");
	printf("\t7->���ڸ���Ӧ�����ȵĶ�̬���ȼ�����\n");
	printf("\t8->�༶�������е���\n");
	printf("\t0->�˳�\n");
	printf("*************************************************\n");
	simple_prin(head);
	fflush(stdin);
	printf("ѡ���㷨:");
	scanf("%d", &n);
	return n;
}
//�򵥵Ĵ�ӡ�۲�������
void simple_prin(pcb* head)
{
	pcb* tmp = NULL;
	tmp = head->next;
	printf("\t----------���̿��ƿ�-------------\n");
	printf("\t������\t|����ʱ��|����ʱ��|���ȼ�\n");
	while (tmp != NULL)
	{
		printf("\t---------------------------------\n");
		printf("\t%4s\t|%4d\t |%4d\t  |%4d\n", tmp->name, tmp->arrivetime, tmp->runtime, tmp->priority);
		tmp = tmp->next;
	}
	printf("\t---------------------------------\n");
}
//����pcb��ֵ
void change_pcb(pcb* p1, pcb* p2)
{
	double tmp_dou;
	int tmp_int;
	char* tmp_str, tmp_char;
	tmp_str = (char*)malloc(sizeof(char) * 50);
	//��������ʱ��
	tmp_int = p1->arrivetime;
	p1->arrivetime = p2->arrivetime;
	p2->arrivetime = tmp_int;
	//��������ʱ��
	tmp_int = p1->end;
	p1->end = p2->end;
	p2->end = tmp_int;
	//�������ȼ�
	tmp_int = p1->priority;
	p1->priority = p2->priority;
	p2->priority = tmp_int;
	//��������ʱ��
	tmp_int = p1->runtime;
	p1->runtime = p2->runtime;
	p2->runtime = tmp_int;
	//������ʼʱ��
	tmp_int = p1->start;
	p1->start = p2->start;
	p2->start = tmp_int;
	//������תʱ��
	tmp_int = p1->turnaroundtime;
	p1->turnaroundtime = p2->turnaroundtime;
	p2->turnaroundtime = tmp_int;
	//������Ȩ��תʱ��
	tmp_dou = p1->pturn;
	p1->pturn = p2->pturn;
	p2->pturn = tmp_dou;
	//����״̬
	tmp_char = p1->state;
	p1->state = p2->state;
	p2->state = tmp_char;
	//��������
	tmp_str = p1->name;
	p1->name = p2->name;
	p2->name = tmp_str;
	//������Ӧʱ��
	tmp_int = p1->restime;
	p1->restime = p2->restime;
	p2->restime = tmp_int;
	//����ʣ��ʱ��
	tmp_int = p1->remain;
	p1->remain = p2->remain;
	p2->remain = tmp_int;
	//����ʱ��Ƭ
	tmp_int = p1->slice;
	p1->slice = p2->slice;
	p2->slice = tmp_int;
}
//���ݵ���ʱ���С��������
void sort_by_arrivetime(pcb* head)
{
	pcb* p, * q, * min;
	p = head->next;
	while (p != NULL)
	{
		min = p;
		q = p->next;
		while (q != NULL)
		{
			if (min->arrivetime > q->arrivetime)
				min = q;
			q = q->next;
		}
		// ����ֵ
		if (min != p)
		{
			change_pcb(min, p);
		}
		p = p->next;
	}
}
//��ӡ�������й���
void prin_head(pcb* head, int pro_num)
{
	double turnall = 0, pturnall = 0;
	pcb* tmp;
	printf("\t---------------------------���̿��ƿ�-----------------------------\n");
	printf("\t������\t|����ʱ��|��ʼʱ��|����ʱ��|����ʱ��|��תʱ��|��Ȩ��תʱ��\n");
	tmp = head->next;
	//���в���
	while (tmp != NULL)
	{
		printf("\t------------------------------------------------------------------\n");
		printf("\t%4s\t|%4d\t |%4d\t  |%4d\t   |%4d    |%4d    |%8.2lf\n", tmp->name, tmp->arrivetime, tmp->start, tmp->runtime, tmp->end, tmp->turnaroundtime, tmp->pturn);
		turnall += tmp->turnaroundtime;
		pturnall += tmp->pturn;
		tmp = tmp->next;
	}
	printf("\t------------------------------------------------------------------\n");
	//��ӡƽ����תʱ�䡢ƽ����Ȩ��תʱ��
	printf("\tƽ����תʱ��Ϊ%.2lfs\n", turnall / pro_num);
	printf("\tƽ����Ȩ��תʱ��Ϊ%.2lfs\n", pturnall / pro_num);
	printf("\t------------------------------------------------------------------\n");
}
//��ʼ�����󣬼�¼�����ڸ����Ƿ����е����
void init_matrix(int matrix[MAX][MAX], int pro_num)
{
	int i, j;
	for (i = 0; i < pro_num; i++)
	{
		for (j = 0; j < MAX; j++)
		{
			matrix[i][j] = 0;
		}
	}
}
void prin_move(int matrix[][MAX], int pro_num, int endtime, pcb* head)
{
	int i, j, p[MAX] = { 0 }, x, y, y0, flag;
	pcb* tmp;
	tmp = head->next;
	for (i = 0; i < endtime; i++)
	{
		printf("--------");
	}
	printf("\n  ");
	for (i = 0; i < endtime; i++)
		printf("| %3d\t", i + 1);
	printf("\n");
	for (i = 0; i < endtime; i++)
	{
		printf("--------");
	}
	printf("\n");
	while (tmp)
	{
		printf("%s: ", tmp->name);
		tmp = tmp->next;
		printf("\n\n");
	}
	x = getX();
	y = getY();
	y0 = y;
	x = x + 2;
	setxy(x, y);
	for (i = 0; i < endtime; i++)
	{
		flag = 0;
		for (j = 0; j < pro_num; j++)
		{
			if (matrix[j][i] == 1)
			{
				x = getX();
				y = y0 - 2 * (pro_num - j);
				setxy(x, y);
				setColor(7, j + 1);
				printf("\t");
				Sleep(1000);
				setColor(7, 0);
				flag = 1;
				break;
			}
		}
		if (!flag)
			printf("\t");
	}
	printf("\n\n\n");
	for (i = 0; i < endtime; i++)
	{
		printf("--------");
	}
	printf("\n");
}
//�ж�����״̬�Ƿ���t
int all_t(pcb* head)
{
	pcb* tmp;
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->state != 't')
			return 0;
		tmp = tmp->next;
	}
	return 1;
}
//�ҵ������еȴ�̬����С��ҵʱ�����
pcb* min_runtime_pcb(pcb* head)
{
	int min = 9999;
	pcb* tmp = NULL, * min_tmp = NULL;
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->runtime < min && tmp->state == 'w')
		{
			min = tmp->runtime;
			min_tmp = tmp;
		}
		tmp = tmp->next;
	}
	return min_tmp;
}
//�ҵ������еȴ�̬����С��ҵʱ�����
pcb* min_priority_pcb(pcb* head)
{
	int min = 9999;
	pcb* tmp = NULL, * min_tmp = NULL;
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->priority < min && tmp->state == 'w')
		{
			min = tmp->priority;
			min_tmp = tmp;
		}
		tmp = tmp->next;
	}
	return min_tmp;
}
//�ҵ������еȴ�̬�����ʣ��ʱ��Ľ���
pcb* min_remain_pcb(pcb* head)
{
	int min = 9999;
	pcb* tmp = NULL, * min_tmp = NULL;
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->remain < min && tmp->state == 'w')
		{
			min = tmp->remain;
			min_tmp = tmp;
		}
		tmp = tmp->next;
	}
	return min_tmp;
}
//�ҵ������еȴ�̬�������Ӧ�ȵĽ���
pcb* max_restime_pcb(pcb* head, int time)
{
	int max = 0;
	pcb* tmp = NULL, * max_tmp = NULL;
	tmp = head->next;
	while (tmp != NULL)
	{
		tmp->restime = (time - tmp->arrivetime + tmp->runtime) / tmp->runtime;
		if (tmp->restime > max && tmp->state == 'w')
		{
			max = tmp->restime;
			max_tmp = tmp;
		}
		tmp = tmp->next;
	}
	return max_tmp;
}
//��Сʱ��Ƭ
int min_slice(pcb* head)
{
	int min = 9999;
	pcb* tmp = NULL;
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->slice < min && tmp->state == 'w')
		{
			min = tmp->slice;
		}
		tmp = tmp->next;
	}
	return min;
}
//�ҵ������еȴ�̬����Сʱ��Ƭ����С����ʱ��Ľ���
pcb* min_slice_arrivetime(pcb* head)
{
	int min = 9999, minslice;
	pcb* tmp = NULL, * min_tmp = NULL;
	tmp = head->next;
	minslice = min_slice(head);
	while (tmp != NULL)
	{
		if (tmp->arrivetime < min && tmp->state == 'w' && tmp->slice == minslice)
		{
			min = tmp->arrivetime;
			min_tmp = tmp;
		}
		tmp = tmp->next;
	}
	return min_tmp;
}
//���ص���ʱ���С�Ľ���
pcb* min_pcb(pcb* x1, pcb* x2)
{
	if (x1 == NULL)
		return x2;
	else if (x2 == NULL)
		return x1;
	else if (x1->arrivetime < x2->arrivetime)
		return x1;
	else
		return x2;
}
//����ת����
int name_to_int(pcb* head, pcb* x)
{
	int i = 0;
	pcb* tmp;
	tmp = head->next;
	while (tmp)
	{
		if (strcmp(tmp->name, x->name) == 0)
			return i;
		tmp = tmp->next;
		i++;
	}
	return 0;
}
//����head��Ϊf
void set_all_f(pcb* head)
{
	pcb* tmp;
	tmp = head->next;
	while (tmp)
	{
		tmp->state = 'f';
		tmp->start = -1;
		tmp->remain = tmp->runtime;
		tmp = tmp->next;
	}
}
//�����ȷ���
void FCFS(pcb* head, int pro_num)
{
	int time = 0, i, j = 0;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp;
	sort_by_arrivetime(head);
	tmp = head->next;
	while (tmp != NULL)
	{
		if (tmp->arrivetime > time)
		{
			time++;
			continue;
		}
		else
		{
			tmp->start = time;
			tmp->end = tmp->start + tmp->runtime;
			tmp->turnaroundtime = tmp->end - tmp->arrivetime;
			tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			tmp->state = 't';
			for (i = time; i < time + tmp->runtime; i++)
				proMatrix[j][i] = 1;
			j++;
			time += tmp->runtime;
			tmp = tmp->next;
		}
	}
	printf("�����ȷ���\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}
//����ҵ����
void SJF(pcb* head, int pro_num)
{
	int time = 0, i;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp = NULL, * tmp2 = NULL, * tmp3 = NULL;
	tmp = head->next;
	tmp2 = tmp;
	//������ʱ����������
	sort_by_arrivetime(head);
	//���еĽ���û�ж�ִ����
	while (!all_t(head))
	{
		//�ҵ���С����ʱ��Ľ���
		tmp3 = min_runtime_pcb(head);
		//�����ǰʱ��δ�ﵽ������һ�����̵Ŀ�ʼʱ�䣬��ʱ���һ
		if (min_pcb(tmp, tmp3)->arrivetime > time)
		{
			time++;
			continue;
		}
		else if (tmp3 != NULL)
		{
			//���еȴ�̬�Ľ���
			while (tmp3 != NULL)
			{
				tmp3->start = time;
				tmp3->end = tmp3->start + tmp3->runtime;
				tmp3->state = 't';
				tmp3->turnaroundtime = tmp3->end - tmp3->arrivetime;
				tmp3->pturn = (double)tmp3->turnaroundtime / tmp3->runtime;
				for (i = time; i < time + tmp3->runtime; i++)
					proMatrix[name_to_int(head, tmp3)][i] = 1;
				time += tmp3->runtime;
				tmp2 = tmp;
				//���й����е���Ľ������
				while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
				{
					tmp2->state = 'w';//��Ϊ�ȴ�̬
					tmp2 = tmp2->next;
				}
				//�ҵ���С����ʱ��Ľ���
				tmp3 = min_runtime_pcb(head);
			}
			tmp = tmp2;
		}
		else
		{
			tmp->start = time;
			tmp->end = tmp->start + tmp->runtime;
			tmp->state = 't';
			tmp->turnaroundtime = tmp->end - tmp->arrivetime;
			tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			for (i = time; i < time + tmp->runtime; i++)
				proMatrix[name_to_int(head, tmp)][i] = 1;
			time += tmp->runtime;
			tmp2 = tmp->next;
			//���й����е���Ľ������
			while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
			{
				tmp2->state = 'w';//��Ϊ�ȴ�̬
				tmp2 = tmp2->next;
			}
			tmp = tmp2;
		}
	}
	printf("����ҵ����\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}
//���ʣ��ʱ������
void SRT(pcb* head, int pro_num)
{
	int time = 0;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp = NULL, * tmp2 = NULL, * tmp3 = NULL;
	tmp = head->next;
	tmp2 = tmp;
	//������ʱ����������
	sort_by_arrivetime(head);
	//���еĽ���û�ж�ִ����
	while (!all_t(head))
	{
		//�ҵ����ʣ��ʱ��Ľ���
		tmp3 = min_remain_pcb(head);
		//�����ǰʱ��δ�ﵽ������һ�����̵Ŀ�ʼʱ�䣬��ʱ���һ
		if (min_pcb(tmp, tmp3)->arrivetime > time)
		{
			time++;
			continue;
		}
		else if (tmp3 != NULL)
		{
			//���еȴ�̬�Ľ���
			while (tmp3 != NULL)
			{
				if (tmp3->state == 'f' || tmp3->start == -1)
				{
					tmp3->start = time;
					tmp3->state = 'w';
				}
				tmp3->remain -= 1;
				if (tmp3->remain == 0)
				{
					tmp3->end = time + 1;
					tmp3->state = 't';
					tmp3->turnaroundtime = tmp3->end - tmp3->arrivetime;
					tmp3->pturn = (double)tmp3->turnaroundtime / tmp3->runtime;
				}
				proMatrix[name_to_int(head, tmp3)][time] = 1;
				time++;
				tmp2 = tmp;
				//���й����е���Ľ������
				while (tmp2 != NULL)
				{
					if (tmp2->arrivetime <= time && tmp2->state == 'f')
					{
						tmp2->state = 'w';//��Ϊ�ȴ�̬
					}
					tmp2 = tmp2->next;
				}
				//�ҵ����ʣ��ʱ��Ľ���
				tmp3 = min_remain_pcb(head);
			}
			tmp = tmp2;
		}
		else
		{
			if (tmp->state == 'f' || tmp->start == -1)
			{
				tmp->start = time;
				tmp->state = 'w';
			}
			tmp->remain -= 1;
			if (tmp->remain == 0)
			{
				tmp->end = time + 1;
				tmp->state = 't';
				tmp->turnaroundtime = tmp->end - tmp->arrivetime;
				tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			}
			proMatrix[name_to_int(head, tmp)][time] = 1;
			time++;
			tmp2 = tmp->next;
			//���й����е���Ľ������
			while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
			{
				tmp2->state = 'w';//��Ϊ�ȴ�̬
				tmp2 = tmp2->next;
			}
			tmp = tmp2;
		}
	}
	printf("���ʣ��ʱ������\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}
//���ھ�̬���ȼ��ĵ���(��ռʽ)
void PSA_grab(pcb* head, int pro_num)
{
	int time = 0;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp = NULL, * tmp2 = NULL, * tmp3 = NULL;
	tmp = head->next;
	tmp2 = tmp;
	//������ʱ����������
	sort_by_arrivetime(head);
	//���еĽ���û�ж�ִ����
	while (!all_t(head))
	{
		//�ҵ�������ȼ��Ľ���
		tmp3 = min_priority_pcb(head);
		//�����ǰʱ��δ�ﵽ������һ�����̵Ŀ�ʼʱ�䣬��ʱ���һ
		if (min_pcb(tmp, tmp3)->arrivetime > time)
		{
			time++;
			continue;
		}
		else if (tmp3 != NULL)
		{
			//���еȴ�̬�Ľ���
			while (tmp3 != NULL)
			{
				if (tmp3->state == 'f' || tmp3->start == -1)
				{
					tmp3->start = time;
					tmp3->state = 'w';
				}
				tmp3->remain -= 1;
				if (tmp3->remain == 0)
				{
					tmp3->end = time + 1;
					tmp3->state = 't';
					tmp3->turnaroundtime = tmp3->end - tmp3->arrivetime;
					tmp3->pturn = (double)tmp3->turnaroundtime / tmp3->runtime;
				}
				proMatrix[name_to_int(head, tmp3)][time] = 1;
				time++;
				tmp2 = tmp;
				//���й����е���Ľ������
				while (tmp2 != NULL)
				{
					if (tmp2->arrivetime <= time && tmp2->state == 'f')
					{
						tmp2->state = 'w';//��Ϊ�ȴ�̬
					}
					tmp2 = tmp2->next;
				}
				//�ҵ���С����ʱ��Ľ���
				tmp3 = min_priority_pcb(head);
			}
			tmp = tmp2;
		}
		else
		{
			if (tmp->state == 'f' || tmp->start == -1)
			{
				tmp->start = time;
				tmp->state = 'w';
			}
			tmp->remain -= 1;
			if (tmp->remain == 0)
			{
				tmp->end = time + 1;
				tmp->state = 't';
				tmp->turnaroundtime = tmp->end - tmp->arrivetime;
				tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			}
			proMatrix[name_to_int(head, tmp)][time] = 1;
			time++;
			tmp2 = tmp->next;
			//���й����е���Ľ������
			while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
			{
				tmp2->state = 'w';//��Ϊ�ȴ�̬
				tmp2 = tmp2->next;
			}
			tmp = tmp2;
		}
	}
	printf("���ھ�̬���ȼ��ĵ���(��ռʽ)\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}
//���ھ�̬���ȼ��ĵ���(����ռʽ)
void PSA(pcb* head, int pro_num)
{
	int time = 0, i;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp = NULL, * tmp2 = NULL, * tmp3 = NULL;
	tmp = head->next;
	tmp2 = tmp;
	sort_by_arrivetime(head);
	while (!all_t(head))
	{
		//�ҵ�������ȼ��Ľ���
		tmp3 = min_priority_pcb(head);
		if (min_pcb(tmp, tmp3)->arrivetime > time)
		{
			time++;
			continue;
		}
		else if (tmp3 != NULL)
		{
			while (tmp3 != NULL)
			{
				tmp3->start = time;
				tmp3->end = tmp3->start + tmp3->runtime;
				tmp3->state = 't';
				tmp3->turnaroundtime = tmp3->end - tmp3->arrivetime;
				tmp3->pturn = (double)tmp3->turnaroundtime / tmp3->runtime;
				tmp3->remain = 0;
				for (i = time; i < time + tmp3->runtime; i++)
					proMatrix[name_to_int(head, tmp3)][i] = 1;
				time += tmp3->runtime;
				tmp2 = tmp;
				while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
				{
					tmp2->state = 'w';//��Ϊ�ȴ�̬
					tmp2 = tmp2->next;
				}
				//�ҵ�������ȼ��Ľ���
				tmp3 = min_priority_pcb(head);
			}
			tmp = tmp2;
		}
		else
		{
			tmp->start = time;
			tmp->end = tmp->start + tmp->runtime;
			tmp->state = 't';
			tmp->turnaroundtime = tmp->end - tmp->arrivetime;
			tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			tmp->remain = 0;
			for (i = time; i < time + tmp->runtime; i++)
				proMatrix[name_to_int(head, tmp)][i] = 1;
			time += tmp->runtime;
			tmp2 = tmp->next;
			while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
			{
				tmp2->state = 'w';//��Ϊ�ȴ�̬
				tmp2 = tmp2->next;
			}
			tmp = tmp2;
		}
	}
	printf("���ھ�̬���ȼ��ĵ���(����ռʽ)\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}
//���ڸ���Ӧ�����ȵĶ�̬���ȼ�����
void HRRN(pcb* head, int pro_num)
{
	int time = 0, i;
	int proMatrix[MAX][MAX] = { 0 };
	pcb* tmp = NULL, * tmp2 = NULL, * tmp3 = NULL;
	tmp = head->next;
	tmp2 = tmp;
	sort_by_arrivetime(head);
	while (!all_t(head))
	{
		//�ҵ������Ӧ�ȵĽ���
		tmp3 = max_restime_pcb(head, time);
		if (min_pcb(tmp, tmp3)->arrivetime > time)
		{
			time++;
			continue;
		}
		else if (tmp3 != NULL)
		{
			while (tmp3 != NULL)
			{
				tmp3->start = time;
				tmp3->end = tmp3->start + tmp3->runtime;
				tmp3->state = 't';
				tmp3->turnaroundtime = tmp3->end - tmp3->arrivetime;
				tmp3->pturn = (double)tmp3->turnaroundtime / tmp3->runtime;
				tmp3->restime = 1;
				tmp3->remain = 0;
				for (i = time; i < time + tmp3->runtime; i++)
					proMatrix[name_to_int(head, tmp3)][i] = 1;
				time += tmp3->runtime;
				tmp2 = tmp;
				while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
				{
					tmp2->state = 'w';//��Ϊ�ȴ�̬
					tmp2 = tmp2->next;
				}
				//�ҵ������Ӧ�ȵĽ���
				tmp3 = max_restime_pcb(head, time);
			}
			tmp = tmp2;
		}
		else
		{
			tmp->start = time;
			tmp->end = tmp->start + tmp->runtime;
			tmp->state = 't';
			tmp->turnaroundtime = tmp->end - tmp->arrivetime;
			tmp->pturn = (double)tmp->turnaroundtime / tmp->runtime;
			tmp->restime = 1;
			tmp->remain = 0;
			for (i = time; i < time + tmp->runtime; i++)
				proMatrix[name_to_int(head, tmp)][i] = 1;
			time += tmp->runtime;
			tmp2 = tmp->next;
			while (tmp2 != NULL && tmp2->arrivetime <= time && tmp2->state == 'f')
			{
				tmp2->state = 'w';//��Ϊ�ȴ�̬
				tmp2 = tmp2->next;
			}
			tmp = tmp2;
		}
	}
	printf("���ڸ���Ӧ�����ȵĶ�̬���ȼ�����\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}

//ʱ��Ƭ��ת
void TIMEROUND(pcb* head, int pro_num) {
	pcb* tmp;
	pcb* executing = NULL;
	int time_slice, time = 0;
	int proMatrix[MAX][MAX] = { 0 };
	queue ready_queue; // ��������

	// ��ʼ��
	ready_queue.front = ready_queue.rear = NULL;

	// ����ʱ��Ƭ����
	printf("ʱ��Ƭ����Ϊ��");
	scanf("%d", &time_slice);

	// �����н��̰�����ʱ������
	sort_by_arrivetime(head);
	while (!all_t(head)) {
		// ���µ���Ľ��̼����������
		tmp = head->next;
		while (tmp != NULL) {
			if (tmp->arrivetime == time && tmp->state == 'f') {
				tmp->state = 'w';
				push(&ready_queue, tmp);
			}
			tmp = tmp->next;
		}

		// �������ִ�еĽ����Ƿ����
		if (executing != NULL) {
			if (executing->remain == 0) {
				executing->state = 't';
				executing->end = time;
				executing->turnaroundtime = executing->end - executing->arrivetime;
				executing->pturn = (double)executing->turnaroundtime / executing->runtime;
				//ͬ����ֵ
				tmp = head->next;
				while (tmp != NULL)
				{
					if (strcmp(tmp->name, executing->name) == 0)
					{
						tmp->state = executing->state;
						tmp->end = executing->end;
						tmp->start = executing->start;
						tmp->turnaroundtime = executing->turnaroundtime;
						tmp->pturn = executing->pturn;
					}
					tmp = tmp->next;
				}
				executing = NULL;
			}
			else if (executing->remain_slice == 0) {
				push(&ready_queue, executing);
				executing = NULL;
			}
		}

		// ѡ��Ҫִ�еĽ���
		if (executing == NULL && ready_queue.front != NULL) {
			executing = ready_queue.front;
			executing->remain_slice = time_slice;
			ready_queue.front = ready_queue.front->next;
			if (ready_queue.front == NULL)
			{
				ready_queue.rear = NULL;
			}
			if (executing->start == -1) {
				executing->start = time;
			}
		}

		// ִ�н���
		if (executing != NULL) {
			executing->remain--;
			executing->remain_slice--;
			proMatrix[name_to_int(head, executing)][time] = 1;
		}

		time++;
	}

	printf("ʱ��Ƭ��ת\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);
}

//�༶��������
void FEEDBACK(pcb* head, int pro_num)
{
	int time = 0;
	int time_slice[3] = { 2, 4, 8 }; // �������е�ʱ��Ƭ���ȣ����μӱ�
	int proMatrix[MAX][MAX] = { 0 };
	queue queues[3]; // �������ȼ�����
	pcb* tmp = head->next;
	pcb* executing = NULL; // ��ǰ����ִ�еĽ���
	int i, j;

	// ��ʼ����������
	for (i = 0; i < 3; i++) {
		queues[i].front = queues[i].rear = NULL;
	}

	// �����н��̰�����ʱ������
	sort_by_arrivetime(head);

	while (!all_t(head)) {
		// ����µ���Ľ��̣������һ������
		tmp = head->next;
		while (tmp != NULL) {
			if (tmp->arrivetime == time && tmp->state == 'f') {
				tmp->state = 'w';
				tmp->slice = 0;
				push(&queues[0], tmp);
			}
			tmp = tmp->next;
		}

		// �������ִ�еĽ����Ƿ����
		if (executing != NULL && executing->remain == 0) {
			executing->state = 't';
			executing->end = time;
			executing->turnaroundtime = executing->end - executing->arrivetime;
			executing->pturn = (double)executing->turnaroundtime / executing->runtime;
			//ͬ����ֵ
			tmp = head->next;
			while (tmp != NULL)
			{
				if (strcmp(tmp->name, executing->name) == 0)
				{
					tmp->state = executing->state;
					tmp->end = executing->end;
					tmp->start = executing->start;
					tmp->turnaroundtime = executing->turnaroundtime;
					tmp->pturn = executing->pturn;
				}
				tmp = tmp->next;
			}
			executing = NULL;
		}
		else if (executing != NULL && executing->remain != 0 && executing->remain_slice == 0)
		{
			push(&queues[((++executing->slice)>=2)?2:executing->slice], executing);
			executing = NULL;
		}

		// �����ȼ�ѡ��Ҫִ�еĽ���
		if (executing == NULL)
		{
			for (i = 0; i < 3; i++) {
				if (queues[i].front != NULL) {
					executing = queues[i].front;
					executing->slice = i;
					executing->remain_slice=time_slice[i];
					queues[i].front = executing->next;
					if (queues[i].front == NULL) {
						queues[i].rear = NULL;
					}
					break;
				}
			}
		}
		else//�������ȼ����ߵĽ�������
		{
			for (i = 0; i < 3; i++) {
				if (queues[i].front != NULL&&i<executing->slice) {
					push(&queues[executing->slice], executing);
					executing = queues[i].front;
					executing->slice = i;
					executing->remain_slice = time_slice[i];
					queues[i].front = executing->next;
					if (queues[i].front == NULL) {
						queues[i].rear = NULL;
					}
					break;
				}
			}
		}

		// ִ�н���
		if (executing != NULL) {
			if (executing->start == -1) {
				executing->start = time;
			}
			executing->remain--;
			executing->remain_slice--;
			proMatrix[name_to_int(head, executing)][time] = 1;

		}

		time++;
	}

	printf("�༶�������е���\n");
	prin_move(proMatrix, pro_num, time, head);
	prin_head(head, pro_num);

}


//�������к���
int promenu()
{
	pcb* head = NULL;
	int pro_num, i = 1;
	head = (pcb*)malloc(sizeof(pcb));
	head->next = NULL;
	while (i)
	{
		printf("��������̵ĸ���\n");
		fflush(stdin);
		scanf("%d", &pro_num);
		printf("********************����Դ************************\n");
		printf("\t\t1->��ȡ�ļ�\n");
		printf("\t\t2->�ֶ�����\n");
		printf("\t\t0->�˳�\n");
		printf("**************************************************\n");
		fflush(stdin);
		scanf("%d", &i);
		switch (i)
		{
		case 1:read_file(pro_num, head); break;//��ȡ�ļ�
		case 2:manually_entered(pro_num, head); break;//�ֶ�����
		case 0:break;//�˳�
		}
		while (i)
		{
			i = menu(head);
			switch (i)
			{
			case 1:set_all_f(head); FCFS(head, pro_num); break;//�����ȷ���
			case 2:set_all_f(head); SJF(head, pro_num); break;//����ҵ����
			case 3:set_all_f(head); SRT(head, pro_num); break;//���ʣ��ʱ������
			case 4:set_all_f(head); TIMEROUND(head, pro_num); read_file(pro_num, head); break;//ʱ��Ƭ��ת
			case 5:set_all_f(head); PSA(head, pro_num); break;//���ھ�̬���ȼ��ĵ���(����ռʽ)
			case 6:set_all_f(head); PSA_grab(head, pro_num); break;//���ھ�̬���ȼ��ĵ���(��ռʽ)
			case 7:set_all_f(head); HRRN(head, pro_num); break;//���ڸ���Ӧ�����ȵĶ�̬���ȼ�����
			case 8:set_all_f(head); FEEDBACK(head, pro_num); break;//�༶��������
			case 0:break;//�˳�
			default:break;//�˳�
			}
		}
	}
	free_pcb(head);
	return 0;
}