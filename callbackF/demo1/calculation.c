#include <stdio.h>
#include <stdlib.h>

//函数指针结构体
typedef struct _OP
{
    float (*p_add)(float, float);
    float (*p_sub)(float, float);
    float (*p_mul)(float, float);
    float (*p_div)(float, float);
} OP;

float ADD(float a, float b)
{
    return a + b;
}

float SUB(float a, float b)
{
    return a - b;
}

float MUL(float a, float b)
{
    return a * b;
}

float DIV(float a, float b)
{
    return a / b;
}

void init_op(OP *op)
{
    op->p_add = ADD;
    op->p_sub = SUB;
    op->p_mul = &MUL;
    op->p_div = &DIV;
}

//库函数
float calculate(float a, float b, float (*op_func)(float, float))
{
    return (*op_func)(a, b);
}

int main()
{
    OP *op = (OP *)malloc(sizeof(OP));
    init_op(op);
    float res;

    //使用函数指针直接调用函数
    res = op->p_add(1.21, 12312.12);
    printf("add = %lf\n", res);

    //调用回调函数
    res = calculate(1.21, 12312.12, ADD);
    printf("add = %lf\n", res);

    return 0;
}
