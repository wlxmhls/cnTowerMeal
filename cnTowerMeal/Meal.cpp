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
		MessageBox(NULL, TEXT("����ע��ʧ�ܣ�"), TEXT("CAL"), MB_ICONERROR);
		return 0;
	}
	
	SYSTEMTIME	systime;
	::GetSystemTime(&systime);
	m_iYear = systime.wYear;
	m_iMonth = systime.wMonth;
	m_iDay = systime.wDay;
	// ��ʼ���ò�����
	if (!LoadMealMap())
		DefaultMealMap();

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CALENDAR), NULL, (DLGPROC)CalendarProc);

	// ����Ϣѭ��:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

// ������ص�����
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_CAL), IMAGE_ICON, 0, 0, LR_SHARED + LR_DEFAULTSIZE);
			SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);        //������Ϣ
			SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

			return TRUE;
		}
		break;
	}

	return FALSE;
}

// �Ի���ص�����
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
				// <-���>
				fMealMap[m_iDay-1] -= M100;
				fTotalFee += M100;
			}
			else {
				// <+���>
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
				// <-���>
				fMealMap[m_iDay-1] -= M010;
				fTotalFee += M010;
			}
			else {
				// <+���>
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
				// <-���>
				fMealMap[m_iDay-1] -= M001;
				fTotalFee += (M001-(float)0.1);
			}
			else {
				// <+���>
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
					menu1.Format(_T("+���"));
					menu2.Format(_T("+���"));
					menu3.Format(_T("+���"));
					menu4.Format(_T("*δ��"));
				}
				else if (fMealMap[m_iDay-1] == M110)
				{
					menu1.Format(_T("-���"));
					menu2.Format(_T("-���"));
					menu3.Format(_T("+���"));
					menu4.Format(_T("*����"));
				}
				else if (fMealMap[m_iDay-1] == M111)
				{
					menu1.Format(_T("-���"));
					menu2.Format(_T("-���"));
					menu3.Format(_T("-���"));
					menu4.Format(_T("*ȫ��"));
				}
				else if (fMealMap[m_iDay-1] == M100)
				{
					menu1.Format(_T("-���"));
					menu2.Format(_T("+���"));
					menu3.Format(_T("+���"));
					menu4.Format(_T("*���"));
				}
				else if (fMealMap[m_iDay-1] == M010)
				{
					menu1.Format(_T("+���"));
					menu2.Format(_T("-���"));
					menu3.Format(_T("+���"));
					menu4.Format(_T("*���"));
				}
				else if (fMealMap[m_iDay-1] == M001)
				{
					menu1.Format(_T("+���"));
					menu2.Format(_T("+���"));
					menu3.Format(_T("-���"));
					menu4.Format(_T("*���"));
				}
				else if (fMealMap[m_iDay-1] == M101)
				{
					menu1.Format(_T("-���"));
					menu2.Format(_T("+���"));
					menu3.Format(_T("-���"));
					menu4.Format(_T("*����"));
				}
				else if (fMealMap[m_iDay-1] == M011)
				{
					menu1.Format(_T("+���"));
					menu2.Format(_T("-���"));
					menu3.Format(_T("-���"));
					menu4.Format(_T("*����"));
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

// ������
void PaintCalc(HWND hDlg)
{
	PaintCalcTitle(hDlg);
	PaintDate(hDlg);
}

// �������ı�ͷ
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
	wchar_t sztext[][3] = { _T("һ"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��"), _T("��") };
	for (int i = 0; i<7; i++)
	{
		TextOut(hDC, startx, rect.top+1, sztext[i], lstrlenW(sztext[i]));
		startx += (rect.right - rect.left) / 7;
	}

	EndPaint(hDlg, &ps);
	DeleteObject(hFont);
}

// ��������
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

	// ����һ����������ı���ɫ
	hDC = GetDC(hDlg);
	GetClientRect(hDlg, &rect);
	rect.left += CAL_LEFTGAP;
	rect.right = rect.left + CAL_WIDTH;
	rect.top += CAL_TOPGAP + TITLE_HEIGHT;
	rect.bottom = rect.top + ROW_HEIGHT * 6;
	FillRect(hDC, &rect, CreateSolidBrush(RGB(255, 255, 255)));

	// ���������С
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = tm.tmHeight;
	lf.lfWidth = 0;
	hFont = CreateFontIndirect(&lf);
	SelectObject(hDC, hFont);

	for (int i=0; i<6; i++, rect.top+=ROW_HEIGHT)	// 6��
	{
		int startx = rect.left + (rect.right-rect.left)/7 - tm.tmHeight;

		for (int j=1; j<=7; j++, startx+=((rect.right-rect.left)/7))	// 7��
		{
			if (j<WeekDay(m_iYear,m_iMonth,1) && i==0)	// ��1��
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

// ����ĳһ��
void PaintOneDay(HDC hDC, WORD &iDay, LONG startx, LONG starty, int border)
{
	wchar_t text[4];
	wsprintf(text, _T("%02d"), iDay);

	// ���Ƶ������ĩ
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
			SetTextColor(hDC, RGB(200, 0, 0));	// δ��
		else if (fMealMap[iDay - 1] == M110)
			SetTextColor(hDC, RGB(0, 0, 0));	// ����
		else if (fMealMap[iDay - 1] == M111)
			SetTextColor(hDC, RGB(0X19, 0X19, 0X70));	// ȫ��
		else if (fMealMap[iDay - 1] == M100)
			SetTextColor(hDC, RGB(0XB3, 0XEE, 0X3A));	// ���
		else if (fMealMap[iDay - 1] == M010)
			SetTextColor(hDC, RGB(0X45, 0X8B, 0X00));	// ���
		else if (fMealMap[iDay - 1] == M001)
			SetTextColor(hDC, RGB(0X36, 0X64, 0X8B));	// ���
		else if (fMealMap[iDay - 1] == M101)
			SetTextColor(hDC, RGB(0X7A, 0X37, 0X8B));	// ����
		else if (fMealMap[iDay - 1] == M011)
			SetTextColor(hDC, RGB(0XD1, 0X5F, 0XEE));	// ����
		
		TextOut(hDC, startx, starty, text, 2);
		SetBkColor(hDC, RGB(255, 255, 255));
		SetTextColor(hDC, RGB(0, 0, 0));
	}

	iDay++;
}

// ����iYear,iMonth,iDay��Ӧ�����ڼ�
WORD WeekDay(WORD iYear, WORD iMonth, WORD iDay)
{
	// ����Ԫ��monthday[i]��ʾ��i������ǰ������������7������
	WORD monthday[] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
	WORD iDays = (iYear - 1) % 7 + (iYear - 1) / 4 - (iYear - 1) / 100 + (iYear - 1) / 400;

	iDays += (monthday[iMonth - 1] + iDay);
	// ���iYear������
	if (IsLeapYear(iYear) && iMonth>2)
		iDays++;

	iDays = iDays % 7;
	// ����: 1,2,3,4,5,6,7 ��ʾ һ�����������ġ��塢������
	if (iDays == 0)
		return 7;
	else
		return iDays;
}

// ����iYear��iMonth�µ�����
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
		//���������
		if (IsLeapYear(iYear))
			return 29;
		else
			return 28;
		break;
	}
	return 0;
}

// ѡ��ĳһ�죬����ѡ��ĳ���������
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

// ��ʾ�ò�����
void ShowMealData(HWND hDlg)
{
	wchar_t	szTmp[30] = { 0 };
	char	strTmp[30] = { 0 };

	wsprintf(szTmp, _T("887.5"));
	SetDlgItemText(hDlg, IDC_EDIT1, szTmp);	// �ܲͷ�

	sprintf_s(strTmp, "%.1f", fTotalFee);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT2, szTmp);	// ʣ��ͷ�

	wsprintf(szTmp, _T("%d"), GetLeftM001());
	SetDlgItemText(hDlg, IDC_EDIT3, szTmp);	// ������ʹ���

	sprintf_s(strTmp, "��� %.1f Ԫ\r\n���� %.1f Ԫ", M100, M010);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT4, szTmp);	// ���ͼ۸�

	SYSTEMTIME	systime;
	::GetSystemTime(&systime);
	sprintf_s(strTmp, "������%d��%d��%d��", systime.wYear, systime.wMonth, systime.wDay);
	MultiByteToWideChar(CP_OEMCP, NULL, strTmp, 30, szTmp, 30);
	SetDlgItemText(hDlg, IDC_EDIT5, szTmp);
}