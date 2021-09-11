#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"

#define PASS_MESSAGE(index, output, expected) \
    (fprintf(stderr, "PASSED. Test: %d. Expected: %s, Actual: %s\n", index, expected, output))

#define FAIL_MESSAGE(index, output, expected) \
    (fprintf(stderr, "FAILED. Test: %d. Expected: %s, Actual: %s\n", index, expected, output))

static const struct {
    char *input;
    char *expected_output;
} pass_tests[] = {
    // { .input = "-3*(3+4)     -  - -1", .expected_output = "-22" },
    // { .input = "( ((3+5))  )/ 2*2", .expected_output = "8" },
    // { .input = "-1", .expected_output = "-1" },
    // { .input = "-1 + 1", .expected_output = "0" },
    // { .input = "var x int =2", .expected_output = "2" },
    // { .input = "-2.0 + 0.03", .expected_output = "-1.97" },
    // { .input = "// 3+5\n1+2", .expected_output = "3" },
    // { .input = "func main() int { \n d := 2 +  4 \n b:= d+3 \n return b \n}", .expected_output = "9" },
    // { .input = "func main() int { \n a := 2\nd := 2 +  4 \n d= d+3 \n return d \n}", .expected_output = "9" },
    // { .input = "func main(int a, float32 b) float32 { \n var c float32 = 2"
    //     "\nd := c +  4.1 \n\n return c --   - - d \n}", .expected_output = "8.10" },
    // { .input = "func divide_input(float32 num, float32 den) float32 { \n return num/den \n}"
    //     "\n\n func main() float32 \n{ \n var x float32 = 1.0 \n y := 2.0 \n return divide_input(x,y)\n }", .expected_output = "0.50" },
    // { .input = "func main() int { \n d := 4 \n if d > 4 { \n d = d + 1 \n } else if d==4 {\n d = d+2 \n} else { d = 0 } \n return d }", .expected_output = "6" },
};

static const struct {
    char *input;
    char *expected_output;
} fail_tests[] = {
    // { .input = "da:=2\na=3\nb = 1", .expected_output = NULL },
    // { .input = "d = 2 +   4 \n\n d=3\na=d", .expected_output = NULL },
    // { .input = "// 3+5", .expected_output = NULL },
    // { .input = "\nb+2", .expected_output = NULL },
    // { .input = "func main() { \n d = 2 +  4 \n b= d+3 \n}", .expected_output = NULL },
    // { .input = "func main() int { \n a := 2\nd := 2 +  4 \n d:= d+3 \n return d \n}", .expected_output = NULL },
};

static int run_pass_tests(struct token *(interpreter(char *input_str)))
{
    int result;
    char *output;

    result = 0;
    for (int i=0; i<LENGTH(pass_tests); i++)
    {
        output = token_get_value(interpreter(pass_tests[i].input));

        if (NULL == output)
        {
            FAIL_MESSAGE(i, output, pass_tests[i].expected_output);
            result = -1;        
        }
        else if (0 != strcmp(output, pass_tests[i].expected_output))
        {
            FAIL_MESSAGE(i, output, pass_tests[i].expected_output);
            result = -1;        
        }
        else
        {
            PASS_MESSAGE(i, output, pass_tests[i].expected_output);           
        }
    }
    
    return result;
}
  
static int run_fail_tests(struct token *(interpreter(char *input_str)))
{
    int result;
    char *output;

    result = 0;
    for (int i=0; i<LENGTH(fail_tests); i++)
    {
        output = token_get_value(interpreter(fail_tests[i].input));
        if (NULL == output)
        {    
            PASS_MESSAGE(i, output, fail_tests[i].expected_output);     
        }
        else
        {
            FAIL_MESSAGE(i, output, fail_tests[i].expected_output);
            result = -1;     
        }
    }

    return result;
}

int unit_test(struct token *(interpreter(char *input_str)))
{
    int result;

    if (0 != run_pass_tests(interpreter) || 0 != run_fail_tests(interpreter))
    {
        fprintf(stderr, "UNIT TESTS FAILED.\n");
        result = -1;
    } 
    else
    {
        fprintf(stderr, "UNIT TESTS PASSED.\n");
        result = 0;
    }
    
    return result;
}
