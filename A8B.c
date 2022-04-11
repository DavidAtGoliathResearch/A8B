#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

// https://en.wikipedia.org/wiki/BMP_file_format

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

void PrintFilter(Filter3x3 ftr)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			printf("Filter[%d, %d] = %f\n", i, j, ftr[i][j]);
}

int main()
{
	Image Img = NULL;
	int32 width;
	int32 height;
	int32 bytesPerPixel;

	printf("<<Blur Filter>>\n");
	PrintFilter(Blur);

	Img = ReadImage("BadDog.bmp", &width, &height, &bytesPerPixel);
	if (bytesPerPixel != 3)
	{
		printf("Unsupported BMP format with %d bytes per pixel!", bytesPerPixel);
		return 1;
	}

	if (Img == NULL)
	{
		printf("Image is too big!");
		return 2;
	}
	
	Image BlurImg = ApplyFilter(Img, width, height, Blur);

	// Compare Img and BlurImg

	for (int32 row = 0; row < height; row++)
		for (int32 col = 0; col < width; col++)
		{
			int Index = ImageIndex(width, height, row, col);

			if (abs(Img[Index].r - BlurImg[Index].r) > 5)
				printf
				(
					"row = %d, col = %d, Index = %d, Img(%d, %d, %d) <> BlurImg(%d, %d, %d)\n",
					row, col, Index, 
					Img[Index].r, Img[Index].g, Img[Index].b,
					BlurImg[Index].r, BlurImg[Index].g, BlurImg[Index].b
				);
		}

	WriteImage("BadDogCopy.bmp", Img, width, height, bytesPerPixel);
	WriteImage("BadDogBlur.bmp", BlurImg, width, height, bytesPerPixel);

	free(Img);
	free(BlurImg);

	return 0;
}