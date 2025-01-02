#include "common.h"
#include "mem.h"
#include <assert.h>
#include <stdio.h>

/* Size of the managed memory
 *
 * For this to work, we expect a call to
 * mem_init(get_memory_adr(), get_memory_size())
 */
#ifndef MEMORY_SIZE
#define MEMORY_SIZE 8192
#endif

static char memory[MEMORY_SIZE];

void *get_memory_adr() {
    return memory;
}

size_t get_memory_size() {
    return MEMORY_SIZE;
}

void *alloc_max(size_t estimate) {
    void *result;
    static size_t last = 0;

    while ((result = mem_alloc(estimate)) == NULL) {
        estimate--;
        if (!estimate) {
            fprintf(stderr, "Allocation failed\n");
            return NULL;
        }
    }
    debug("Alloced %zu bytes at %p\n", estimate, result);
    if (last) {
        // Idempotence test
        assert(estimate == last);
    } else
        last = estimate;
    return result;
}
