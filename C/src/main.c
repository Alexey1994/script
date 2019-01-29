#include <system.h>
#include <expression-parser.h>

/*
void print_expression_in_postfix_notation (Buffer* expression, void (*print_operand)(Byte* operand))
{
    Expression_Node* nodes;
    N_32             length;

    nodes = expression->data;
    length = expression->length / sizeof(Expression_Node);

    cycle(0, length, 1)
        Expression_Node* current_node = nodes + i;

        if(current_node->type == OPERAND)
            printf("UTF+%d ", current_node->data);

        if(current_node->type == BINARY_OPERATION)
            printf("%s ", ((Operation*)current_node->data)->name);
    end
}


N_32 read_operand (Input *input)
{
    N_32 number = read_UTF_8_character(input);
    skip_spaces(input);
    return number;

error:
    return 0;
}


Operation operations[] = {
    {BINARY_OPERATION, "*", 1},
    {BINARY_OPERATION, "+", 2},
    {UNARY_OPERATION, "-", 0}
};


int fgetc( Byte * filestream );
int feof ( Byte * filestream );


void print_operand (char operand)
{
    printf("%c", operand);
}


N_32 main ()
{
    Input   input;
    Byte*   input_file;
    Buffer  expression;
    N_32    is_success;

    input_file = fopen("test", "rb");
    initialize_input(&input, input_file, &fgetc);
    input.end_of_data = &feof;

    is_success = parse_expression (
        &expression,
        &input,
        operations,
        sizeof(operations) / sizeof(Operation),
        &input,
        &read_operand);

    if(!is_success)
        printf("error parsing\n");

    print_expression_in_postfix_notation(&expression, &print_operand);

    return 0;
}
*/


int fgetc( Byte * filestream );
int feof ( Byte * filestream );


Boolean is_number_character(N_32 character)
{
    return character >= '0' && character <= '9';
}


Boolean is_latin_character(N_32 character)
{
    return character >= 'a' && character <= 'z' || character >= 'A' && character <= 'Z';
}


N_32 read_token (Buffer* token, Input* input)
{
    N_32 character;

    token->length = 0;

    if(end_of_input(input))
        goto error;

    character = input_data(input);

    if(!is_latin_character(character) && character != '_')
        goto error;

    printf("%d ", character);

    //while(!end_of_input(input))
    for(;;)
    {
        read_input(input);
        character = input_data(input);

        if(!is_latin_character(character) && !is_number_character(character))
            break;

        printf("%d ", character);

        if(end_of_input(input))
            break;
    }

    return 1;

error:
    return 0;
}


N_32 main()
{
    Input   input;
    Byte*   input_file;
    Buffer  expression;
    N_32    is_success;
    Buffer  token;

    input_file = fopen("test", "rb");
    initialize_input(&input, input_file, &fgetc);
    input.end_of_data = &feof;

    initialize_buffer(&token, 20);

    read_token(&token, &input);

    return 0;
}
