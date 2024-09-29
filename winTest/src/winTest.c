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
	LARGE_INTEGER perf_counter_freq_result;
	QueryPerformanceFrequency(&perf_counter_freq_result);
	int64_t perf_counter_freq = perf_counter_freq_result.QuadPart;

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

			LARGE_INTEGER last_counter;
			QueryPerformanceCounter(&last_counter);
			int64_t last_cycle_count = __rdtsc();

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
				rotateModelAboutY(model, M_PI/180.0);
				drawModel(g_buffer.memory, g_buffer.width, g_buffer.height, model, 2.0f, 1.2f, 300);

				struct WindowDim dimension = getWindowDimension(window);
				displayBuffer(&g_buffer, device_context, dimension.width, dimension.height);

				int64_t end_cycle_count = __rdtsc();

				LARGE_INTEGER end_counter;
				QueryPerformanceCounter(&end_counter);

				int64_t cycles_elapsed = end_cycle_count - last_cycle_count;
				int64_t counter_elapsed = end_counter.QuadPart - last_counter.QuadPart;
				int32_t ms_per_frame = (int32_t)(((1000*counter_elapsed) / perf_counter_freq));
				int32_t fps = (int32_t)(perf_counter_freq / counter_elapsed);
				int32_t mcpf = cycles_elapsed/(1000*1000);

				wchar_t char_buffer[256];
				wsprintf(char_buffer, L"%dms/f, %df/s, %dmc/f\n", ms_per_frame, fps, mcpf);
				OutputDebugString(char_buffer);

				last_counter = end_counter;
				last_cycle_count = end_cycle_count;
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