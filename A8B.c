#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "bmp.h"

// https://en.wikipedia.org/wiki/BMP_file_format

Filter3x3 Copy =
{
	{0, 0, 0},
	{0, 9, 0},
	{0, 0, 0}
};

Filter3x3 Blur =
{
	{1, 1, 1},
	{1, 1, 1},
	{1, 1, 1}
};

Filter3x3 Sharpen =
{
	{-1, -1, -1},
	{-1, +17, -1},
	{-1, -1, -1}
};

void PrintFilter(Filter3x3 ftr)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			printf("Filter[%d, %d] = %f\n", i, j, ftr[i][j]);
}

int main(int argc, char *argv[])
{
	Image Img = NULL;
	int32 width;
	int32 height;
	int32 bytesPerPixel;

	if (argc < 4)
	{
		printf("Usage: A8B <input-image> <output-image> [C(opy) | B(lur) | S(harpen)]\n");
		return 1;
	}

	Img = ReadImage(argv[1], &width, &height, &bytesPerPixel);
	if (bytesPerPixel != 3)
	{
		printf("Unsupported BMP format with %d bytes per pixel!", bytesPerPixel);
		return 2;
	}

	if (Img == NULL)
	{
		printf("Image is too big (or memory is scarce)!");
		return 3;
	}

	Filter3x3* f;
	switch (toupper(argv[3][0]))
	{
	case 'C':
		f = Copy;
		break;
	case 'B':
		f = Blur;
		break;
	case 'S':
		f = Blur;
		break;
	default:
		printf("Unknown filter %s!", argv[3]);
		return 4;
	}

	Image OutImg = ApplyFilter(Img, width, height, *f, 1, 0);

	WriteImage(argv[2], OutImg, width, height, bytesPerPixel);

	free(Img);
	free(OutImg);

	return 0;
}