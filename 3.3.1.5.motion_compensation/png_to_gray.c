#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "png_to_gray.h"

#define DEFAULT_BUF_SIZE 4
#define MAGIC_NUMBER_SIZE 8
#define IS_IEND_CHUNK(chunk_type) ((chunk_type[0] == 73) && (chunk_type[1] == 69) && (chunk_type[2] == 78) && (chunk_type[3] == 68) ? (1) : (0))
#define IS_IHDR_CHUNK(chunk_type) ((chunk_type[0] == 73) && (chunk_type[1] == 72) && (chunk_type[2] == 68) && (chunk_type[3] == 82) ? (1) : (0))
#define IS_IDAT_CHUNK(chunk_type) ((chunk_type[0] == 73) && (chunk_type[1] == 68) && (chunk_type[2] == 65) && (chunk_type[3] == 84) ? (1) : (0))
#define IS_pHYs_CHUNK(chunk_type) ((chunk_type[0] == 112) && (chunk_type[1] == 72) && (chunk_type[2] == 89) && (chunk_type[3] == 115) ? (1) : (0))

int is_png_format(FILE *fp)
{
    // unsigned char should be used to prevent sign extention.
    // https://stackoverflow.com/questions/31090616/printf-adds-extra-ffffff-to-hex-print-from-a-char-array
    unsigned char magic_buffer[MAGIC_NUMBER_SIZE];
    int png_magic_number[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    fread(magic_buffer, sizeof(magic_buffer), 1, fp);
    for (int i = 0; i < MAGIC_NUMBER_SIZE; i++)
    {
        if (magic_buffer[i] != png_magic_number[i])
        {
            // printf("magic_buffer[%d] : %x\n", i, magic_buffer[i]);
            // printf("png_magic_number[%d] : %x\n", i, png_magic_number[i]);
            return -1;
        }
    }

    return 0;
}

int get_png_info(const unsigned char *data, PngInfo *png_info)
{
    printf("IHDR\n");

    // https://www.w3.org/TR/PNG/#11IHDR
    // Width	4 bytes
    // Height	4 bytes
    // Bit depth	1 byte
    // Colour type	1 byte
    // Compression method	1 byte
    // Filter method	1 byte
    // Interlace method	1 byte

    return 0;
}

int parse_chunks(FILE *fp, int buff_index)
{
    Chunks chunk;
    PngInfo png_info;
    unsigned char buff[DEFAULT_BUF_SIZE];
    bool is_iend = false;
    int unit = 0;

    while (!is_iend)
    {
        // 1. Get the length of chunks
        chunk.length = 0;
        fseek(fp, buff_index, SEEK_SET);
        fread(buff, sizeof(buff), 1, fp);
        for (int i = 0; i < DEFAULT_BUF_SIZE; i++)
        {
            unit = buff[i] * pow(pow(2, 8), (3 - i));
            chunk.length += unit;
        }
        // printf("chunk.length: %d\n", chunk.length);
        buff_index += DEFAULT_BUF_SIZE;

        // 2. Get the chunk type of chunks
        fseek(fp, buff_index, SEEK_SET);
        fread(chunk.chunk_type, sizeof(chunk.chunk_type), 1, fp);
        memcpy(buff, chunk.chunk_type, DEFAULT_BUF_SIZE);
        printf("chunk_type: %s\n", buff);

        if (IS_IEND_CHUNK(chunk.chunk_type) == 1)
            is_iend = true;

        buff_index += DEFAULT_BUF_SIZE;

        // 3. Get the Chunk data of chunks
        chunk.chunk_data = malloc(sizeof(unsigned char) * chunk.length);
        fseek(fp, buff_index, SEEK_SET);
        fread(chunk.chunk_data, sizeof(chunk.chunk_data), 1, fp);
        // for (int i = 0; i < chunk.length; i++)
        // {
        //     printf("chunk.chunk_data[%d]: %x\n", i, chunk.chunk_data[i]);
        // }
        if (IS_IHDR_CHUNK(chunk.chunk_type))
        {
            if (get_png_info(chunk.chunk_data, &png_info) != 0)
            {
                printf("FAILED");
            }
        }
        else if (IS_IDAT_CHUNK(chunk.chunk_type))
        {
        }
        else if (IS_pHYs_CHUNK(chunk.chunk_type))
        {
        }

        buff_index += chunk.length;

        // 4. Get the CRC of chunks
        fseek(fp, buff_index, SEEK_SET);
        fread(chunk.crc, sizeof(chunk.crc), 1, fp);
        // for (int i = 0; i < DEFAULT_BUF_SIZE; i++)
        // {
        //     printf("chunk.crc: %x\n", chunk.crc[i]);
        // }
        buff_index += DEFAULT_BUF_SIZE;
    }

    return 0;
}

int main()
{
    struct Chunks chunk;

    FILE *fp;
    int buff_index = 0;

    fp = fopen("../BBB-360-png/big_buck_bunny_00401.png", "r");
    if (fp == NULL)
    {
        perror("Error opening file");
        fclose(fp);
        return (-1);
    }

    if (is_png_format(fp) != 0)
    {
        perror("Invalid PNG format");
        fclose(fp);
        return (-1);
    }

    buff_index += MAGIC_NUMBER_SIZE;
    printf("Succeess to read png, buf indx : %d\n", buff_index);

    if (parse_chunks(fp, buff_index) == -1)
    {
        perror("Failed to parse chunks");
        fclose(fp);
        return (-1);
    }

    fclose(fp);

    return 0;
}