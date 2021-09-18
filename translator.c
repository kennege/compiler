#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "integer.h"
#include "float.h"
#include "string.h"
#include "scope_table.h"
#include "stack.h"
#include "translator.h"

#define TRUE "1"
#define FALSE "0"

struct memory
{
    struct stack *stack;
    struct scope *scope;
    int main_visited;
    int semantic_error;
};

struct token *translator_visit(const struct node *node, struct memory *memory);

static char *translator_get_types(const struct token *left, const struct token *right, struct memory *memory)
{
    if (0 != strcmp(token_get_type(left), token_get_type(right)))
    {
        if ((0 == strcmp(token_get_type(left), FLOAT) && 0 == strcmp(token_get_type(right), INT)) ||
        (0 == strcmp(token_get_type(right), FLOAT) && 0 == strcmp(token_get_type(left), INT)))
        {  
            return INT;
        }
        else
        {
            memory->semantic_error = 1;
            fprintf(stderr, "ERROR: operation between different types.\n");
            return NULL;
        }
    }
    else
    {
        return token_get_type(left);
    }

    return NULL;
}

static struct token *translator_operate_unary(const struct token *token, const char *op, struct memory *memory)
{
    debug_print("%s\n",token_get_display(token));
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*operations)(const struct token *token, const char *op);
    } op_types[] = {
        { .type = INT, .operations = integer_unary_operations },
        { .type = FLOAT, .operations = float_unary_operations },
    };

    if (NULL == token || NULL == op)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    type = token_get_type(token);
    if (NULL == type)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    
    for (int i=0; i<LENGTH(op_types); i++)
    {
        if (0 == strcmp(type, op_types[i].type))
        {
            result = op_types[i].operations(token, op);
            if (NULL != result)
            {
                return result;
            }
            memory->semantic_error = 1;
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_operate_binary(const struct token *left, const struct token *right, const char *op, struct memory *memory)
{
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*operations)(const struct token *left, const struct token *right, const char *op);
    } op_types[] = {
        { .type = INT, .operations = integer_binary_operations },
        { .type = FLOAT, .operations = float_binary_operations },
        { .type = STRING, .operations = string_binary_operations },
    };

    if (NULL == left || NULL == right || NULL == op)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    type = translator_get_types(left, right, memory);
    if (NULL == type)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    
    for (int i=0; i<LENGTH(op_types); i++)
    {
        if (0 == strcmp(type, op_types[i].type))
        {
            result = op_types[i].operations(left, right, op);
            if (NULL != result)
            {
                return result;
            }
            memory->semantic_error = 1;
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_compare(const struct token *left, const struct token *right, const char *op, struct memory *memory)
{
    debug_print("comparing %s %s %s\n",token_get_value(left), op, token_get_value(right));
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*comparisons)(const struct token *left, const struct token *right, const char *op);
    } comp_types[] = {
        { .type = INT, .comparisons = integer_comparisons },
        { .type = FLOAT, .comparisons = float_comparisons },
    };

    if (NULL == left || NULL == right || NULL == op)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    type = translator_get_types(left, right, memory);
    if (NULL == type)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    
    for (int i=0; i<LENGTH(comp_types); i++)
    {
        if (0 == strcmp(type, comp_types[i].type))
        {
            result = comp_types[i].comparisons(left, right, op);
            if (NULL != result)
            {
                debug_print("comparison result: %s\n",token_get_value(result));               
                return result;
            }
            memory->semantic_error = 1;
            fprintf(stderr, "ERROR. Comparison %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_update_bool(const struct token *current, const struct token *prev, const struct token *comparison)
{
    struct token *new;

    if (NULL == current)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    if (NULL == comparison)
    {
        return NULL;
    }

    new = integer_comparisons(current, prev, token_get_value(comparison));
    if (NULL == new)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    debug_print("bool result: %s\n", token_get_value(new));

    return new;
}

static struct token *translator_visit_program_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    return translator_visit(node->left, memory);
}

static struct token *translator_visit_function_argument_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    if (NULL == node)
    {
        return NULL;
    }

    if (0 != stack_append(&(memory->stack), node->op, NULL))
    {
        ERROR_MESSAGE;
        return NULL;
    }
    if (0 != stack_function_arg_append(memory->stack, node->op))
    {
        ERROR_MESSAGE;
        return NULL;
    }

    return translator_visit(node->next->node, memory);
}

static struct token *translator_visit_function_node(const struct node *node, struct memory *memory)
{
    /* if name == main: traverse all function nodes, else: just add function name to memory */
    debug_print("%s\n", ast_node_display(node));
    const struct node *next_node;
    struct token *return_token;
    size_t n_nodes;

    return_token = NULL;
    n_nodes = ast_num_nodes(node);
    for (int i=0; i<n_nodes; i++)
    {
        next_node = ast_node_index(&node, i);
        if ((0 == strcmp(token_get_value(next_node->op), MAIN) && !memory->main_visited) ||
            (0 != strcmp(token_get_value(next_node->op), MAIN) && memory->main_visited))
        {
            memory->main_visited = 1;
            if (0 != scope_add(&(memory->scope), token_get_value(next_node->op))) 
            {
                ERROR_MESSAGE;
                return NULL;
            }   
            if (0 != stack_append(&(memory->stack), next_node->op, translator_visit(next_node->left, memory)))
            {
                ERROR_MESSAGE;
                return NULL;
            }
            
            scope_revert(&(memory->scope));
            
            if (NULL != next_node->right)
            {         
                return_token = token_cpy(translator_visit(next_node->right, memory));
                if (NULL == return_token)
                {
                    ERROR_MESSAGE;
                    return NULL;
                }
                if (0 != scope_insert(memory->scope, token_cpy(next_node->op)))
                {
                    ERROR_MESSAGE;
                    return token_destroy(return_token);
                } 
            }

            if (0 != stack_destroy_local(&(memory->stack), next_node->op))
            {
                ERROR_MESSAGE;
                return token_destroy(return_token);
            }
        } 
        else
        {
            if (0 != stack_function_declaration_append(&(memory->stack), next_node->op, next_node))
            {
                ERROR_MESSAGE;
                return NULL;
            }  
            translator_visit(next_node->arguments, memory);
        }
    }
    
    return return_token;
}

static struct token *translator_visit_function_call_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    size_t n_args;
    const struct node *next_node, *current_node;
    const struct token *arg_name, *arg_val;

    current_node = node->left;
    n_args = ast_num_nodes(current_node);
    for (int i=0; i<n_args; i++)
    {
        next_node = ast_node_index(&current_node, i);
        arg_name = stack_get_function_arg(memory->stack, node->op, i);
        arg_val = stack_extract(memory->stack, next_node->op);
        if (NULL == arg_name || NULL == arg_val)
        {
            ERROR_MESSAGE;
            return NULL;
        }
        if (0 != stack_append(&(memory->stack), arg_name, arg_val))
        {
            ERROR_MESSAGE;
            return NULL;
        }      
    }

    return translator_visit(stack_get_address(memory->stack, node->op), memory);
}

static struct token *translator_visit_loop_node(const struct node *node, struct memory *memory)
{
    /* left = declaration, right = comparison, arguments = assignment, next = statement_list */
    debug_print("%s\n", ast_node_display(node));
    struct token *bool_current, *declaration, *assignment, *statement_list;

    declaration = translator_visit(node->left, memory);
    if (NULL == declaration)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    while (1)
    {
        bool_current = translator_visit(node->right, memory);
        if (NULL == bool_current)
        {
            ERROR_MESSAGE;
            return NULL;
        }
        
        if (0 == strcmp(token_get_value(bool_current), FALSE))
        {
            break;
        }

        statement_list = translator_visit(node->body, memory);
        if (NULL == statement_list)
        {
            ERROR_MESSAGE;
            return NULL;
        }

        assignment = translator_visit(node->arguments, memory);
        if (NULL == assignment)
        {
            ERROR_MESSAGE;
            return NULL;
        }
    }

    return statement_list;
}

static struct token *translator_visit_comparison_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    size_t n_comparisons;
    struct token *bool_current, *bool_prev, *left, *right;
    const struct node *next_node;

    n_comparisons = ast_num_nodes(node);
    bool_current = NULL;
    for (int i=0; i<n_comparisons; i++)
    {
        next_node = ast_node_index(&node, i);
        left = translator_visit(next_node->left, memory);
        right = translator_visit(next_node->right, memory);
        if (NULL == left || NULL == right)
        {
            ERROR_MESSAGE;
            return NULL;
        }
        bool_current = translator_compare(left, right, token_get_value(next_node->op), memory);
        bool_prev = translator_update_bool(bool_current, bool_prev, next_node->next->operator);
    }

    return bool_current;
}

static struct token *translator_visit_condition_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    size_t n_conditions;
    struct token *comparison;
    const struct node *condition, *next_condition;

    condition = node->body;
    n_conditions = ast_num_nodes(condition);
    for (int i=0; i<n_conditions; i++)
    {
        next_condition = ast_node_index(&condition, i);
        if (NULL == next_condition->left) // else node
        {
            return translator_visit(next_condition->right, memory);
        }
        comparison = translator_visit(next_condition->left, memory);
        if (NULL == comparison)
        {
            ERROR_MESSAGE;
            return NULL;
        }
        if (0 == strcmp(token_get_value(comparison), TRUE))
        {
            return translator_visit(next_condition->right, memory);
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_visit_declaration_node(const struct node *node, struct token *value, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    if (scope_variable_exists(memory->scope, node->left->op))
    {
        // memory->semantic_error = 1;
        // fprintf(stderr, "ERROR: variable '%s' defined more than once ", token_get_value(node->left->op));
        // ERROR_MESSAGE;
        return token_destroy(value);
    }

    if (0 != scope_insert(memory->scope, token_cpy(node->left->op)))
    {
        ERROR_MESSAGE;
        return token_destroy(value);
    }

    if (0 != stack_append(&(memory->stack), node->left->op, value)) 
    {
        ERROR_MESSAGE;
        return token_destroy(value);
    }

    return NULL;
}

static struct token *translator_visit_assignment_node(const struct node *node, struct token *value, struct memory *memory)
{
    debug_print("%s. assigning: %s %s %s\n", __func__, 
        token_get_value(node->left->op), token_get_value(node->op), token_get_value(value));
    if (0 != stack_append(&(memory->stack), node->left->op, value)) 
    {
        ERROR_MESSAGE;
        return token_destroy(value);
    }   

    return NULL;
}

static struct token *translator_visit_statement_node(const struct node *node, struct memory *memory)
{
    const struct node *next_node;
    struct token *value;
    size_t n_nodes;
    int visited;

    static const struct {
        char *node_type;
        struct token *(*fn)(const struct node *node, struct token *value, struct memory *memory);
    } tab[] = {
        { .node_type = DECLARATION, .fn = translator_visit_declaration_node },
        { .node_type = ASSIGNMENT, .fn = translator_visit_assignment_node },
    };

    value = NULL;
    n_nodes = ast_num_nodes(node);
    for (int i=0; i<n_nodes; i++)
    {
        visited = 0;
        next_node = ast_node_index(&node, i);
        
        for (int i=0; i<LENGTH(tab); i++)
        {        
            if (0 == strcmp(next_node->type, tab[i].node_type))
            {
                value = translator_visit(next_node->right, memory);                
                tab[i].fn(next_node, value, memory);
                visited = 1;
            }
        }

        if (!visited)
        {
            value = translator_visit(next_node, memory);
        }        
    }
    
    return value;
}

static struct token *translator_visit_variable_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    struct token *output;

    output = token_cpy(stack_extract(memory->stack, node->op));
    if (NULL == output)
    {
        memory->semantic_error = 1;
        fprintf(stderr, "variable not found: %s ", token_get_display(node->op));
        ERROR_MESSAGE;
        return NULL;
    }

    return output;
}

static struct token *translator_visit_binary_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    struct token *left, *right;

    struct {
        char *op;
    } binary_ops[] = {
        { .op = PLUS },
        { .op = MINUS },
        { .op = MULTIPLY },
        { .op = DIVIDE },
    };

    for (int i=0; i<LENGTH(binary_ops); i++)
    {
        if (0 == token_compare(node->op, binary_ops[i].op))
        {    
            left = translator_visit(node->left, memory);
            right = translator_visit(node->right, memory);
            if (NULL == left || NULL == right)
            {
                ERROR_MESSAGE;
                return NULL;
            }
            return translator_operate_binary(left, right, binary_ops[i].op, memory); 
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_visit_unary_node(const struct node *node, struct memory *memory)
{  
    debug_print("%s, %s\n", __func__, ast_node_display(node));
    struct token *output;

    struct {
        char *op;
    } unary_ops[] = {
        { .op = PLUS },
        { .op = MINUS },
    };
    
    for (int i=0; i<LENGTH(unary_ops); i++)
    {
        if (0 == token_compare(node->op, unary_ops[i].op))
        {
            output = translator_visit(node->left, memory);
            return translator_operate_unary(output, unary_ops[i].op, memory);
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct token *translator_visit_value_node(const struct node *node, struct memory *memory)
{
    debug_print("%s\n", ast_node_display(node));
    return translator_operate_unary(node->op, PLUS, memory);
}

struct token *translator_visit(const struct node *node, struct memory *memory)
{
    if (NULL == node || NULL == memory)
    {
        return NULL;
    }

    static const struct {
        char *node_type;
        struct token *(*fn)(const struct node *node, struct memory *memory);
    } tab[] = {
        { .node_type = UNARY, .fn = translator_visit_unary_node },
        { .node_type = VALUE, .fn = translator_visit_value_node },
        { .node_type = VARIABLE, .fn = translator_visit_variable_node },
        { .node_type = BINARY, .fn = translator_visit_binary_node },
        { .node_type = ASSIGNMENT, .fn = translator_visit_statement_node },
        { .node_type = DECLARATION, .fn = translator_visit_statement_node },
        { .node_type = COMPARISON, .fn = translator_visit_comparison_node },
        { .node_type = CONDITION, .fn = translator_visit_condition_node },
        { .node_type = LOOP, .fn = translator_visit_loop_node },
        { .node_type = FUNCTION_ARGUMENT, .fn = translator_visit_function_argument_node },
        { .node_type = FUNCTION_CALL, .fn = translator_visit_function_call_node },
        { .node_type = FUNCTION, .fn = translator_visit_function_node },
        { .node_type = PROGRAM, .fn = translator_visit_program_node },
    };
    for (int i=0; i<LENGTH(tab); i++)
    {
        if (0 == strcmp(node->type, tab[i].node_type))
        {
            debug_print("%s\n",ast_node_display(node));
            return tab[i].fn(node, memory);
        }
    }

    ERROR_MESSAGE;
    return NULL;
}

static struct memory *translator_memory_deinit(struct memory *memory)
{
    if (NULL == memory)
    {
        return NULL;
    }
    
    if (NULL != memory->scope)
    {
        memory->scope = scope_destroy(memory->scope);
    }

    if (NULL != memory->stack)
    {
        memory->stack = stack_destroy_all(memory->stack);
    }

    free(memory);

    return NULL;
}

static struct memory *translator_memory_init()
{
    struct memory *memory;

    memory = malloc(sizeof(*memory));
    if (NULL == memory)
    {
        return NULL;
    }

    memory->scope = scope_create("global");
    if (NULL == memory->scope)
    {
        return translator_memory_deinit(memory);
    }

    memory->stack = NULL;
    memory->main_visited = 0;
    memory->semantic_error = 0;

    return memory;
}

struct token *translator_translate(const struct node *ast)
{
    struct token *result;
    struct memory *memory;

    if (NULL == ast)
    {
        ERROR_MESSAGE;
        return NULL;
    }
    // ast_print(ast, 0, "root");

    memory = translator_memory_init();
    if (NULL == memory)
    {
        ERROR_MESSAGE;
        return NULL;
    }

    result = translator_visit(ast, memory);

    if (memory->semantic_error)
    {
        memory = translator_memory_deinit(memory);
        ERROR_MESSAGE;
        return NULL;
    }

    memory = translator_memory_deinit(memory);


    return result;
}
