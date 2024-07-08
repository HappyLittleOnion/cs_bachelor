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

#define BLOCKSIZE      1024         // 磁盘块大小
#define SIZE           1024000      // 1000个磁盘块构成总虚拟磁盘空间
#define END            65535        // FAT中文件结束标志
#define FREE           0            // FAT中盘块空闲标志
#define ROOTBLOCKNUM   2            //数据区前两块分配给根目录文件
#define MAXOPENFILE    10           //最多同时打开文件个数
#define ERROR          -1
#define OK             1
#define MAXTEXT        4096         //text最大大小
#define MAXFILENAME    20            //文件名最大长度
#define MAXUSER        10           //最多用户数
#define MAXPWD         10           //密码最大长度
#define MAXNAME        10           //用户名最大长度
#define PUBUSER        "public"     //公共用户
#define ROOTUSER       "root"       //管理员用户

//文件控制块(目录项）
typedef struct FCB {
    char filename[8];               //文件名
    char exname[5];                 //文件扩展名
    unsigned char attribute;        //文件属性字段，0：目录文件，1：数据文件
    unsigned short time;            //文件创建时间
    unsigned short date;            //文件创建日期
    unsigned short first;           //文件起始盘块号
    unsigned long length;           //文件长度（字节数）
    char free;                      //表示目录项是否为空，若值为0，则表示空；值为1，表示已分配。
    char owner[MAXNAME];            //文件所有者
    bool right;                     //文件权限
}fcb;

typedef struct FAT {
    unsigned short id;              //unsigned short 占两个字节，0-65535
}fat;

typedef struct USEROPEN {
    char filename[8];               //文件名
    char exname[5];                 //文件扩展名
    unsigned char attribute;        //文件属性字段，0：目录文件，1：数据文件 ？？？
    unsigned short time;            //文件创建时间
    unsigned short date;            //文件创建日期
    unsigned short first;           //文件起始盘块号
    unsigned long length;           //文件长度（字节数）
    char owner[MAXNAME];            //文件所有者
    bool right;                     //文件权限
    //以上是文件FCB的内容，下面是记录有关文件使用的动态信息。
    int father;                     //父目录的文件描述符
    char dir[80];                   //打开文件所在路径，以便快速检查指定文件是否已经打开。怎么检查？？？
    int count;                      //读写指针的位置
    char fcbstate;                  //文件的FCB是否已经被修改，如果修改了，则置为1；否则，为0
    char topenfile;                 //打开表项是否为空，若值为0，则表示已被占用
}useropen;

typedef struct BLOCK0 {
    char information[200];          //存储一些描述信息，如磁盘块大小，磁盘块数量等
    unsigned short root;            //根目录文件的起始盘块号
    unsigned char* startblock;      //虚拟磁盘上数据区开始位置
}block0;

typedef struct USER {
    char username[MAXNAME];         //用户名
    char password[MAXPWD];          //密码
    USER* next;                     //下一个用户
}user;

//全局变量定义
extern unsigned char* myvhard;             //指向虚拟磁盘的起始地址
extern useropen openfile[MAXOPENFILE];     //用户打开文件表数组，最多同时打开10个文件
extern int curdir;                         //当前目录的文件描述符fd
extern char currentdir[80];                //记录当前目录的目录名
extern unsigned char* startp;              //记录虚拟磁盘上数据区开始位置
extern time_t c_time;                      //文件系统启动时间
extern char filesys_name[];                //文件系统文件的文件名
extern unsigned char* buf;
extern user* users;                        //用户链表
extern user* curuser;                      //当前用户
extern char copybuf[MAXTEXT];              //用于文件复制的缓冲区
extern char cpfilename[MAXFILENAME];       //复制文件的文件名

void startsys();                    //初始化文件系统
void my_format();                   //对虚拟磁盘进行格式化
int my_cd(char* dirname);           //将当前目录改为指定的目录
void my_mkdir(char* dirname);       //在当前目录下创建名为dirname的子目录
void my_rmdir(char* dirname);       //在当前目录下删除名为dirname的子目录
void my_ls();                       //显示当前目录的内容（子目录和文件信息）
int my_create(char* filename);      //创建名为filename的新文件。
void my_rm(char* filename);         //删除名为filename的文件
int my_open(char* filename);        //打开当前目录下名为filename的文件
int my_read(int fd, int len);	    //读取fd所指文件中从当前读写指针开始的长度为len的内容
void my_close(int fd);              //关闭之前由my_open()打开的文件描述符为fd的文件
int my_write(int fd);               //将用户通过键盘输入的内容写到fd所指定的文件中
int my_copy();                      //复制文件
int my_paste();                     //粘贴文件
int do_write(int fd, char* text, int len, char wstyle); //将缓冲区中的内容写到指定文件中
int do_read(int fd, int len, char* text);   //读出指定文件中从读写指针开始的长度为len的内容到用户空间的text中
void my_exitsys();                  //退出文件系统
void extractDir(char* dirname, char* dir, char* filename);
int findFreeBlock();
void printOpenFile();
void printFcb(int fd);
int filemenu();                     //文件管理系统菜单
int login();                        //登录
