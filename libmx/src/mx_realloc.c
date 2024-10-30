#include "libmx.h"
#include <malloc.h>


void *mx_realloc(void *ptr, size_t size) {
    if (!ptr) { return NULL; }

    void *new_ptr =  malloc(size);
    
    size_t usefull_info_size = malloc_usable_size(ptr);
    size_t bytes_to_read = (usefull_info_size < size) ? usefull_info_size : size;

    mx_memcpy(new_ptr, ptr, bytes_to_read);

    free(ptr);


    return new_ptr;
}


