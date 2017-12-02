
#define UINT unsigned short
#define DWORD long
#define LONG long
#define WORD short
#define BYTE unsigned char
#define BI_RGB 0L

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
  Header, 14 bytes.
    16 bytes FileType;        Magic number: "BM",
    32 bytes FileSize;        Size of file in 32 byte integers,
    16 bytes Reserved1;       Always 0,
    16 bytes Reserved2;       Always 0,
    32 bytes BitmapOffset.    Starting position of image data, in bytes.
*/

typedef struct tagBITMAPFILEHEADER {    /* bmfh */
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;

/*
  The bitmap header is 40 bytes long.
    32 bytes unsigned Size;            Size of this header, in bytes.
    32 bytes Width;                    Image width, in pixels.   
    32 bytes Height;                   Image height, in pixels.  (Pos/Neg, origin at bottom, top)
    16 bytes Planes;                   Number of color planes (always 1).
    16 bytes BitsPerPixel;             1 to 24.  1, 4, 8 and 24 legal.  16 and 32 on Win95.
    32 bytes unsigned Compression;     0, uncompressed; 1, 8 bit RLE; 2, 4 bit RLE; 3, bitfields.
    32 bytes unsigned SizeOfBitmap;    Size of bitmap in bytes. (0 if uncompressed).
    32 bytes HorzResolution;           Pixels per meter. (Can be zero)
    32 bytes VertResolution;           Pixels per meter. (Can be zero)
    32 bytes unsigned ColorsUsed;      Number of colors in palette.  (Can be zero).
    32 bytes unsigned ColorsImportant. Minimum number of important colors. (Can be zero).
*/

typedef struct tagBITMAPINFOHEADER{
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;

typedef BITMAPINFOHEADER *LPBITMAPINFOHEADER;
typedef BITMAPINFOHEADER *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD;



