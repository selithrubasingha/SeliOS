

typedef struct {
    unsigned int num_of_files;

}main_header;

typedef struct {
    unsigned int magic_number;//to know if file isn't corrupted
    unsigned int file_length;
    unsigned int starting_offset;
    char file_name[128];
}file_header;