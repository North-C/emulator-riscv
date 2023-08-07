#include "emulator.h"

void stack_push(stack_t *stack, u64 val) {
    assert(stack->top < STACK_CAP);

    for (int i = 0; i < stack->top; i++) {
        if (stack->elems[i] == val) return;
    }
    stack->elems[stack->top++] = val;
}

bool stack_pop(stack_t *stack, u64 *val) {
    if (stack->top <= 0) return false;
    *val = stack->elems[--stack->top];
    return true;
}

void stack_reset(stack_t *stack) { stack->top = 0; }

void stack_print(stack_t *stack) {
    printf("[ ");
    for (int i = 0; i < stack->top; i++) {
        printf("0x%lx ", stack->elems[i]);
    }
    printf("]\n");
}