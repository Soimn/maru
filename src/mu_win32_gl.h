#include <GL/gl.h>
#include <vendor/Win32GL/glcorearb.h>
#include <vendor/Win32GL/glext.h>
#include <vendor/Win32GL/wglext.h>

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB       = 0;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 0;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB   = 0;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT                 = 0;

#define LIST_GL_CORE_FUNCS                                          \
	MU_X(PFNGLUSEPROGRAMPROC,              glUseProgram)              \
	MU_X(PFNGLUNIFORM1FPROC,               glUniform1f)               \
	MU_X(PFNGLUNIFORM2FPROC,               glUniform2f)               \
	MU_X(PFNGLUNIFORM4FPROC,               glUniform4f)               \
	MU_X(PFNGLUNIFORMMATRIX4FVPROC,        glUniformMatrix4fv)        \
	MU_X(PFNGLCREATEPROGRAMPROC,           glCreateProgram)           \
	MU_X(PFNGLCREATESHADERPROC,            glCreateShader)            \
	MU_X(PFNGLSHADERSOURCEPROC,            glShaderSource)            \
	MU_X(PFNGLCOMPILESHADERPROC,           glCompileShader)           \
	MU_X(PFNGLGETSHADERIVPROC,             glGetShaderiv)             \
	MU_X(PFNGLGETPROGRAMIVPROC,            glGetProgramiv)            \
	MU_X(PFNGLATTACHSHADERPROC,            glAttachShader)            \
	MU_X(PFNGLLINKPROGRAMPROC,             glLinkProgram)             \
	MU_X(PFNGLVALIDATEPROGRAMPROC,         glValidateProgram)         \
	MU_X(PFNGLDELETESHADERPROC,            glDeleteShader)            \
	MU_X(PFNGLGETSHADERINFOLOGPROC,        glGetShaderInfoLog)        \
	MU_X(PFNGLBINDFRAMEBUFFERPROC,         glBindFramebuffer)         \
	MU_X(PFNGLDEBUGMESSAGECALLBACKPROC,    glDebugMessageCallback)    \
	MU_X(PFNGLGENVERTEXARRAYSPROC,         glGenVertexArrays)         \
	MU_X(PFNGLBINDVERTEXARRAYPROC,         glBindVertexArray)         \
	MU_X(PFNGLGENBUFFERSPROC,              glGenBuffers)              \
	MU_X(PFNGLBINDBUFFERPROC,              glBindBuffer)              \
	MU_X(PFNGLBUFFERDATAPROC,              glBufferData)              \
	MU_X(PFNGLVERTEXATTRIBPOINTERPROC,     glVertexAttribPointer)     \
	MU_X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray) \
	MU_X(PFNGLDELETEBUFFERSPROC,           glDeleteBuffers)           \
	MU_X(PFNGLGETPROGRAMINFOLOGPROC,       glGetProgramInfoLog)       \

#define MU_X(type, name) type name = 0;
LIST_GL_CORE_FUNCS
#undef MU_X

GLuint UnitRectVAO     = 0;
GLuint PushRectProgram = 0;
GLuint PushLineProgram = 0;

void
ClearScreen(V4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void
PushRect(Rect rect, V4 corner_radii, f32 line_thickness, V4 color)
{
	V2 center = V2((rect.min.x + rect.max.x)/2, (rect.min.y + rect.max.y)/2);
	V2 dim    = V2_Sub(rect.max, rect.min);

	f32 aa_pad = 2; // NOTE: 2 pixel padding for anti aliasing
	V2 padded_dim  = V2(dim.x + line_thickness + 2*aa_pad, dim.y + line_thickness + 2*aa_pad);

	M4 transform = {
		.i = V4(2*padded_dim.x/Engine->window_dim.x,                                   0, 0, 0),
		.j = V4(                                  0, 2*padded_dim.y/Engine->window_dim.y, 0, 0),
		.k = V4(                                  0,                                   0, 1, 0),
		.l = V4(2*center.x/Engine->window_dim.x - 1, 2*center.y/Engine->window_dim.y - 1, 0, 1),
	};

	glUseProgram(PushRectProgram);

	glBindVertexArray(UnitRectVAO);
	glUniformMatrix4fv(1, 1, false, transform.e);
	glUniform4f(2, color.r, color.g, color.b, color.a);
	glUniform4f(3, center.x, center.y, dim.x/2, dim.y/2);
	glUniform1f(4, line_thickness);
	glUniform4f(5, corner_radii.x, corner_radii.y, corner_radii.w, corner_radii.z);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}

void
PushLine(V2 p0, V2 p1, f32 line_thickness, V4 color)
{
	glUseProgram(PushLineProgram);

	V2 center  = V2((p1.x + p0.x)/2, (p1.y + p0.y)/2);
	V2 p0p1    = V2_Sub(p1, p0);
	f32 length = V2_Length(p0p1);
	V2 p0p1_n  = V2_Scale(p0p1, 1/length);

	f32 aa_pad = 2; // NOTE: 2 pixel padding for anti aliasing
	V2 padded_dim = V2(line_thickness + 2*aa_pad, length + 2*aa_pad);

	f32 phi = (p0p1.y < 0 ? 1 : -1) * Acos(p0p1_n.x);
	f32 s = Sin(phi);
	f32 c = Cos(phi);

	M4 transform = {
		.i = V4(2*s*padded_dim.x/Engine->window_dim.x,  2*c*padded_dim.x/Engine->window_dim.y, 0, 0),
		.j = V4(2*c*padded_dim.y/Engine->window_dim.x, -2*s*padded_dim.y/Engine->window_dim.y, 0, 0),
		.k = V4(                                    0,                                      0, 1, 0),
		.l = V4(  2*center.x/Engine->window_dim.x - 1,    2*center.y/Engine->window_dim.y - 1, 0, 1),
	};

	glBindVertexArray(UnitRectVAO);
	glUniformMatrix4fv(1, 1, false, transform.e);
	glUniform4f(2, color.r, color.g, color.b, color.a);
	glUniform4f(3, p0.x, p0.y, p1.x, p1.y);
	glUniform2f(4, p0p1_n.x, p0p1_n.y);
	glUniform1f(5, line_thickness/2);
	glUniform1f(6, length/2);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(0);
}

// TODO: error handling
bool
GL_CreateProgram(char* vertex_code, char* fragment_code, GLuint* program)
{
	bool succeeded = false;
	GLint status;

	do
	{
		*program = glCreateProgram();

		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_code, 0);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			char buffer[1024];
			glGetShaderInfoLog(vertex, sizeof(buffer), 0, buffer);
			OutputDebugStringA(buffer);
			break;
		}

		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragment_code, 0);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			char buffer[1024];
			glGetShaderInfoLog(fragment, sizeof(buffer), 0, buffer);
			OutputDebugStringA(buffer);
			break;
		}

		glAttachShader(*program, vertex);
		glAttachShader(*program, fragment);

		glLinkProgram(*program);

		glGetProgramiv(*program, GL_LINK_STATUS, &status);
		if (!status)
		{
			char buffer[1024];
			glGetProgramInfoLog(*program, sizeof(buffer), 0, buffer);
			OutputDebugStringA(buffer);
			break;
		}

		glValidateProgram(*program);
		glGetProgramiv(*program, GL_VALIDATE_STATUS, &status);
		if (!status)
		{
			char buffer[1024];
			glGetProgramInfoLog(*program, sizeof(buffer), 0, buffer);
			OutputDebugStringA(buffer);
			break;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		succeeded = true;
	} while (0);

	return succeeded;
}

static void APIENTRY
Win32_GLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user)
{
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

bool
Win32_InitGL(HWND window, HDC* dc, HGLRC* gl_context, Renderer_Link* renderer_link)
{
	bool succeeded = false;
	// This uses tricks learnt from mmozeiko's example https://gist.github.com/mmozeiko/6825cb94d393cb4032d250b8e7cc9d14

	wglChoosePixelFormatARB    = 0;
	wglCreateContextAttribsARB = 0;
	wglGetExtensionsStringARB  = 0;
	wglSwapIntervalEXT         = 0;

	HWND dummy_window   = 0;
	HDC dummy_dc        = 0;
	HGLRC dummy_context = 0;

	dummy_window = CreateWindowExW(0, L"STATIC", L"OpenGL func ptr loading", WS_OVERLAPPEDWINDOW,
																 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
																 0, 0, 0, 0);
	if (dummy_window != 0)
	{
		dummy_dc = GetDC(dummy_window);
		if (dummy_dc != 0)
		{
			PIXELFORMATDESCRIPTOR dummy_pixel_format = {
				.nSize           = sizeof(PIXELFORMATDESCRIPTOR),
				.nVersion        = 1,
				.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				.iPixelType      = PFD_TYPE_RGBA,
				.cColorBits      = 24,
			};

			int dummy_pixel_format_index = ChoosePixelFormat(dummy_dc, &dummy_pixel_format);
			if (dummy_pixel_format_index != 0)
			{
				if (DescribePixelFormat(dummy_dc, dummy_pixel_format_index, sizeof(PIXELFORMATDESCRIPTOR), &dummy_pixel_format) &&
						SetPixelFormat(dummy_dc, dummy_pixel_format_index, &dummy_pixel_format))
				{
					dummy_context = wglCreateContext(dummy_dc);
					if (dummy_context != 0 && wglMakeCurrent(dummy_dc, dummy_context))
					{
						wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

						if (wglGetExtensionsStringARB != 0)
						{
							const char* extensions_string = wglGetExtensionsStringARB(dummy_dc);

							char* required_extensions[] = {
								"WGL_ARB_pixel_format",
								"WGL_ARB_create_context",
								"WGL_EXT_swap_control",
								"WGL_ARB_create_context_profile",
							};

							bool found_all_extensions = true;
							for (unsigned int i = 0; i < ARRAY_SIZE(required_extensions); ++i)
							{
								char* scan = (char*)extensions_string;

								bool found_extension = false;
								while (*scan != 0)
								{
									while (*scan == ' ') ++scan;

									if (CString_HasPrefix(scan, required_extensions[i]))
									{
										found_extension = true;
										break;
									}
									else
									{
										while (*scan != 0 && *scan != ' ') ++scan;
										continue;
									}
								}

								if (!found_extension) found_all_extensions = false;
							}

							if (found_all_extensions)
							{
								wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
								wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
								wglSwapIntervalEXT         = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
							}
						}
					}
				}
			}
		}
	}

	wglMakeCurrent(0, 0);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummy_window, dummy_dc);
	DestroyWindow(dummy_window);

	if (wglGetExtensionsStringARB && wglChoosePixelFormatARB && wglCreateContextAttribsARB && wglSwapIntervalEXT)
	{
		*dc = GetDC(window);
		if (*dc != 0)
		{
			int pixel_format_attribute_list[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB , GL_TRUE,
				WGL_PIXEL_TYPE_ARB    , WGL_TYPE_RGBA_ARB,
				WGL_COLOR_BITS_ARB    , 24,
				WGL_DEPTH_BITS_ARB    , 24,
				WGL_STENCIL_BITS_ARB  , 8,
				0, // End
			};

			int pixel_format;
			UINT num_formats = 0;
			if (wglChoosePixelFormatARB(*dc, pixel_format_attribute_list, 0, 1, &pixel_format, &num_formats) && num_formats != 0)
			{
				PIXELFORMATDESCRIPTOR pixel_format_desc = { .nSize = sizeof(PIXELFORMATDESCRIPTOR) };
				if (DescribePixelFormat(*dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pixel_format_desc) &&
						SetPixelFormat(*dc, pixel_format, &pixel_format_desc))
				{
					int context_attribute_list[] = {
						WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
						WGL_CONTEXT_MINOR_VERSION_ARB, 5,
						WGL_CONTEXT_PROFILE_MASK_ARB , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

#ifdef MU_DEBUG
						WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif

						0, // End
					};

					*gl_context = wglCreateContextAttribsARB(*dc, 0, context_attribute_list);
					if (*gl_context != 0 && wglMakeCurrent(*dc, *gl_context) && wglSwapIntervalEXT(1))
					{
						do
						{
#define MU_X(type, name) name = (type)wglGetProcAddress(#name); if (!name) break;
							LIST_GL_CORE_FUNCS
#undef MU_X

							glDebugMessageCallback(&Win32_GLDebugCallback, NULL);
							glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

							succeeded = true;
						} while (0);
					}
				}
			}
		}
	}

	/// Setup immediate mode rendering API
	if (succeeded)
	{
		// TODO:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		succeeded = false;
		do
		{
			// TODO: error checking
			glGenVertexArrays(1, &UnitRectVAO);
			glBindVertexArray(UnitRectVAO);

			V2 unit_rect_verts[] = {
				V2(-0.5, -0.5), V2( 0.5, -0.5), V2( 0.5,  0.5),
				V2( 0.5,  0.5), V2(-0.5,  0.5), V2(-0.5, -0.5),
			};

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(unit_rect_verts), unit_rect_verts, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, 0);
			glEnableVertexAttribArray(0);

			glBindVertexArray(0);

			char* vertex_code =
				"#version 450\n"
				"\n"
				"layout(location=0) in vec2 pos;\n"
				"layout(location=1) uniform mat4 transform;\n"
				"\n"
				"void\n"
				"main()\n"
				"{\n"
				"\tgl_Position = transform*vec4(pos.x, pos.y, 0, 1);\n"
				"}\n";

			char* fragment_code =
				"#version 450\n"
				"\n"
				"layout(location=2) uniform vec4 color;\n"
				"layout(location=3) uniform vec4 center_hdim;\n"
				"layout(location=4) uniform float line_thickness;\n"
				"layout(location=5) uniform vec4 corner_radii; // corners are labeled 0, 1, 3, 2 cw from -1,-1, corner_radii has w and z flipped so this can be an index op\n"
				"\n"
				"out vec4 frag_color;\n"
				"\n"
				"void\n"
				"main()\n"
				"{\n"
				"\t// based on box sdf learnt from: https://youtu.be/62-pRVZuS5c\n"
				"\t//                rounding from: https://youtu.be/s5NGeUV2EyU\n"
				"\t//              smoothstep from: https://youtu.be/60VoL-F-jIQ\n"
				"\t//     and rescaling trick from: https://stackoverflow.com/questions/71926442/how-to-round-the-corners-of-an-sdf-without-changing-its-size-in-glsl\n"
				"\tvec2 center   = center_hdim.xy;\n"
				"\tvec2 half_dim = center_hdim.wz;\n"
				"\tfloat pad     = 2;\n"
				"\n"
				"\tvec2 q  = gl_FragCoord.xy - center;\n"
				"\t// selecting radius for current corner based on the labeling 0,1,3,2 cw from -1,-1 with the input corner_radii swizzled from 0,1,2,3 to 0,1,3,2 to avoid this in the shader\n"
				"\tfloat r = corner_radii[int(sign(q.x) > 0) + 2*int(sign(q.y) > 0)];\n"
				"\tvec2 rq = abs(q) - (half_dim - vec2(r));                    // -r in both dims to shrink the box such that an enlargment of r will give it the original size\n"
				"\tfloat d = length(max(rq, 0)) + min(max(rq.x, rq.y), 0) - r; // distance to box border enlarged by r (which round of corners, r < sqrt(2)*r)\n"
				"\n"
				"\tfrag_color = color;\n"
				"\tif (line_thickness != 0) frag_color.a *= 1 - smoothstep(0, pad, abs(d) - line_thickness/2); // line_thickness == 0 is used for switching to filled instead of outlined\n"
				"\telse                     frag_color.a *= smoothstep(pad, 0, d);\n"
				"}\n";

			char* line_fragment_code =
				"#version 450\n"
				"\n"
				"layout(location=2) uniform vec4 color;\n"
				"layout(location=3) uniform vec4 p0_and_p1;\n"
				"layout(location=4) uniform vec2 p0p1;\n"
				"layout(location=5) uniform float half_line_thickness;\n"
				"layout(location=6) uniform float half_line_length;\n"
				"\n"
				"out vec4 frag_color;\n"
				"\n"
				"void\n"
				"main()\n"
				"{\n"
				"\tvec2 p0   = p0_and_p1.xy;\n"
				"\tvec2 p1   = p0_and_p1.zw;\n"
				"\tvec2 v    = p0p1;\n"
				"\tfloat pad = 2;\n"
				"\n"
				"\tvec2 p        = gl_FragCoord.xy - p0;\n"
				"\tfloat p_dot_v = dot(p, v);\n"
				"\tfloat d       = length(p - p_dot_v*v);\n"
				"\n"
				"\tfrag_color = color;\n"
				"\tfrag_color.a *= 1 - smoothstep(0, pad, abs(p_dot_v - half_line_length) - half_line_length);\n"
				"\tfrag_color.a *= 1 - smoothstep(0, pad, d - half_line_thickness);\n"
				"}\n";

			if (!GL_CreateProgram(vertex_code, fragment_code, &PushRectProgram)) break;
			if (!GL_CreateProgram(vertex_code, line_fragment_code, &PushLineProgram)) break;

			succeeded = true;
		} while (0);
	}

	if (succeeded)
	{
		*renderer_link = (Renderer_Link){
			.ClearScreen = &ClearScreen,
			.PushRect    = &PushRect,
			.PushLine    = &PushLine,
		};
	}

	return succeeded;
}
