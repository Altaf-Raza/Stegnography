
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   
    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr_secret)
 {
    fseek(fptr_secret, 0, SEEK_END);
    uint file_size = ftell(fptr_secret);
    fseek(fptr_secret, 0, SEEK_SET);
    if(file_size!=0)
    return file_size;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("Opening required Files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s\n",encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s\n",encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO : Opened %s\n",encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}

/* check capacity */
Status check_capacity(EncodeInfo *encInfo) {
    printf("INFO : Checking Capacity\n");
    // Get the size of the BMP image
    uint image_size = get_image_size_for_bmp(encInfo->fptr_src_image);
   
    // Compute the encoding size
    uint encode_size = 54 * 8;  // Header size

    // Get the size of the secret file
    printf("INFO : Checking for %s size\n",encInfo->secret_fname);
    uint secret_file_size = get_file_size(encInfo->fptr_secret);
    encode_size += secret_file_size * 8; // Secret file data size

    printf("Enter the magic string : "); //taking magic string form the user
    scanf("%s",encInfo->magic_string);
    
    encode_size += strlen(encInfo->magic_string) * 8; // Dynamic magic string size

    encode_size += strlen(encInfo->secret_file_extension) * 8; // Extension size


    // Compare image size with encoding size
    printf("INFO : Checking for %s Capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if (image_size > encode_size) {
        printf("INFO : Done. Found Ok\n");
        return e_success;
    } else {
        printf("Error: Image size is not sufficient for encoding.\n");
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image) {
    char header[54];
    rewind(fptr_src_image);//bringing offset to starting position;
    // Read the BMP header from the source image
    if (fread(header, 1, 54, fptr_src_image) != 54) {
        printf("Error: Unable to read BMP header from source image.\n");
        return e_failure;
    }

    // Write the BMP header to the stego image
    printf("INFO : Copying Image Header\n");
    if (fwrite(header, 1, 54, fptr_stego_image) != 54) {
        printf("Error: Unable to write BMP header to stego image.\n");
        return e_failure;
    }
    printf("INFO : Done!\n");
    return e_success;
}

Status encode_magic_string_len(long len, EncodeInfo *encInfo)
{
    printf("INFO : Encoding Magic String Length\n");
    //call encode_integer_to_lsb function
    encode_integer_to_lsb(len, encInfo);
    printf("INFO : DONE !\n");
    return e_success;
}

Status encode_integer_to_lsb(long len, EncodeInfo *encInfo)
{
    //read 32 bytes from source image and store it in an array
    char integerbytes[32];
    fread(integerbytes,32,1,encInfo->fptr_src_image);

    for(int i=0;i<32;i++)
    {
	    integerbytes[i] = (integerbytes[i] & (~1) ) | (unsigned)((len & (1 << i)) >> i);  
    }
    //write those 32-bits from integerbytes[32] array to file stream of outputfile(stego image)
    fwrite(integerbytes,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO : Encoding Magic String Signature\n");
    //call encode_character_to_lsb function.
    encode_character_to_lsb(magic_string, encInfo);
    printf("INFO : Done !\n");
    
    return e_success;
}

Status encode_character_to_lsb(char *data, EncodeInfo *encInfo)
{
    //run loop for length of data pointer times.
    //out loop indicates for one character inner loop runs 8 times --> 8-bit times
    for(int i=0; i<strlen(data); i++)
    {
        //read 8 bytes from source file
        char characterbytes[8];     //array to store bytes of data read from source file.
        fread(characterbytes, 8 , 1 , encInfo->fptr_src_image);
        //again run loop for each character conatain 8 bits = 1byte 
        for(int j=0;j<8;j++)
        {
            characterbytes[j] = characterbytes[j] & (~1) |  ((data[i] & (1<<j))>> j);    //replace the lsb with our data string of each character. 
        }
        fwrite(characterbytes, 8 , 1 , encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_len(const char *file_extn, EncodeInfo *encInfo)  
{
    printf("INFO : Encoding Secret File Extension Length\n");
    //call encode integer to lsb function.
    encode_integer_to_lsb(strlen(file_extn), encInfo);
    printf("INFO : DONE !\n");
    
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO : Encoding Secret File Extension\n");
    encode_character_to_lsb(encInfo->secret_file_extension,encInfo);
    printf("INFO : DONE !\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO : Encoding Secret File Size\n");
    encode_integer_to_lsb(file_size,encInfo);
    printf("INFO : DONE !\n");
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO : Encoding Secret File Data\n");
    int total_bytes = get_file_size(encInfo -> fptr_secret);
    char secret_data[total_bytes];
	fseek(encInfo -> fptr_secret,0,SEEK_SET); // Move file pointer to the start position.

    fread( secret_data, total_bytes , 1 ,encInfo -> fptr_secret ); // reading data of size of file at one time. 
	char str[8];        //for storing source (or) image data.
	for ( int i = 0; i <  total_bytes; i++ )
	{
		fread(str,8,1,encInfo -> fptr_src_image);
        for(int j=0;j<8;j++)
        {
            str[j]= str[j]& (~1) |  ((secret_data[i] & (1<<j))>>j);   //replace the lsb with our data string of each character. 
        }
		fwrite(str,8,1,encInfo -> fptr_stego_image);
	}
    printf("INFO : DONE !\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO : Copying Remaining Data ..\n");
    char ch;
    while(fread(&ch, 1, 1, fptr_src) > 0)   //read one time one byte to ch variable (8 bits/1 byte). 
    {
        fwrite(&ch, 1, 1, fptr_dest);  //write to output i.e stego_image.
    }
    printf("INFO : DONE !\n");
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("SUCCESS: check_operation function completed\n");
    printf("-----Your Choosen Operation is ENCODE----\n");
    //open files
    if (open_files(encInfo) == e_failure)
    {
    	printf("INFO: ERROR, %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("INFO : Done\n" );
    }
    
    printf("## Encoding Procedure started ##\n");
    //check capacity
    if(check_capacity(encInfo) == e_success)    //if true.
    {
        // copy_bmp_header
        if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)     //if true.
        {  
            //encode magic string len(int) (beacuse it tells how many next bytes should to fetch).
            if(encode_magic_string_len(strlen(encInfo->magic_string),encInfo) == e_success)        //if true.
            {
                //encode magic string(char)
                if( encode_magic_string(encInfo->magic_string, encInfo) == e_success)
                {
                    //encode secret file extension length(int)
                   if( encode_secret_file_extn_len(encInfo->secret_file_extension, encInfo) == e_success)
                   {
                        //encode secret file extension.
                        if(encode_secret_file_extn(encInfo->secret_file_extension,  encInfo) == e_success )
                        {
                            //encode secret file size.
                            if(encode_secret_file_size(get_file_size(encInfo -> fptr_secret), encInfo) == e_success)
                            {
                                //encode secret file data.
                                if( encode_secret_file_data(encInfo) == e_success)
                                {
                                    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("INFO : Encoding done sucessfully\n");
                                    }
                                }
                            }
                        }
                   }
                }
            }
        }  
    }
}


