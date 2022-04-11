#include "filters.h"

Image ApplyFilter
(
	Image		img,
	int32		width,
	int32		height,
	Filter3x3* ftr
)
{
	Image fimg = AllocateImage(width, height, 3);

	for (int row = 1; row < height - 1; row++)
		for (int col = 1; col < width - 1; col++)
		{
			double r, g, b = 0;
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
				{
					int Index = ImageIndex(width, height, row + j, col + i);
					r += *ftr[i, j] * fimg[Index].r;
				}

		}

	return fimg;
}