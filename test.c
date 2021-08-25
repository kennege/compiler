
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"

static const struct {
    char *input;
    char *expected_output;
} tests[] = {
    // { .input = "-3*(3+4)     -  - -1", .expected_output = -22 },
    // { .input = "( ((3+5))  )/ 2*2", .expected_output = 8 },
    // { .input = "-1", .expected_output = -1 },
    // { .input = "-1 + 1", .expected_output = 0 },
    // { .input = "da:=2\na=3\nb = 1", .expected_output = -1 },
    // { .input = "var x int =2", .expected_output = -1 },
    { .input = "d = 2 +   4 \n b= d+3", .expected_output = NULL },
    // { .input = "int f =  2- ((2+4))", .expected_output = -1 },
    // { .input = "func main() { \n d = 2 +  4 \n b= d+3 \n}", .expected_output = 9 },
    // { .input = "-2 + 0.03", .expected_output = -1.97 },
    // { .input = "// 3+5", .expected_output = -1 },
    // { .input = "// 3+5\n1+2", .expected_output = 3 },
    { .input = "\nb+2", .expected_output = NULL },
};

// static int test_write(char *output)
// {

//     return 0;
// }

int unit_test(struct token *(interpreter(char *input_str)))
{
    int result;
    char *output;

    result = 0;
    for (int i=0; i<LENGTH(tests); i++)
    {
        output = token_get_value(interpreter(tests[i].input));
        if ((NULL == output && NULL == tests[i].expected_output) ||
            (0 == strcmp(output, tests[i].expected_output)))
        {
            fprintf(stderr, "PASSED. Test: %d. Expected: %s, Actual: %s\n", 
                i, tests[i].expected_output, output);                                
        }     
        else
        {
            fprintf(stderr, "FAILED. Test: %d. Expected: %s, Actual: %s\n", 
                i, tests[i].expected_output, output);
            result = -1;           
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
  