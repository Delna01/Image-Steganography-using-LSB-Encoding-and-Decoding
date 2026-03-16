#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET); 
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    long size_secret_file=ftell(fptr);
    rewind(fptr);
    return size_secret_file;    
}



Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
   char *ptr_source=(strstr(argv[2],".bmp"));   
   char *ptr_secret=(strstr(argv[3],"."));   
    if(ptr_source!=NULL && strcmp(ptr_source,".bmp")==0)
    {
        encInfo->src_image_fname = argv[2];   
        if((ptr_secret!=NULL && strcmp(ptr_secret,".txt")==0) || (ptr_secret!=NULL && strcmp(ptr_secret,".c")==0) || (ptr_secret!=NULL && strcmp(ptr_secret,".sh")==0))
        {
            encInfo->secret_fname = argv[3];  
            if(argv[4]==NULL)
            {

                encInfo->stego_image_fname="default.bmp";
                printf("Successfully validated encoding arguments.\n");
                return e_success;
            }
            else
            {
                encInfo->stego_image_fname =argv[4];
                char *ptr_dest=(strstr(argv[4],".bmp"));   
                if(ptr_dest!=NULL && strcmp(ptr_dest,".bmp")==0)
                {
                    encInfo->stego_image_fname =argv[4];
                    printf("Successfully validated encoding arguments.\n");
                    return e_success;
                }
                else
                {
                    printf("Error! Destination file is not in .bmp format.\n");
                    return e_failure;
                }
            }
        }
        else
        {
           printf("Error! Secret file is not in .txt, .c or .sh format.\n");
           return e_failure;
        }
        
    }
    else
    {
        printf("Error! Source file is not in .bmp format.\n");
        return e_failure;
    }    
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
     if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
   encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
   encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
 
   if((encInfo->image_capacity)>(16+32+32+32+(encInfo->size_secret_file*8)))  
   {
       return e_success;
   }
   else
   {
       return e_failure;
   }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char imagebuffer[54];
    fread(imagebuffer,sizeof(char),54,fptr_src_image);   
    fwrite(imagebuffer,sizeof(char),54,fptr_dest_image);   
    if(ftell(fptr_src_image)==ftell(fptr_dest_image))   
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imagebuffer[8];   
    int magic_stringlen=strlen(magic_string);  
    for(int i=0;i<magic_stringlen;i++)
    {
        fread(imagebuffer,sizeof(char),8,encInfo->fptr_src_image);   
        encode_byte_to_lsb(magic_string[i],imagebuffer);  
        fwrite(imagebuffer,sizeof(char),8,encInfo->fptr_stego_image);
    }
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imagebuffer[32];
    fread(imagebuffer,sizeof(char),32,encInfo->fptr_src_image);
    encode_size_to_lsb(size,imagebuffer);
    fwrite(imagebuffer,sizeof(char),32,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {

        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imagebuffer[8];
    for(int i=0;i<strlen(file_extn);i++)
    {
        fread(imagebuffer,sizeof(char),8,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],imagebuffer);
        fwrite(imagebuffer,sizeof(char),8,encInfo->fptr_stego_image);
    }    
        if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imagebuffer[32];
    fread(imagebuffer,sizeof(char),32,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,imagebuffer);
    fwrite(imagebuffer,sizeof(char),32,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}



Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char secret_data[encInfo->size_secret_file];
       

    fread(secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret);

    if (secret_data[encInfo->size_secret_file - 1] == '\n')
    {
        encInfo->size_secret_file--;  
    }
    char imagebuffer[8];
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
       fread(imagebuffer,sizeof(char),8,encInfo->fptr_src_image);
       encode_byte_to_lsb(secret_data[i],imagebuffer);  
       fwrite(imagebuffer,sizeof(char),8,encInfo->fptr_stego_image);
    }    
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   char temp[10];
   while(fread(temp,sizeof(char),1,fptr_src)==1)
   {
       fwrite(temp,sizeof(char),1,fptr_dest);
   }
   if(ftell(fptr_src)==ftell(fptr_dest))
   {
        return e_success;
   }
   else
   {
        return e_failure;
   }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i] = image_buffer[i] & ~(1 << 0);  
        image_buffer[i]=image_buffer[i] | ((data >> i) & 1);  
    }

}
Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for(int i=0;i<32;i++)
    {
        imageBuffer[i] = imageBuffer[i] & ~(1 << 0);  
        imageBuffer[i]=imageBuffer[i] | ((size >> i) & 1);
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
        printf("File opened successfully.\n");
    }
    else
    {
        return e_failure;
    }
    if(check_capacity(encInfo)==e_success)
    {
        printf("Excellent storage capacity.\n");
    }
    else
    {
        printf("Error! Insufficient image capacity.\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("Header copied successfully.\n");
    }
    else
    {
        printf("Error! Failed to copy the image header.\n");
        return e_failure;
    }





    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("Magic string encoded successfully.\n");
    }
    else
    {
        printf("Error! Failed to encode magic string.\n");
        return e_failure;
    }
    char *secret_ext= strstr(encInfo->secret_fname,".");
    strcpy(encInfo->extn_secret_file,secret_ext);
    int size_of_ext=strlen(encInfo->extn_secret_file);
    if(encode_secret_file_extn_size(size_of_ext,encInfo)==e_success)
    {
        printf("Secret file extension size encoded successfully.\n");
    }
    else
    {
        printf("Error!Failed to encode file extension size.\n");
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
    {
        printf("Secret file extension encoded successfully.\n");
    }
    else
    {
        printf("Error! Failed to encode file extension.\n");
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
 

    {
        printf("Secret file size encoded successfully.\n");
    }
    else
    {
        printf("Error! Failed to encode file size.\n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("Secret file data encoded successfully.\n");
    }
    else
    {
        printf("Error Failed to encode secret file data.\n");
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("Remaining data copied successfully.\n");
        return e_success;
    }
    else
    {
        printf("Error! Failed to copy remaining image data.\n");
        return e_failure;
    }
    
}