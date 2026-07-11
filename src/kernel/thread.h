

typedef struct thread {
    unsigned int esp_t;
    struct thread *next_thread;
} thread_t;