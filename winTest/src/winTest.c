#include <windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "software-renderer/src/draw.h"

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


static LRESULT CALLBACK windowCallback(HWND window, UINT message, WPARAM w_param, LPARAM l_param);
static struct WindowDim getWindowDimension(HWND window);
static void resizeDIBSection(struct Buffer *buffer, int width, int height);
static void displayBuffer(struct Buffer *buffer, HDC device_context, int window_width, int window_height);


int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, int cmd_show)
{
	

	WNDCLASS window_class = {0};

	resizeDIBSection(&g_buffer, 1280, 720);

	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = windowCallback;
	window_class.hInstance = instance;
	window_class.lpszClassName = L"winTest_class";

	if (RegisterClass(&window_class)) {
		HWND window = CreateWindow(
			window_class.lpszClassName, L"winTest",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, instance, NULL);

		if (window)	{
			HDC device_context = GetDC(window);

			struct ObjModel *model = objRead("head.obj");

			g_running = true;
			while (g_running) {
				MSG message;
				while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
						g_running = false;
					TranslateMessage(&message);
					DispatchMessage(&message);
				}

				
				fill(g_buffer.memory, g_buffer.width, g_buffer.height, BLACK);
				rotateModelAboutX(model, M_PI/360.0);
				rotateModelAboutY(model, M_PI/360.0);
				rotateModelAboutZ(model, M_PI/360.0);
				drawModel(g_buffer.memory, g_buffer.width, g_buffer.height, model, 2.0f, 1.2f, 300);

				struct WindowDim dimension = getWindowDimension(window);
				displayBuffer(&g_buffer, device_context, dimension.width, dimension.height);

			}
		} else {
			OutputDebugString(L"Failed to register window\n");
			return 1;
		}
	} else {
		OutputDebugString(L"Failed to register window\n");
		return 1;
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