#include <stdio.h>
#include "decode.h"
#include<stdlib.h>
#include "types.h"
#include <string.h>



Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if(argc>2)
    {
            if(strchr(argv[2],'.')==NULL)
        {
            printf("INFO : Pass the encoded image file with proper extnesion!\n");
            return e_failure;
        }
        else if(!strcmp(strrchr(argv[2],'.'),".bmp"))
        {
            strcpy(decInfo->stego_image_fname,argv[2]); //storing stego file name
        }
        else
        {
            printf("INFO : Stego file extension is Invalid!\n");
            return e_failure;
        }
        if(argv[3]==NULL)
        {
            printf("INFO : Output file not mentioned, taking default.txt as file name\n");
            //decInfo->secret_fname="default.txt";
             strcpy(decInfo->secret_fname,"default");
            return e_success;
        }
        else
        {
            //decInfo->secret_fname=argv[3];
            strcpy(decInfo->secret_fname,argv[3]);
            return e_success;
        }
    }
    else e_failure;
}

Status open_files_Dec(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
    if(decInfo->fptr_stego_image==NULL)
    {
        printf("INFO : Unable to open the %s file\n",decInfo->stego_image_fname);
        return e_failure;
    }
    return e_success;
}

Status skip_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image,54,SEEK_SET);
    if(ftell(decInfo->fptr_stego_image)!=54)
    {
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string_length(DecodeInfo *decInfo)
{
    printf("INFO : Decoding magic string length\n");
    decInfo->decoded_magic_string_length = decode_integer_to_lsb(decInfo);
    return e_success;
}

uint decode_integer_to_lsb(DecodeInfo *decInfo)
{
    char bytes[32];
    fread(bytes,32,1,decInfo->fptr_stego_image);
    uint length=0;
    for(int i=0;i<32;i++)
    {
        length=length|(bytes[i]&1)<<i;
    }
    return length;
}

Status decode_magic_string_data(DecodeInfo *decInfo)
{
  decInfo->decoded_magic_string=(char*)malloc(decInfo->decoded_magic_string_length+1);
  printf("INFO : Decoding Magic string\n");
  for(int i=0;i<decInfo->decoded_magic_string_length;i++)
  {
    decInfo->decoded_magic_string[i]=0;
   decInfo->decoded_magic_string[i]=decode_charact_to_lsb(decInfo->decoded_magic_string[i],decInfo);
  }
  
  decInfo->user_magic_string=(char*)malloc(decInfo->decoded_magic_string_length+1);

  printf("Enter the magic string : ");
  scanf("%s",decInfo->user_magic_string);
  if(strcmp(decInfo->user_magic_string,decInfo->decoded_magic_string)== e_success)
  {
    printf("INFO : Magic String Matched\n");
    return e_success;
  }
    return e_failure;
}

char decode_charact_to_lsb(char ch,DecodeInfo *decInfo)
{
    char bytes[8];
    fread(bytes,8,1,decInfo->fptr_stego_image);
    for(int i=0;i<8;i++)
    {
        ch=ch | ((bytes[i]&1)<<i);
    } 
    return ch;
}

/* decode secret file file extension length */
Status decode_secret_file_extn_len(DecodeInfo *decInfo)
{
  printf("INFO : Decoding secret file extension length\n");
  decInfo->extension_length=decode_integer_to_lsb(decInfo);
  return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decInfo->secret_file_extension=(char*)malloc(decInfo->extension_length+1);
    printf("INFO : Decoding Extension of secret file\n");
    for(int i=0;i<decInfo->extension_length;i++)
    {
        decInfo->secret_file_extension[i]=decode_charact_to_lsb(decInfo->secret_file_extension[i],decInfo);
    }
    return e_success;

}

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO : Decoding Secret file size..\n");
    decInfo->decoded_secret_file_size=decode_integer_to_lsb(decInfo);
    return e_success;
}

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->decoded_secret_file_data=(char*)malloc(decInfo->decoded_secret_file_size+1);
    printf("INFO : Decoding secret file data..\n");
    for(int i=0;i<decInfo->decoded_secret_file_size;i++)
    {
        decInfo->decoded_secret_file_data[i]= 0;
        decInfo->decoded_secret_file_data[i]=decode_charact_to_lsb(decInfo->decoded_secret_file_data[i],decInfo);
    }
    return e_success;
}

Status write_to_file(DecodeInfo *decInfo)
{
    //concatenate the output file name with the retrived extension.
   strcpy( decInfo->secret_fname, strcat(decInfo->secret_fname,decInfo->secret_file_extension) );
    decInfo->fptr_decoded_file = fopen(decInfo->secret_fname,"w");

    //Error Handling
    if(decInfo->fptr_decoded_file == NULL)
    {
        printf("Opening output File Failed\n");
        return e_failure;
    }
    //writing secret data to output file.
    fwrite(decInfo->decoded_secret_file_data,decInfo->decoded_secret_file_size,1,decInfo->fptr_decoded_file);
    return e_success;  
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("SUCCESS: check_operation function completed\n");
    printf("-----Your Choosen Operation is DECODE----\n");
    printf("## Decoding procedure started ##\n");
    //1. open enocoded image(stego_image.bmp)
    if(open_files_Dec(decInfo) == e_success)
    {
        //2. skip header
        if(skip_header(decInfo) == e_success)
        {
            //3.decode length of magic string
            //to do that read read magic string from user.
            if(decode_magic_string_length(decInfo) == e_success)
            {
                printf("INFO : Done\n");
                //4.decode magic string based on length of user entered magic string.
                //store decoded magic string int an array.
                if(decode_magic_string_data(decInfo) == e_success)
                {   
                    printf("INFO : Done\n");
                    //5.Decode extension length of secret file
                    if(decode_secret_file_extn_len(decInfo) == e_success)
                    {
                        printf("INFO : Done\n");
                        //6. Decode Extension of secret file.
                        if(decode_secret_file_extn(decInfo) == e_success)
                        {
                            printf("INFO : Done\n");
                            //7.Decode sceret file length/size.
                            if(decode_secret_file_size(decInfo) == e_success)
                            {
                                printf("INFO : Done\n");
                                //8.Decode Secret File Data.
                                if(decode_secret_file_data(decInfo) == e_success)
                                {
                                    printf("INFO : Done\n");
                                    printf("\nINFO : ## Decoding Done Sucessfully ##\n\n");
                                    /* 9. open output file pointer to store retrived data.
                                         open output file by concatinating extension to output file name entered
                                         and write decoded secret data to it. */
                                    if (write_to_file(decInfo) == e_success)
                                    {
                                        return e_success;
                                    }
                                    else
                                    {
                                        printf("INFO : Failed to write to output file.\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("INFO : Decoding Secret File Data Failed\n");
                                    return e_failure;
                                }

                            }
                            else
                            {
                                printf("INFO : Decoding Secret File Length/Size Failed\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Info : Decoding File Extension Failed\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("INFO : Decoding File Extension Length Failed\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("INFO : ERROR, Entered Magic String(Password) is Incorrect\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO: Decoding Magic String Length Failed\n");
                return e_failure;
            }
        }
        else
        {
            printf("INFO: Header Skip Failed..\n");
            return e_failure;
        }
    }
    else
    {
        printf("INFO: Opening Encoded Image Failed\n");
        return e_failure;
    } 
}