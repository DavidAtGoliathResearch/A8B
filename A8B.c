#include <stdio.h>
#include "bmp.h"

// https://en.wikipedia.org/wiki/BMP_file_format

int main()
{
	byte* pixels;	// pixels[height][width]
	int32 width;
	int32 height;
	int32 bytesPerPixel;
	PixelRGB24* Img = 0;

	ReadImage("img.bmp", &pixels, &width, &height, &bytesPerPixel);
	if (bytesPerPixel != 24)
	{
		printf("Unsupported BMP format with %d bytes per pixel!", bytesPerPixel);
		return 1;
	}

	Img = (PixelRGB24 * )pixels;

	// ProcessImage

	WriteImage("img2.bmp", pixels, width, height, bytesPerPixel);
	free(pixels);

	return 0;
}