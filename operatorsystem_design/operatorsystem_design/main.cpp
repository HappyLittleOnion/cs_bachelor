#include "filemanager.h"
#include "processmanager.h"

int main()
{
	printf("\t请选择所要进行的控制台\n1、文件管理系统\t\t2、进程调度系统\t\t0、退出\n");
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
			printf("输入错误\n");
		}
		system("cls");
		printf("\t请选择所要进行的控制台\n1、文件管理系统\t\t2、进程调度系统\t\t0、退出\n");
		scanf("%d", &choice);
		getchar();
	}
}
