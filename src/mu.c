#include "mu.h"

void
Tick(Engine_Link* engine_link)
{
	Engine = engine_link;

	Engine->ClearScreen(V4(0, 0, 0, 1));
	Engine->PushRect(Rect_FromPosDim(V2_Add(Engine->mouse_pos, V2(-300,    0)), V2( 50,  50)), V4(0, 0, 0, 0),     50, V4(1, 1, 1, 1));
	Engine->PushRect(Rect_FromPosDim(V2_Add(Engine->mouse_pos, V2(   0,    0)), V2(100, 100)), V4(0, 0, 0, 0),     10, V4(1, 1, 1, 1));
	Engine->PushRect(Rect_FromPosDim(V2_Add(Engine->mouse_pos, V2(+300,    0)), V2(100, 100)), V4(20, 20, 20, 20), 10, V4(1, 1, 1, 1));
	Engine->PushRect(Rect_FromPosDim(V2_Add(Engine->mouse_pos, V2(   0, +300)), V2( 50,  50)), V4(50, 50, 50, 50), 50, V4(1, 1, 1, 1));
	Engine->PushRect(Rect_FromPosDim(V2_Add(Engine->mouse_pos, V2(   0, -300)), V2(100, 100)), V4(50, 50, 50, 50), 10, V4(1, 1, 1, 1));
}
