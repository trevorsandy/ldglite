///////////////////////////////////////
// DL1 Quantization

#ifndef QUANT_H
#define QUANT_H

typedef struct {
    DWORD r, g, b;
    DWORD pixel_count;
    DWORD pixels_in_cube;
    BYTE children;
    BYTE palette_index;
} CUBE;

typedef struct {
    BYTE level;
    WORD index;
} FCUBE;

typedef struct {
    BYTE palette_index, red, green, blue;
    DWORD distance;
    DWORD squares[255+255+1];
} CLOSEST_INFO;

BOOL dl1quant(BYTE *inbuf, BYTE *outbuf, int width, int height, int quant_to, int dither, BYTE userpal[3][256]);

static void copy_pal(BYTE userpal[3][256]);
static void dlq_init(void);
static int  dlq_start(void);
static void dlq_finish(void);
static int  build_table(BYTE *image, DWORD pixels);
static void fixheap(DWORD id);
static void reduce_table(int num_colors);
static void set_palette(int index, int level);
static void closest_color(int index, int level);
static int  quantize_image(BYTE *in, BYTE *out, int width, int height, int dither);
static int  bestcolor(int r, int g, int b);

#endif

