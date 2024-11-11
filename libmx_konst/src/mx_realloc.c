#include <stddef.h>
#include <malloc.h>

void* mx_memcpy(void* restrict dst, const void* restrict src, size_t n);


static int need_malloc(const size_t need, const size_t have) {
	if ((have <= 24 && need <= 24) || (have > 24 && have <= 40 && need <= 40 && need != 0)) {
		return 0;
	}
	if ((have > need && have - need >= 32) || need > have)
		return 1;
	return 0;
}

void* mx_realloc(void* ptr, size_t size) {
	if ((!ptr && size) || (!size && !ptr)) {
		free(ptr);
		return malloc(size);
	}
	if (!size) {
		free(ptr);
		return NULL;
	}
	const size_t have_size = malloc_usable_size(ptr);
	if (need_malloc(size, have_size)) {
		void* temp_ptr = malloc(size);
		if (temp_ptr == NULL)
			return NULL;
		mx_memcpy(temp_ptr, ptr, (have_size < size) ? have_size : size);
		free(ptr);
		return temp_ptr;
	}
	return ptr;
}
