// Reading and Writing uncompressed BMP images

#define DATA_OFFSET_OFFSET		0x000A
#define WIDTH_OFFSET			0x0012
#define HEIGHT_OFFSET			0x0016
#define BITS_PER_PIXEL_OFFSET	0x001C
#define HEADER_SIZE				14
#define INFO_HEADER_SIZE		40
#define NO_COMPRESION			0
#define MAX_NUMBER_OF_COLORS	0
#define ALL_COLORS_REQUIRED		0

typedef unsigned int int32;
typedef short int16;
typedef unsigned char byte;

// The 8 - bit per pixel(8bpp) format supports 256 distinct colors and stores 1 pixel per 1 byte.Each byte is an index into a table of up to 256 colors.
// The 16 - bit per pixel(16bpp) format supports 65536 distinct colors and stores 1 pixel per 2 - byte WORD.Each WORD can define the alpha, red, greenand blue samples of the pixel.
// The 24 - bit per pixel(24bpp) format supports 16, 777, 216 distinct colors and stores 1 pixel value per 3 bytes.
//   Each pixel value defines the Red, Green and Blue samples of the pixel(8.8.8.0.0 in RGBAX notation).
//	 Specifically, in the order : Blue, Green and Red(8 bits per each sample).[5]
// The 32 - bit per pixel(32bpp) format supports 4,294,967,296 distinct colors and stores 1 pixel per 4 - byte DWORD.
//	 Each DWORD can define the Alpha, Red, Green and Blue samples of the pixel.

typedef struct
{
	byte b;	// Blue
	byte g;	// Green
	byte r;	// Red
} PixelRGB24;

typedef PixelRGB24 *Image;

typedef double Filter3x3[3][3];

int ImageSize
(
	int32 width,
	int32 height,
	int32 bytesPerPixel
);

Image AllocateImage
(
	int32 width,			// Integer variable to store the width of the image in pixels
	int32 height,			// Integer variable to store the height of the image in pixels 
	int32 bytesPerPixel		// Integer variable to store the number of bytes per pixel used in the image
);

int ImageIndex
(
	int32 width,			// width (columns)
	int32 height,			// height (rows)
	int32 r,				// row
	int32 c					// column
);

Image ReadImage
(
	// Inputs
	const char* fileName,	// The name of the file to open (*.BMP) 
	// Outputs
	int32* width,			// Integer variable to store the width of the image in pixels
	int32* height,			// Integer variable to store the height of the image in pixels 
	int32* bytesPerPixel	// Integer variable to store the number of bytes per pixel used in the image
);

void WriteImage
(
	const char* fileName,	// The name of the file to save  
	Image img,				// Pointer to the pixel data array 
	int32 width,			// The width of the image in pixels
	int32 height,			// The height of the image in pixels
	int32 bytesPerPixel		// The number of bytes per pixel that are used in the image
);

Image ApplyFilter
(
	Image		img,
	int32		width,
	int32		height,
	Filter3x3	ftr,
	double		factor,
	double		bias
);
