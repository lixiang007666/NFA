/*将正规式变成NFA的算法*/
/*
*  (a|b)*|b要改成: ((a|b)*)|b
*  (a*b)*ba(a|b)* 可以运行
*/

#include <iostream>
#include<cstdio>
#include "conio.h"
#include "malloc.h"
#include "string.h"
using namespace std;
#define X 0  /*初始状态X的宏定义*/
#define Y -1  /*最终状态Y的宏定义*/
#define TRUE   1
#define FALSE  0

int state = 1; /*全局变量State,表示当前的状态次序*/


/*
*  有向弧:start ---a---> end(从start状态,输入a后,变成状态end);
*  在本程序里将有向弧用链表的数据结构表示;
*  其中有向弧结构体定义如下：
*/

typedef struct YouXiangHu
{
    int start;    /*有向弧的起始start状态*/
    int end;      /*有向弧的到达的end状态*/
    char ShiZi[32];    /*从start状态到end状态的转换式*/
    int IsSingleChar;  /*判断此有向弧转换式是否是单个字符*/
    struct YouXiangHu *next;   /*指向下一个节点的指针域*/
}Hu;

/*
*  文件输出临时格式结构体.
*/

typedef struct File_temp_Record
{
    int start;  /*有向弧的起始start状态*/
    char ShiZi[32];  /*从start状态到end状态的转换式*/
    int end;    /*有向弧的到达的end状态*/
} Record;

/*
*  函数预先声明
*
*/
void MainDoneProcess(Hu *head,char *ZhengGuiShi);

void SubZhengGuiShi(Hu *pre,Hu *position);

void SubString(int sub_start,int sub_end,int fa_start,char *sub_ShiZi,char *ShiZi);

int JudgeIsOver(Hu **pre,Hu **position);

void FuZhi(Hu *temp,int start,int end,char *ShiZi,Hu *next);

void Connect(int start,int end,char* SubShiZi,char *ShiZi,Hu *pre,Hu *position);

void BiBao(int start,int end,char *SubShiZi,Hu *pre,Hu *position);

void Huo(int start,int end,char *SubShiZi,char *ShiZi,Hu *pre,Hu *position);

void PrintResult(Hu *head);

int IsSingleChar(char *string);

void Record_to_file(Hu *head);

/*
*  main()
*/
 int main()
{
    char ZhengGuiShi[32];
    Hu *head = (Hu *)malloc(sizeof(Hu));

    printf("请输入正规式: \n");
    scanf("%s",ZhengGuiShi);
    printf("\n");

    /* 此处最好加上对输入的正规式进行检查的过程 CheckInPut();*/

    MainDoneProcess(head,ZhengGuiShi);

    /* 将结果写入文件,保存在磁盘上 */
    Record_to_file(head);

    PrintResult(head);
    return 0;
}

/*
*  MainDoneProcess()是主要的处理控制函数.
*  参数是用户输入的正规式;
*  隐式返回一个有向弧的链表头指针.
*/
void MainDoneProcess(Hu *head,char *ZhengGuiShi)
{
    Hu *pre,*position;
    Hu *first = (Hu *)malloc(sizeof(Hu));

    FuZhi(first,X,Y,ZhengGuiShi,NULL);

    head->next = first;
    pre = head;

    while(TRUE)
    {
        position = pre->next;
        if(JudgeIsOver(&pre,&position) == TRUE)
            break;

        SubZhengGuiShi(pre,position);
    }
}

/*
*  SubZhengGuiShi()是对正规式从左到右扫描,
*  逐个从左到右抽取正规式子中子式
*  具体情况如下注释:
*/
void SubZhengGuiShi(Hu *pre,Hu *position)
{
    int No_Left_KuoHao=0,i;
    char ShiZi[32],sub_ShiZi[32];

    strcpy(ShiZi,position->ShiZi);
/*-------------当首字符是'('时的情况：-------------*/
    if(ShiZi[0]=='(')
    {
        No_Left_KuoHao ++;

        for(i=1;ShiZi[i]!='\0';i++)
        {
            if(No_Left_KuoHao==0)
                break;
            if(ShiZi[i]=='(')
                No_Left_KuoHao ++;
            else if(ShiZi[i]==')')
                No_Left_KuoHao --;

        }
        /*当首字符是'('时,搜索到')'后是否有'*'闭包运算*/
        if(ShiZi[i]=='*')
        {
            if(ShiZi[i+1]=='\0')   /*当搜索完形如: "(a*b)*" 之后到达字符串结尾*/
            {
                SubString(1,i-2,i+1,sub_ShiZi,ShiZi);
                BiBao(position->start,position->end,sub_ShiZi,pre,position);
                return;
            }
            else{      /*当搜索完形如："(a*b)*"之后还有字符或字符串 */
                SubString(0,i,i+1,sub_ShiZi,ShiZi);
                Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
                return;
            }
        }
            /*当首字符是'('时,搜索到')'后是否有'|'或运算 eg:"(a*b)|b" */
        else if(ShiZi[i]=='|')
        {
            SubString(1,i-2,i+1,sub_ShiZi,ShiZi);
            Huo(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }

        else{    /*首字符是'('时,搜索到')'后是字母 eg:"(a*b)b" */
            SubString(1,i-2,i,sub_ShiZi,ShiZi);
            if(strcmp(ShiZi,"")==0)
                strcpy(position->ShiZi,sub_ShiZi);
            else
                Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
    }
/*-------------当首字符字是母时的情况：-------------*/
    if(ShiZi[0]>='a'&&ShiZi[0]<='z'|| ShiZi[0]>='A'&&ShiZi[0]<='Z')
    {
        /*当首字符是字母时,并且后面没有'*'运算和'或'运算 eg:"ab"*/
        if(ShiZi[1]!='*'&&ShiZi[1]!='|')
        {
            SubString(0,0,1,sub_ShiZi,ShiZi);
            Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
            /*当首字符是字母时,并且后面是'*'运算 eg:"a*b"*/
        else if(ShiZi[1]=='*')
        {
            if(ShiZi[2]=='\0')
            {
                SubString(0,0,2,sub_ShiZi,ShiZi);
                BiBao(position->start,position->end,sub_ShiZi,pre,position);
                return;
            }
            else
            {
                SubString(0,1,2,sub_ShiZi,ShiZi);
                Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
                return;
            }
        }
            /*当首字符是字母时,并且后面是'或'运算 eg:"a|b" */
        else if(ShiZi[1]=='|')
        {
            SubString(0,0,2,sub_ShiZi,ShiZi);
            Huo(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
    }
}

/*
*  SubString()按一定的要求抽取子式:
*  其中子式是从sub_start开始;到sub_end结束;
*  而原式从fa_start到结尾.
*/

void SubString(int sub_start,int sub_end,int fa_start,char *sub_ShiZi,char *ShiZi)
{
    int i=0,ShiZi_flag = TRUE;
    for(i=sub_start;i<=sub_end;i++)
    {
        sub_ShiZi[i-sub_start] = ShiZi[i];
    }
    sub_ShiZi[i-sub_start] = '\0';
    for(i=fa_start;ShiZi[i]!='\0';i++)
    {
        ShiZi_flag = FALSE;
        ShiZi[i-fa_start] = ShiZi[i];
    }
    ShiZi[i-fa_start] = '\0';
    if(ShiZi_flag) strcpy(ShiZi,"");
}


/*
*  JudgeIsOver()函数的功能是:
*      判断所有有向弧的转换式子是否是单个字符
*  如果碰到的有向弧节点的转换式不是单个字符,则返回此节点的头指针
*  和前一个节点的头指针。
*/
int JudgeIsOver(Hu **pre,Hu **position)
{
    while((*position) != NULL)
    {
        if((*position)->IsSingleChar != 1)
        {
            return FALSE;   /*还有转换式不是单字符的有向弧*/
        }
        else{
            (*pre) = *position;
            *position = (*position)->next;
        }
    }
    return TRUE;
}


/*
*  FuZhi()函数的功能是:给一个已有的Hu结构体变量赋值
*/
void FuZhi(Hu *temp,int start,int end,char *ShiZi,Hu *next)
{
    temp->start = start;
    temp->end = end;
    strcpy(temp->ShiZi,ShiZi);
    temp->IsSingleChar =  IsSingleChar(temp->ShiZi);
    temp->next = next;
}

/*
*  Connect()函数是:连接运算;
*  将新生成的有向弧插入到合适的位置;
*  最后释放没有用的有向弧节点.
*/
void Connect(int start,int end,char* SubShiZi,char *ShiZi,Hu *pre,Hu *position)
{
    Hu *Pre_Hu = (Hu *)malloc(sizeof(Hu));
    Hu *Later_Hu = (Hu *)malloc(sizeof(Hu));

    FuZhi(Pre_Hu,start,state,SubShiZi,Later_Hu);

    FuZhi(Later_Hu,state,end,ShiZi,position->next);

    state ++;

    pre->next = Pre_Hu;
    free(position);
}
/*
*  BiBao()函数是:实现闭包运算:
*  生成新的有向弧,并将弧插入到合适位置
*  最后释放没有用的有向弧节点.
*/
void BiBao(int start,int end,char *SubShiZi,Hu *pre,Hu *position)
{
    Hu *Bibao_1 = (Hu *)malloc(sizeof(Hu));
    Hu *Bibao_2 = (Hu *)malloc(sizeof(Hu));
    Hu *Bibao_3 = (Hu *)malloc(sizeof(Hu));

    FuZhi(Bibao_1,start,state,"#",Bibao_2);

    FuZhi(Bibao_2,state,state,SubShiZi,Bibao_3);

    FuZhi(Bibao_3,state,end,"#",position->next);

    state ++;
    pre->next = Bibao_1;
    free(position);
    /*position = NULL;*/
}

/*
*  Huo()函数是:实现或运算:
*  生成新的有向弧,并将弧插入到合适位置
*  最后释放没有用的有向弧节点.
*/
void Huo(int start,int end,char *SubShiZi,char *ShiZi,Hu *pre,Hu *position)
{
    Hu *huo1 = (Hu *)malloc(sizeof(Hu));
    Hu *huo2 = (Hu *)malloc(sizeof(Hu));

    FuZhi(huo1,start,end,SubShiZi,huo2);
    FuZhi(huo2,start,end,ShiZi,NULL);

    pre->next = huo1;
    huo2->next = position->next;

    free(position);
}

/*
*  PrintResult()函数打印结果:
*  输入的是链表头指针;
*  释放malloc()申请的资源;
*  用标准输出输出结果.
*/
void PrintResult(Hu *head)
{
    Hu *point = head->next,*temp;
    free(head);
    printf("开始节点:\t有向弧输入:\t到达节点:\n");
    while(point != NULL)
    {
        point->start==X ? printf("From:  S\t"):printf("From:  %d\t",point->start);
        printf("Input: %s\t",point->ShiZi);
        point->end == Y ? printf("To:  E\n"):printf("To:  %d\n",point->end);
        temp = point;
        point = point->next;
        free(temp);
    }
}

/*
*  IsSingleChar()函数是:判断字符串是不是单个字符;
*  如果是返回1;否则返回字符个数.
*/
int IsSingleChar(char *string)
{
    int i=0;
    while(string[i++]!='\0');
    return i-1;
}

/* 
*  Record_to_file()函数:将结论保存在外存的文件上
*/
void Record_to_file(Hu *head)
{
    FILE *fp;
    Hu * temp = head->next;
    Record * record = (Record *)malloc(sizeof(Record));

    if((fp=fopen("NFA_Result.txt","r+"))==NULL)
    {
        printf("Cannot open file normally and strike any key exit!");
        getch();
        return;
    }
    while(temp != NULL)
    {
        record->start = temp->start;
        strcpy(record->ShiZi,temp->ShiZi);
        record->end = temp->end;

        fprintf(fp,"%d %s %d\n",record->start,record->ShiZi,record->end);

        temp = temp->next;
    }
    fclose(fp);
}
