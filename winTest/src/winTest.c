#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <timeapi.h>
#include "software-renderer/src/draw.h"

#pragma comment(lib, "winmm.lib")

struct Buffer
{
	BITMAPINFO info;
	void *memory;
	int width;
	int height;
};

struct WindowDim
{
	int width;
	int height;
};

static bool g_running;
static struct Buffer g_buffer;
LARGE_INTEGER g_freq;

static LRESULT CALLBACK windowCallback(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
static struct WindowDim getWindowDimension(HWND window);
static void resizeDIBSection(struct Buffer *buffer, int width, int height);
static void displayBuffer(struct Buffer *buffer, HDC device_context, int window_width, int window_height);


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, int cmd_show)
{
	LARGE_INTEGER t1, t2, microseconds_per_frame, target_microseconds_per_frame;
	QueryPerformanceFrequency(&g_freq); // ticks/s
	if (timeBeginPeriod(1) == TIMERR_NOERROR)
		OutputDebugString(L"works\n");

	target_microseconds_per_frame.QuadPart = 16666; // 16666 nanoseconds/f -> 60fps


	WNDCLASS window_class = {0};
	resizeDIBSection(&g_buffer, 1280, 720);

	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = windowCallback;
	window_class.hInstance = instance;
	window_class.lpszClassName = L"winTest_class";

	if (!RegisterClass(&window_class)) {
		MessageBox(NULL, L"Failed to register window", L"Error",  MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	HWND window = CreateWindow(
		window_class.lpszClassName, L"winTest",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, instance, NULL);

	if (window == NULL) {
		MessageBox(NULL, L"Failed to create window", L"Error", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}

	HDC device_context = GetDC(window);

	struct ObjModel *model = objRead("head.obj");

	g_running = true;
	QueryPerformanceCounter(&t1);
	while (g_running) {
		

		MSG message;
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				g_running = false;
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		rotateModelAboutX(model, M_PI/360.0);
		rotateModelAboutY(model, M_PI/360.0);
		//rotateModelAboutZ(model, M_PI/360.0);

		fill(g_buffer.memory, g_buffer.width, g_buffer.height, BLACK);
		drawModel(g_buffer.memory, g_buffer.width, g_buffer.height, model, 2.0f, 1.2f, 300);

		struct WindowDim dimension = getWindowDimension(window);
		displayBuffer(&g_buffer, device_context, dimension.width, dimension.height);

		QueryPerformanceCounter(&t2);
		microseconds_per_frame.QuadPart = t2.QuadPart - t1.QuadPart;
		microseconds_per_frame.QuadPart *= 1'000'000;
		microseconds_per_frame.QuadPart /= g_freq.QuadPart;
		
		while (target_microseconds_per_frame.QuadPart > microseconds_per_frame.QuadPart) {
			Sleep((target_microseconds_per_frame.QuadPart - microseconds_per_frame.QuadPart) / 1000);
			QueryPerformanceCounter(&t2);
			microseconds_per_frame.QuadPart = t2.QuadPart - t1.QuadPart;
			microseconds_per_frame.QuadPart *= 1'000'000;
			microseconds_per_frame.QuadPart /= g_freq.QuadPart;
		}
		t1 = t2;
	}
}

static struct WindowDim getWindowDimension(HWND window)
{
	struct WindowDim result = { 0 };

	RECT client_rect;
	GetClientRect(window, &client_rect);
	result.width = client_rect.right - client_rect.left;
	result.height = client_rect.bottom - client_rect.top;

	return result;
}

static void resizeDIBSection(struct Buffer *buffer, int width, int height)
{
	if (buffer->memory)
		VirtualFree(buffer->memory, 0, MEM_RELEASE);

	buffer->width = width;
	buffer->height = height;

	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = buffer->height;
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;

	int bitmap_size = 4 * buffer->width * buffer->height;
	buffer->memory = VirtualAlloc(NULL, bitmap_size, MEM_COMMIT, PAGE_READWRITE);
}

static void displayBuffer(struct Buffer *buffer, HDC device_context, int window_width, int window_height)
{
	StretchDIBits(device_context, 0, 0, window_width, window_height, 0, 0, buffer->width,
				  buffer->height, buffer->memory, &buffer->info, DIB_RGB_COLORS, SRCCOPY);
}

static LRESULT CALLBACK windowCallback(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	LRESULT result = 0;

	switch (message) {
		case WM_DESTROY:
			g_running = false;
			break;

		case WM_CLOSE:
			g_running = false;
			break;

		case WM_PAINT: {
			PAINTSTRUCT paint;
			HDC device_context = BeginPaint(window, &paint);

			struct WindowDim dimension = getWindowDimension(window);

			displayBuffer(&g_buffer, device_context, dimension.width, dimension.height);
			EndPaint(window, &paint);
		} break;

		default:
			result = DefWindowProc(window, message, w_param, l_param);
			break;
	}

	return result;
}