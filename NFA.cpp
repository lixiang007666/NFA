/*������ʽ���NFA���㷨*/
/*
*  (a|b)*|bҪ�ĳ�: ((a|b)*)|b
*  (a*b)*ba(a|b)* ��������
*/

#include <iostream>
#include<cstdio>
#include "conio.h"
#include "malloc.h"
#include "string.h"
using namespace std;
#define X 0  /*��ʼ״̬X�ĺ궨��*/
#define Y -1  /*����״̬Y�ĺ궨��*/
#define TRUE   1
#define FALSE  0

int state = 1; /*ȫ�ֱ���State,��ʾ��ǰ��״̬����*/


/*
*  ����:start ---a---> end(��start״̬,����a��,���״̬end);
*  �ڱ������ｫ��������������ݽṹ��ʾ;
*  �������򻡽ṹ�嶨�����£�
*/

typedef struct YouXiangHu
{
    int start;    /*���򻡵���ʼstart״̬*/
    int end;      /*���򻡵ĵ����end״̬*/
    char ShiZi[32];    /*��start״̬��end״̬��ת��ʽ*/
    int IsSingleChar;  /*�жϴ�����ת��ʽ�Ƿ��ǵ����ַ�*/
    struct YouXiangHu *next;   /*ָ����һ���ڵ��ָ����*/
}Hu;

/*
*  �ļ������ʱ��ʽ�ṹ��.
*/

typedef struct File_temp_Record
{
    int start;  /*���򻡵���ʼstart״̬*/
    char ShiZi[32];  /*��start״̬��end״̬��ת��ʽ*/
    int end;    /*���򻡵ĵ����end״̬*/
} Record;

/*
*  ����Ԥ������
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

    printf("����������ʽ: \n");
    scanf("%s",ZhengGuiShi);
    printf("\n");

    /* �˴���ü��϶����������ʽ���м��Ĺ��� CheckInPut();*/

    MainDoneProcess(head,ZhengGuiShi);

    /* �����д���ļ�,�����ڴ����� */
    Record_to_file(head);

    PrintResult(head);
    return 0;
}

/*
*  MainDoneProcess()����Ҫ�Ĵ�����ƺ���.
*  �������û����������ʽ;
*  ��ʽ����һ�����򻡵�����ͷָ��.
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
*  SubZhengGuiShi()�Ƕ�����ʽ������ɨ��,
*  ��������ҳ�ȡ����ʽ������ʽ
*  �����������ע��:
*/
void SubZhengGuiShi(Hu *pre,Hu *position)
{
    int No_Left_KuoHao=0,i;
    char ShiZi[32],sub_ShiZi[32];

    strcpy(ShiZi,position->ShiZi);
/*-------------�����ַ���'('ʱ�������-------------*/
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
        /*�����ַ���'('ʱ,������')'���Ƿ���'*'�հ�����*/
        if(ShiZi[i]=='*')
        {
            if(ShiZi[i+1]=='\0')   /*������������: "(a*b)*" ֮�󵽴��ַ�����β*/
            {
                SubString(1,i-2,i+1,sub_ShiZi,ShiZi);
                BiBao(position->start,position->end,sub_ShiZi,pre,position);
                return;
            }
            else{      /*�����������磺"(a*b)*"֮�����ַ����ַ��� */
                SubString(0,i,i+1,sub_ShiZi,ShiZi);
                Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
                return;
            }
        }
            /*�����ַ���'('ʱ,������')'���Ƿ���'|'������ eg:"(a*b)|b" */
        else if(ShiZi[i]=='|')
        {
            SubString(1,i-2,i+1,sub_ShiZi,ShiZi);
            Huo(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }

        else{    /*���ַ���'('ʱ,������')'������ĸ eg:"(a*b)b" */
            SubString(1,i-2,i,sub_ShiZi,ShiZi);
            if(strcmp(ShiZi,"")==0)
                strcpy(position->ShiZi,sub_ShiZi);
            else
                Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
    }
/*-------------�����ַ�����ĸʱ�������-------------*/
    if(ShiZi[0]>='a'&&ShiZi[0]<='z'|| ShiZi[0]>='A'&&ShiZi[0]<='Z')
    {
        /*�����ַ�����ĸʱ,���Һ���û��'*'�����'��'���� eg:"ab"*/
        if(ShiZi[1]!='*'&&ShiZi[1]!='|')
        {
            SubString(0,0,1,sub_ShiZi,ShiZi);
            Connect(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
            /*�����ַ�����ĸʱ,���Һ�����'*'���� eg:"a*b"*/
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
            /*�����ַ�����ĸʱ,���Һ�����'��'���� eg:"a|b" */
        else if(ShiZi[1]=='|')
        {
            SubString(0,0,2,sub_ShiZi,ShiZi);
            Huo(position->start,position->end,sub_ShiZi,ShiZi,pre,position);
            return;
        }
    }
}

/*
*  SubString()��һ����Ҫ���ȡ��ʽ:
*  ������ʽ�Ǵ�sub_start��ʼ;��sub_end����;
*  ��ԭʽ��fa_start����β.
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
*  JudgeIsOver()�����Ĺ�����:
*      �ж��������򻡵�ת��ʽ���Ƿ��ǵ����ַ�
*  ������������򻡽ڵ��ת��ʽ���ǵ����ַ�,�򷵻ش˽ڵ��ͷָ��
*  ��ǰһ���ڵ��ͷָ�롣
*/
int JudgeIsOver(Hu **pre,Hu **position)
{
    while((*position) != NULL)
    {
        if((*position)->IsSingleChar != 1)
        {
            return FALSE;   /*����ת��ʽ���ǵ��ַ�������*/
        }
        else{
            (*pre) = *position;
            *position = (*position)->next;
        }
    }
    return TRUE;
}


/*
*  FuZhi()�����Ĺ�����:��һ�����е�Hu�ṹ�������ֵ
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
*  Connect()������:��������;
*  �������ɵ����򻡲��뵽���ʵ�λ��;
*  ����ͷ�û���õ����򻡽ڵ�.
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
*  BiBao()������:ʵ�ֱհ�����:
*  �����µ�����,���������뵽����λ��
*  ����ͷ�û���õ����򻡽ڵ�.
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
*  Huo()������:ʵ�ֻ�����:
*  �����µ�����,���������뵽����λ��
*  ����ͷ�û���õ����򻡽ڵ�.
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
*  PrintResult()������ӡ���:
*  �����������ͷָ��;
*  �ͷ�malloc()�������Դ;
*  �ñ�׼���������.
*/
void PrintResult(Hu *head)
{
    Hu *point = head->next,*temp;
    free(head);
    printf("��ʼ�ڵ�:\t��������:\t����ڵ�:\n");
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
*  IsSingleChar()������:�ж��ַ����ǲ��ǵ����ַ�;
*  ����Ƿ���1;���򷵻��ַ�����.
*/
int IsSingleChar(char *string)
{
    int i=0;
    while(string[i++]!='\0');
    return i-1;
}

/* 
*  Record_to_file()����:�����۱����������ļ���
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
