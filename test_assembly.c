#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main () 
{
    // "312 *   ((91+2) /3)"
    int a=312;
    int b=91;
    int c=2;
    int d=3;
    int result = a*((b+c)/d);
    printf("%d\n",result);
    return 0;
}