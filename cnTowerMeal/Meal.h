#pragma once
#include "stdafx.h"
#include "resource.h"
#include <Commctrl.h>
#include <locale.h>
#include <atltime.h>

#pragma comment(lib, "ComCtl32.lib")

#define WM_SHOWCAL	 2001

/*

默认情况，
周末 <未吃> : 可选菜单 <+早餐> <+午餐> <+晚餐>，RGB(200,0,0)
工作日 <早午> : 可选菜单 <-早餐> <-午餐> <+晚餐>, RGB(0,0,0)
其他状态有, 
<全吃> : 可选菜单 <-早餐> <-午餐> <-晚餐>, RGB(0X19,0X19,0X70)
<早餐> : 可选菜单 <-早餐> <+午餐> <+晚餐>, RGB(0XB3,0XEE,0X3A)
<午餐> : 可选菜单 <+早餐> <-午餐> <+晚餐>, RGB(0X45,0X8B,0X00)
<晚餐> : 可选菜单 <+早餐> <+午餐> <-晚餐>, RGB(0X36,0X64,0X8B)
<早晚> : 可选菜单 <-早餐> <+午餐> <-晚餐>, RGB(0X7A,0X37,0X8B)
<午晚> : 可选菜单 <+早餐> <-午餐> <-晚餐>, RGB(0XD1,0X5F,0XEE)
*/

// 日历定位相关变量
#define	CAL_TOPGAP		5
#define	CAL_LEFTGAP		7
#define	CAL_WIDTH		200
#define TITLE_HEIGHT	21
#define ROW_HEIGHT		21

// 全局变量
HINSTANCE	g_hInst = NULL;
BOOL		g_fShown = FALSE;
WORD		m_iYear, m_iMonth, m_iDay;

// 对话框回调函数
LRESULT CALLBACK CalendarProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 主程序回调函数
LRESULT CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 画日历
void PaintCalc(HWND hDlg);
// 画日历的标头
void PaintCalcTitle(HWND hDlg);
// 绘制日期
void PaintDate(HWND hDlg);
// 绘制某一天
void PaintOneDay(HDC hDC, WORD &iDay, LONG startx, LONG starty, int border);
//判断iYear是不是闰年
static BOOL IsLeapYear(WORD iYear)
{
	return !(iYear % 4) && (iYear % 100) || !(iYear % 400);
}
// 计算iYear,iMonth,iDay对应是星期几
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay);
// 返回iYear年iMonth月的天数
WORD MonthDays(WORD iYear, WORD iMonth);
// 选中某一天，返回选中某天的索引号
WORD SelectDay(HWND hDlg, POINT *pt);
// 显示用餐数据
void ShowMealData(HWND hDlg);

// 用餐相关数据及函数
const float M000 = 0.0;		// 未吃
const float	M100 = 12.5;	// 早餐
const float M010 = 25.0;	// 午餐
const float M001 = (float)(25.0 + 0.1);	// 晚餐
const float M110 = M100 + M010;	// 早午(12.5+25.0=37.5)
const float M101 = M100 + M001;	// 早晚(12.5+25.0+0.1=37.6)
const float M011 = M010 + M001;	// 午晚(25.0+25.0+0.1=50.1)
const float M111 = M100 + M010 + M001;	// 全餐(12.5+25.0+25.0+0.1=62.6)
float	fTotalFee = 887.5;	// 每月总共的餐费
float	fMealMap[31] = { 0.0 };	// 记录每天用餐情况

// 初始化用餐情况
void DefaultMealMap()
{
	for (int i = 1; i <= MonthDays(m_iYear, m_iMonth); i++)	// 遍历每一天
	{
		if (WeekDay(m_iYear, m_iMonth, i) == 6 || WeekDay(m_iYear, m_iMonth, i) == 7)
			fMealMap[i-1] = M000;	// 周末不用餐
		else
			fMealMap[i-1] = M110;	// 工作日早午餐

		fTotalFee -= fMealMap[i-1];	// 扣餐费
	}
}
// 计算剩余晚餐次数
int GetLeftM001()
{
	return (int)(fTotalFee / (M001-(float)0.1));
}
// 加载用餐数据
BOOL LoadMealMap()
{
	FILE *fp;
	char mealFile[30] = { 0 };

	sprintf_s(mealFile, "mealMap%d%02d.txt", m_iYear, m_iMonth);
	if (fopen_s(&fp, mealFile, "r"))	// fopen_s成功返回0，失败返回非0
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
// 保存每天用餐数据
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
