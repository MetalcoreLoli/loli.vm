#pragma once
#include <stdlib.h>
#include <stdio.h>

typedef enum loli_panic {
    PANIC_STACK_OVERFLOW,
    PANIC_STACK_UNDERFLOW,
    PANIC_DIV_BY_ZERO,
    PANIC_NOT_IMPLIMENTED,
    PANIC_ILLEGAL_INST,
    PANIC_ILLEGAL_INST_ACCESS,
    PANIC_ILLEGAL_FILE_ACCESS,
    PANIC_SEGMENTATION_FAULT,
    PANIC_OK,
} loli_panic_t;


const char *panic_cstr (loli_panic_t panic); 
void loli_is_panicing(loli_panic_t panic);
