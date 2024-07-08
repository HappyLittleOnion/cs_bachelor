#define _CRT_SECURE_NO_WARNINGS
#define MAX 100
typedef struct pcb
{
	int runtime;//����ʱ��
	char* name;//������
	int arrivetime;//����ʱ��
	int priority;//���ȼ�
	int turnaroundtime;//��תʱ��
	double pturn;//��Ȩ��תʱ��
	int start;//��ʼʱ��
	int end;//����ʱ��
	char state;//״̬
	int restime;//��Ӧʱ��
	int remain;//ʣ��ʱ��
	int slice;//ʱ��Ƭ
	int remain_slice;//ʣ��ʱ��Ƭ
	struct pcb* next;//��һ�����
}pcb;

//���нṹ��
typedef struct queue
{
	pcb* front;
	pcb* rear;
}queue;
void init_pcb(pcb* p);//��ʼ��pcb����
int promenu();//�������к���
void manually_entered(int pro_num, pcb* head);//�ֶ����뺯��
void read_file(int pro_num, pcb* head);//��ȡ�ļ�
void free_pcb(pcb* head);//�ͷ�����Ŀռ�
int menu(pcb* head);//����������㷨�˵�����
void FCFS(pcb* head);//�����ȷ���
void change_pcb(pcb* p1, pcb* p2);//����pcb��ֵ
void sort_by_arrivetime(pcb* head);//���ݵ���ʱ���С������
void prin_head(pcb* head, int pro_num);//��ӡ�������й���
void simple_prin(pcb* head);//�򵥵Ĵ�ӡ�۲�������
void SJF(pcb* head, int pro_num);//����ҵ����
void copy_pcb(pcb* node, pcb* x);//pcb���ƣ�node���Ƶ�x
void set_all_f(pcb* head);//����head��Ϊf
void HRRN(pcb* head, int pro_num);//���ڸ���Ӧ�����ȵĶ�̬���ȼ�����
void PSA(pcb* head, int pro_num);//���ھ�̬���ȼ��ĵ���(����ռʽ)
void PSA_grab(pcb* head, int pro_num);//���ھ�̬���ȼ��ĵ���(��ռʽ)
void SRT(pcb* head, int pro_num);//���ʣ��ʱ������
int name_to_int(pcb* head, pcb* x);//����ת����
pcb* min_pcb(pcb* x1, pcb* x2);//���ص���ʱ���С�Ľ���
pcb* max_restime_pcb(pcb* head, int time);//�ҵ������еȴ�̬�������Ӧ�ȵĽ���
pcb* min_remain_pcb(pcb* head);//�ҵ������еȴ�̬�����ʣ��ʱ��Ľ���
pcb* min_priority_pcb(pcb* head);//�ҵ������еȴ�̬����С��ҵʱ�����
pcb* min_runtime_pcb(pcb* head);//�ҵ������еȴ�̬����С��ҵʱ�����
pcb* min_slice_arrivetime(pcb* head);//�ҵ������еȴ�̬����Сʱ��Ƭ����С����ʱ��Ľ���
int min_slice(pcb* head);//��Сʱ��Ƭ
int all_t(pcb* head);//�ж�����״̬�Ƿ���t
void init_matrix(int matrix[MAX][MAX], int pro_num);//��ʼ�����󣬼�¼�����ڸ����Ƿ����е����
void prin_matrix(int matrix[][MAX], int pro_num, int endtime, pcb* head);//��ӡ����
void TIMEROUND(pcb* head, int pro_num);//ʱ��Ƭ��ת
void FEEDBACK(pcb* head, int pro_num);//�༶��������

