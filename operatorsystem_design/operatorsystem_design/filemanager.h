#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ctype.h>
#include <stdbool.h>
#include <fstream>
#include <windows.h>
#include <iomanip>
using namespace std;

#define BLOCKSIZE      1024         // ���̿��С
#define SIZE           1024000      // 1000�����̿鹹����������̿ռ�
#define END            65535        // FAT���ļ�������־
#define FREE           0            // FAT���̿���б�־
#define ROOTBLOCKNUM   2            //������ǰ����������Ŀ¼�ļ�
#define MAXOPENFILE    10           //���ͬʱ���ļ�����
#define ERROR          -1
#define OK             1
#define MAXTEXT        4096         //text����С
#define MAXFILENAME    20            //�ļ�����󳤶�
#define MAXUSER        10           //����û���
#define MAXPWD         10           //������󳤶�
#define MAXNAME        10           //�û�����󳤶�
#define PUBUSER        "public"     //�����û�
#define ROOTUSER       "root"       //����Ա�û�

//�ļ����ƿ�(Ŀ¼�
typedef struct FCB {
    char filename[8];               //�ļ���
    char exname[5];                 //�ļ���չ��
    unsigned char attribute;        //�ļ������ֶΣ�0��Ŀ¼�ļ���1�������ļ�
    unsigned short time;            //�ļ�����ʱ��
    unsigned short date;            //�ļ���������
    unsigned short first;           //�ļ���ʼ�̿��
    unsigned long length;           //�ļ����ȣ��ֽ�����
    char free;                      //��ʾĿ¼���Ƿ�Ϊ�գ���ֵΪ0�����ʾ�գ�ֵΪ1����ʾ�ѷ��䡣
    char owner[MAXNAME];            //�ļ�������
    bool right;                     //�ļ�Ȩ��
}fcb;

typedef struct FAT {
    unsigned short id;              //unsigned short ռ�����ֽڣ�0-65535
}fat;

typedef struct USEROPEN {
    char filename[8];               //�ļ���
    char exname[5];                 //�ļ���չ��
    unsigned char attribute;        //�ļ������ֶΣ�0��Ŀ¼�ļ���1�������ļ� ������
    unsigned short time;            //�ļ�����ʱ��
    unsigned short date;            //�ļ���������
    unsigned short first;           //�ļ���ʼ�̿��
    unsigned long length;           //�ļ����ȣ��ֽ�����
    char owner[MAXNAME];            //�ļ�������
    bool right;                     //�ļ�Ȩ��
    //�������ļ�FCB�����ݣ������Ǽ�¼�й��ļ�ʹ�õĶ�̬��Ϣ��
    int father;                     //��Ŀ¼���ļ�������
    char dir[80];                   //���ļ�����·�����Ա���ټ��ָ���ļ��Ƿ��Ѿ��򿪡���ô��飿����
    int count;                      //��дָ���λ��
    char fcbstate;                  //�ļ���FCB�Ƿ��Ѿ����޸ģ�����޸��ˣ�����Ϊ1������Ϊ0
    char topenfile;                 //�򿪱����Ƿ�Ϊ�գ���ֵΪ0�����ʾ�ѱ�ռ��
}useropen;

typedef struct BLOCK0 {
    char information[200];          //�洢һЩ������Ϣ������̿��С�����̿�������
    unsigned short root;            //��Ŀ¼�ļ�����ʼ�̿��
    unsigned char* startblock;      //�����������������ʼλ��
}block0;

typedef struct USER {
    char username[MAXNAME];         //�û���
    char password[MAXPWD];          //����
    USER* next;                     //��һ���û�
}user;

//ȫ�ֱ�������
extern unsigned char* myvhard;             //ָ��������̵���ʼ��ַ
extern useropen openfile[MAXOPENFILE];     //�û����ļ������飬���ͬʱ��10���ļ�
extern int curdir;                         //��ǰĿ¼���ļ�������fd
extern char currentdir[80];                //��¼��ǰĿ¼��Ŀ¼��
extern unsigned char* startp;              //��¼�����������������ʼλ��
extern time_t c_time;                      //�ļ�ϵͳ����ʱ��
extern char filesys_name[];                //�ļ�ϵͳ�ļ����ļ���
extern unsigned char* buf;
extern user* users;                        //�û�����
extern user* curuser;                      //��ǰ�û�
extern char copybuf[MAXTEXT];              //�����ļ����ƵĻ�����
extern char cpfilename[MAXFILENAME];       //�����ļ����ļ���

void startsys();                    //��ʼ���ļ�ϵͳ
void my_format();                   //��������̽��и�ʽ��
int my_cd(char* dirname);           //����ǰĿ¼��Ϊָ����Ŀ¼
void my_mkdir(char* dirname);       //�ڵ�ǰĿ¼�´�����Ϊdirname����Ŀ¼
void my_rmdir(char* dirname);       //�ڵ�ǰĿ¼��ɾ����Ϊdirname����Ŀ¼
void my_ls();                       //��ʾ��ǰĿ¼�����ݣ���Ŀ¼���ļ���Ϣ��
int my_create(char* filename);      //������Ϊfilename�����ļ���
void my_rm(char* filename);         //ɾ����Ϊfilename���ļ�
int my_open(char* filename);        //�򿪵�ǰĿ¼����Ϊfilename���ļ�
int my_read(int fd, int len);	    //��ȡfd��ָ�ļ��дӵ�ǰ��дָ�뿪ʼ�ĳ���Ϊlen������
void my_close(int fd);              //�ر�֮ǰ��my_open()�򿪵��ļ�������Ϊfd���ļ�
int my_write(int fd);               //���û�ͨ���������������д��fd��ָ�����ļ���
int my_copy();                      //�����ļ�
int my_paste();                     //ճ���ļ�
int do_write(int fd, char* text, int len, char wstyle); //���������е�����д��ָ���ļ���
int do_read(int fd, int len, char* text);   //����ָ���ļ��дӶ�дָ�뿪ʼ�ĳ���Ϊlen�����ݵ��û��ռ��text��
void my_exitsys();                  //�˳��ļ�ϵͳ
void extractDir(char* dirname, char* dir, char* filename);
int findFreeBlock();
void printOpenFile();
void printFcb(int fd);
int filemenu();                     //�ļ�����ϵͳ�˵�
int login();                        //��¼
