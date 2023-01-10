typedef struct V2
{
	f32 x, y;
} V2;

typedef struct V3
{
	union
	{
		struct { f32 x, y, z; };
		struct { V2 xy; f32 _0; };
		struct { f32 _1; V2 yz; };

		struct { f32 r, g, b; };
		struct { V2 rg; f32 _2; };
		struct { f32 _3; V2 gb; };
	};
} V3;

typedef struct V4
{
	union
	{
		struct { f32 x, y, z, w; };
		struct { V3 xyz; f32 _0; };
		struct { f32 _1; V3 yzw; };
		struct { V2 xy; V2 zw; };
		struct { f32 _2; V2 yz; f32 _3; };

		struct { f32 r, g, b, a; };
		struct { V3 rgb; f32 _4; };
		struct { f32 _5; V3 bga; };
		struct { V2 rg; V2 ba; };
		struct { f32 _6; V2 gb; f32 _7; };
	};
} V4;

#define V2(X, Y) (V2){ .x = (f32)(X), .y = (f32)(Y) }

#define V3(X, Y, Z) (V3){ .x = (f32)(X), .y = (f32)(Y), .z = (f32)(Z) }

#define V4(X, Y, Z, W) (V4){ .x = (f32)(X), .y = (f32)(Y), .z = (f32)(Z), .w = (f32)(W) }

V2
V2_Add(V2 a, V2 b)
{
	return V2(a.x + b.x, a.y + b.y);
}

V3
V3_Add(V3 a, V3 b)
{
	return V3(a.x + b.x, a.y + b.y, a.z + b.z);
}

V4
V4_Add(V4 a, V4 b)
{
	return V4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

V2
V2_Sub(V2 a, V2 b)
{
	return V2(a.x - b.x, a.y - b.y);
}

V3
V3_Sub(V3 a, V3 b)
{
	return V3(a.x - b.x, a.y - b.y, a.z - b.z);
}

V4
V4_Sub(V4 a, V4 b)
{
	return V4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

V2
V2_Scale(V2 a, f32 n)
{
	return V2(a.x*n, a.y*n);
}

V3
V3_Scale(V3 a, f32 n)
{
	return V3(a.x*n, a.y*n, a.z*n);
}

V4
V4_Scale(V4 a, f32 n)
{
	return V4(a.x*n, a.y*n, a.z*n, a.w*n);
}

V2
V2_Hadamard(V2 a, V2 b)
{
	return V2(a.x*b.x, a.y*b.y);
}

V3
V3_Hadamard(V3 a, V3 b)
{
	return V3(a.x*b.x, a.y*b.y, a.z*b.z);
}

V4
V4_Hadamard(V4 a, V4 b)
{
	return V4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
}

V3
V3_RgbFromHex(u32 hex)
{
	return V3((f32)(hex >> 16) / 255, (f32)((hex >> 8) & 0xFF) / 255, (f32)(hex & 0xFF) / 255);
}

u32
V3_HexFromRgb(V3 rgb)
{
	return ((u32)rgb.r*255 << 16) | ((u32)rgb.g*255 << 8) | (u32)rgb.b*255;
}

typedef struct M2
{
	union
	{
		struct
		{
			V2 i;
			V2 j;
		};
		f32 e[4];
	};
} M2;

typedef struct M3
{
	union
	{
		struct
		{
			V3 i;
			V3 j;
			V3 k;
		};
		f32 e[9];
	};
} M3;

typedef struct M4
{
	union
	{
		struct
		{
			V4 i;
			V4 j;
			V4 k;
			V4 l;
		};
		f32 e[16];
	};
} M4;

typedef struct Rect
{
	V2 min;
	V2 max;
} Rect;

Rect
Rect_FromMinMax(V2 min, V2 max)
{
	return (Rect){ .min = min, .max = max };
}

Rect
Rect_FromPosDim(V2 pos, V2 dim)
{
	V2 half_dim = V2_Scale(dim, 0.5f);

	return (Rect){
		.min = V2_Sub(pos, half_dim),
		.max = V2_Add(pos, half_dim),
	};
}
