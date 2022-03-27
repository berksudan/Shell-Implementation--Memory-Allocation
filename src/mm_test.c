#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>


/* Function pointers to hw3 functions */
void* (*mm_malloc)(size_t);
void* (*mm_realloc)(void*, size_t);
void (*mm_free)(void*);

void (*mm_print_mem)(void);

void load_alloc_functions() {
    void *handle = dlopen("hw3lib.so", RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    char* error;
    mm_malloc = dlsym(handle, "mm_malloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_realloc = dlsym(handle, "mm_realloc");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_free = dlsym(handle, "mm_free");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }

    mm_print_mem = dlsym(handle, "mm_print_mem");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", dlerror());
        exit(1);
    }
}

int main() {
    load_alloc_functions();

    int *data = (int*) mm_malloc(sizeof(int));
     int *data1 = (int*) mm_malloc(sizeof(int));
      int *data2 = (int*) mm_malloc(sizeof(int));
       int *data3 = (int*) mm_malloc(sizeof(int));

    assert(data != NULL);
    data[0] = 0x162;

    mm_print_mem();
    mm_free(data1);
     mm_free(data2);
     printf("----------------------\n");
     mm_print_mem();

   mm_print_mem();

    printf("malloc test successful!\n");
    return 0;
}
