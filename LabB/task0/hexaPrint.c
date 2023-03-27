#include <stdio.h>
#include <stdlib.h>

/* Print the hexadecimal values of each byte in the buffer*/
void hexaPrint(const char *buffer, size_t length)
{
    for (int i = 0; i < length; i++) {
        printf("%02X ", (unsigned char) buffer[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    char inChar;
    long filesize = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    FILE *inFile = fopen(filename, "rb");
    if (!inFile) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return 1;
    }

    // Get the file size
    while((inChar = fgetc(inFile)) != EOF){
        filesize++;
    }

    fseek(inFile, 0, SEEK_SET); //had troubles returnning to beginning of file (after the while fgetc), found better way then closing and opening again on stackOverFlow:
                                // https://stackoverflow.com/questions/32366665/resetting-pointer-to-the-start-of-file

    // printf("filesize:%li\n",filesize);

    // Allocate a buffer for the file content
    char *buffer = (char *) malloc(filesize);
    if (!buffer) {
        fprintf(stderr, "Error: could not allocate memory\n");
        fclose(inFile);
        return 1;
    }

    // Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, filesize, inFile);
    if (bytes_read != filesize) {
        fprintf(stderr, "bytesRead: %i ,Error: could not read entire file\n",bytes_read);
        free(buffer);
        fclose(inFile);
        return 1;
    }

    hexaPrint(buffer, filesize);


    // Clean up
    free(buffer);
    fclose(inFile);
    return 0;
}
