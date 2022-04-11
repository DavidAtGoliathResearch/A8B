// Image Filters
#include "bmp.h"

typedef double Filter3x3[3][3];

Filter3x3 Blur =
{
	{1, 1, 1},
	{1, 1, 1},
	{1, 1, 1}
};

Filter3x3 Sharpen =
{
	{-1, -1, -1},
	{-1, +9, -1},
	{-1, -1, -1}
};

Image ApplyFilter
(
	Image		img,
	int32		width,
	int32		height,
	Filter3x3*	ftr
);
