#include <windows.h>

#define ROWS 3
#define COLS 4
LPCSTR arr[ROWS][COLS] = { {"One\0", "Two\0", "Three\0", "Four\0"},
						{"a\0", "b\0", "c\0", "d\0"},
						{"s\0", "a\0", "s\0", "a\naaa\0"} };

int rowWidth;
int charHeight = 80;
int workHeight = 0;
int charAmount = 0;

RECT gRect;

/* This is where all the input to the window goes to */
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch (Message) {
		/* Upon destruction, tell the main thread to stop */
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT: {
		int newHeight, curHeight = 0;
		int avgCharWidth = rowWidth * 0.7 / charAmount;
		workHeight = 0;
		RECT rect;
		PAINTSTRUCT ps;
		
		
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &gRect, (HBRUSH)(COLOR_WINDOW + 1));
		HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		SelectObject(hdc, hPen);
		HFONT hFont = CreateFont(charHeight, avgCharWidth, 0, 0, 0, 0, 0, 0,
			OEM_CHARSET,
			0, 0, 0, VARIABLE_PITCH,
			"Times New Roman");
		SelectObject(hdc, hFont);
		for (int i = 0; i < ROWS; i++) {
			for (int j = 0; j < COLS; j++) {
				//√раницы отрисовки текста
				rect.left = rowWidth * (j + 0.1);
				rect.top = workHeight + charHeight * 0.1;
				rect.right = rowWidth * (j + 0.9);
				rect.bottom = 9999; //низ дл€ таблицы подсчитаетс€ далее изход€ из зан€того места
				
				//ѕолучение самого высокого текста
				newHeight = DrawTextA(hdc, arr[i][j], strlen(arr[i][j]), &rect, DT_NOCLIP or DT_CALCRECT);
				if (newHeight > curHeight) {
					curHeight = newHeight;
				}
			}
			//ќтрисовка горизонтальных линий
			workHeight += curHeight + charHeight * 1 / 5; // 1/5 это пространство иизначального отступа (0.1 + 0.1)
			MoveToEx(hdc, 0, workHeight, NULL);
			LineTo(hdc, rowWidth * COLS, workHeight);
			curHeight = 0;
		}
		//ќтрисовка вертикальных линий
		for (int j = 0; j <= ROWS; j++) {
			for (int i = 0; i <= COLS; i++) {
				MoveToEx(hdc, rowWidth * i, 0, NULL);
				LineTo(hdc, rowWidth * i, workHeight);
			}
		}
		DeleteObject(hFont);
		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_SIZE: {
		GetClientRect(hwnd, &gRect);
		rowWidth = gRect.right / COLS;
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	
	case WM_GETMINMAXINFO: {
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
    	lpMMI->ptMinTrackSize.y = workHeight + 45;
		break;
	}
	
	/* All other messages (a lot of them) are processed using default procedures */
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG Msg; /* A temporary location for all messages */

	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Caption",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		640, /* width */
		480, /* height */
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			int len = strcspn(arr[i][j], "\n");
			if (len > charAmount)
				charAmount = len;	
		}
	}

	/*
		This is the heart of our program where all input is processed and 
		sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
		this loop will not produce unreasonably high CPU usage
	*/
	while(GetMessage(&Msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&Msg); /* Translate key codes to chars if present */
		DispatchMessage(&Msg); /* Send it to WndProc */
	}
	return Msg.wParam;
}
