#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <string.h>
#include <shellapi.h>
#include "bitmapitres.h"

HINSTANCE hinst2;
POINT mouse;

void DrawBackgroundPattern(HWND hWnd);
void ShutCommand(INT comanda);
void CheckAA(HWND hwnd);
static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	INITCOMMONCONTROLSEX cc;
	HWND hwndDlg;
	MSG msg;

	memset(&wc,0,sizeof(wc));
	wc.lpfnWndProc = DefDlgProc;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wc.lpszClassName = "Tray ShutDown Ver 1.0";
	RegisterClass(&wc);
	memset(&cc,0,sizeof(cc));
	cc.dwSize = sizeof(cc);
	cc.dwICC = 0xffffffff;
	InitCommonControlsEx(&cc);
	hinst2=hinst;

	if(!(hwndDlg=CreateDialog(hinst, MAKEINTRESOURCE(IDD_MAINDIALOG), NULL, (DLGPROC) DialogFunc)));
	while (GetMessage (&msg, NULL, 0, 0)) {
		if (IsDialogMessage (hwndDlg, &msg)) {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
	}

	return msg.wParam;
}



static BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LOGBRUSH logbrush;
	static COLORREF BkColor, FgColor;
	static HBRUSH hBkBrush;
	static NOTIFYICONDATA NotifyIconData;
	static HMENU hMenu;

	switch (msg) {

		case WM_RBUTTONUP:
			GetCursorPos (&mouse);
			TrackPopupMenu (GetSubMenu (hMenu, 0), TPM_RIGHTALIGN | TPM_LEFTBUTTON, mouse.x, mouse.y, 0, hwndDlg, NULL);
		break;

		case WM_INITDIALOG:

			hMenu = LoadMenu (hinst2, MAKEINTRESOURCE (1000));

			NotifyIconData.cbSize = sizeof (NOTIFYICONDATA);
			NotifyIconData.hWnd = hwndDlg;
			NotifyIconData.uID = 0;
			NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			NotifyIconData.uCallbackMessage = WM_USER;
			NotifyIconData.hIcon = LoadImage (hinst2, MAKEINTRESOURCE (200), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); // load 16 x 16 pixels icon
			lstrcpy (NotifyIconData.szTip, "Double Click for Auto Actions, Right Click for menu");
			Shell_NotifyIcon (NIM_ADD, &NotifyIconData);

			logbrush.lbStyle = BS_SOLID;
			logbrush.lbHatch = 0;
			BkColor = RGB (192,220,192);
			logbrush.lbColor = BkColor;
			hBkBrush = CreateBrushIndirect (&logbrush);
			FgColor = RGB (0, 0, 0);

			SendDlgItemMessage(hwndDlg,ID_SSDD,BM_SETCHECK,(WPARAM) BST_CHECKED,0);//checkuiesc Shut Down ca fiind default
			SendDlgItemMessage(hwndDlg,ID_ICON   ,STM_SETIMAGE ,(WPARAM) IMAGE_ICON   ,(LPARAM) LoadIcon(hinst2,MAKEINTRESOURCE(200)));
			SendDlgItemMessage(hwndDlg,ID_ABOUT   ,STM_SETIMAGE ,(WPARAM) IMAGE_ICON   ,(LPARAM) LoadIcon(hinst2,MAKEINTRESOURCE(300)));
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK:
					ShowWindow(hwndDlg,SW_HIDE);
					break;
				case IDCANCEL:
					DestroyWindow(hwndDlg);
					break;
				case ID_ICON:
					ExitWindows(0,0);
					break;
				case ID_ABOUT:
					ExitWindows(EWX_REBOOT,0);
					break;
				case ID_OPEN:
					ShowWindow (hwndDlg, SW_SHOWNORMAL);
					break;
				case ID_BYE:
					DestroyWindow(hwndDlg);
					break;
				case ID_AA:
					CheckAA(hwndDlg);
					break;
				case ID_MOI:

					MessageBox(hwndDlg,
					"Tray Shut Down \n CopyRight 2003 Costea Bogdan \n 2nd version, 15hours work \n eMail me : bcostea@email.ro \n Disclaimer: \n The usual: I'm NOT responsable for any damage done by the software! \n \n Licence: \n Isn't it obvious!! There are no banners or nag screens are there? The only thing that I request is that you email me - just 4 fun - or 2 send me bugs you have encountered or things that I should add!",
					"About what?",MB_OK);
					break;
				case ID_HELP:
					WinHelp(hwndDlg,"TShut.hlp",HELP_CONTENTS,0);

					break;
				case ID_HELP2:
					WinHelp(hwndDlg,"TShut.hlp",HELP_CONTENTS,0);

					break;
				case ID_MONIT:
					SendMessage(hwndDlg, WM_SYSCOMMAND, SC_MONITORPOWER, 0);
					break;
				}
			break;

		case WM_PAINT:
			DrawBackgroundPattern(hwndDlg);
			break;
		
		case WM_CTLCOLORSTATIC:
				// process this message to set STATIC and READONLY EDIT control colors
				// lParam holds hwnd of individual control to be painted
				SetBkColor ((HDC) wParam, BkColor);
				SetTextColor ((HDC) wParam, FgColor);
				return (BOOL) hBkBrush;
		case WM_USER:
			switch (lParam)	{

				case WM_LBUTTONUP:
					ShowWindow(hwndDlg,SW_SHOW);
				break;

				case WM_RBUTTONUP:
					GetCursorPos (&mouse);
					TrackPopupMenu (GetSubMenu (hMenu, 0), TPM_RIGHTALIGN | TPM_LEFTBUTTON, mouse.x, mouse.y, 0, hwndDlg, NULL);
				break;
				case WM_LBUTTONDBLCLK:
					CheckAA(hwndDlg);
				break;
			}
		break;
		case WM_CLOSE:
			DestroyWindow(hwndDlg);
			return TRUE;
		case WM_DESTROY:
		case WM_ENDSESSION:
			Shell_NotifyIcon (NIM_DELETE, &NotifyIconData);
			DestroyMenu (hMenu);
			PostQuitMessage (0);
		break;
	}
	return FALSE;
}

 void DrawBackgroundPattern(HWND hWnd)
 {
	HDC hDC = GetDC(hWnd);
	RECT rectFill;
	RECT rectClient;
	HBRUSH hBrush;

	GetClientRect(hWnd, &rectClient);

	hBrush = CreateSolidBrush(RGB(192, 220, 192 ));
	FillRect(hDC, &rectClient, hBrush);


	SetRect(&rectClient,0,0,256,40);
	hBrush = CreateSolidBrush(RGB(255, 255, 255 ));
	FillRect(hDC, &rectClient, hBrush);

   	DeleteObject(hBrush);
   	ReleaseDC(hWnd, hDC);
 	SendDlgItemMessage(hWnd,ID_ICON   ,STM_SETIMAGE ,(WPARAM) IMAGE_ICON   ,(LPARAM) LoadIcon(hinst2,MAKEINTRESOURCE(200)));
	SendDlgItemMessage(hWnd,ID_ABOUT   ,STM_SETIMAGE ,(WPARAM) IMAGE_ICON   ,(LPARAM) LoadIcon(hinst2,MAKEINTRESOURCE(300)));
 }


void ShutCommand(INT comanda){
	HWND hwndWI;
	switch (comanda){
		case 1:
			ExitWindowsEx(EWX_REBOOT,0);
		break;
		case 2:
			ShellExecute(0,"open","rundll.exe","user.exe,exitwindowsexec","",SW_HIDE);
		break;
		case 3:
			ShellExecute(0,"open","rundll.exe","user.exe,exitwindows","",SW_HIDE);
		break;
		case 4:
			hwndWI = FindWindow("Progman", NULL);
			PostMessage(hwndWI, WM_CLOSE, 0, 0L);
		break;
	}
}


void CheckAA(HWND hwnd){
	LONG lState;
	
	lState=SendDlgItemMessage(hwnd,ID_SSDD,BM_GETCHECK,0,0);
	if(lState==BST_CHECKED){
		ShutCommand(4);
	}
	
	lState=SendDlgItemMessage(hwnd,ID_RST,BM_GETCHECK,0,0);
	if(lState==BST_CHECKED){
		ShutCommand(1);
	}

	lState=SendDlgItemMessage(hwnd,ID_RBT,BM_GETCHECK,0,0);
	if(lState==BST_CHECKED){
		ShutCommand(2);
	}

	lState=SendDlgItemMessage(hwnd,ID_SHT,BM_GETCHECK,0,0);
	if(lState==BST_CHECKED){
		ShutCommand(3);
	}

}
