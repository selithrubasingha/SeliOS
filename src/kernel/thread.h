

typedef struct thread {
    unsigned int esp;
    struct thread *next_thread;
} thread_t;