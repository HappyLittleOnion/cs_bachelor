#include "filemanager.h"
#include "processmanager.h"

int main()
{
	printf("\t��ѡ����Ҫ���еĿ���̨\n1���ļ�����ϵͳ\t\t2�����̵���ϵͳ\t\t0���˳�\n");
	int choice;
	scanf("%d", &choice);
	getchar();
	while (choice) {
		system("cls");
		if (choice == 1)
		{
			filemenu();
		}
		else if (choice == 2)
		{
			promenu();
		}
		else
		{
			printf("�������\n");
		}
		system("cls");
		printf("\t��ѡ����Ҫ���еĿ���̨\n1���ļ�����ϵͳ\t\t2�����̵���ϵͳ\t\t0���˳�\n");
		scanf("%d", &choice);
		getchar();
	}
}
