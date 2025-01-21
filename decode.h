#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding output image to collect secret file data
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    //char* secret_fname;
    char secret_fname[30];
    FILE *fptr_secret;
    
    /* Stego Image Info */
    char stego_image_fname[100];
    FILE *fptr_stego_image;
    
    uint decoded_magic_string_length;
    char* decoded_magic_string;
    char* user_magic_string;
    uint extension_length;
    char* secret_file_extension;
    //secret file data after decoding.
    uint decoded_secret_file_size;
    char *decoded_secret_file_data;
    FILE *fptr_decoded_file;
} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_Dec(DecodeInfo *decInfo);

/*skipping header*/
Status skip_header(DecodeInfo *decInfo);

/* Decode Magic String length */
Status decode_magic_string_length(DecodeInfo *decInfo);

/*decode integer from lsb of image data array*/
uint decode_integer_to_lsb(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_charact_to_lsb(char ch,DecodeInfo *decInfo);

/* decode secret file file extension size */
Status decode_secret_file_extn_len(DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/*opening file and write to it the secret data*/
Status write_to_file(DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

#endif
