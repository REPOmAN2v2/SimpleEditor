#include "../include/resources.h"
#include "../include/editor.h"

HWND hEdit;
HDC hdc;
HFONT hfDefault = NULL;
COLORREF textColour = RGB(0, 0, 0);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	wc.cbSize		= sizeof(WNDCLASSEX);	
	wc.style		= 0;					
	wc.lpfnWndProc	= WndProc;				
	wc.cbClsExtra	= 0;					
	wc.cbWndExtra	= 0;					
	wc.hInstance 	= hInstance;			
	wc.hIcon		= LoadIcon(NULL, IDI_APPLICATION); 	
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);		
	wc.hbrBackground= (HBRUSH)(COLOR_WINDOW+1);			
	wc.lpszMenuName	= NULL;					
	wc.lpszClassName= className;		
	wc.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);	

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Failed to register the window.", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(	WS_EX_ACCEPTFILES, className, "SimpleEdit", (WS_BORDER ), CW_USEDEFAULT, CW_USEDEFAULT, 640, 720, NULL, NULL, hInstance, NULL );

	if(hwnd == NULL) {
		MessageBox(NULL, "Failed to create the window.", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//SetWindowLongPtr(hwnd, GWL_STYLE, 0); // removes titlebar and borders
	/*	This will remove the title bar but not the borders: */
	SetWindowLongPtr(hwnd, GWL_STYLE, WS_BORDER | WS_THICKFRAME); 
	SetWindowPos(hwnd, 0, 360, 270, 640, 720, SWP_FRAMECHANGED);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL1));

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if(!TranslateAccelerator(hwnd, hAccel, &msg)) {
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
			createEditor(hwnd);
			break;
		case WM_SIZE:
			resizeEditor(hwnd);
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0);
					break;
				case ID_FILE_NEW:
					SetDlgItemText(hwnd, IDC_MAIN_EDIT, "");
					break;
				case ID_FILE_OPEN:
					openFile(hwnd);
					break;
				case ID_FILE_SAVEAS:
					saveFile(hwnd);
					break;
				case ID_EDIT_FONT:
					changeFont(hwnd);
					//InvalidateRect(hEdit, NULL, TRUE);
					//UpdateWindow(hEdit);
					break;
			}
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void createEditor(HWND hwnd)
{
	long lfHeight;

	hdc = GetDC(NULL);
	lfHeight = -MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72);

	hEdit = CreateWindowEx(0, "EDIT", "", 
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
		0, 0, 100, 100, hwnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
	if(hEdit == NULL)
		MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

	//hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	hfDefault = CreateFont(lfHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "DejaVu Sans Mono");

	if(!hfDefault) {
		MessageBox(hwnd, "Font creation failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
	}

	SendMessage(hEdit, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));
}

void resizeEditor(HWND hwnd)
{
	HWND hEdit;
	RECT clientRect;

	GetClientRect(hwnd, &clientRect);

	hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
	SetWindowPos(hEdit, NULL, 0, 0, clientRect.right, clientRect.bottom, SWP_NOZORDER);
}

void openFile(HWND hwnd)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "txt";

	if(GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		loadText(hEdit, fileName);
	}
}

void saveFile(HWND hwnd)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		writeText(hEdit, fileName);
	}
}

BOOL loadText(HWND hEdit, LPCTSTR fileName)
{
	HANDLE hFile;
	BOOL success = FALSE;

	hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD fileSize;

		fileSize = GetFileSize(hFile, NULL);

		if(fileSize != 0xFFFFFFFF) {
			LPTSTR text;

			text = GlobalAlloc(GPTR, fileSize + 1);

			if(text != NULL) {
				DWORD bytesRead;

				if(ReadFile(hFile, text, fileSize, &bytesRead, NULL)) {
					text[fileSize] = 0; // add null terminator

					if(SetWindowText(hEdit, text)) {
						success = TRUE;
					}
				}

				GlobalFree(text);
			}
		}

		CloseHandle(hFile);
	}

	return success;
}

BOOL writeText(HWND hEdit, LPCTSTR fileName)
{
	HANDLE hFile;
	BOOL success = FALSE;

	hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile != INVALID_HANDLE_VALUE) {
		DWORD textLength;

		textLength = GetWindowTextLength(hEdit);

		if(textLength > 0) {
			LPTSTR text;
			DWORD buffer = textLength + 1;

			text = GlobalAlloc(GPTR, buffer);

			if(text != NULL) {
				
				if(GetWindowText(hEdit, text, buffer)) {
					DWORD bytesWritten;

					if(WriteFile(hFile, text, textLength, &bytesWritten, NULL)) {
						success = TRUE;
					}
				}

				GlobalFree(text);

			}
		}

		CloseHandle(hFile);
	}

	return success;
}

BOOL isKeyDown(int vk)
{
	return (GetKeyState(vk) & 0x8000);
}

void changeFont(HWND hwnd)
{
	CHOOSEFONT cf;
	LOGFONT logfont;

	GetObject(hfDefault, sizeof(LOGFONT), &logfont);

	ZeroMemory(&cf, sizeof(cf));

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	cf.hwndOwner = hwnd;
	cf.lpLogFont = &logfont;
	cf.rgbColors = textColour;

	if(ChooseFont(&cf)) {
		HFONT hfont = CreateFontIndirect(&logfont);
		textColour = cf.rgbColors;
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
		DeleteObject(hfDefault);

		if(hfont) {
			hfDefault = hfont;
		} else {
			MessageBox(hwnd, "Failed to create font.", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}
}