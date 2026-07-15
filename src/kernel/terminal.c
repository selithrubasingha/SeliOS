#include "terminal.h"
#include "utils.h"
#include "string.h"
#include "fs.h"

int shell_active = 0;
int enter_pressed = 0;

static char input_buffer[BUFFER_MAX];
static unsigned int buffer_len = 0;

extern unsigned int total_ram_bytes;
extern unsigned int kernel_size;
extern fs_node_t *fs_root;  

// Simple keyboard map for raw translation inside the buffer
static const char kbd_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

void init_terminal() {
    clear_screen();
    // Use inline hex formatting \x0E to toggle Yellow, \x0B for Cyan, \x07 for Light Grey
    printf(DEVICE_FB, "\x0EWelcome to the SeliOS Interactive Shell Environment\n");
    printf(DEVICE_FB, "\x0BType 'help' to see the list of architectural system options.\n\n\x07");
    printf(DEVICE_FB, "selith@SeliOS / # ");
    buffer_len = 0;
    input_buffer[0] = '\0';
}



void terminal_handle_scan(unsigned char scancode) {
    // Check if it's a key release event (break code)
    if (scancode & 0x80) return;

    // Enter Key pressed
    if (scancode == 0x1C) {
        if (!shell_active) {
            enter_pressed = 1;
            shell_active = 1;
        } else {
            putchar(DEVICE_FB, '\n');
            input_buffer[buffer_len] = '\0';
            
            if (buffer_len > 0) {
                terminal_run_command(input_buffer);
            }
            
            if (shell_active) {
                printf(DEVICE_FB, "selith@SeliOS / # ");
            }
            buffer_len = 0;
            input_buffer[0] = '\0';
        }
        return;
    }

    // Backspace pressed
    if (scancode == 0x0E) {
        if (buffer_len > 0 && shell_active) {
            buffer_len--;
            input_buffer[buffer_len] = '\0';
            cursor_pos--;
            fb_write_cell(cursor_pos * 2, ' ', DEFAULT_FG, DEFAULT_BG);
            fb_move_cursor(cursor_pos);
        }
        return;
    }

    // Normal Character processing
    if (shell_active) {
        char ascii = kbd_map[scancode];
        if (ascii != 0 && buffer_len < (BUFFER_MAX - 1)) {
            input_buffer[buffer_len++] = ascii;
            putchar(DEVICE_FB, ascii);
        }
    }
}


void terminal_run_command(const char *cmd) {
    if (strcmp(cmd, "help") == 0) {
        printf(DEVICE_FB, "Available commands:\n");
        printf(DEVICE_FB, "  \x0Bhelp\x07      - Display this command dictionary.\n");
        printf(DEVICE_FB, "  \x0B" "clear\x07     - Clear the VGA display buffer.\n");
        printf(DEVICE_FB, "  \x0B" "whoami\x07    - Reveal current system privileges.\n");
        printf(DEVICE_FB, "  \x0B" "selifetch\x07 - Output modern system architecture stats.\n");
        printf(DEVICE_FB, "  \x0B" "memory\x07    - Display physical RAM and Kernel footprint.\n");
    } 
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    } 
    else if (strcmp(cmd, "whoami") == 0) {
        printf(DEVICE_FB, "root (Ring 0 - Full Kernel Clearances)\n");
    }
    else if (strcmp(cmd, "memory") == 0) {
        printf(DEVICE_FB, "\x0B=== System Memory Intelligence ===\x07\n");
        
        printf(DEVICE_FB, "Total RAM Available : ");
        print_hex(total_ram_bytes);
        printf(DEVICE_FB, " Bytes\n");
        
        printf(DEVICE_FB, "Kernel Footprint    : ");
        print_hex(kernel_size);
        printf(DEVICE_FB, " Bytes\n\n");
    }
    else if (strcmp(cmd, "selifetch") == 0) {
        printf(DEVICE_FB, "\x0A   ____       _ _  ___  ____   \x07 Host: Lenovo Legion Pro 5\n");
        printf(DEVICE_FB, "\x0A  / ___|  ___| (_)/ _ \\/ ___|  \x07 OS:   SeliOS x86 Microkernel\n");
        printf(DEVICE_FB, "\x0A  \\___ \\ / _ \\ | | | | \\___ \\  \x07 Architecture: i686-elf\n");
        printf(DEVICE_FB, "\x0A   ___) |  __/ | | |_| |___) | \x07 Target Environment: Ring 0 Kernel\n");
        printf(DEVICE_FB, "\x0A  |____/ \\___|_|_|\\___/|____/  \x07 UI Framework: Text Mode 80x25\n\n");
    }
    else if (strcmp(cmd, "ls") == 0) {
        if (fs_root != NULL) {
            int i = 0;
            struct dirent *node = 0;
            
            // Loop through directory indices until it returns nothing
            while ((node = readdir_fs(fs_root, i)) != 0) {
                // Print the file names in bright Cyan so they look like files!
                printf(DEVICE_FB, "  \x0B"); 
                printf(DEVICE_FB, node->name);
                printf(DEVICE_FB, "\x07\n");
                i++;
            }
        } else {
            printf(DEVICE_FB, "\x0CVFS not mounted.\x07\n");
        }
    }
    // Hardcoded string check: Does it start with "cat " ?
    else if (cmd[0] == 'c' && cmd[1] == 'a' && cmd[2] == 't' && cmd[3] == ' ') {
        if (fs_root != NULL) {
            // Extract the filename by reading the string starting at index 4
            char *filename = (char *)&cmd[4]; 
            
            // Search the VFS for the file
            fs_node_t *file_node = finddir_fs(fs_root, filename);

            if (file_node != NULL) {
                char buffer[256];
                // Read the file's contents into our buffer
                unsigned int bytes_read = read_fs(file_node, 0, 255, buffer);
                buffer[bytes_read] = '\0'; // Safely cap the string
                
                // Print the file contents!
                printf(DEVICE_FB, buffer);
                printf(DEVICE_FB, "\n");
            } else {
                printf(DEVICE_FB, "\x0CError: File '");
                printf(DEVICE_FB, filename);
                printf(DEVICE_FB, "' not found in root directory.\x07\n");
            }
        }
    }
    else {
        // Use inline color \x0C (Red) for the warning, print the cmd, then switch back to \x07 (Grey)
        printf(DEVICE_FB, "\x0C" "Error: Unknown command executable '");
        printf(DEVICE_FB, cmd);
        printf(DEVICE_FB, "'.\n\x07");
    }
}
