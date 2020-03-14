#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define STACK_OVERFLOW  -100
#define STACK_UNDERFLOW -101
#define OUT_OF_MEMORY   -102

#define STACK_SIZE 8

struct stack_t;

typedef struct stack_t stack_t;
typedef char T;

struct stack_t {
    T *_array;
    unsigned char _max_length;
    short _top;
    short (*length)(stack_t * const);
    T (*peek)(stack_t * const);
    void (*push)(const T, stack_t * const);
    T (*pop)(stack_t * const);
    void (*destroy)(stack_t *);
};

void destroy(stack_t *this) {
    free(this->_array);
}

T pop(stack_t * const this) {
    if ( this->_top == 0 ) {
        printf("Stack underflow!\n");
        exit(STACK_UNDERFLOW);
    }
    return this->_array[--this->_top];
}

void push(const T value, stack_t * const this) {
    if ( this->_top >= this->_max_length ) {
        printf("Stack overflow!\n");
        exit(STACK_OVERFLOW);
    }
    this->_array[this->_top++] = value;
}

char peek(stack_t * const this) {
    if ( this->_top == 0 ) {
        printf("Stack underflow!\n");
        exit(STACK_UNDERFLOW);
    }
    return this->_array[this->_top-1];
}

short length(stack_t * const this) {
    return this->_top;
}

void init_stack(stack_t *stack, unsigned char size) {
    stack->_array = malloc(size * sizeof(T));
    if (stack->_array == NULL) {
        printf("Out of memory!\n");
        exit(OUT_OF_MEMORY);
    }
    stack->_top = 0;
    stack->_max_length = size;
    stack->length = &length;
    stack->peek = &peek;
    stack->push = &push;
    stack->pop = &pop;
    stack->destroy = &destroy;
}

bool is_operator (const char c) {
    return c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

signed char priority (const char op) {
    return
            op == '+' || op == '-' ? 0x01 :
            op == '*' || op == '/' ? 0x02 :
            op == '^' ? 0x03 : 0xFF;
}

int main() {

    stack_t stack;

    init_stack(&stack, STACK_SIZE);

    const char *eq = "5 + 7 - 5 + 3 / ( 4 - 5 * d) + 1"; // eq_ptr to char array in read only segment of memory

    char out[strlen(eq) + 1]; char *outPtr = out;

    for (int i = 0; eq[i] != '\0'; ++i) {

        if ( eq[i] == ' ' ) {
            continue;
        } else if ( eq[i] == '(' ) {
            stack.push(eq[i], &stack);
        } else if ( eq[i] == ')' ) {
            while ( (*outPtr++ = stack.pop(&stack)) != '(' ) {} --outPtr;
        } else if ( is_operator(eq[i]) ) {
            while ( stack.length(&stack) > 0 && priority(stack.peek(&stack)) >= priority(eq[i]) ) {
                *outPtr++ = stack.pop(&stack);
            }
            stack.push(eq[i], &stack);
        } else { // если операнд
            *outPtr++ = eq[i];
        }
    }

    while ( stack.length(&stack) > 0 ) { *outPtr++ = stack.pop(&stack); } *outPtr = '\0';

    printf("%s", out);

    stack.destroy(&stack);
    return 0;
}
