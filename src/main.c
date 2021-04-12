#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "panic.h"
#include "../dep/Collectoins/headers/clist.h"
#include "../dep/Collectoins/headers/carray.h"

#define VM_STACK_CAPACITY 1024 
#define VM_PROGAM_CAPACITY 1024 
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
    INST_HALT,
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

    inst_t program[VM_PROGAM_CAPACITY];
    size_t size_of_program;

    word_t iptr;

    _Bool halt;
} loliVM_t;


loli_panic_t vm_execute_inst (loliVM_t *vm) {
    if (vm->iptr < 0 || vm->iptr >= (word_t)vm->size_of_program) {
        return PANIC_ILLEGAL_INST_ACCESS;
    }
    inst_t inst = vm->program[vm->iptr];
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

    case INST_HALT: 
        vm->halt = 1;
        return PANIC_OK;
        
    default: return PANIC_ILLEGAL_INST;
    }
    vm->iptr++;
    return PANIC_OK;
}
loliVM_t vm = {0};

char * instAsCstr(inst_type_t type) 
{
    switch (type) 
    {
    case INST_PUSH: 
        return "PUSH";
        
    case INST_MUL: 
        return "MUL";
            
    case INST_DIV: 
        return "DIV";
        
    case INST_SUM: 
        return "SUM";
        
    case INST_SUB: 
        return "SUB";
            
    case INST_JE: 
        return "JE";

    default: 
        loli_is_panicing (PANIC_ILLEGAL_INST_ACCESS);
    }
}

void vm_dump (const loliVM_t *vm) {
    fprintf (stdout, "Stack:\n");
    for (size_t i = 0; i < vm->stack_size; i++) {
        fprintf (stdout, "\t%ld\n", vm->stack[i]);
    }
}

void run_program(loliVM_t *vm, const program_t *program) {
    for (;(size_t)vm->iptr < program->size;) {
        loli_panic_t panic = vm_execute_inst (vm);
        if (panic != PANIC_OK) {
            vm_dump (vm);
            loli_is_panicing (panic);
        }
        //vm_dump (vm);
    }
    vm->iptr = 0;
}

loli_panic_t vm_load_prog_from_memory (loliVM_t *vm, program_t *program) {
    if (program->size > VM_PROGAM_CAPACITY) {
        return PANIC_STACK_OVERFLOW;
    }

    memcpy (vm->program, program->body, sizeof (program->body[0]) * program->size);    
    vm->size_of_program = program->size;

    return PANIC_OK;
}


int cstrToNum (char *num, size_t size)
{
    int powValue = size;
    int result = 0;

    for (int i = 0; i < size - 1; i++) 
    {
        int charToNum = abs ((int)num[i] - (int)'0');
        result += charToNum * pow(10, size - 2 - i);
    }
    return result;
}

carray_t *splitstr(char str[], char delim) 
{
    carray_t *buffer;
    size_t lenOfOldStr = strlen (str);
    char * nomalizedStr = malloc (lenOfOldStr + 1);
    memcpy(nomalizedStr, str, lenOfOldStr);
    nomalizedStr[lenOfOldStr] = ' ';
    nomalizedStr[lenOfOldStr - 1] = ' ';

    if (carray_new (&buffer) == COL_ALLOC_ERR)
        loli_is_panicing (PANIC_SEGMENTATION_FAULT);

    size_t prevwordend = 0;
    size_t words = 0;
    for (size_t i = 0; i < strlen(nomalizedStr); i++)
    {
        if (nomalizedStr[i] == delim)
        {
            size_t sizeOfNewWord = i - prevwordend;
            char *word  = malloc (sizeof (char) * sizeOfNewWord);
            
            for (size_t j = 0; j < sizeOfNewWord; j++) 
                word[j] = nomalizedStr[j + prevwordend];

            carray_add(buffer, word);

            prevwordend = i;
            words++;
        }
    }
    
    free (nomalizedStr);
    return buffer;
}

loli_panic_t getInstrationFromLine(char * line, size_t size, inst_t **out)
{
    carray_t *pword = splitstr (line, ' ');

    char *inst;
    char *arg;

    carray_get_at(pword, 0, (void*)&inst);
    carray_get_at(pword, 1, (void*)&arg);

    inst_t *pinst = NULL;

    if (strcmp (inst, "push") == 0) 
        pinst = &MAKE_INST_PUSH (cstrToNum(arg, ARRAY_SIZE(arg)));
    else if (strcmp (inst, "je") == 0)   
        pinst = &MAKE_INST_JE (cstrToNum(arg, ARRAY_SIZE(arg)));
    else if (strcmp (inst, "sum") == 0)  
        pinst = &MAKE_INST_SUM;
    else 
        return PANIC_ILLEGAL_INST;

    *out = pinst;
    return PANIC_OK;
}

loli_panic_t vm_load_prog_from_file(const char* name, program_t **pr)
{
    FILE* fpIn;

    if ((fpIn = fopen (name, "r")) == NULL) 
        loli_is_panicing (PANIC_ILLEGAL_FILE_ACCESS);
    
    clist_t *body;
    if (clist_new (&body) == COL_OK) {

        const size_t lenghtOfLine = 25;
        char line[lenghtOfLine];
        while (fgets(line, lenghtOfLine, fpIn) != NULL)
        {
            inst_t *pout = NULL;
            loli_panic_t panic = getInstrationFromLine(line, lenghtOfLine, &pout);
            if (panic != PANIC_OK)
                loli_is_panicing (panic);
            
            clist_add (body, pout);
        }
    }
    program_t *program = malloc (sizeof (program_t)); 

    if (program == NULL)
        return PANIC_SEGMENTATION_FAULT;

    program->body = malloc (sizeof (inst_t) * clist_size (body));

    size_t idx = 0;
    inst_t *item;
    clist_iter_t *iter;
    clist_iter_new (body, (void *)&iter);
    while (clist_iter_next(iter, (void *)&item) != COL_ITER_END)
    {
        printf ("%s", instAsCstr(item->type));
    }

    *pr = program;
    fclose (fpIn);
    return PANIC_OK;
}

void program_dump (const inst_t *ar, size_t size)
{
    printf ("program:\n");
    for (size_t i = 0; i < size; i++) 
        printf ("\t%s%ld\n", instAsCstr (ar[i].type), ar[i].operand);
}


int main ()
{
    inst_t body[] = {
        MAKE_INST_PUSH  (1), // 0
        MAKE_INST_PUSH  (2), // 1
        MAKE_INST_SUM,       // 2
        MAKE_INST_PUSH  (3), // 3
        MAKE_INST_JE    (2), // 4
        MAKE_INST_SUM,
        MAKE_INST_PUSH  (3),
        MAKE_INST_DIV
    };

    program_t program = {
        .size = ARRAY_SIZE (body),
        .body = body
    };
    program_t *pr = malloc (sizeof (program_t));
    if (pr == NULL)
        loli_is_panicing (PANIC_SEGMENTATION_FAULT);

    //vm_load_prog_from_memory(&vm, &program);
    //program_dump (program.body, program.size);
    //run_program(&vm, &program);
    
    if (vm_load_prog_from_file ("test.loliasm", &pr) != PANIC_OK)
        loli_is_panicing (PANIC_SEGMENTATION_FAULT);
    
    program_dump (pr->body, pr->size);
    //run_program(&vm, &pr);
    vm_dump (&vm);
    free (pr);
    return 0;
}
