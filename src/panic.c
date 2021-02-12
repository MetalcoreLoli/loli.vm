#include "panic.h"

const char *panic_cstr (loli_panic_t panic) {
    switch (panic) {
    case PANIC_OK:                      return "PANIC_OK";
    case PANIC_STACK_OVERFLOW:          return "PANIC_STACK_OVERFLOW";
    case PANIC_STACK_UNDERFLOW:         return "PANIC_STACK_UNDERFLOW";
    case PANIC_DIV_BY_ZERO:             return "PANIC_DIV_BY_ZERO";
    case PANIC_NOT_IMPLIMENTED:         return "PANIC_NOT_IMPLIMENTED";
    case PANIC_ILLEGAL_INST:            return "PANIC_ILLEGAL_INST";
    case PANIC_ILLEGAL_INST_ACCESS:     return "PANIC_ILLEGAL_INST_ACCESS";
    }
    return "PANIC_OK";
}

void loli_is_panicing(loli_panic_t panic) {
    fprintf (stderr, "ERROR: %s\n", panic_cstr(panic));
    exit (1);
}


