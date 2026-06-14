// Define some basic color codes for the VGA text mode
#define FB_BLACK 0
#define FB_GREEN 2

void kmain() {
    // Create a pointer to the exact physical address of the VGA framebuffer
    char *fb = (char *) 0x000B8000;

    // Write the letter 'S' to the first slot on the screen
    fb[0] = 'S';
    
    // Write the color for the 'S' (Green text on Black background)
    // The bitwise math combines the foreground and background colors into one byte
    fb[1] = ((FB_BLACK & 0x0F) << 4) | (FB_GREEN & 0x0F);

    // Write the letter 'E' to the second slot
    fb[2] = 'E';
    fb[3] = ((FB_BLACK & 0x0F) << 4) | (FB_GREEN & 0x0F);

    // Write the letter 'L' to the third slot
    fb[4] = 'L';
    fb[5] = ((FB_BLACK & 0x0F) << 4) | (FB_GREEN & 0x0F);

    // Write the letter 'I' to the fourth slot
    fb[6] = 'I';
    fb[7] = ((FB_BLACK & 0x0F) << 4) | (FB_GREEN & 0x0F);
}