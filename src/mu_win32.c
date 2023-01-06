
#define STRICT
#define UNICODE
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX            1

#include <windows.h>
#include <timeapi.h>

#undef STRICT
#undef UNICODE
#undef WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#undef far
#undef near

#include "mu.h"
#include "mu_win32_gl.h"

typedef struct Game_Code
{
	Tick_Func* tick;
} Game_Code;

LRESULT
WndProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_QUIT || msg == WM_CLOSE || msg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	else return DefWindowProcW(window, msg, wparam, lparam);
}

void __stdcall
WinMainCRTStartup()
{
	HINSTANCE instance = GetModuleHandle(0);

	 WNDCLASSEXW window_class_info = {
		.cbSize        = sizeof(WNDCLASSEXW),
		.style         = CS_OWNDC,
		.lpfnWndProc   = &WndProc,
		.hInstance     = instance,
		.hIcon         = LoadIcon(0, IDI_APPLICATION),
		.hCursor       = LoadCursor(0, IDC_ARROW),
		.lpszClassName = L"MU_WINDOW_CLASS_NAME",
	};

	if (!RegisterClassExW(&window_class_info)) NOT_IMPLEMENTED;
	else
	{
		HWND window = CreateWindowW(window_class_info.lpszClassName, L"TITLE ME", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, 0, 0);
		if (window == 0) NOT_IMPLEMENTED;
		else
		{
			bool setup_failed = false;

			Engine_Link engine_link = {0};
			Engine = &engine_link;

			/// Set working directory to dir of executable
			if (!setup_failed)
			{
				// TODO:
				WCHAR buffer[MAX_PATH - 2];
				if (!GetModuleFileNameW(0, buffer, ARRAY_SIZE(buffer))) setup_failed = true;
				else
				{
					LPWSTR last_slash = buffer;
					unsigned int len  = 0;
					for (LPWSTR scan = buffer; *scan; ++scan, ++len)
					{
						if (*scan == '\\' || *scan == '/') last_slash = scan;
					}

					if (len == ARRAY_SIZE(buffer)) setup_failed = true;
					else
					{
						*last_slash = 0;

						setup_failed = !SetCurrentDirectoryW(buffer);
					}
				}
			}

			/// Load game code
			Game_Code game_code = {0};
			if (!setup_failed)
			{
				HMODULE game_dll = LoadLibraryExW(L".\\game.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
				if (game_dll == 0) setup_failed = true;
				else
				{
					game_code.tick = (Tick_Func*)GetProcAddress(game_dll, "Tick");

					if (game_code.tick == 0) setup_failed = true;
				}
			}

			/// Initialize an OpenGL context
			HDC dc           = 0;
			HGLRC gl_context = 0;
			if (!setup_failed)
			{
				setup_failed = !Win32_InitGL(window, &dc, &gl_context, &Engine->renderer_link);
			}

			if (setup_failed)
			{
				MessageBoxW(0, L"Failed to setup engine, TODO", L"Engine Error", MB_OK | MB_ICONERROR);
			}
			else
			{
				LARGE_INTEGER perf_counter_freq = {0};
				timeBeginPeriod(1);
				QueryPerformanceFrequency(&perf_counter_freq);
				LARGE_INTEGER flip_time;
				QueryPerformanceCounter(&flip_time);
				f32 acc_frame_time = 0;
				unsigned int frame_counter = 0;

				ShowWindow(window, SW_SHOW);

				bool is_running = true;
				while (is_running)
				{
					MSG msg;
					while (PeekMessageW(&msg, 0, 0, 0, PM_REMOVE))
					{
						if (msg.message == WM_QUIT || msg.message == WM_CLOSE)
						{
							is_running = false;
							break;
						}
						else DispatchMessage(&msg);
					}

					{
						POINT p;
						GetCursorPos(&p);
						ScreenToClient(window, &p);
						RECT r;
						GetClientRect(window, &r);
						Engine->window_dim = V2(r.right - r.left, r.bottom - r.top);
						Engine->mouse_pos = V2(p.x, p.y);
						glViewport(0, 0, (GLsizei)Engine->window_dim.x, (GLsizei)Engine->window_dim.y);
					}

					game_code.tick(Engine);

					SwapBuffers(dc);

					LARGE_INTEGER end_time;
					QueryPerformanceCounter(&end_time);
					acc_frame_time += (f32)(end_time.QuadPart - flip_time.QuadPart) / (f32)perf_counter_freq.QuadPart;
					flip_time = end_time;

					if (++frame_counter == 5)
					{
						f32 avg_frame_time = acc_frame_time/frame_counter;

						WCHAR window_title_buffer[256];
						wsprintfW(window_title_buffer, L"TITLE ME - avg last %u frames: %u ms, %u fps", frame_counter, (unsigned int)(avg_frame_time*1000), (unsigned int)(1/avg_frame_time));
						SetWindowTextW(window, window_title_buffer);

						frame_counter  = 0;
						acc_frame_time = 0;
					}
				}

				ReleaseDC(window, dc);
			}
		}
	}

	ExitProcess(0);
}
