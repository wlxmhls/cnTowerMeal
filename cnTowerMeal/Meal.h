#pragma once
#include "stdafx.h"
#include "resource.h"
#include <Commctrl.h>
#include <locale.h>
#include <atltime.h>

#pragma comment(lib, "ComCtl32.lib")

#define WM_SHOWCAL	 2001

/*

Ĭ�������
��ĩ <δ��> : ��ѡ�˵� <+���> <+���> <+���>��RGB(200,0,0)
������ <����> : ��ѡ�˵� <-���> <-���> <+���>, RGB(0,0,0)
����״̬��, 
<ȫ��> : ��ѡ�˵� <-���> <-���> <-���>, RGB(0X19,0X19,0X70)
<���> : ��ѡ�˵� <-���> <+���> <+���>, RGB(0XB3,0XEE,0X3A)
<���> : ��ѡ�˵� <+���> <-���> <+���>, RGB(0X45,0X8B,0X00)
<���> : ��ѡ�˵� <+���> <+���> <-���>, RGB(0X36,0X64,0X8B)
<����> : ��ѡ�˵� <-���> <+���> <-���>, RGB(0X7A,0X37,0X8B)
<����> : ��ѡ�˵� <+���> <-���> <-���>, RGB(0XD1,0X5F,0XEE)
*/

// ������λ��ر���
#define	CAL_TOPGAP		5
#define	CAL_LEFTGAP		7
#define	CAL_WIDTH		200
#define TITLE_HEIGHT	21
#define ROW_HEIGHT		21

// ȫ�ֱ���
HINSTANCE	g_hInst = NULL;
BOOL		g_fShown = FALSE;
WORD		m_iYear, m_iMonth, m_iDay;

// �Ի���ص�����
LRESULT CALLBACK CalendarProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ������ص�����
LRESULT CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ������
void PaintCalc(HWND hDlg);
// �������ı�ͷ
void PaintCalcTitle(HWND hDlg);
// ��������
void PaintDate(HWND hDlg);
// ����ĳһ��
void PaintOneDay(HDC hDC, WORD &iDay, LONG startx, LONG starty, int border);
//�ж�iYear�ǲ�������
static BOOL IsLeapYear(WORD iYear)
{
	return !(iYear % 4) && (iYear % 100) || !(iYear % 400);
}
// ����iYear,iMonth,iDay��Ӧ�����ڼ�
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay);
// ����iYear��iMonth�µ�����
WORD MonthDays(WORD iYear, WORD iMonth);
// ѡ��ĳһ�죬����ѡ��ĳ���������
WORD SelectDay(HWND hDlg, POINT *pt);
// ��ʾ�ò�����
void ShowMealData(HWND hDlg);

// �ò�������ݼ�����
const float M000 = 0.0;		// δ��
const float	M100 = 12.5;	// ���
const float M010 = 25.0;	// ���
const float M001 = (float)(25.0 + 0.1);	// ���
const float M110 = M100 + M010;	// ����(12.5+25.0=37.5)
const float M101 = M100 + M001;	// ����(12.5+25.0+0.1=37.6)
const float M011 = M010 + M001;	// ����(25.0+25.0+0.1=50.1)
const float M111 = M100 + M010 + M001;	// ȫ��(12.5+25.0+25.0+0.1=62.6)
float	fTotalFee = 887.5;	// ÿ���ܹ��Ĳͷ�
float	fMealMap[31] = { 0.0 };	// ��¼ÿ���ò����

// ��ʼ���ò����
void DefaultMealMap()
{
	for (int i = 1; i <= MonthDays(m_iYear, m_iMonth); i++)	// ����ÿһ��
	{
		if (WeekDay(m_iYear, m_iMonth, i) == 6 || WeekDay(m_iYear, m_iMonth, i) == 7)
			fMealMap[i-1] = M000;	// ��ĩ���ò�
		else
			fMealMap[i-1] = M110;	// �����������

		fTotalFee -= fMealMap[i-1];	// �۲ͷ�
	}
}
// ����ʣ����ʹ���
int GetLeftM001()
{
	return (int)(fTotalFee / (M001-(float)0.1));
}
// �����ò�����
BOOL LoadMealMap()
{
	FILE *fp;
	char mealFile[30] = { 0 };

	sprintf_s(mealFile, "mealMap%d%02d.txt", m_iYear, m_iMonth);
	if (fopen_s(&fp, mealFile, "r"))	// fopen_s�ɹ�����0��ʧ�ܷ��ط�0
		return FALSE;
	for (int i=1; i<=MonthDays(m_iYear,m_iMonth); i++) {
		char tmpBuffer[10] = { 0 };
		if (fgets(tmpBuffer,10,fp) == NULL)
			return FALSE;
		fMealMap[i-1] = (float)atof(tmpBuffer);
		if (fMealMap[m_iDay - 1] == M001 || fMealMap[m_iDay - 1] == M101 || fMealMap[m_iDay - 1] == M011 || fMealMap[m_iDay - 1] == M111)
			fTotalFee -= (fMealMap[i - 1] - (float)0.1);
		else
			fTotalFee -= fMealMap[i - 1];
	}

	fclose(fp);
	return TRUE;
}
// ����ÿ���ò�����
void SaveMealMap()
{
	FILE *fp;
	char mealFile[30] = { 0 };

	sprintf_s(mealFile, "mealMap%d%02d.txt", m_iYear, m_iMonth);
	fopen_s(&fp, mealFile, "w");
	for (int i=1; i<=MonthDays(m_iYear,m_iMonth); i++) {
		char tmpBuffer[10];
		sprintf_s(tmpBuffer, "%.1f\n", fMealMap[i-1]);
		fwrite(tmpBuffer, 1, strlen(tmpBuffer), fp);
	}

	fclose(fp);
}
