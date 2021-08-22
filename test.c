
#include <stdio.h>

#include "utils.h"

static const struct {
    char *input;
    int expected_output;
} tests[] = {
    // { .input = "-3*(3+4)     -  - -1", .expected_output = -22 },
    // { .input = "( ((3+5))  )/ 2*2", .expected_output = 8 },
    // { .input = "-1", .expected_output = -1 },
    // { .input = "-1 + 1", .expected_output = 0 },
    { .input = "da:=2", .expected_output = 2 },
    { .input = "int f =  2", .expected_output = 2 },
    { .input = "var x int =2", .expected_output = 2 },
    { .input = "d = 2 +  4 \n b= d+3", .expected_output = 9 },
    // { .input = "func main() { \n d = 2 +  4 \n b= d+3 \n}", .expected_output = 9 },
};

int unit_test(int (interpreter(char *input_str, FILE *fp)), FILE *fp)
{
    int result, output;

    result = 0;
    for (int i=0; i<LENGTH(tests); i++)
    {
        output = interpreter(tests[i].input, fp);
        if (output != tests[i].expected_output)
        {
            fprintf(stderr, "FAILED. Test: %d. Expected: %d, Actual: %d\n", 
                i, tests[i].expected_output, output);
            result = -1;
        }
        else
        {
            fprintf(stderr, "PASSED. Test: %d. Expected: %d, Actual: %d\n", 
                i, tests[i].expected_output, output);           
        }
    }

    if (0 == result)
    {
        fprintf(stderr, "UNIT TESTS PASSED\n");
    }
    else
    {
        fprintf(stderr, "UNIT TESTS FAILED\n");
    }
    
    return result;
}
  