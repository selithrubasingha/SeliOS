#ifndef TERMINAL_H
#define TERMINAL_H

#define BUFFER_MAX 256

extern int shell_active;
extern int enter_pressed;

void init_terminal();
void terminal_handle_scan(unsigned char scancode);
void terminal_run_command(const char *cmd);

#endif