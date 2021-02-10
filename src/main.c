#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define VM_STACK_CAPACITY 5
#define MAKE_INST_PUSH(val) ((inst_t) {.type=INST_PUSH, .operand=(val)})
#define MAKE_INST_JMP(addr) ((inst_t) {.type=INST_JMP, .operand=(addr)})
#define MAKE_INST_JE(addr) ((inst_t) {.type=INST_JE, .operand=(addr)})

#define MAKE_INST_SUM ((inst_t) {.type=INST_SUM})
#define MAKE_INST_SUB ((inst_t) {.type=INST_SUB})
#define MAKE_INST_MUL ((inst_t) {.type=INST_MUL})
#define MAKE_INST_DIV ((inst_t) {.type=INST_DIV})

typedef int64_t word_t;
typedef enum loli_panic {
    PANIC_STACK_OVERFLOW,
    PANIC_STACK_UNDERFLOW,
    PANIC_DIV_BY_ZERO,
    PANIC_NOT_IMPLIMENTED,
    PANIC_ILLEGAL_INST,
    PANIC_OK,
} loli_panic_t;

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

const char *panic_cstr (loli_panic_t panic) {
    switch (panic) {
    case PANIC_OK:              return "PANIC_OK";
    case PANIC_STACK_OVERFLOW:  return "PANIC_STACK_OVERFLOW";
    case PANIC_STACK_UNDERFLOW: return "PANIC_STACK_UNDERFLOW";
    case PANIC_DIV_BY_ZERO:     return "PANIC_DIV_BY_ZERO";
    case PANIC_NOT_IMPLIMENTED: return "PANIC_NOT_IMPLIMENTED";
    case PANIC_ILLEGAL_INST:    return "PANIC_ILLEGAL_INST";
    }
    return "PANIC_OK";
}

void loli_is_panicing(loli_panic_t panic) {
    fprintf (stderr, "ERROR: %s\n", panic_cstr(panic));
    exit (1);
}

void vm_dump (const loliVM_t *vm) {
    fprintf (stdout, "Stack:\n");
    for (size_t i = 0; i < vm->stack_size; i++) {
        fprintf (stdout, "\t%ld\n", vm->stack[i]);
    }
}

loliVM_t vm = {0};

void run_program(loliVM_t *vm, const inst_t *program, size_t size_of_program, size_t count_of_runs) {
    for (;vm->iptr < size_of_program;) {
        loli_panic_t panic = vm_execute_inst (vm, program[vm->iptr]);
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
    inst_t program[] = {
        MAKE_INST_PUSH (1), // 0
        MAKE_INST_PUSH (2), // 1
        MAKE_INST_SUM,      // 2
        MAKE_INST_PUSH (3), // 3
        MAKE_INST_JE (2),   // 4
    };

    const size_t size_of_program = sizeof (program) / sizeof(program[0]);

    for (size_t i = 0;  i < 1; i++) {
        run_program(&vm, program, size_of_program, 1);
    }
    //    run_program(&vm, program, size_of_program);

    vm_dump (&vm);
    return 0;
}
