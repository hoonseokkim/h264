#include <stdio.h>
#include <math.h>

typedef struct Chunks
{
    int length;                  // 4 bytes
    unsigned char chunk_type[4]; // 4 bytes
    unsigned char *chunk_data;   // length bytes
    unsigned char crc[4];        // 4 bytes
} Chunks;

typedef struct PngInfo
{
    int width;                        // 4 bytes
    int height;                       // 4 bytes
    unsigned char bit_depth;          // 1 byte
    unsigned char color_type;         // 1 byte
    unsigned char compression_method; // 1 byte
    unsigned char filter_method;      // 1 byte
    unsigned char interlace_method;   // 1 byte
} PngInfo;

int is_png_format(FILE *fp);
int parse_chunks(FILE *fp, int buff_index);
int get_png_info(const unsigned char *data, PngInfo *png_info);