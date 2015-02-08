// GameLoop

#include <Windows.h>
#include <iostream>
#include <time.h>
using namespace std;

const string GAME_TITLE = "Game Loop";
HWND window;
HDC device;
bool gameOver = false;

HBITMAP image;

void LoadBitmap(char* fileName)
{
	image = (HBITMAP)LoadImage(0, fileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

void DrawBitmap(char* fileName, int x, int y)
{
	// Load image
	//image = (HBITMAP)LoadImage(0, "Splash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	BITMAP bm;
	GetObject(image, sizeof(BITMAP), &bm);

	HDC hdcImage = CreateCompatibleDC(device);
	HBITMAP bitmap = (HBITMAP) SelectObject(hdcImage, image);

	// draw the bitmap to the window
	BitBlt(
		device,
		x, y,
		bm.bmWidth, bm.bmHeight,
		hdcImage,
		0, 0,
		SRCCOPY);

	DeleteDC(hdcImage);
	DeleteObject((HBITMAP)image);
}

bool GameInit()
{
	srand(time(NULL));
	LoadBitmap("Splash.bmp");
	return 1;
}

void GameRun()
{
	if (gameOver) return;

	RECT rect;
	GetClientRect(window, &rect);

	int x = rand() % (rect.right - rect.left) / 2;
	int y = rand() % (rect.bottom - rect.top) / 2;
	printf("%d", x);
	printf("%d", y);
	DrawBitmap("Splash.bmp", x, y);
	
}

void GameEnd()
{
	// free device context
	ReleaseDC(window, device);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			gameOver = true;
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

short MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GAME_TITLE.c_str();
	wc.hIconSm = NULL;

	return RegisterClassEx(&wc);
}

bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	window = CreateWindow(
		GAME_TITLE.c_str(),
		GAME_TITLE.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1024, 768,
		NULL, NULL,
		hInstance,
		NULL);

	if (window == 0) return 0;

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	device = GetDC(window);

	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, nCmdShow)) return 0;
	
	if (!GameInit()) return 0;

	while (!gameOver)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		GameRun();
	}

	GameEnd();

	return msg.wParam;
}