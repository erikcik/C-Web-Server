// #include <stdio.h>
// #include <stdlib.h>
// #include <zlib.h>
// #include <string.h>
// #include <assert.h>

// void displayError(char *msg)
// {
//     perror(msg);
//     exit(1);
// }

// #define CHUNK 16384

// void encompressFile(FILE *source, char *outputBuffer)
// {
//     int returnValue, flush;
//     z_stream strm;
//     unsigned char inputBuffer[CHUNK];

//     strm.zalloc = Z_NULL;
//     strm.zfree = Z_NULL;
//     strm.opaque = Z_NULL;
//     strm.adler = (long unsigned int)"CRC-32";

//     returnValue = deflateInit2(&strm, 6, Z_DEFLATED, 16, 8, Z_DEFAULT_STRATEGY);
    
//     printf("value is: %d\n",returnValue);
//     do
//     {
//         strm.avail_in = fread(inputBuffer, 1, CHUNK, source);
//         if (ferror(source))
//         {
//             (void)deflateEnd(&strm);
//             displayError("Error happened while reading the data");
//         }
//         flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//         strm.next_in = inputBuffer;

//         do
//         {
//             strm.avail_out = CHUNK;
//             strm.next_out = (Bytef *)outputBuffer;
//             returnValue = deflate(&strm, flush);
//             if (returnValue == Z_STREAM_ERROR)
//             {
//                 displayError("Error happened while compressing the data");
//                 (void)deflateEnd(&strm);
//             }
//             size_t compressedBytes = CHUNK - strm.avail_out;
//             outputBuffer += compressedBytes;
//         } while (strm.avail_out == 0);
//     } while (flush != Z_FINISH);

//     deflateEnd(&strm);
// }

// // void encompressFile(FILE* input, char* output, int size)
// // {
// //     char *inputBuffer = (char *)malloc(size);
// //     if(inputBuffer == NULL){
// //         printf("No memory allocated for input buffer");
// //         exit(1);
// //     }

// //     z_stream zs;
// //     zs.zalloc = Z_NULL;
// //     zs.zfree = Z_NULL;
// //     zs.opaque = Z_NULL;
// //     zs.avail_in = fread(inputBuffer, 1, size, input);
// //     zs.next_in = inputBuffer;snap info code
// //     zs.avail_out = (uInt)size;
// //     zs.next_out = (Bytef *)output;

// //     // hard to believe they don't have a macro for gzip encoding, "Add 16" is the best thing zlib can do:
// //     // "Add 16 to windowBits to write a simple gzip header and trailer around the compressed data instead of a zlib wrapper"
// //     deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16, 8, Z_DEFAULT_STRATEGY);
// //     deflate(&zs, Z_FINISH);
// //     deflateEnd(&zs);
// //     free(inputBuffer);
// // }

// int def(FILE *source, FILE *dest, int level)
// {
//     int ret, flush;
//     unsigned have;
//     z_stream strm;
//     unsigned char in[CHUNK];
//     unsigned char out[CHUNK];

//     /* allocate deflate state */
//     strm.zalloc = Z_NULL;
//     strm.zfree = Z_NULL;
//     strm.opaque = Z_NULL;
//     ret = deflateInit2(&strm, 6, Z_DEFLATED, 16, 8,Z_DEFAULT_STRATEGY);
//     if (ret != Z_OK)
//         return ret;

//     /* compress until end of file */
//     do {
//         strm.avail_in = fread(in, 1, CHUNK, source);
//         if (ferror(source)) {
//             (void)deflateEnd(&strm);
//             return Z_ERRNO;
//         }
//         flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//         strm.next_in = in;

//         /* run deflate() on input until output buffer not full, finish
//            compression if all of source has been read in */
//         do {
//             strm.avail_out = CHUNK;
//             strm.next_out = out;
//             ret = deflate(&strm, flush);    /* no bad return value */
//             assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//             have = CHUNK - strm.avail_out;
//             if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                 (void)deflateEnd(&strm);
//                 return Z_ERRNO;
//             }
//         } while (strm.avail_out == 0);
//         assert(strm.avail_in == 0);     /* all input will be used */

//         /* done when last data in file processed */
//     } while (flush != Z_FINISH);
//     assert(ret == Z_STREAM_END);        /* stream will be complete */

//     /* clean up and return */
//     (void)deflateEnd(&strm);
//     return Z_OK;
// }