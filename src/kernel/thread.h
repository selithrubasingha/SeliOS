void create_thread(void (*thread_function)());

typedef struct thread {
    unsigned int esp;
    struct thread *next_thread;
} thread_t;