#include "Meal.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_hInst = hInstance;
	setlocale(LC_ALL, "");

	WNDCLASSEX wndclass;
	ZeroMemory(&wndclass, sizeof(wndclass));
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = g_hInst;
	wndclass.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_CAL));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = _T("minCal");
	wndclass.hIconSm = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_CAL));
	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("窗口注册失败！"), TEXT("CAL"), MB_ICONERROR);
		return 0;
	}
	
	SYSTEMTIME	systime;
	::GetSystemTime(&systime);
	m_iYear = systime.wYear;
	m_iMonth = systime.wMonth;
	m_iDay = systime.wDay;
	// 初始化用餐数据
	if (!LoadMealMap())
		DefaultMealMap();

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CALENDAR), NULL, (DLGPROC)CalendarProc);

	// 主消息循环:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

// 主程序回调函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_CAL), IMAGE_ICON, 0, 0, LR_SHARED + LR_DEFAULTSIZE);
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);        //发送消息
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

			return TRUE;
		}
		break;
	}

	return FALSE;
}

// 对话框回调函数
inline LRESULT CALLBACK CalendarProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				EndDialog(hDlg, TRUE);
				return TRUE;
			}
			break;
		case IDM_EATZAC:
			if (fMealMap[m_iDay-1]==M100 || fMealMap[m_iDay-1]==M101 || fMealMap[m_iDay-1]==M110 || fMealMap[m_iDay-1]==M111)
			{
				// <-早餐>
				fMealMap[m_iDay-1] -= M100;
				fTotalFee += M100;
			}
			else {
				// <+早餐>
				fMealMap[m_iDay-1] += M100;
				fTotalFee -= M100;
			}
			ShowMealData(hDlg);
			SaveMealMap();
			return TRUE;
			break;
		case IDM_EATWUC:
			if (fMealMap[m_iDay-1]==M010 || fMealMap[m_iDay-1]==M011 || fMealMap[m_iDay-1]==M110 || fMealMap[m_iDay-1]==M111)
			{
				// <-午餐>
				fMealMap[m_iDay-1] -= M010;
				fTotalFee += M010;
			}
			else {
				// <+午餐>
				fMealMap[m_iDay-1] += M010;
				fTotalFee -= M010;
			}
			ShowMealData(hDlg);
			SaveMealMap();
			return TRUE;
			break;
		case IDM_EATWAC:
			if (fMealMap[m_iDay-1]==M001 || fMealMap[m_iDay-1]==M101 || fMealMap[m_iDay-1]==M011 || fMealMap[m_iDay-1]==M111)
			{
				// <-晚餐>
				fMealMap[m_iDay-1] -= M001;
				fTotalFee += (M001-(float)0.1);
			}
			else {
				// <+晚餐>
				fMealMap[m_iDay-1] += M001;
				fTotalFee -= (M001-(float)0.1);

				//wchar_t szTmp[50] = { 0 };
				//char strTmp[50] = { 0 };
				//sprintf_s(strTmp, "fMealMap[m_iDay-1]: %.1f, fTotalFee: %.1f", fMealMap[m_iDay - 1], fTotalFee);
				//MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 50, szTmp, 50);
				//MessageBox(NULL, szTmp, _T("OK"), MB_OK);
			}
			ShowMealData(hDlg);
			SaveMealMap();
			return TRUE;
			break;
		}
		return FALSE;
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			GetCursorPos(&pt);
			m_iDay = (WORD)SelectDay(hDlg, &pt);
			if (m_iDay == 0) {
				return TRUE;
			}
			PaintCalc(hDlg);
			if (uMsg == WM_RBUTTONDOWN) {
				CString menu1, menu2, menu3, menu4;
				if (fMealMap[m_iDay-1] == M000)
				{
					menu1.Format(_T("+早餐"));
					menu2.Format(_T("+午餐"));
					menu3.Format(_T("+晚餐"));
					menu4.Format(_T("*未吃"));
				}
				else if (fMealMap[m_iDay-1] == M110)
				{
					menu1.Format(_T("-早餐"));
					menu2.Format(_T("-午餐"));
					menu3.Format(_T("+晚餐"));
					menu4.Format(_T("*早午"));
				}
				else if (fMealMap[m_iDay-1] == M111)
				{
					menu1.Format(_T("-早餐"));
					menu2.Format(_T("-午餐"));
					menu3.Format(_T("-晚餐"));
					menu4.Format(_T("*全吃"));
				}
				else if (fMealMap[m_iDay-1] == M100)
				{
					menu1.Format(_T("-早餐"));
					menu2.Format(_T("+午餐"));
					menu3.Format(_T("+晚餐"));
					menu4.Format(_T("*早餐"));
				}
				else if (fMealMap[m_iDay-1] == M010)
				{
					menu1.Format(_T("+早餐"));
					menu2.Format(_T("-午餐"));
					menu3.Format(_T("+晚餐"));
					menu4.Format(_T("*午餐"));
				}
				else if (fMealMap[m_iDay-1] == M001)
				{
					menu1.Format(_T("+早餐"));
					menu2.Format(_T("+午餐"));
					menu3.Format(_T("-晚餐"));
					menu4.Format(_T("*晚餐"));
				}
				else if (fMealMap[m_iDay-1] == M101)
				{
					menu1.Format(_T("-早餐"));
					menu2.Format(_T("+午餐"));
					menu3.Format(_T("-晚餐"));
					menu4.Format(_T("*早晚"));
				}
				else if (fMealMap[m_iDay-1] == M011)
				{
					menu1.Format(_T("+早餐"));
					menu2.Format(_T("-午餐"));
					menu3.Format(_T("-晚餐"));
					menu4.Format(_T("*午晚"));
				}

				ClientToScreen(hDlg, &pt);
				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu, MF_STRING, IDM_EATZAC, menu1);
				AppendMenu(hMenu, MF_STRING, IDM_EATWUC, menu2);
				AppendMenu(hMenu, MF_STRING, IDM_EATWAC, menu3);
				AppendMenu(hMenu, MF_STRING|MF_DISABLED|MF_GRAYED, IDM_EATNOW, menu4);
				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, NULL, hDlg, NULL);
			}
			return TRUE;
		}
		break;
	case WM_WINDOWPOSCHANGED:
		PostMessage(hDlg, WM_SHOWCAL, 0, 0);
		return TRUE;
		break;
	case WM_SHOWCAL:
		PaintCalc(hDlg);
		ShowMealData(hDlg);
		return TRUE;
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return FALSE;
}

// 画日历
void PaintCalc(HWND hDlg)
{
	PaintCalcTitle(hDlg);
	PaintDate(hDlg);
}

// 画日历的标头
void PaintCalcTitle(HWND hDlg)
{
	RECT		rect;
	HDC			hDC, hSysDC;
	LOGFONT		lf;
	HFONT		hFont;
	PAINTSTRUCT	ps;
	TEXTMETRIC	tm;

	hSysDC = BeginPaint(GetDlgItem(hDlg, IDC_EDIT1), &ps);
	GetTextMetrics(hSysDC, &tm);

	GetClientRect(hDlg, &rect);
	rect.left += CAL_LEFTGAP;
	rect.top += CAL_TOPGAP;
	rect.right = rect.left + CAL_WIDTH;
	rect.bottom = rect.top + TITLE_HEIGHT;

	hDC = GetDC(hDlg);
	FillRect(hDC, &rect, CreateSolidBrush(RGB(0xCD, 0xC5, 0xBF)));
	SetBkColor(hDC, RGB(0xCD, 0xC5, 0xBF));
	SetTextColor(hDC, RGB(0x30, 0x30, 0x30));
	
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = tm.tmHeight;
	lf.lfWidth = 0;
	hFont = CreateFontIndirect(&lf);
	SelectObject(hDC, hFont);

	int startx = rect.left + (rect.right - rect.left) / 7 - tm.tmHeight;
	wchar_t sztext[][3] = { _T("一"), _T("二"), _T("三"), _T("四"), _T("五"), _T("六"), _T("日") };
	for (int i = 0; i<7; i++)
	{
		TextOut(hDC, startx, rect.top+1, sztext[i], lstrlenW(sztext[i]));
		startx += (rect.right - rect.left) / 7;
	}

	EndPaint(hDlg, &ps);
	DeleteObject(hFont);
}

// 绘制日期
void PaintDate(HWND hDlg)
{
	RECT		rect;
	HDC			hDC, hSysDC;
	WORD		day = 1;
	PAINTSTRUCT	ps;
	TEXTMETRIC	tm;
	LOGFONT		lf;
	HFONT		hFont;

	hSysDC = BeginPaint(GetDlgItem(hDlg, IDC_EDIT1), &ps);
	GetTextMetrics(hSysDC, &tm);

	// 设置一下日期区域的背景色
	hDC = GetDC(hDlg);
	GetClientRect(hDlg, &rect);
	rect.left += CAL_LEFTGAP;
	rect.right = rect.left + CAL_WIDTH;
	rect.top += CAL_TOPGAP + TITLE_HEIGHT;
	rect.bottom = rect.top + ROW_HEIGHT * 6;
	FillRect(hDC, &rect, CreateSolidBrush(RGB(255, 255, 255)));

	// 设置字体大小
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = tm.tmHeight;
	lf.lfWidth = 0;
	hFont = CreateFontIndirect(&lf);
	SelectObject(hDC, hFont);

	for (int i=0; i<6; i++, rect.top+=ROW_HEIGHT)	// 6行
	{
		int startx = rect.left + (rect.right-rect.left)/7 - tm.tmHeight;

		for (int j=1; j<=7; j++, startx+=((rect.right-rect.left)/7))	// 7列
		{
			if (j<WeekDay(m_iYear,m_iMonth,1) && i==0)	// 第1行
				continue;
			if (day > MonthDays(m_iYear,m_iMonth))
				break;
			int border = rect.left + j * (rect.right-rect.left)/7;
			PaintOneDay(hDC, day, startx, rect.top+1, border);
		}
	}

	EndPaint(hDlg, &ps);
	DeleteObject(hFont);
}

// 绘制某一天
void PaintOneDay(HDC hDC, WORD &iDay, LONG startx, LONG starty, int border)
{
	wchar_t text[4];
	wsprintf(text, _T("%02d"), iDay);

	// 绘制当天或周末
	if (iDay==m_iDay)
	{
		RECT rc;
		rc.left = startx-2;
		rc.top = starty-1;
		rc.right = border;
		rc.bottom = starty+ROW_HEIGHT;

		FillRect(hDC, &rc, CreateSolidBrush(RGB(150, 150, 230)));
			
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkColor(hDC, RGB(150, 150, 230));

		TextOut(hDC, startx, starty, text, 2);
		SetBkColor(hDC, RGB(255, 255, 255));
		SetTextColor(hDC, RGB(0, 0, 0));
	}
	else
	{
		if (fMealMap[iDay - 1] == M000)
			SetTextColor(hDC, RGB(200, 0, 0));	// 未吃
		else if (fMealMap[iDay - 1] == M110)
			SetTextColor(hDC, RGB(0, 0, 0));	// 早午
		else if (fMealMap[iDay - 1] == M111)
			SetTextColor(hDC, RGB(0X19, 0X19, 0X70));	// 全吃
		else if (fMealMap[iDay - 1] == M100)
			SetTextColor(hDC, RGB(0XB3, 0XEE, 0X3A));	// 早餐
		else if (fMealMap[iDay - 1] == M010)
			SetTextColor(hDC, RGB(0X45, 0X8B, 0X00));	// 午餐
		else if (fMealMap[iDay - 1] == M001)
			SetTextColor(hDC, RGB(0X36, 0X64, 0X8B));	// 晚餐
		else if (fMealMap[iDay - 1] == M101)
			SetTextColor(hDC, RGB(0X7A, 0X37, 0X8B));	// 早晚
		else if (fMealMap[iDay - 1] == M011)
			SetTextColor(hDC, RGB(0XD1, 0X5F, 0XEE));	// 午晚
		
		TextOut(hDC, startx, starty, text, 2);
		SetBkColor(hDC, RGB(255, 255, 255));
		SetTextColor(hDC, RGB(0, 0, 0));
	}

	iDay++;
}

// 计算iYear,iMonth,iDay对应是星期几
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay)
{
	// 数组元素monthday[i]表示第i个月以前的总天数除以7的余数
	WORD monthday[] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
	WORD iDays = (iYear - 1) % 7 + (iYear - 1) / 4 - (iYear - 1) / 100 + (iYear - 1) / 400;

	iDays += (monthday[iMonth - 1] + iDay);
	// 如果iYear是闰年
	if (IsLeapYear(iYear) && iMonth>2)
		iDays++;

	iDays = iDays % 7;
	// 返回: 1,2,3,4,5,6,7 表示 一、二、三、四、五、六、日
	if (iDays == 0)
		return 7;
	else
		return iDays;
}

// 返回iYear年iMonth月的天数
WORD MonthDays(WORD iYear, WORD iMonth)
{
	switch (iMonth)
	{
	case 1:	case 3:	case 5:	case 7:	case 8:	case 10: case 12:
		return 31;
		break;
	case 4:	case 6:	case 9:	case 11:
		return 30;
		break;
	case 2:
		//如果是闰年
		if (IsLeapYear(iYear))
			return 29;
		else
			return 28;
		break;
	}
	return 0;
}

// 选中某一天，返回选中某天的索引号
WORD SelectDay(HWND hDlg, POINT *pt)
{
	RECT	rect;
	GetClientRect(hDlg, &rect);
	ScreenToClient(hDlg, pt);

	LONG iRow = (pt->y - rect.top - TITLE_HEIGHT - CAL_TOPGAP) / ROW_HEIGHT;
	LONG iCol = 7 * (pt->x - rect.left - CAL_LEFTGAP) / CAL_WIDTH + 1;

	WORD startcol, endrow, endcol;

	startcol = WeekDay(m_iYear, m_iMonth, 1);
	endcol = WeekDay(m_iYear, m_iMonth, MonthDays(m_iYear, m_iMonth));

	endrow = (MonthDays(m_iYear, m_iMonth) + startcol - 1) / 7;
	if ((iRow == 0 && iCol<startcol) || (iRow == endrow && iCol>endcol) || iRow > endrow || iCol > 7) {
		return 0;
	}

	return (WORD)(iRow * 7 + iCol + 1 - startcol);
}

// 显示用餐数据
void ShowMealData(HWND hDlg)
{
	wchar_t	szTmp[30] = { 0 };
	char	strTmp[30] = { 0 };

	wsprintf(szTmp, _T("887.5"));
	SetDlgItemText(hDlg, IDC_EDIT1, szTmp);	// 总餐费

	sprintf_s(strTmp, "%.1f", fTotalFee);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT2, szTmp);	// 剩余餐费

	wsprintf(szTmp, _T("%d"), GetLeftM001());
	SetDlgItemText(hDlg, IDC_EDIT3, szTmp);	// 可用晚餐次数

	sprintf_s(strTmp, "早餐 %.1f 元\r\n午晚 %.1f 元", M100, M010);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT4, szTmp);	// 各餐价格

	SYSTEMTIME	systime;
	::GetSystemTime(&systime);
	sprintf_s(strTmp, "今天是%d年%d月%d日", systime.wYear, systime.wMonth, systime.wDay);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT5, szTmp);
}