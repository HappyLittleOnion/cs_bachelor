#include "filemanager.h"

//ȫ�ֱ�������
unsigned char* myvhard;             //ָ��������̵���ʼ��ַ
useropen openfile[MAXOPENFILE];      //�û����ļ������飬���ͬʱ��10���ļ�
int curdir;                         //��ǰĿ¼���ļ�������fd
char currentdir[80];                //��¼��ǰĿ¼��Ŀ¼��
unsigned char* startp;              //��¼�����������������ʼλ��
time_t c_time;                      //�ļ�ϵͳ����ʱ��
char filesys_name[] = "FAT16";      //�ļ�ϵͳ�ļ����ļ���
unsigned char* buf;				    //������
user* users = new user;             //�û�����
user* curuser;                      //��ǰ�û�
char copybuf[MAXTEXT];              //�����ļ����ƵĻ�����
char cpfilename[MAXFILENAME];       //�����ļ����ļ���

//�����ļ�����startsys()
void startsys()
{
    int pos = 0; //��¼д��������̵�λ��
    int i, j;
    FILE* fp;
    fcb* root;
    block0* guide;

    //�����ļ�ϵͳ�����������
    if ((fp = fopen(filesys_name, "r")) == NULL)
    {
        printf("The file system doesn't exist, and then it will be created by myformat().\n");
        my_format();
    }
    else
    {
        printf("The file system exists, and then it will be written to Myvhard.\n");
        for (i = 0; i < (SIZE / BLOCKSIZE); i++)
        {
            fread(buf, BLOCKSIZE, 1, fp);
            for (j = 0; j < BLOCKSIZE; j++)
            {
                myvhard[pos + j] = buf[j];
            }
            pos += BLOCKSIZE;
        }
        fclose(fp);
    }
    //��ʼ��rootĿ¼
    root = (fcb*)(myvhard + 5 * BLOCKSIZE);
    strcpy(openfile[0].filename, root->filename);
    strcpy(openfile[0].exname, root->exname);
    openfile[0].attribute = root->attribute;
    openfile[0].time = root->time;
    openfile[0].date = root->date;
    openfile[0].first = root->first;
    openfile[0].length = root->length;
    strcpy(openfile[0].owner, root->owner);
    openfile[0].right = root->right;
    //printf("In startsys,openfile[0].length:%d\n",openfile[0].length);
    openfile[0].father = 0;
    strcpy(openfile[0].dir, "\\root\\");
    openfile[0].count = 0;
    openfile[0].fcbstate = 0;
    openfile[0].topenfile = 1;  //�򿪱����Ƿ�Ϊ�գ�1Ϊ�ǿգ�0Ϊ��
}


//���̸�ʽ������my_format()
void my_format()
{
    //���ﶼֻ���ô�������̿ռ��з���ռ����������Fat1��fat2������ֱ�ӳɱ���
    FILE* fp;
    block0* guide;
    fat* fat1, * fat2;
    fcb* root;
    int i;
    time_t now_time;
    struct tm* lc_time;

    guide = (block0*)myvhard;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    root = (fcb*)(myvhard + BLOCKSIZE * 5);

    //��ʼ��������
    strcpy(guide->information, "Every block's size is 1024 bytes,and the number of the block is 1000.");
    guide->root = 5;
    guide->startblock = (unsigned char*)root;          //���������������ʼλ��

    //��ʼ��fat1,fat2,ǰ5�����̿��Ѿ������ȥ��
    for (i = 0; i < 5; i++)
    {
        fat1->id = END;
        fat2->id = END;
        fat1++;
        fat2++;
    }
    fat1->id = 6;
    fat2->id = 6;
    fat1++;
    fat2++;
    fat1->id = END;
    fat2->id = END;
    fat1++;
    fat2++;
    for (i = 7; i < (SIZE / BLOCKSIZE); i++)
    {
        fat1->id = FREE;
        fat2->id = FREE;
        fat1++;
        fat2++;
    }

    //��ʼ����Ŀ¼�ļ�
    strcpy(root->filename, ".");
    strcpy(root->exname, "");
    root->attribute = 0;
    /*
        time_t time(time_t *calptr)
        parameters:
            calptc:time_t���ͱ�����ַ
        return:
            ��1970-1-1,00:00:00�����ڵ�����
    */
    time(&now_time);                //ʱ�䣺��
    /*
        struct tm *localtime(const time_t * calptr);
        params:
            calptc:time_t���ͱ�����ַ;
        returns:
            ����һ���ṹ��
    */
    lc_time = localtime(&now_time);
    root->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    root->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(fcb);   //�ļ��峤��
    root->free = 1;   //��ʾĿ¼���Ƿ�Ϊ�գ�0��ʾ�գ�1��ʾ�ѷ��� ������
    strcpy(root->owner, PUBUSER);
    root->right = true;

    root++;
    time(&now_time);
    lc_time = localtime(&now_time);
    strcpy(root->filename, "..");
    strcpy(root->exname, "");
    root->attribute = 0;
    root->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    root->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(fcb);
    root->free = 1;
    strcpy(root->owner, PUBUSER);
    root->right = true;

    //���ļ�ϵͳ�е��ļ�д�����������
    fp = fopen(filesys_name, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
}

//���ĵ�ǰĿ¼����
int my_cd(char* dirname)
{
    char* dir, * next, * p;
    int fd, tmp, father;

    dir = strtok_s(dirname, "\\", &p);
    next = strtok_s(NULL, "\\", &p);
    //����!next��Ŀ�����жϺ����Ƿ�������Ŀ¼
    if (strcmp(dir, ".") == 0)
    {
        if (!next)
        {
            printf("It's current dir!\n");
            return OK;
        }
        else
        {
            dir = next;
            next = strtok_s(NULL, "\\", &p);
        }
    }
    else if (strcmp(dir, "..") == 0)
    {
        if (curdir)  //�ж��ǲ��Ǹ�Ŀ¼,��Ŀ¼���û��򿪱���Ϊ0
        {
            tmp = openfile[curdir].father;      //��Ŀ¼���ô����Ǳ��������ڴ�������
            my_close(curdir);
            curdir = tmp;
            if (!next)
                return OK;
            else
            {
                dir = next;
                next = strtok_s(NULL, "\\", &p);
            }
        }
        else
        {
            printf("the current dir is root dir!\n");
            return ERROR;
        }
    }
    else if (strcmp(dir, "root") == 0)
    {
        while (curdir)   //ѭ��ֱ����ǰĿ¼Ϊ��Ŀ¼
        {
            tmp = openfile[curdir].father;
            my_close(curdir);
            curdir = tmp;
        }
        dir = next;
        next = strtok_s(NULL, "\\", &p);
    }
    while (dir)
    {
        //���fd�������򿪵�Ŀ¼��Ϊ��ǰĿ¼
        fd = my_open(dir);
        if (openfile[fd].attribute)
        {
            printf("this is not a dir.\n");
            return ERROR;
        }
        else
        {
            if (fd == -2)
                return ERROR;
            else if (fd != ERROR)
                curdir = fd;
            else
            {
                printf("Failed to open the dir��\n");
                return ERROR;
            }
        }
        dir = next;
        next = strtok_s(NULL, "\\", &p);
    }
    //printOpenFile();
    return OK;
}

//������Ŀ¼����
void my_mkdir(char* dirname)
{
    int num, blk_num, i, fd, value = 0, father;
    fcb* fcbptr, * pos;
    fat* fat1, * fat2;
    time_t now;
    struct tm* lc_time;
    char text[MAXTEXT];
    char dir[80] = "", filename[MAXFILENAME], dir_tmp[80];
    char* fname, * exname;
    //����dir_tmp����Ϊ����ֱ�Ӱ�openfile[curdir].dir����my_cd()
    strcpy(dir_tmp, openfile[curdir].dir);
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //��ȡ��ָ��Ŀ¼�ʹ�����Ŀ¼�������л���ָ��Ŀ¼
    extractDir(dirname, dir, filename);
    if (!(strcmp(dir, "") == 0))
        value = my_cd(dir);
    if (value == ERROR)
        return;
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    if (exname != NULL)
    {
        printf("the dir doesn't have the exname!\n");
        my_cd(dir_tmp);
        return;
    }
    //��ȡ��ǰĿ¼�����ݣ��ж���Ŀ¼�Ƿ����
    openfile[curdir].count = 0;
    num = do_read(curdir, openfile[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        if ((!strcmp(fcbptr->filename, fname)))
        {
            printf("The filename to create has been exist!\n");
            return;
        }
        fcbptr++;
    }
    //Ѱ�ҿ���Ŀ¼���λ��
    fcbptr = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        if (fcbptr->free == 0)
            break;
        fcbptr++;
    }
    //Ѱ�ҿ��д��̿�
    blk_num = findFreeBlock();
    if (blk_num == ERROR)
        return;
    (fat1 + blk_num)->id = END;
    (fat2 + blk_num)->id = END;
    //��ʼ��Ŀ¼�ļ���fcb
    strcpy(fcbptr->filename, fname);
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = 0;
    time(&now);
    lc_time = localtime(&now);
    fcbptr->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    fcbptr->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    fcbptr->first = blk_num;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    strcpy(fcbptr->owner, curuser->username);
    fcbptr->right = true;
    //��Ŀ¼ָ�붨λ������Ŀ¼��
    openfile[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);

    //���µ�ǰĿ¼�е�һ��fcb������
    fcbptr = (fcb*)text;
    fcbptr->length = openfile[curdir].length;
    openfile[curdir].count = 0;
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);

    //����Ŀ¼�ļ�������"."��".."����Ŀ¼��
    fd = my_open(fname);
    if (fd == ERROR)
        return;
    if ((fcbptr = (fcb*)malloc(sizeof(fcb))) == NULL) {
        printf("malloc failed!\n");
        return;
    }
    strcpy(fcbptr->filename, ".");
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = 0;
    time(&now);
    lc_time = localtime(&now);
    fcbptr->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    fcbptr->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    fcbptr->first = blk_num;
    fcbptr->length = 2 * sizeof(fcb);
    fcbptr->free = 1;
    strcpy(fcbptr->owner,PUBUSER);
    fcbptr->right = true;
    do_write(fd, (char*)fcbptr, sizeof(fcb), 2);

    pos = (fcb*)text;
    strcpy(fcbptr->filename, "..");
    strcpy(fcbptr->exname, "");
    fcbptr->attribute = pos->attribute;
    fcbptr->time = pos->time;
    fcbptr->date = pos->date;
    fcbptr->first = pos->first;
    fcbptr->length = pos->length;
    fcbptr->free = 1;
    strcpy(fcbptr->owner, PUBUSER);
    fcbptr->right = true;
    if(do_write(fd, (char*)fcbptr, sizeof(fcb), 2) < 0)
        return;
    my_close(fd);

    //���µ�ǰĿ¼�ĸ�Ŀ¼�и���ĳ���
    father = openfile[curdir].father;
    openfile[father].count = 0;
    if ((num = do_read(father, openfile[father].length, text)) < 0) {
        return;
    }
    fcbptr = (fcb*)text;
    //printFcb(father);
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        if ((!strcmp(fcbptr->filename, openfile[curdir].filename)) && (!strcmp(fcbptr->exname, openfile[curdir].exname)))
        {
            fcbptr->length = openfile[curdir].length;
            break;
        }
        fcbptr++;
    }
    openfile[father].count = i * sizeof(fcb);
    do_write(father, (char*)fcbptr, sizeof(fcb), 2);
    openfile[curdir].fcbstate = 1;
    //printFcb(father);
    //printOpenFile();

    my_cd(dir_tmp);
}

//ɾ����Ŀ¼����
void my_rmdir(char* dirname)
{
    int num1, num2, blk_num, i, fd, j;
    fcb* fcbptr, * fcbptr2;
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    char text[MAXTEXT], text2[MAXTEXT];
    char dir[80] = "", filename[MAXFILENAME] = "", dir_tmp[80] = "";

    //����dir_tmp����Ϊ����ֱ�Ӱ�openfile[curdir].dir����ȥ
    strcpy(dir_tmp, openfile[curdir].dir);
    //��ȡ·����Ŀ¼��
    extractDir(dirname, dir, filename);
    if (!(strcmp(dir, "") == 0))
        my_cd(dir);
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);

    //�ж�Ҫɾ�����ǲ��ǵ�ǰĿ¼���߸�Ŀ¼
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
    {
        printf("Can't remove the current dir and the father dir!\n");
        return;
    }
    //��ȡ��Ŀ¼�ļ�����
    openfile[curdir].count = 0;
    num1 = do_read(curdir, openfile[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < (num1 / sizeof(fcb)); i++)
    {
        if (strcmp(fcbptr->filename, filename) == 0 && strcmp(fcbptr->exname, "") == 0)
            break;
        fcbptr++;
    }
    if (i == (num1 / sizeof(fcb)))
    {
        printf("The dir to delete doesn't exist!\n");
        return;
    }
    //��Ҫɾ�����ļ�
    fd = my_open(filename);
    num2 = do_read(fd, openfile[fd].length, text2);
    fcbptr2 = (fcb*)text2;
    //�ж�ָ���ļ��ܲ���ɾ��
    for (j = 0; j < (num2 / sizeof(fcb)); j++)
    {
        if (strcmp(fcbptr2->filename, ".") && strcmp(fcbptr2->filename, "..") && strcmp(fcbptr2->filename, ""))
        {
            my_close(fd);
            printf("Failed to remove this dir.Directory not empty!\n");
            return;
        }
        fcbptr2++;
    }
    //����ָ��Ŀ¼ռ�ݵĴ��̿�
    blk_num = openfile[fd].first;
    while (blk_num != END)
    {
        fatptr1 = fat1 + blk_num;
        fatptr2 = fat2 + blk_num;
        blk_num = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    my_close(fd);
    strcpy(fcbptr->filename, "");
    fcbptr->free = 0;
    openfile[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)fcbptr, sizeof(fcb), 2);
    openfile[curdir].fcbstate = 1;

    my_cd(dir_tmp);
}

//��ʾĿ¼����
void my_ls()
{
    fcb* fcbptr;
    char text[MAXTEXT];
    int num, i;

    openfile[curdir].count = 0;
    num = do_read(curdir, openfile[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < num / sizeof(fcb); i++)
    {
        if (fcbptr->free)
        {
            if (!fcbptr->attribute) {
                printf("%s\\\t\t<DIR>\t\t%d/%d/%d\t%02d:%02d:%02d\t%s\t", fcbptr->filename, (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x001f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x003f, fcbptr->time & 0x001f * 2, fcbptr->owner);
                if (fcbptr->right)
					printf("T\n");
				else
					printf("F\n");
            }
            else
            {
                printf("%s.%s\t\t%dB\t\t%d/%d/%d\t%02d:%02d:%02d\t%s\t", fcbptr->filename, fcbptr->exname, (int)(fcbptr->length), (fcbptr->date >> 9) + 1980, (fcbptr->date >> 5) & 0x000f, fcbptr->date & 0x001f, fcbptr->time >> 11, (fcbptr->time >> 5) & 0x3f, fcbptr->time & 0x1f * 2,fcbptr->owner);
                if(fcbptr->right)
                    printf("T\n");
                else
                    printf("F\n");
            }
        }
        fcbptr++;
    }
}

//�����ļ�����
int my_create(char* dirname)
{
    int num, blk_num, i, father;
    time_t now;
    struct tm* lc_time;
    fat* fat1, * fat2;
    fcb* pos; //pos������λ��������ʲôĿ¼��
    char* fname, * exname;
    char dest_dir[80] = "", filename[MAXFILENAME] = "", text[MAXTEXT] = "", dir_tmp[80] = ""; //Ҫ�����ļ���Ŀ¼

    strcpy(dir_tmp, openfile[curdir].dir);
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    extractDir(dirname, dest_dir, filename);
    if (!(strcmp(dest_dir, "") == 0))
        my_cd(dest_dir);
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    printf("fname:%s,exname:%s\n", fname, exname);
    if (strcmp(fname, "") == 0)
    {
        printf("the filename is NULL!\n");
        return ERROR;
    }
    if (!exname)
    {
        printf("the extern name is NULL!\n");
        return ERROR;
    }

    //Ҫ������Ŀ¼�ļ����Ƿ��Ѿ���ͬ���ļ�,��Ŀ¼�ļ��д洢�����ļ���FCB
    openfile[curdir].count = 0;
    num = do_read(curdir, openfile[curdir].length, text);
    if (num == ERROR)
    {
        printf("Read father file failed!\n");
        return ERROR;
    }
    pos = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        //�����Ƿ���ͬ���ļ�
        if ((!strcmp(pos->filename, fname)) && (!(strcmp(pos->exname, exname))))
        {
            printf("The filename to create has been exist!\n");
            return ERROR;
        }
        pos++;
    }

    //ȷ����û�������ļ���Ȼ��ʼ�����ļ���
    //Ҫ�ڸ�Ŀ¼��������ļ���Ŀ¼��
    pos = (fcb*)text;
    for (i = 0; i < num / sizeof(fcb); i++)
    {
        if (pos->free == 0)
            break;
        pos++;
    }
    blk_num = findFreeBlock();
    if (blk_num == ERROR)
        return ERROR;
    (fat1 + blk_num)->id = END;
    (fat2 + blk_num)->id = END;
    //��ʼ�����ļ���fcb
    strcpy(pos->filename, fname);
    strcpy(pos->exname, exname);
    pos->attribute = 1;
    time(&now);
    lc_time = localtime(&now);
    pos->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    pos->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    pos->first = blk_num;
    pos->length = 0;   //Ӧ�����ļ��峤�Ȱ�
    pos->free = 1;   //��ʾĿ¼���Ƿ�Ϊ�գ�0��ʾ�գ�1��ʾ�ѷ���
    strcpy(pos->owner, curuser->username);
    pos->right = false;

    openfile[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)pos, sizeof(fcb), 2);

    //���µ�ǰĿ¼�ĵ�һ��Ŀ¼��
    pos = (fcb*)text;
    pos->length = openfile[curdir].length;
    openfile[curdir].count = 0;
    if (do_write(curdir, (char*)pos, sizeof(fcb), 2) < 0)
		return ERROR;
    openfile[curdir].fcbstate = 1;

    //���¸�Ŀ¼�еĵ�ǰĿ¼��Ŀ¼��
    father = openfile[curdir].father;
    openfile[father].count = 0;
    if ((num = do_read(father, openfile[father].length, text)) < 0)
        return ERROR;
    pos = (fcb*)text;
    //printFcb(father);
    for (i = 0; i < (int)(num / sizeof(fcb)); i++)
    {
        if ((!strcmp(pos->filename, openfile[curdir].filename)) && (!strcmp(pos->exname, openfile[curdir].exname)))
        {
            pos->length = openfile[curdir].length;
            break;
        }
        pos++;
    }
    openfile[father].count = i * sizeof(fcb);
    do_write(father, (char*)pos, sizeof(fcb), 2);

    my_cd(dir_tmp);
}

//ɾ���ļ�����
void my_rm(char* dirname)
{
    fcb* pos;
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    char* fname, * exname, text[MAXTEXT];
    int num, i, blk_num;
    char dir[80] = "", filename[MAXFILENAME] = "", dir_tmp[80] = "";

    strcpy(dir_tmp, openfile[curdir].dir);
    //��ȡ·����Ŀ¼��
    extractDir(dirname, dir, filename);
    if (!(strcmp(dir, "") == 0))
        my_cd(dir);
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    fname = strtok(filename, ".");
    exname = strtok(NULL, ".");
    if (strcmp(fname, "") == 0)
    {
        printf("the filename is NULL!\n");
        return;
    }
    if (!exname)
    {
        printf("the extern name is NULL!\n");
        return;
    }
    openfile[curdir].count = 0;
    num = do_read(curdir, openfile[curdir].length, text);
    pos = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        if (strcmp(pos->filename, fname) == 0 && strcmp(pos->exname, exname) == 0)
            break;
        pos++;
    }
    if (i == num / sizeof(fcb))
    {
        printf("Error,the file is not exist.\n");
        return;
    }
    if (strcmp(curuser->username, ROOTUSER) != 0 && strcmp(pos->owner, curuser->username) != 0) {
        printf("You don't have the right to delete this file\n");
        return;
    }
    blk_num = pos->first;
    while (blk_num != END)
    {
        fatptr1 = fat1 + blk_num;
        fatptr2 = fat2 + blk_num;
        blk_num = fatptr1->id;
        fatptr1->id = FREE;
        fatptr2->id = FREE;
    }
    strcpy(pos->filename, "");
    pos->free = 0;
    openfile[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)pos, sizeof(fcb), 2);
    openfile[curdir].fcbstate = 1;


    my_cd(dir_tmp);
}

//���ļ�����
int my_open(char* dirname)
{
    int i, num, value;
    int fd = -1;
    fcb* fcbptr;
    char* fname, exname[5] = "", * tmp;
    char dest_dir[80] = "", filename[MAXFILENAME] = "", text[MAXTEXT]; //Ҫ�����ļ���Ŀ¼

    if (strlen(dirname) == 0)
        return -2;
    extractDir(dirname, dest_dir, filename);
    if (!(strcmp(dest_dir, "") == 0))
        value = my_cd(dest_dir);
    fname = strtok(filename, ".");
    tmp = strtok(NULL, ".");
    /*
        char* strtok(char* str,constchar* delimiters );
        params:
            str:Ҫ�ָ���ַ�����
            delimiters:���ڷָ�ķ���
        return:
             s��ͷ��ʼ��һ�������ָ�Ĵ�����s�е��ַ����ҵ�ĩβʱ������NULL��
        ˵�����ڵ��õ�һ��strtok֮�󣬶�ȡ�¸��Ӵ���Ҫ���µ���strtok��str��Ҫ����ΪNULL
    */
    if (tmp)
        strcpy(exname, tmp);
    else
        strcpy(exname, "");
    for (i = 0; i < MAXOPENFILE; i++)
    {
        /*
            extern int strcmp(const char *s1,const char *s2);�Ƚ������ַ�����С
            return:
                value:
                    if s1<s2: value < 0;
                    if s1 == s2: value = 0;
                    if s1 > s2 : values > 0;
        */
        if (!strcmp(fname, openfile[i].filename) && !strcmp(exname, openfile[i].exname))
        {
            return i;
        }
    }
    //�鿴��Ŀ¼�е�fcb
    openfile[curdir].count = 0;
    num = do_read(curdir, openfile[curdir].length, text);
    fcbptr = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0 && strcmp(fcbptr->exname, exname) == 0)
            break;
        fcbptr++;
    }
    if (i == num / sizeof(fcb))
    {
        printf("Error,the file is not exist.\n");
        return ERROR;
    }
    if (!fcbptr->right && strcmp(fcbptr->owner, PUBUSER) != 0 && strcmp(curuser->username, ROOTUSER) != 0) {
        if (strcmp(fcbptr->owner, curuser->username) != 0) {
            printf("You don't have the right to open the file!\n");
            return ERROR;
        }
    }
    //�����û��򿪱�Ŀ���λ��
    for (i = 0; i < MAXOPENFILE; i++)
    {
        if (openfile[i].topenfile == 0)
        {
            fd = i;
            break;
        }
    }
    if (fd == -1)
    {
        printf("The user opens the table with no free entries!\n");
        return ERROR;
    }
    strcpy(openfile[fd].filename, fname);
    strcpy(openfile[fd].exname, fcbptr->exname);
    openfile[fd].attribute = fcbptr->attribute;
    openfile[fd].time = fcbptr->time;
    openfile[fd].date = fcbptr->date;
    openfile[fd].first = fcbptr->first;
    openfile[fd].length = fcbptr->length;
    strcpy(openfile[fd].owner, fcbptr->owner);
    openfile[fd].right = fcbptr->right;

    strcpy(openfile[fd].dir, openfile[curdir].dir);
    strcat(openfile[fd].dir, filename);
    if (!fcbptr->attribute)
        strcat(openfile[fd].dir, "\\");
    openfile[fd].father = curdir;
    openfile[fd].count = 0;
    openfile[fd].fcbstate = 0;
    openfile[fd].topenfile = 1;
    return fd;
}

//�ر��ļ�����
void my_close(int fd)
{
    fcb* fcbptr, * pos;
    int father, num, i;
    char text[MAXTEXT];

    //���fd����Ч��
    if (fd < 0 || fd >= MAXOPENFILE)
    {
        printf("the input fd is out of range!\n");
        return;
    }
    if (openfile[fd].fcbstate)
    {
        fcbptr = (fcb*)malloc(sizeof(fcb));
        strcpy(fcbptr->filename, openfile[fd].filename);
        strcpy(fcbptr->exname, openfile[fd].exname);
        fcbptr->attribute = openfile[fd].attribute;
        fcbptr->time = openfile[fd].time;
        fcbptr->date = openfile[fd].date;
        fcbptr->first = openfile[fd].first;
        fcbptr->length = openfile[fd].length;
        strcpy(fcbptr->owner, openfile[fd].owner);
        fcbptr->right = openfile[fd].right;
        father = openfile[fd].father;

        //��ȡ��Ŀ¼�ļ�����
        openfile[father].count = 0;
        num = do_read(father, openfile[father].length, text);
        pos = (fcb*)text;
        //��λ���ļ���Ӧ��Ŀ¼��
        for (i = 0; i < (num / sizeof(fcb)); i++)
        {
            if (strcmp(pos->filename, openfile[fd].filename) == 0 && strcmp(pos->exname, openfile[fd].exname) == 0)
                break;
            pos++;
        }
        openfile[father].count = i * sizeof(fcb);
        do_write(father, (char*)fcbptr, sizeof(fcb), 2);
        openfile[fd].fcbstate = 0;
    }

    //��ʼ���û��򿪱�
    strcpy(openfile[fd].filename, "");
    strcpy(openfile[fd].exname, "");
    openfile[fd].topenfile = 0;
    //printOpenFile();
}


//д�ļ�����
int my_write(int fd)
{
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    int wstyle, num_cur, num = 0, len;
    char text[MAXTEXT];
    unsigned short blk_num;

    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //���fd����Ч��
    if (fd < 0 || fd >= MAXOPENFILE)
    {
        printf("the input fd is out of range!\n");
        return ERROR;
    }
    //ѡ��д�뷽ʽ
    while (1)
    {
        printf("Please choose the number of the style that the order file be written:\n1.rewrite\t2.cover write\t3.append write\n");
        cin >> wstyle;
        if (wstyle > 0 && wstyle < 4)
            break;
        printf("the wstyle must be in [1,3]!");
    }
    //�Ӽ�����������
    getchar();  //��Ҫ��֮ǰ�������еĻ��з�������������Ժ������text����Ӱ�졣
    switch (wstyle)
    {
    case 1:
        blk_num = openfile[fd].first;
        fatptr1 = fat1 + blk_num;
        fatptr2 = fat2 + blk_num;
        blk_num = fatptr1->id;
        fatptr1->id = END;
        fatptr2->id = END;
        while (blk_num != END)
        {
            fatptr1 = fat1 + blk_num;
            fatptr2 = fat2 + blk_num;
            blk_num = fatptr1->id;
            fatptr1->id = FREE;
            fatptr2->id = FREE;
        }
        openfile[fd].count = 0;
        openfile[fd].length = 0;
        break;
    case 2:
        openfile[fd].count = 0;
        //�ļ��Ķ�дָ��Ӧ�ò���Ҫ��
        break;
    case 3:
        openfile[fd].count = openfile[fd].length;
    default:
        break;
    }
    printf("Please input text that you want to write:\n");
    while (gets_s(text))
    {
        len = strlen(text);
        num_cur = do_write(fd, text, len, wstyle);
        if (num_cur != ERROR)
            num += num_cur;
        if (num_cur < len)
        {
            printf("Write ERROR!\n");
            break;
        }
    }
    //printf("In my_write, the fd is %d, the filename is %s,the length is %d\n",fd, openfile[fd].filename,openfile[fd].length);
    return num;
}

//ʵ��д�ļ�����
int do_write(int fd, char* text, int len, char wstyle)
{
    fat* fat1, * fat2, * fatptr1, * fatptr2;
    unsigned short blk_num;
    int blk_off, i, num, cur_blk, father;
    unsigned char* pos;
    fcb* fcbptr;
    char text2[MAXTEXT];

    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //�߼�����
    blk_num = openfile[fd].count / BLOCKSIZE;
    //����ƫ����
    blk_off = openfile[fd].count;
    fatptr1 = fat1 + openfile[fd].first;
    fatptr2 = fat2 + openfile[fd].first;
    cur_blk = openfile[fd].first;
    //����fat���ҵ�Ҫд�����ʼ�߼����λ��
    for (i = 0; i < blk_num; i++)
    {
        cur_blk = fatptr1->id;
        blk_off = blk_off - BLOCKSIZE;
        fatptr1 = fat1 + cur_blk;
    }
    //д��ͬʱ��Ҫ����д������
    num = 0;
    while (num < len)
    {
        pos = (unsigned char*)(myvhard + cur_blk * BLOCKSIZE);
        //��Ҫ����һ�����дҲҪ����һ��д
        for (i = 0; i < BLOCKSIZE; i++)
            buf[i] = pos[i];
        for (; blk_off < BLOCKSIZE; blk_off++)
        {
            buf[blk_off] = text[num];
            num++;
            openfile[fd].count++;
            //����д�볤��С�ڣ����̿��С - ��ʼ����ƫ��������ʱ��
            if (num == len)
                break;
        }
        for (i = 0; i < BLOCKSIZE; i++)
        {
            pos[i] = buf[i];
        }
        //���д�볤�ȴ���һ��(���̿��С - ��ʼ����ƫ����)��ʱ��
        if (num < len)
        {
            cur_blk = fatptr1->id;
            if (cur_blk == END)
            {
                cur_blk = findFreeBlock();
                if (cur_blk == ERROR)
                    break;
                fatptr1->id = cur_blk;
                fatptr2->id = cur_blk;
                fatptr1 = fatptr1 + cur_blk;
                fatptr2 = fatptr2 + cur_blk;
                fatptr1->id = END;
                fatptr2->id = END;
            }
            else
            {
                fatptr1 = fat1 + cur_blk;
                fatptr1 = fat1 + cur_blk;
            }
            blk_off = 0;
        }
    }
    if (openfile[fd].count > openfile[fd].length)
    {
        openfile[fd].length = openfile[fd].count;
    }

    //�ļ���fcb�Ƿ��޸�
    openfile[fd].fcbstate = 1;
    return num;
}

//���ļ�����
int my_read(int fd, int len)
{
    int num;
    char text[MAXTEXT];

    if (fd < 0 || fd >= MAXOPENFILE)
    {
        printf("the input fd is out of range!\n");
        return ERROR;
    }
    openfile[fd].count = 0;
    num = do_read(fd, len, text);
    if (num == ERROR)
    {
        printf("Read fails!\n");
        return ERROR;
    }
    else
    {
        printf("%s\n", text);
    }
    return num;
}

//ʵ�ʶ��ļ�����
int do_read(int fd, int len, char* text)
{
    fat* fat1;
    fat* fat_ptr;
    int cur_blk, i;
    unsigned short blk_num;
    unsigned char* pos;
    int blk_off, num = 0; // num��¼��ȡ����

    //�ж϶�ȡ�����Ƿ񳬹��ļ�����
    if ((openfile[fd].count + len) > openfile[fd].length)
    {
        printf("the read length is more than the file's length!\n");
        return ERROR;
    }

    //�߼�����
    blk_num = openfile[fd].count / BLOCKSIZE;
    //����ƫ����
    blk_off = openfile[fd].count;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat_ptr = fat1 + openfile[fd].first;
    cur_blk = openfile[fd].first;
    //����fat���ҵ�Ҫ��ȡ����ʼ�߼����λ��
    for (i = 0; i < blk_num; i++)
    {
        cur_blk = fat_ptr->id;
        blk_off = blk_off - BLOCKSIZE;
        fat_ptr = fat1 + cur_blk;
    }
    while (num < len)
    {
        //pos��ǵ�ǰ��д���̿�ĳ�ʼλ��
        pos = (unsigned char*)(myvhard + cur_blk * BLOCKSIZE);
        for (i = 0; i < BLOCKSIZE; i++)
        {
            buf[i] = pos[i];
        }
        for (; blk_off < BLOCKSIZE; blk_off++)
        {
            text[num] = buf[blk_off];
            num++;
            openfile[fd].count++;
            //Ҫ�ж��ǲ����Ѿ�����ָ��������
            if (num == len)
                break;
        }
        if (num < len)
        {
            cur_blk = fat_ptr->id;
            blk_off = 0;
            fat_ptr = fat1 + cur_blk;
        }
    }
    text[num] = '\0';
    return num;
}

//�˳��ļ�ϵͳ����
void my_exitsys()
{
    int father;
    FILE* fp;

    fp = fopen(filesys_name, "w");
    while (curdir)
    {
        father = openfile[curdir].father;
        my_close(curdir);
        curdir = father;
    }
    //������������д�����ָ���ļ���
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
    free(buf);
    free(myvhard);
}

//���û���������ȡ��·��
void extractDir(char* dirname, char* dir, char* filename)
{
    int len;
    char* tmp, * next, * p;
    tmp = strtok_s(dirname, "\\", &p);
    while (tmp)
    {
        if (!(next = strtok_s(NULL, "\\", &p)))
        {
            if (((strcmp(tmp, ".") == 0) || (strcmp(tmp, "..") == 0)))
            {
                break;
            }
            strcpy(filename, tmp);
            break;
        }
        strcat(dir, tmp);
        strcat(dir, "\\");
        tmp = next;
    }
    len = strlen(dir);
    if (dir[len - 1] == '\\')
    {
        dir[len - 1] = '\0';
    }
}

//���ҿ��п�
int findFreeBlock()
{
    unsigned char i;
    fat* fat1, * pos;
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    for (i = 7; i < (SIZE / BLOCKSIZE); i++)
    {
        pos = fat1 + i;
        if (pos->id == FREE)
            return i;
    }
    printf("Can't find a free block");
    return ERROR;
}

void printOpenFile()
{
    int i, father;
    printf("fd\t\t�ļ���\t\t�ļ�����\t\t��Ŀ¼\t\t��Ŀ¼fd\n");
    for (i = 0; i < MAXOPENFILE; i++)
    {
        if (openfile[i].topenfile != 0)
        {
            father = openfile[i].father;
            printf("%d\t\t%s.%s\t\t%d\t\t%s\t\t%d\n", i, openfile[i].filename, openfile[i].exname, (int)openfile[i].length, openfile[father].filename, father);
        }

    }
}

void printFcb(int fd)
{
    int i, num, tmp;
    fcb* fcbptr;
    char text[MAXTEXT];

    tmp = openfile[fd].count;
    openfile[fd].count = 0;
    num = do_read(fd, openfile[fd].length, text);
    printf("��Ŀ¼��%s �ж�ȡ�� %d �ֽ�.\n", openfile[fd].filename, num);
    fcbptr = (fcb*)text;
    //��λ���ļ���Ӧ��Ŀ¼��
    printf("�ļ���\t\t�ļ�����\t\t\n");
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
        printf("%s\t\t%d\n", fcbptr->filename, (int)fcbptr->length);
        fcbptr++;
    }
    openfile[fd].count = tmp;
}


//ע�ắ��
int reguser(char* username,char* password) {
    user* userptr;
    //����û����Ƿ��Ѿ�����
    userptr = users->next;
    while (userptr != NULL)
    {
        if (strcmp(userptr->username, username) == 0)
        {
			printf("The username has been exist!\n");
			return ERROR;
		}
		userptr = userptr->next;
	}
    //�����û���Ϣд���û�����
	userptr = new user;
	strcpy(userptr->username, username);
	strcpy(userptr->password, password);
	user* up = users->next;
    while (up->next != NULL)
    {
		up = up->next;
	}
    up->next = userptr;
    userptr->next = NULL;
	
	fstream uf;
	uf = fstream("userlist", ios::out | ios::app);
    if (!uf)
    {
		cout << "open userlist failed��" << endl;
		return ERROR;
	}
	//�����û�д���û��ļ�������д��
    uf << userptr->username << " " << userptr->password << endl;
	uf.close();
	printf("register success!\n");
	return 0;
}


int login(char* username,char* password) {
    int i;
    user* userptr;
    userptr = users->next;
    while (userptr != NULL)
    {
        if (strcmp(userptr->username, username) == 0 && strcmp(userptr->password, password) == 0)
        {
            curuser = userptr;
            printf("login success!\n");
            int i = 0;
            while (i <= 100)
            {
                cout << "\r";//�ص�����λ��
                //������ 6��������λС��
                cout << setw(8) << fixed << setprecision(2) << (float)i << "%";
                Sleep(rand()%10);
                i++;
            }
            cout << endl << "Hello World" << endl;
            Sleep(1000);
            system("cls");
            return 0;
        }
        userptr = userptr->next;
    }
    printf("username or password is wrong��\n");
    return ERROR;
}

int inituserlist() {
    fstream uf;
    uf = fstream("userlist", ios::in);
    if (!uf)
    {
        cout << "open userlist failed��" << endl;
        return ERROR;
    }
    int i = 0;
    user* up = users;
    while (!uf.eof() && i++ < MAXUSER)
    {
        user* userptr = new user;
        uf >> userptr->username >> userptr->password;
        up->next = userptr;
        userptr->next = NULL;
        up = up->next;
    }
    uf.close();
    return 0;
}

//��ʽ���û���Ϣ�ļ�
int formatuserlist() {
	fstream uf;
	uf = fstream("userlist", ios::out);
    if (!uf)
    {
		cout << "open userlist failed��" << endl;
		return ERROR;
	}
    uf << "root root" << endl;
	uf.close();
	return 0;
}

void showuserlist() {
    user* userptr = users->next;
    printf("****************************************************************\n\n");
    while (userptr != NULL)
    {
		printf("%s\n", userptr->username);
		userptr = userptr->next;
	}
    printf("****************************************************************\n\n");
}

int my_setpow(char* power, int fd){
    if (fd < 0 || fd >= MAXOPENFILE) 
    {
        printf("the input fd is out of range!\n");
        return ERROR;
    }
    if (strcmp(openfile[fd].owner, curuser->username) != 0) {
		printf("You don't have the right to set the file power!\n");
		return ERROR;
	}
    if (strcmp(power, "false") == 0) {
        openfile[fd].right = false;
    }
    else if (strcmp(power, "true") == 0) {
        openfile[fd].right = true;
    }
    else {
        printf("Please input the right command.\n");
		return ERROR;
    }
    openfile[fd].fcbstate = 1;
	return 0;
}

//���ƺ���-�ڴ��ļ�״̬�¸����ļ������ƻ�����
int my_copy() {
    int num;
    char cpexname[8];
    openfile[curdir].count = 0;
    memset(copybuf, 0, MAXTEXT);
    memset(cpfilename, 0, MAXFILENAME);
	num = do_read(curdir, openfile[curdir].length, copybuf);
    if (num == ERROR)
    {
		printf("copy fails!\n");
		return ERROR;
	}
    strcpy(cpfilename, openfile[curdir].filename);
    strcpy(cpexname, openfile[curdir].exname);
    //�ϲ��ļ�������չ��
    strcat(cpfilename, ".");
    strcat(cpfilename, cpexname);
	printf("copy success!\n");
	return num;
}

//ճ������-�ڸ��ƻ�����״̬��ճ���ļ�����ǰĿ¼
int my_paste() {
	int num, blk_num, i, father,is_rename = 0,flen;
	time_t now;
	struct tm* lc_time;
	fat* fat1, * fat2;
	fcb* pos; //pos������λ��������ʲôĿ¼��
	char* fname, * exname;
	char dest_dir[80] = "", filename[MAXFILENAME] = "", text[MAXTEXT] = "", dir_tmp[80] = ""; //Ҫ�����ļ���Ŀ¼

	strcpy(dir_tmp, openfile[curdir].dir);
	fat1 = (fat*)(myvhard + BLOCKSIZE);
	fat2 = (fat*)(myvhard + 3 * BLOCKSIZE);
    //��鸴���ļ����Ƿ�Ϊ��
    if (strcmp(cpfilename, "") == 0)
    {
        printf("The filename to paste is NULL!\n");
        return ERROR;
    }
	strcpy(filename, cpfilename);
    fname = strtok(filename, ".");
	exname = strtok(NULL, ".");
    flen = strlen(fname);

	//Ҫ������Ŀ¼�ļ����Ƿ��Ѿ���ͬ���ļ�,��Ŀ¼�ļ��д洢�����ļ���FCB
	openfile[curdir].count = 0;
	num = do_read(curdir, openfile[curdir].length, text);
	pos = (fcb*)text;
    for (i = 0; i < (num / sizeof(fcb)); i++)
    {
		//�����Ƿ���ͬ���ļ��������ļ����������(1)
        if ((!strncmp(pos->filename, fname, flen)) && (!(strcmp(pos->exname, exname))))
            is_rename++;
		pos++;
	}

    //ȷ����û�������ļ���Ȼ��ʼ�����ļ���
    //Ҫ�ڸ�Ŀ¼��������ļ���Ŀ¼��
    pos = (fcb*)text;
    for (i = 0; i < num / sizeof(fcb); i++)
    {
        if (pos->free == 0)
            break;
        pos++;
    }
    blk_num = findFreeBlock();
    if (blk_num == ERROR)
        return ERROR;
    (fat1 + blk_num)->id = END;
    (fat2 + blk_num)->id = END;
    //��ʼ�����ļ���fcb
    strcpy(pos->filename, fname);
    strcpy(pos->exname, exname);
    if (is_rename)
    {
		char tmp[8];
		sprintf(tmp, "(%d)", is_rename);
		strcat(pos->filename, tmp);
        //���¸����ļ�������չ��
        strcpy(cpfilename, pos->filename);
	    strcat(cpfilename, ".");
        strcat(cpfilename, pos->exname);
    }
    pos->attribute = 1;
    time(&now);
    lc_time = localtime(&now);
    pos->time = lc_time->tm_hour * 2048 + lc_time->tm_min * 32 + lc_time->tm_sec / 2;
    pos->date = (lc_time->tm_year - 80) * 512 + (lc_time->tm_mon + 1) * 32 + lc_time->tm_mday;
    pos->first = blk_num;
    pos->length = 0;   //Ӧ�����ļ��峤�Ȱ�
    pos->free = 1;   //��ʾĿ¼���Ƿ�Ϊ�գ�0��ʾ�գ�1��ʾ�ѷ���
    strcpy(pos->owner, curuser->username);
    pos->right = false;

    openfile[curdir].count = i * sizeof(fcb);
    do_write(curdir, (char*)pos, sizeof(fcb), 2);

    //���µ�ǰĿ¼�ĵ�һ��Ŀ¼��
    pos = (fcb*)text;
    pos->length = openfile[curdir].length;
    openfile[curdir].count = 0;
    if (do_write(curdir, (char*)pos, sizeof(fcb), 2) < 0)
        return ERROR;
    openfile[curdir].fcbstate = 1;

    //���¸�Ŀ¼�еĵ�ǰĿ¼��Ŀ¼��
    father = openfile[curdir].father;
    openfile[father].count = 0;
    if ((num = do_read(father, openfile[father].length, text)) < 0)
        return ERROR;
    pos = (fcb*)text;
    //printFcb(father);
    for (i = 0; i < (int)(num / sizeof(fcb)); i++)
    {
        if ((!strcmp(pos->filename, openfile[curdir].filename)) && (!strcmp(pos->exname, openfile[curdir].exname)))
        {
            pos->length = openfile[curdir].length;
            break;
        }
        pos++;
    }
    openfile[father].count = i * sizeof(fcb);
    do_write(father, (char*)pos, sizeof(fcb), 2);
    my_cd(dir_tmp);
    //�򿪴������ļ��������ƻ�����������д��
    int fd = my_open(cpfilename);
    if (fd == ERROR)
		return ERROR;
    openfile[fd].count = 0;
	do_write(fd, copybuf, strlen(copybuf), 3);
	my_close(fd);
	printf("paste success!\n");
	return 0;
}

int logmenu() {
    int flag = 1, i = 0,cmdn;
    char s[30], * sp;
    char* username;
    char* password;
    //��¼����
    printf("************************������û���¼**************************\n\n");
    printf("login\t\t�û��� ����\t\t��¼\n");
    printf("register\t�û��� ����\t\tע��\n");
    printf("showuser\t��\t\t\t��ʾ�û��б�\n");
    printf("exit\t\t��\t\t\t�˳�\n\n");
    printf("****************************************************************\n\n");
    while (flag)
    {
        printf("Please input the command:");
        cin.getline(s, 30);
        cmdn = -1;
        if (strcmp(s, ""))//�����Ϊ�գ�ִ������
        {
            sp = strtok(s, " ");
            if (strcmp(sp, "login") == 0)
                cmdn = 0;
            else if (strcmp(sp, "register") == 0)
                cmdn = 1;
            else if (strcmp(sp, "exit") == 0)
                cmdn = 2;
            else if (strcmp(sp, "showuser") == 0)
                cmdn = 3;
            else
                cmdn = 4;
            switch (cmdn) {
            case 0:
                if (i >= 3) {
                    printf("login failed 3 times,exit!\n");
                    return ERROR;
                }
                else{
                    username = strtok(NULL, " ");
                    password = strtok(NULL, " ");
                    if (username == NULL || password == NULL) {
						printf("Please input the right command.\n");
						break;
					}
                    if (login(username, password) == ERROR) {
                        i++;
                        printf("login failed,try again!\n");
                    }
                    else
                        return 0;
                }
                break;
            case 1:
                username = strtok(NULL, " ");
                password = strtok(NULL, " ");
                if (username == NULL || password == NULL) {
                    printf("Please input the right command.\n");
                    break;
                }
                reguser(username, password);
                break;
            case 2:
                return ERROR;
            case 3:
                showuserlist();
				break;
            default:
                printf("Please input the right command.\n");
                break;
            }
        }
    }
    return 0;
}

int filemenu() {
    char cmd[17][10] = { "cd", "mkdir", "rmdir", "ls", "create", "rm", "open", "close", "write", "read", "format","exit","rm-r","popen","setpow","copy","paste"};
    char* s = new char[30], * sp;
    int cmdn, flag = 1, i, father;
    block0* guide;

    //��ʼ��ȫ�ֱ���
    //����������̿ռ�
    myvhard = (unsigned char*)calloc(SIZE, 1);
    //��ʼ���û��򿪱�
    for (i = 0; i < MAXOPENFILE; i++)
    {
        openfile[i].topenfile = 0;
    }
    //����Ŀ¼����Ϊ��ǰĿ¼
    strcpy(currentdir, "\\root\\");
    //��ʼ���ļ����������ļ�����������һ���û��򿪱������ֵ��ָ��ǰ�򿪵��ļ�
    curdir = 0;
    //��ʼ�������������������ʼλ��
    if ((guide = (block0*)myvhard) == NULL) {
        printf("malloc falied!\n");
        return ERROR;
    }
    startp = guide->startblock;
    //������
    buf = (unsigned char*)malloc(BLOCKSIZE);
    if (!buf)
    {
        printf("malloc failed!\n");
        return ERROR;
    }

    //��ʼ���û���Ϣ��
    if (inituserlist() == ERROR) {
        printf("init userlist failed!\n");
        return ERROR;
    }
    if(logmenu() == ERROR)
		return ERROR;
    startsys();
    //printf("In main,openfile[0].length is%d\n",openfile[0].length);
    printf("*********************��ӭ����FAT�ļ�ϵͳ*****************************\n\n");
    printf("������\t\t�������\t\t����˵��\n\n");
    printf("cd\t\tĿ¼��(·����)\t\t�л���ǰĿ¼��ָ��Ŀ¼\n");
    printf("mkdir\t\tĿ¼��\t\t\t�ڵ�ǰĿ¼������Ŀ¼\n");
    printf("rmdir\t\tĿ¼��\t\t\t�ڵ�ǰĿ¼ɾ��ָ��Ŀ¼\n");
    printf("ls\t\t��\t\t\t��ʾ��ǰĿ¼�µ�Ŀ¼���ļ�\n");
    printf("create\t\t�ļ���\t\t\t�ڵ�ǰĿ¼�´���ָ���ļ�\n");
    printf("rm\t\t�ļ���\t\t\t�ڵ�ǰĿ¼��ɾ��ָ���ļ�\n");
    printf("open\t\t�ļ���\t\t\t�ڵ�ǰĿ¼�´�ָ���ļ�\n");
    printf("write\t\t��\t\t\t�ڴ��ļ�״̬�£�д���ļ�\n");
    printf("read\t\t��\t\t\t�ڴ��ļ�״̬�£���ȡ���ļ�\n");
    printf("close\t\t��\t\t\t�ڴ��ļ�״̬�£��رո��ļ�\n");
    printf("setpow\t\t����(true)˽��(false)\t�ڴ��ļ�״̬�£����ø��ļ�Ȩ��\n");
    printf("copy\t\t��\t\t\t�ڴ��ļ�״̬�£������ļ������ƻ�����\n");
    printf("paste\t\t��\t\t\tճ�����Ƶ��ļ�����ǰĿ¼\n");
    printf("format\t\t��\t\t\t��ʽ��\n");
    printf("exit\t\t��\t\t\t�˳�ϵͳ\n\n");
    printf("*********************************************************************\n\n");
    while (flag)
    {
        printf("%s>", openfile[curdir].dir);
        cin.getline(s, 30);
        cmdn = -1;
        if (strcmp(s, ""))//�����Ϊ�գ�ִ������
        {
            sp = strtok(s, " ");
            for (i = 0; i < 18; i++)
            {
                if (strcmp(sp, cmd[i]) == 0)
                {
                    cmdn = i;
                    break;
                }
            }
            switch (cmdn) {
            case 0:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                    my_cd(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 1:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                    my_mkdir(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 2:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                    my_rmdir(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 3:
                if (!openfile[curdir].attribute)
                    my_ls();
                else
                    printf("Please input the right command.\n");
                break;
            case 4:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                    my_create(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 5:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                    my_rm(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 6:
                sp = strtok(NULL, " ");
                if (sp && (!openfile[curdir].attribute))
                {
                    if (strchr(sp, '.')) {//����sp��'.'�״γ��ֵ�λ��
                        int tmp_curdir = curdir;
                        curdir = my_open(sp);
                        if (curdir == ERROR)
                            curdir = tmp_curdir;
                    }
                    else
                        printf("the openfile should have exname.\n");
                }
                else
                    printf("Please input the right command.\n");
                break;
            case 7:
                if (!(openfile[curdir].attribute == 0))
                {
                    father = openfile[curdir].father;
                    my_close(curdir);
                    curdir = father;
                }
                else
                    printf("No files opened.\n");
                break;
            case 8:
                if (!(openfile[curdir].attribute == 0))
                    my_write(curdir);
                else
                    printf("No files opened.\n");
                break;
            case 9:
                if (!(openfile[curdir].attribute == 0))
                    my_read(curdir, openfile[curdir].length);
                else
                    printf("No files opened.\n");
                break;
            case 10:
                //���ж��û��Ƿ�Ϊadministrator
                if (strcmp(curuser->username, ROOTUSER) != 0) {
                    printf("You don't have the right to format!\n");
                    break;
                }
                my_format();
                formatuserlist();
                startsys();
                break;
            case 11:
                if (openfile[curdir].attribute == 0)
                {
                    my_exitsys();
                    flag = 0;
                }
                else
                    printf("Please input the right command.\n");
                break;
            case 12:
                sp = strtok(NULL, " ");
                if (sp && (openfile[curdir].attribute == 0))
                    my_rmdir(sp);
                else
                    printf("Please input the right command.\n");
                break;
            case 13:
                printOpenFile();
                break;
            case 14:
                sp = strtok(NULL, " ");
                if (sp && !(openfile[curdir].attribute == 0))
                    my_setpow(sp, curdir);
                else
                    printf("Please input the right command.\n");
                break;
            case 15:
                if (sp && !(openfile[curdir].attribute == 0))
                    my_copy();
                else
                    printf("Please input the right command.\n");
                break;
            case 16:
                if (openfile[curdir].attribute == 0)
                    my_paste();
                else
                    printf("Please input the right command.\n");
                break;
            default:
                printf("Please input the right command.\n");
                break;
            }
        }
    }
    return 0;
}