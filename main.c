#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


OperationType check_operation_type(char *);
EncodeInfo encInfo;
DecodeInfo decInfo;

int main(int argc, char *argv[])
{
    
    if(argc<2)
    {
      printf("Insufficient number of arguments.\n");
      printf("For Encoding:\033[0m ./a.out -e source_image.bmp secret.txt output_image.bmp\n");
      printf("For Decoding:\033[0m ./a.out -d output_image.bmp output_file\n");
      return e_failure;
    }

    if(check_operation_type(argv[1])==e_encode)
    {
        if(argc>=4)
        {
            if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
               if(do_encoding(&encInfo)==e_success)
               {
                  printf("Encoding process completed successfully.\n");
               }
               else
               {
                  printf("Encoding process failed.\n");
                  return e_failure;
               }
            }
            else
            {
               return e_failure;
            }
        }
        else
        {
            printf("Insufficient arguments for encoding.\n");
            printf("For Encoding:\033[0m ./a.out -e source_image.bmp secret.txt(.c or .sh) output_image.bmp\n");
            return e_failure;
        }
    }
    else if(check_operation_type(argv[1])==e_decode)
    {
        if(argc >= 3)
        {
            if (read_and_validate_decode_args(argv,&decInfo)==d_success)
            {
               if(do_decoding(&decInfo)==d_success)
               {
                  printf("Decoding process completed successfully");
               }
               else
               {
                  printf("Decoding process failed.\n");
                  return e_failure;
               }
            }
            else
            {
               return d_failure;
            }
        }
        else
        {
            printf("Insufficient arguments for decoding.\n");
            printf("For Decoding:\033[0m ./a.out -d output_image.bmp output_file\n");
            return d_failure;
        }
    }
    else
    {
        printf("Unsupported operation type,Use -e for encode or -d for decode.\n");
        return e_failure;
    }
  
  return 0;
}


OperationType check_operation_type(char *symbol)
{
    if(strcmp(symbol,"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(symbol,"-d")==0)
    {
        return e_decode;
    }
    else
    {
       return e_unsupported;
    }
}



