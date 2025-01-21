#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
int main(int argc,char*argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    Status option=e_unsupported;
    if(argc>1){
      option =check_operation_type(argv);
    }
    else
    {
        printf("Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\nDecoding : ./a.out -d <.bmp file> [output file]\n");
        return 0;
    }
    //if option is e_encode --->user wants to perform encode;
    if(option==e_encode)
    {
        printf("Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
        if(read_and_validate_encode_args(argc,argv,&encInfo)==e_success)
        {
          printf("INFO : Read and Validate for Encode args is successful!\n");
          do_encoding(&encInfo);
          return e_success;
        }
        
    }
    else if(option==e_decode)
    {
        printf("Decoding : ./a.out -d <.bmp file> [output file]\n");
        if(read_and_validate_decode_args(argc,argv,&decInfo)==e_success)
        {
            printf("INFO : Read and Validate for Decode args is successful!\n");
            do_decoding(&decInfo);
            return e_success;
        }
    }   
    else{
         printf("INFO : Checking Operation Type\n");
         printf("ERROR : Your operation should be either -e or -d\n");
         printf("Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\nDecoding : ./a.out -d <.bmp file> [output file]\n");
    }

    return 0;
}

//check_operation_type
OperationType check_operation_type(char*argv[])
{
    //if argv[1] is "-e"
    if(!strcmp(argv[1],"-e"))
        return e_encode;

    //else if argv[1] is"-d
    else if (!strcmp(argv[1],"-d"))
    return e_decode;

    else return e_unsupported;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    if(argc>3)
    {
       if(strrchr(argv[2], '.')!=NULL && !strcmp(strrchr(argv[2], '.'),".bmp")) //for source file name
       strcpy(encInfo->src_image_fname,argv[2]);  //storing source file name
       else
       {
        printf("Source file is Invalid!\n");
        return e_failure;
       }
       
       if(strrchr(argv[3],'.')!=NULL) //for secret file name and secret file extension
       {
        strcpy(encInfo->secret_fname,argv[3]);  //storing secret file name
        strcpy(encInfo->secret_file_extension,strrchr(argv[3],'.')); //storing secret file extension
       }
       else
       {
        printf("Secret file is Invalid!\n");
         return e_failure;
       } 
    }
    else return e_failure;

    if(argc==5)
    {
        strcpy(encInfo->stego_image_fname,argv[4]); //storing output image file name
    }
    else if(argc==4)
    {
        printf("INFO : Output file not mentioned.  creating stego_img as default\n");
        strcpy(encInfo->stego_image_fname,"stego_img"); ////storing output image file name by default
    }
    strcat(encInfo->stego_image_fname, ".bmp"); //adding .bmp extension to the output image file name

    return e_success;
}