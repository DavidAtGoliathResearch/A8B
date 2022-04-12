#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include "bmp.h"

int ImageSize
(
	int32 width,
	int32 height,
	int32 bytesPerPixel
)
{
	return width * height * bytesPerPixel;
}

Image AllocateImage
(
	int32 width,			// Integer variable - width of the image in pixels
	int32 height,			// Integer variable - height of the image in pixels 
	int32 bytesPerPixel		// Integer variable - number of bytes per pixel in the image
)
{
	int TotalSize = ImageSize(width, height, bytesPerPixel);
	Image img = malloc(TotalSize);
	if (img == NULL)
		printf("Insufficient memory available\n");
	else
		memset(img, 0, TotalSize);

	return img;
}

int ImageIndex
(
	int32 width,			// width (columns)
	int32 height,			// height (rows)
	int32 r,				// row
	int32 c					// column
)
{
	return (height - 1 - r) * width + c;
}

Image ReadImage
(
	// Inputs
	const char* fileName,	// The name of the file to open (*.BMP) 
	// Outputs
	int32* width,			// Integer variable - width of the image in pixels
	int32* height,			// Integer variable - height of the image in pixels 
	int32* bytesPerPixel	// Integer variable - number of bytes per pixel in the image
)
{
	//Open the file for reading in binary mode
	FILE* imageFile = fopen(fileName, "rb");

	//Read data offset
	int32 dataOffset;
	fseek(imageFile, DATA_OFFSET_OFFSET, SEEK_SET);
	fread(&dataOffset, 4, 1, imageFile);

	//Read width
	fseek(imageFile, WIDTH_OFFSET, SEEK_SET);
	fread(width, 4, 1, imageFile);

	//Read height
	fseek(imageFile, HEIGHT_OFFSET, SEEK_SET);
	fread(height, 4, 1, imageFile);

	//Read bits per pixel
	int16 bitsPerPixel;
	fseek(imageFile, BITS_PER_PIXEL_OFFSET, SEEK_SET);
	fread(&bitsPerPixel, 2, 1, imageFile);

	//Allocate a pixel array
	*bytesPerPixel = ((int32)bitsPerPixel) / 8;

	//Rows are stored bottom-up
	//Each row is padded to be a multiple of 4 bytes. 
	//We calculate the padded row size in bytes
	int paddedRowSize = (int)(4 * ceil((float)(*width) / 4.0f)) * (*bytesPerPixel);

	int unpaddedRowSize = (*width) * (*bytesPerPixel);

	//Get memory to store all the pixels
	Image img = AllocateImage(*width, *height, *bytesPerPixel);

	if (img == NULL)
		return img;

	//Read the pixel data Row by Row.
	//Data is padded and stored bottom-up
	//Point to the last row of our pixel array (unpadded)
	for (int32 r = 0; r < *height; r++)
	{
		//put file cursor in the next row from top to bottom
		fseek(imageFile, dataOffset + (r * paddedRowSize), SEEK_SET);

		//read only unpaddedRowSize bytes (we can ignore the padding bytes)
		int Index = ImageIndex(*width, *height, r, 0);
		fread(&img[Index], 1, unpaddedRowSize, imageFile);
	}

	fclose(imageFile);

	return img;
}

void WriteImage
(
	const char* fileName,	// The name of the file to save  
	Image img,				// Pointer to the pixel data array 
	int32 width,			// The width of the image in pixels
	int32 height,			// The height of the image in pixels
	int32 bytesPerPixel		// The number of bytes per pixel in the image
)
{
	//Open file in binary mode
	FILE* outputFile = fopen(fileName, "wb");

	//*****HEADER************//
	//write signature
	const char* BM = "BM";
	fwrite(&BM[0], 1, 1, outputFile);
	fwrite(&BM[1], 1, 1, outputFile);

	//Write file size considering padded bytes
	int paddedRowSize = (int)(4 * ceil((float)width / 4.0f)) * bytesPerPixel;
	int32 fileSize = paddedRowSize * height + HEADER_SIZE + INFO_HEADER_SIZE;
	fwrite(&fileSize, 4, 1, outputFile);

	//Write reserved
	int32 reserved = 0x0000;
	fwrite(&reserved, 4, 1, outputFile);

	//Write data offset
	int32 dataOffset = HEADER_SIZE + INFO_HEADER_SIZE;
	fwrite(&dataOffset, 4, 1, outputFile);

	//*******INFO*HEADER******//
	//Write size
	int32 infoHeaderSize = INFO_HEADER_SIZE;
	fwrite(&infoHeaderSize, 4, 1, outputFile);

	//Write width and height
	fwrite(&width, 4, 1, outputFile);
	fwrite(&height, 4, 1, outputFile);

	//Write planes
	int16 planes = 1; //always 1
	fwrite(&planes, 2, 1, outputFile);

	//write bits per pixel
	int16 bitsPerPixel = bytesPerPixel * 8;
	fwrite(&bitsPerPixel, 2, 1, outputFile);

	//write compression
	int32 compression = NO_COMPRESION;
	fwrite(&compression, 4, 1, outputFile);

	//write image size (in bytes)
	int32 imageSize = width * height * bytesPerPixel;
	fwrite(&imageSize, 4, 1, outputFile);

	//write resolution (in pixels per meter)
	int32 resolutionX = 11811; //300 dpi
	int32 resolutionY = 11811; //300 dpi
	fwrite(&resolutionX, 4, 1, outputFile);
	fwrite(&resolutionY, 4, 1, outputFile);

	//write colors used 
	int32 colorsUsed = MAX_NUMBER_OF_COLORS;
	fwrite(&colorsUsed, 4, 1, outputFile);

	//Write important colors
	int32 importantColors = ALL_COLORS_REQUIRED;
	fwrite(&importantColors, 4, 1, outputFile);

	//write data
	for (int32 i = 0; i < height; i++)
	{
		//start writing from the beginning of last row in the pixel array
		int Index = ImageIndex(width, height, i, 0);
		fwrite(&img[Index], 1, paddedRowSize, outputFile);
	}

	fclose(outputFile);
}

Image ApplyFilter
(
	Image		img,
	int32		width,
	int32		height,
	Filter3x3   ftr,
	double		factor,
	double		bias
)
{
	Image fimg = AllocateImage(width, height, 3);

	memcpy(fimg, img, ImageSize(width, height, 3));

	for (int32 row = 1; row < height - 1; row++)
		for (int32 col = 1; col < width - 1; col++)
		{
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			int Idx = 0;

			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
				{
					Idx = ImageIndex(width, height, row + i, col + j);

					double k = ftr[i + 1][j + 1];

					//printf
					//(
					//	"Idx[%d, %d] = %d, f[%d,%d] = %f * Img(%d, %d, %d)\n",
					//	row + i, col + j, Idx,
					//	i, j, k,
					//	img[Idx].r, img[Idx].g, img[Idx].b
					//);

					r += k * img[Idx].r;
					g += k * img[Idx].g;
					b += k * img[Idx].b;
				}

			Idx = ImageIndex(width, height, row, col);

			fimg[Idx].r = (byte)round(factor * (r / 9) + bias);
			fimg[Idx].g = (byte)round(factor * (g / 9) + bias);
			fimg[Idx].b = (byte)round(factor * (b / 9) + bias);
		}

	return fimg;
}