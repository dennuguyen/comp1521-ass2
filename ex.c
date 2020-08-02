#include <stdio.h>
#include <assert.h>

// typedef void *b_t(void *bb);

// int a(void *aa)
// {
//     printf("before\n");
//     b_t *quack = (void *)aa;
//     quack(3);
//     printf("after\n");
//     return 1;
// }

// void b(int bb)
// {
//     printf("inside %d\n", bb);
// }

union un
{
    int a;
    float b;
};

int main()
{
    union un a;
    a.b = 2;
    printf("%d\n", a.a);
    printf("%f\n", a.b);
}