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

struct memory
{
    struct stack *stack;
    struct scope *scope;
};

struct token *translator_visit(const struct node *node, struct memory *memory);

static struct token *translator_operate_unary(const struct token *token, const char *op)
{
    char *type;
    struct token *result;

    struct {
        char *type;
        struct token *(*operations)(const struct token *token, const char *op);
    } op_types[] = {
        { .type = INT, .operations = integer_unary_operations },
        { .type = FLOAT, .operations = float_unary_operations },
    };

    type = token_get_type(token);
    if (NULL == type)
    {
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
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_operate_binary(const struct token *left, const struct token *right, const char *op)
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

    if (0 != strcmp(token_get_type(left), token_get_type(right)))
    {
        fprintf(stderr, "ERROR: operation between different types.\n");
        return NULL;
    }
    type = token_get_type(left);
    
    for (int i=0; i<LENGTH(op_types); i++)
    {
        if (0 == strcmp(type, op_types[i].type))
        {
            result = op_types[i].operations(left, right, op);
            if (NULL != result)
            {
                return result;
            }
            fprintf(stderr, "ERROR. Operation %s could not be performed on type %s\n", op, type);
            return NULL;
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_program_node(const struct node *node, struct memory *memory)
{
    return translator_visit(node->left, memory);
}

static struct token *translator_visit_function_node(const struct node *node, struct memory *memory)
{
    const struct node *next_node;
    struct token *return_token;
    size_t n_nodes;

    n_nodes = ast_num_nodes(node);
    return_token = NULL;
    for (int i=0; i<n_nodes; i++)
    {
        next_node = ast_node_index(node, i);
        if (0 != scope_add(&(memory->scope), token_get_value(next_node->op))) 
        {
            DEBUG;
            return NULL;
        }            
        if (0 != stack_append(&(memory->stack), next_node->op, translator_visit(next_node->left, memory)))
        {
            DEBUG;
            return NULL;
        }
        
        scope_revert(&(memory->scope));
        
        if (NULL != next_node->right)
        {
            return_token = token_cpy(translator_visit(next_node->right, memory));
            if (NULL == return_token)
            {
                DEBUG;
                return NULL;
            }
            if (0 != scope_insert(memory->scope, token_cpy(next_node->op)))
            {
                DEBUG;
                return NULL;
            }   
        }

        if (0 != stack_destroy_local(&(memory->stack), next_node->op))
        {
            DEBUG;
            return NULL;
        }
    }
    
    return return_token;
}

static struct token *translator_visit_declaration_or_assignment_node(const struct node *node, struct memory *memory)
{
    const struct node *next_node;
    struct token *value;
    size_t n_assignment_nodes;

    n_assignment_nodes = ast_num_nodes(node);
    value = NULL;
    for (int i=0; i<n_assignment_nodes; i++)
    {
        next_node = ast_node_index(node, i);
        value = translator_visit(next_node->right, memory);
        if (0 != stack_append(&(memory->stack), next_node->left->op, value)) 
        {
            DEBUG;
            return NULL;
        }
        if (0 != scope_insert(memory->scope, token_cpy(next_node->left->op)))
        {
            DEBUG;
            return NULL;
        }
    }
    
    return value;
}

static struct token *translator_visit_variable_node(const struct node *node, struct memory *memory)
{
    struct token *output;

    output = stack_extract(memory->stack, node->op);
    if (NULL == output)
    {
        fprintf(stderr, "ERROR: variable not found: %s\n", token_get_display(node->op));
        return NULL;
    }

    return output;
}

static struct token *translator_visit_binary_node(const struct node *node, struct memory *memory)
{
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
                DEBUG;
                return NULL;
            }
            return translator_operate_binary(left, right, binary_ops[i].op); 
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_unary_node(const struct node *node, struct memory *memory)
{  
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
            return translator_operate_unary(output, unary_ops[i].op);
        }
    }

    DEBUG;
    return NULL;
}

static struct token *translator_visit_value_node(const struct node *node, struct memory *memory)
{
    return translator_operate_unary(node->op, PLUS);
}

struct token *translator_visit(const struct node *node, struct memory *memory)
{
    static const struct {
        char *node_type;
        struct token *(*fn)(const struct node *node, struct memory *memory);
    } tab[] = {
        { .node_type = UNARY, .fn = translator_visit_unary_node },
        { .node_type = VALUE, .fn = translator_visit_value_node },
        { .node_type = VARIABLE, .fn = translator_visit_variable_node },
        { .node_type = BINARY, .fn = translator_visit_binary_node },
        { .node_type = ASSIGNMENT, .fn = translator_visit_declaration_or_assignment_node },
        { .node_type = DECLARATION, .fn = translator_visit_declaration_or_assignment_node },
        { .node_type = FUNCTION, .fn = translator_visit_function_node },
        { .node_type = PROGRAM, .fn = translator_visit_program_node },
    };

    for (int i=0; i<LENGTH(tab); i++)
    {
        if (0 == strcmp(node->type, tab[i].node_type))
        {
            return tab[i].fn(node, memory);
        }
    }

    DEBUG;
    return NULL;
}

struct memory *translator_memory_deinit(struct memory *memory)
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

struct memory *translator_memory_init()
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

    return memory;
}

struct token *translator_translate(const struct node *ast)
{
    struct token *result;
    struct memory *memory;

    if (NULL == ast)
    {
        DEBUG;
        return NULL;
    }

    memory = translator_memory_init();
    if (NULL == memory)
    {
        return NULL;
    }

    result = translator_visit(ast, memory);
    
    if (DEBUG_PRINT)
    {
        stack_print(memory->stack);
        scope_print(memory->scope);
    }

    memory = translator_memory_deinit(memory);

    return result;
}
