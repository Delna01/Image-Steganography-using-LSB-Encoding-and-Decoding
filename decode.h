
#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char output_fname[50];    
    FILE *fptr_output;        

    int output_extn_size;     
    char extn_output_file[10];  

    int size_output_file;      

    char *stego_image_fname;  
    FILE *fptr_stego_image;   

} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_destination_file(DecodeInfo *decInfo);

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_output_file_extn_size(DecodeInfo *decInfo);
Status decode_output_file_extn(DecodeInfo *decInfo);
Status decode_output_file_size(DecodeInfo *decInfo);
Status decode_output_file_data(DecodeInfo *decInfo);

char decode_byte_from_lsb(char *image_buffer);
int decode_size_from_lsb(char *imageBuffer);

Status do_decoding(DecodeInfo *decInfo);

#endif
