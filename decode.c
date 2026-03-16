#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ptr = strstr(argv[2], ".bmp");

    if (ptr && strcmp(ptr, ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf("Destination file is NOT a .bmp file!\n");
        return d_failure;
    }

    if (argv[3] == NULL)
    {
        strcpy(decInfo->output_fname, "output");
    }
    else
    {
        char *noext = strtok(argv[3], ".");
        strcpy(decInfo->output_fname, noext);
    }

    printf("All Command Line Arguments Validated Successfully\n");
    return d_success;
}

Status open_destination_file(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf("ERROR: Unable to open file %s\n",
               decInfo->stego_image_fname);
        return d_failure;
    }
    return d_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_magic[10];

    int len = strlen(magic_string);

    for (int i = 0; i < len; i++)
    {
        fread(image_buffer, sizeof(char), 8, decInfo->fptr_stego_image);
        decoded_magic[i] = decode_byte_from_lsb(image_buffer);
    }
    decoded_magic[len] = '\0';

    printf("Decoded Magic String : %s\n", decoded_magic);

    return (strcmp(decoded_magic, magic_string) == 0) ? d_success : d_failure;
}

Status decode_output_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer, sizeof(char), 32, decInfo->fptr_stego_image);

    decInfo->output_extn_size = decode_size_from_lsb(image_buffer);

    printf("Decoded Secret File Extension Size : %d\n", decInfo->output_extn_size);

    return (decInfo->output_extn_size > 0) ? d_success : d_failure;
}

Status decode_output_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];
    for (int i = 0; i < decInfo->output_extn_size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decInfo->extn_output_file[i] = decode_byte_from_lsb(image_buffer);
    }

    decInfo->extn_output_file[decInfo->output_extn_size] = '\0';

    strcat(decInfo->output_fname, decInfo->extn_output_file);

    printf("Decoded Secret File Extension : %s\n", decInfo->extn_output_file);

    decInfo->fptr_output = fopen(decInfo->output_fname, "wb");
    return (decInfo->fptr_output) ? d_success : d_failure;
}

Status decode_output_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, sizeof(char), 32, decInfo->fptr_stego_image);

    decInfo->size_output_file = decode_size_from_lsb(buffer);

    printf("Decoded Secret File Data Size : %d bytes\n",
            decInfo->size_output_file);

    return (decInfo->size_output_file > 0) ? d_success : d_failure;
}

Status decode_output_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_byte;

    printf("Decoded Secret File Data : ");

    for (int i = 0; i < decInfo->size_output_file; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decoded_byte = decode_byte_from_lsb(image_buffer);
        printf("%c", decoded_byte);
        fwrite(&decoded_byte, 1, 1, decInfo->fptr_output);
    }

    printf("\n");
    fclose(decInfo->fptr_output);

    return d_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;

    for (int i = 0; i < 8; i++)
        data=data | ((image_buffer[i] & 1) << i);

    return data;
}

int decode_size_from_lsb(char *imageBuffer)
{
    unsigned int size = 0;

    for (int i = 0; i < 32; i++)
        size =size| ((imageBuffer[i] & 1) << i);

    return size;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("You Have Chosen Decoding...");

    if (open_destination_file(decInfo) == d_success)
        printf("Destination File Opened Successfully\n");
    else
        return d_failure;

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string(MAGIC_STRING, decInfo) == d_success)
        printf("Magic String Decoded Successfully from Destination File\n");
    else
        return d_failure;

    if (decode_output_file_extn_size(decInfo) == d_success)
        printf("Secret File Extension Size Decoded Successfully from Destination File\n");
    else
        return d_failure;

    if (decode_output_file_extn(decInfo) == d_success)
        printf("Secret File Extension Decoded Successfully from Destination File\n");
    else
        return d_failure;

    if (decode_output_file_size(decInfo) == d_success)
        printf("Secret File Data Size Decoded Successfully from Destination File\n");
    else
        return d_failure;

    if (decode_output_file_data(decInfo) == d_success)
    {
        printf("Secret File Data Decoded Successfully from Destination File\n");
        printf("Decoding Process is Completed Successfully\n");
        return d_success;
    }

    return d_failure;
}
