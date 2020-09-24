#pragma once

struct Complex
{
	double real, imag;
};

struct ScreenDim
{
	int width, height;
};

struct Bounds
{
	double left, right, down, up;
};

union Color
{
	struct
	{
		unsigned char r, g, b, a;
	};
	unsigned int value;
};

struct Cords
{
	int x, y;
};