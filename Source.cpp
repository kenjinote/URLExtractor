#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi")

#include <windows.h>
#include <shlwapi.h>

#include <iostream>
#include <string>
#include <regex>

TCHAR szClassName[] = TEXT("Window");

//  文字列を置換する
std::string Replace(std::string String1, std::string String2, std::string String3)
{
	std::string::size_type  Pos(String1.find(String2));

	while (Pos != std::string::npos)
	{
		String1.replace(Pos, String2.length(), String3);
		Pos = String1.find(String2, Pos + String3.length());
	}

	return String1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1, hEdit2, hEdit3, hButton;
	static HWND hStatic1, hStatic2;
	static HWND hCheck;
	switch (msg)
	{
	case WM_CREATE:
		hStatic1 = CreateWindow(TEXT("Static"), TEXT("対象リスト"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit1, EM_LIMITTEXT, 0, 0);
		hCheck = CreateWindow(TEXT("Button"), TEXT("HTMLｴｽｹｰﾌﾟ"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTOCHECKBOX, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("Button"), TEXT(">変換>"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hStatic2 = CreateWindow(TEXT("Static"), TEXT("結果リスト"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit2, EM_LIMITTEXT, 0, 0);
		DragAcceptFiles(hWnd, TRUE);
		break;
	case WM_DROPFILES:
		{
			const HDROP hDrop = (HDROP)wParam;
			const UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFiles == 1)
			{
				TCHAR szFileName[MAX_PATH];
				DragQueryFile(hDrop, 0, szFileName, sizeof(szFileName));
				const HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					const DWORD dwFileSize = GetFileSize(hFile, 0);
					DWORD dwReadSize;
					LPSTR lpszText = (LPSTR)GlobalAlloc(0, dwFileSize + 1);
					ReadFile(hFile, lpszText, dwFileSize, &dwReadSize, 0);
					lpszText[dwReadSize] = 0;
					SetWindowTextA(hEdit1, lpszText);
					GlobalFree(lpszText);
					CloseHandle(hFile);
				}
			}
			DragFinish(hDrop);
		}
		break;
	case WM_SIZE:
		{
			const int button_width = 128;
			const int width = (LOWORD(lParam) - button_width - 20) / 2;
			MoveWindow(hStatic1, 0, 0, width, 32, TRUE);
			MoveWindow(hEdit1, 0, 32, width, HIWORD(lParam) - 32, TRUE);
			MoveWindow(hCheck, width + 10, HIWORD(lParam) / 2 - 16 - 10 - 32, button_width, 32, TRUE);
			MoveWindow(hEdit3, width + 10, HIWORD(lParam) / 2 - 16, button_width, 32, TRUE);
			MoveWindow(hButton, width + 10, HIWORD(lParam) / 2 + 16 + 10, button_width, 32, TRUE);
			MoveWindow(hStatic2, width + button_width + 20, 0, width, 32, TRUE);
			MoveWindow(hEdit2, width + button_width + 20, 32, width, HIWORD(lParam) - 32, TRUE);
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EnableWindow(hButton, FALSE);
			SetWindowTextA(hEdit2, 0);
			const INT_PTR nEditLineCount1 = SendMessageA(hEdit1, EM_GETLINECOUNT, 0, 0);
			const INT_PTR bHtmlEscape = SendMessageA(hCheck, BM_GETCHECK, 0, 0);
			LPSTR lpszEdit3 = 0;
			const int nEditLenght3 = GetWindowTextLengthA(hEdit3);
			if (nEditLenght3)
			{
				lpszEdit3 = (LPSTR)GlobalAlloc(0, sizeof(CHAR) * (nEditLenght3 + 1));
				GetWindowTextA(hEdit3, lpszEdit3, nEditLenght3 + 1);
			}
			BOOL bIsFound;
			for (INT_PTR i = 0; i < nEditLineCount1; ++i)
			{
				bIsFound = FALSE;
				const INT_PTR nEditLineIndex1 = SendMessageA(hEdit1, EM_LINEINDEX, i, 0);
				const INT_PTR nEditLineLength1 = SendMessageA(hEdit1, EM_LINELENGTH, nEditLineIndex1, 0);
				if (!nEditLineLength1) continue;
				LPSTR lpszEditLine1 = (LPSTR)GlobalAlloc(0, sizeof(CHAR)* (nEditLineLength1 + 1 + 2)); // 改行文字2文字分追加
				*(WORD *)lpszEditLine1 = (WORD)nEditLineLength1;
				SendMessageA(hEdit1, EM_GETLINE, i, (LPARAM)lpszEditLine1);
				lpszEditLine1[nEditLineLength1] = 0;
				{
					const std::string target(lpszEditLine1);
					std::regex re(R"(https?://[-_.!~*\'()a-zA-Z0-9;/?:@&=+$,%#]+)");
					std::smatch match;
					for (auto it = target.begin(); regex_search(it, target.end(), match, re); it += match.position(0) + match.length(0))
					{
						std::string str(match.str(0));
						if (bHtmlEscape)
						{
							str = Replace(str, "&amp;", "&");
							str = Replace(str, "%3D", "=");
							str = Replace(str, "%22", "\"");
							str = Replace(str, "%20", " ");
						}
						if (!lpszEdit3 || StrStrIA(str.c_str(), lpszEdit3))
						{
							INT_PTR dwEditTextLength = SendMessageA(hEdit2, WM_GETTEXTLENGTH, 0, 0);
							SendMessageA(hEdit2, EM_SETSEL, dwEditTextLength, dwEditTextLength);
							SendMessageA(hEdit2, EM_REPLACESEL, 0, (LPARAM)str.c_str());
							SendMessageA(hEdit2, EM_REPLACESEL, 0, (LPARAM)"\r\n");
						}
					}
				}
				GlobalFree(lpszEditLine1);
			}
			if (lpszEdit3)
			{
				GlobalFree(lpszEdit3);
			}
			SendMessageA(hEdit2, EM_SETSEL, 0, -1);
			SetFocus(hEdit2);
			EnableWindow(hButton, TRUE);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	const WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	const HWND hWnd = CreateWindow(
		szClassName,
		TEXT("入力されたテキストからURLだけを抽出する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
