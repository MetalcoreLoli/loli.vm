#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "panic.h"

#define VM_STACK_CAPACITY 5
#define MAKE_INST_PUSH(val) ((inst_t) {.type=INST_PUSH, .operand=(val)})
#define MAKE_INST_JMP(addr) ((inst_t) {.type=INST_JMP, .operand=(addr)})
#define MAKE_INST_JE(addr) ((inst_t) {.type=INST_JE, .operand=(addr)})

#define MAKE_INST_SUM ((inst_t) {.type=INST_SUM})
#define MAKE_INST_SUB ((inst_t) {.type=INST_SUB})
#define MAKE_INST_MUL ((inst_t) {.type=INST_MUL})
#define MAKE_INST_DIV ((inst_t) {.type=INST_DIV})

typedef int64_t word_t;


typedef enum {
    INST_PUSH,
    INST_SUM,
    INST_SUB,
    INST_MUL,
    INST_DIV,
    INST_JMP,
    INST_JE,
} inst_type_t;

typedef struct inst {
    inst_type_t type;
    word_t operand;
} inst_t;

typedef struct program {
    inst_t *body;
    size_t size;
} program_t;

typedef struct loliVM {
    word_t stack[VM_STACK_CAPACITY];
    size_t stack_size;

    inst_t *program;
    word_t iptr;
} loliVM_t;


loli_panic_t vm_execute_inst (loliVM_t *vm, inst_t inst) {
    switch (inst.type) {
    case INST_PUSH: 
        if (vm->stack_size + 1 > VM_STACK_CAPACITY) {
            return PANIC_STACK_OVERFLOW;
        }
        vm->stack[vm->stack_size++] = inst.operand;
        break;
    case INST_SUM:
        if (vm->stack_size < 2) {
            return PANIC_STACK_UNDERFLOW;
        }
        vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
        vm->stack_size--; 
        break;
    case INST_SUB:
        if (vm->stack_size < 2) {
            return PANIC_STACK_UNDERFLOW;
        }
        vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
        vm->stack_size--; 
        break;
    
    case INST_MUL:
        if (vm->stack_size < 2) {
            return PANIC_STACK_UNDERFLOW;
        }
        vm->stack[vm->stack_size - 2] *= vm->stack[vm->stack_size - 1];
        vm->stack_size--; 
        break;
    
    case INST_DIV:
        if (vm->stack_size < 2) {
            return PANIC_STACK_UNDERFLOW;
        }
        if (vm->stack[vm->stack_size - 2] == 0 || vm->stack[vm->stack_size - 1] == 0) {
            return PANIC_DIV_BY_ZERO;
        }
        vm->stack[vm->stack_size - 2] /= vm->stack[vm->stack_size - 1];
        vm->stack_size--; 
        break;

    case INST_JMP: 
        vm->iptr = inst.operand;
        return PANIC_OK;
    
    case INST_JE: 
        if (vm->stack_size < 2) {
            return PANIC_STACK_UNDERFLOW;
        }
        if (vm->stack[vm->stack_size - 2] == vm->stack[vm->stack_size - 1]) {
            vm->iptr = inst.operand;
            return PANIC_OK;
        }
        break;
    default: return PANIC_ILLEGAL_INST;
    }
    vm->iptr++;
    return PANIC_OK;
}
loliVM_t vm = {0};


void vm_dump (const loliVM_t *vm) {
    fprintf (stdout, "Stack:\n");
    for (size_t i = 0; i < vm->stack_size; i++) {
        fprintf (stdout, "\t%ld\n", vm->stack[i]);
    }
}


void run_program(loliVM_t *vm, const program_t *program) {
    for (;(size_t)vm->iptr < program->size;) {
        loli_panic_t panic = vm_execute_inst (vm, program->body[vm->iptr]);
        if (panic != PANIC_OK) {
            vm_dump (vm);
            loli_is_panicing (panic);
        }
        //vm_dump (vm);
    }
    vm->iptr = 0;
}

int main ()
{
    inst_t body[] = {
        MAKE_INST_PUSH (1), // 0
        MAKE_INST_PUSH (2), // 1
        MAKE_INST_SUM,      // 2
        MAKE_INST_PUSH (3), // 3
        MAKE_INST_JE (2),   // 4
        MAKE_INST_SUM,
    };

    const size_t size_of_program = sizeof (body) / sizeof(body[0]);

    program_t program = {
        .size = size_of_program,
        .body = body
    };

    run_program(&vm, &program);

    vm_dump (&vm);
    return 0;
}
