#include <stdio.h>
#include <stdlib.h>
#include <string.h>





typedef struct {
    unsigned int num_of_files;

}main_header_t;

typedef struct {
    unsigned int magic_number;//to know if file isn't corrupted
    unsigned int file_length;
    unsigned int starting_offset;
    char file_name[128];
}file_header_t;


// Helper function to get the size of a file in bytes
unsigned int getFileSize(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Could not open %s to check size.\n", filename);
        return 0;
    }
    
    // Jump to the absolute end of the file
    fseek(file, 0, SEEK_END);
    
    // Read the current position (which gives us the total size)
    unsigned int size = ftell(file);
    
    fclose(file);
    return size;
}

// Helper function to copy data from the source file into the initrd blob
void dumpFileData(const char *filename, FILE *outputFile) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Could not open %s to read data.\n", filename);
        return;
    }
    
    // Read and write in 4KB chunks to safely handle large files
    char buffer[4096];
    size_t bytesRead;
    
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytesRead, outputFile);
    }
    
    fclose(file);
}


int main() {
    // Open the output file for writing
    FILE *outputFile = fopen("initrd.img", "wb");
    if (outputFile == NULL) {
        perror("Failed to create initrd.img");
        return EXIT_FAILURE;
    }

    // Define the number of files to pack
    int numFiles = 2;
    fwrite(&numFiles, sizeof(int), 1, outputFile);

    // Calculate offsets and write file headers
    long offset = sizeof(int) + numFiles * sizeof(file_header_t);
    file_header_t headers[2];

    // Prepare first file header
    strcpy(headers[0].file_name, "hello.txt");
    headers[0].file_length = getFileSize("hello.txt");
    headers[0].starting_offset = offset;

    // Prepare second file header
    strcpy(headers[1].file_name, "test.txt");
    headers[1].file_length = getFileSize("test.txt");
    headers[1].starting_offset = offset + headers[0].file_length;

    // Write file headers to output file
    fwrite(headers, sizeof(file_header_t), numFiles, outputFile);

    // Dump the data for each file
    dumpFileData("hello.txt", outputFile);
    dumpFileData("test.txt", outputFile);

    // Close the output file
    fclose(outputFile);
    
    return EXIT_SUCCESS;
}