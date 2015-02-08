#include <Windows.h>
#include <d3d9.h>

#include <time.h>
#include <iostream>

using namespace std;

// flag for fullscreen
#define FULLSCREEN_ENABLED (0)

// Program Settings
const string APP_TITLE = "Direct3D_Windowed";
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

// Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;

bool gameOver = false;

// Macro to detect keypresses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

// Game Init function
bool GameInit(HWND window)
{
	MessageBox(window, "GameInit", "BREAKPOINT", 0);

	// init Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		MessageBox(window, "Error initializing Direct3D", "Error", MB_OK);
		return 0;
	}

	// set presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	#if(FULLSCREEN_ENABLED)
	d3dpp.Windowed = false;
	#else
	d3dpp.Windowed = true;
	#endif
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.hDeviceWindow = window;

	// create Direct3D Device
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

	// device did not initialize correctly
	if (d3d == NULL)
	{
		MessageBox(window, "Error creating Direct3D device", "Error", MB_OK);
		return 0;
	}

	return true;
}

// Update function
void GameRun(HWND hwnd)
{
	// make sure Direct3D device is valid
	if (!d3ddev) return;

	// clear the backbuffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0F, 0);

	// start rendering
	if (d3ddev->BeginScene())
	{
		// do stuff

		// stop rendering
		d3ddev->EndScene();

		// copy backbuffer to screen
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	// check for program exit
	if (KEY_DOWN(VK_ESCAPE))
	{
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	}
}

// Game Exit function
void GameEnd(HWND hwnd)
{
	// display close message
	MessageBox(hwnd, "Program is about to end", "GameEnd", MB_OK);

	// free memory
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
}

// Window Event Handling function
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			gameOver = true;
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Windows Entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// set the new window's properties
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APP_TITLE.c_str();
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);

	// create a new window
	HWND window = NULL;
	#if(FULLSCREEN_ENABLED)
	window = CreateWindow(APP_TITLE.c_str(), APP_TITLE.c_str(), 
								WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP, 
								CW_USEDEFAULT, CW_USEDEFAULT, 
								SCREEN_WIDTH, SCREEN_HEIGHT,
								NULL, NULL, hInstance, NULL);
	#else
	window = CreateWindow(APP_TITLE.c_str(), APP_TITLE.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);
	#endif
	// window creation error
	if (window == 0) return 0;

	// display window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	// initialize the game
	if (!GameInit(window)) return 0;

	// main message loop
	MSG message;
	while (!gameOver)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		GameRun(window); 
	}

	GameEnd(window);

	return message.wParam;
}