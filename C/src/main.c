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

Boolean is_cyrillic_character(N_32 character)
{
    return character >= 0x0410 && character <= 0x044F || character == 0x0451 || character == 0x0401;
}


void write_UTF_8_character_in_buffer (Buffer* buffer, N_32 character)
{
    cycle(0, 4, 1)
    write_in_buffer(buffer, ((N_8*)&character)[i]);
    end
}


N_32 read_token (Buffer* token, Input* input)
{
    N_32 character;

    token->length = 0;

    if(end_of_input(input))
        goto error;

    character = input_UTF_8_data(input);//read_UTF_8_character(input);

    if(!is_latin_character(character) && !is_cyrillic_character(character) && character != '_')
        goto error;

    read_UTF_8_character(input);

    //printf("%d ", character);
    write_UTF_8_character_in_buffer(token, character);

    for(;;)
    {
        character = input_UTF_8_data(input);

        if(!is_latin_character(character) && !is_cyrillic_character(character) && !is_number_character(character) && character != '_')
            break;

        character = read_UTF_8_character(input);

        //printf("%d ", character);
        write_UTF_8_character_in_buffer(token, character);

        if(end_of_input(input))
            break;
    }

    return 1;

error:
    return 0;
}


N_32 read_next_UTF_8_character_from_string(N_8** string)
{
    N_8  current_byte;
    N_32 number_of_bytes;
    N_32 result;

    current_byte = **string;
    ++*string;

    if(!(current_byte & 0b10000000))
        return current_byte;

    if((current_byte & 0b11110000) == 0b11110000)
    {
        number_of_bytes = 4;
        result = (current_byte & 0b00001111) << 18;
    }
    else if((current_byte & 0b11100000) == 0b11100000)
    {
        number_of_bytes = 3;
        result = (current_byte & 0b00011111) << 12;
    }
    else if((current_byte & 0b11000000) == 0b11000000)
    {
        number_of_bytes = 2;
        result = (current_byte & 0b00111111) << 6;
    }
    else
        goto error;

    cycle(0, number_of_bytes - 1, 1)
    current_byte = **string;
    ++*string;
    result |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
    end

    return result;

error:
    return 1;
}


Z_32 compare_token(Buffer* token, N_8* string)
{
    N_32* token_data;
    N_32  token_length;
    Z_32  result;

    token_data = token->data;
    token_length = token->length / 4;

    for(; token_length && *string; --token_length, ++token_data)
    {
        result = *token_data - read_next_UTF_8_character_from_string(&string);

        if(result)
            return result;
    }

    if(*string)
        return read_next_UTF_8_character_from_string(&string);
    else if(token_length)
        return *token_data;

    return 0;
}


typedef struct
{
    //N_32 last_character;
    Input* input;
    Buffer token;
    Buffer operands;
}
Parser;


N_8 parse_expression_operand(Parser* parser)
{
    N_8 operand_type;

    read_token(&parser->token, parser->input);
    skip_spaces(parser->input);
    operand_type = input_data(parser->input);

    switch(operand_type)
    {
    case '[':
        if(!parser->token.length)
        {
            printf("allocate array expression, ");
        }
        else
        {
            printf("array index expression, ");
        }

        read_input(parser->input);

        break;

    case '{':
        if(!parser->token.length)
        {
            printf("function declaration, ");
        }
        else
        {
            printf("error: expected function name");
        }
        break;

    case '(':
        if(!parser->token.length)
        {
            printf("error: expected function name");
        }
        else
        {
            printf("function call expression, ");
        }

        read_input(parser->input);
        break;

    case '"':
        printf("allocate string expression, ");
        read_input(parser->input);
        break;

    default:
        if(is_number_character(input_data(parser->input)))
        {
            printf("%d, ", read_N_32(parser->input));
        }
    }

    skip_spaces(parser->input);

    return 1;
}


Operation operations[] = {
    {BINARY_OPERATION, "*", 1},
    {BINARY_OPERATION, "+", 2},
    {UNARY_OPERATION, "-", 0}
};


N_8 read_expression (Parser* parser)
{
    Buffer expression;

    return parse_expression (
        &expression,
        parser->input,
        operations,
        sizeof(operations) / sizeof(Operation),
        parser,
        &parse_expression_operand);
}


N_32 allocate_operand(Parser* parser)
{
    N_32 new_operand_offset;

    new_operand_offset = parser->operands.length;

    cycle(0, sizeof(Buffer), 1)
        write_in_buffer(&parser->operands, 0);
    end

    initialize_buffer(parser->operands.data + new_operand_offset, 20);

    return new_operand_offset;
}


Buffer* get_operand(Parser* parser, N_32 operand_offset)
{
    return parser->operands.data + operand_offset;
}


typedef enum
{
    VARIABLE
}
Operand_Node_Type;


typedef struct
{
    Operand_Node_Type type;
    Byte*             data;
}
Operand_Node;


void add_in_operand (Parser* parser, N_32 operand_offset, Operand_Node_Type type)
{

}


void add_operand_variable (Parser* parser, N_32 operand_offset, Buffer* variable_name)
{
    printf("add variable, ");
}


N_8 parse_left_operand (Parser* parser, N_32 operand_offset)
{
    Buffer* operand;

    //operand = get_operand(parser, operand_offset);

    if(!parser->token.length)
    {
        printf("not operand\n");
        goto error;
    }

    add_operand_variable(parser, operand_offset, &parser->token);
    skip_spaces(parser->input);

read_operand:

    switch(input_UTF_8_data(parser->input))
    {
    case '[':
        read_UTF_8_character(parser->input);
        skip_spaces(parser->input);
        printf("array index, ");

        read_expression(parser);

        if(input_UTF_8_data(parser->input) != ']')
        {
            printf("error: expected ]\n");
            goto error;
        }

        read_UTF_8_character(parser->input);
        skip_spaces(parser->input);

        goto read_operand;
        break;

    case '(':
        read_UTF_8_character(parser->input);
        skip_spaces(parser->input);
        printf("function call, ");

        goto read_operand;
        break;

    case '.':
        read_UTF_8_character(parser->input);
        skip_spaces(parser->input);
        printf("system function call, ");

        goto read_operand;
        break;

    default:
        printf("end of operand\n");
    }

    return 1;

error:
    return 0;
}


N_8 parse (Input* input)
{
    Parser parser;

    parser.input = input;
    initialize_buffer(&parser.token, 20);
    initialize_buffer(&parser.operands, 20);

    while(!end_of_input(input))
    {
        read_token(&parser.token, input);

        if(!compare_token(&parser.token, "если"))
        {
            printf("if\n");
        }
        else
        {
            N_32 operand_offset = allocate_operand(&parser);
            if(!parse_left_operand(&parser, operand_offset))
            {
                printf("error parse operand\n");
                goto error;
            }

            if(input_UTF_8_data(input) == '=')
            {
                printf("ASSIGNMENT ");

                read_UTF_8_character(input);
                skip_spaces(input);
                //parse_expression
            }
        }

        //skip_spaces(input);
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

    Parser parser;

    input_file = fopen("test", "rb");
    initialize_input(&input, input_file, &fgetc);
    input.end_of_data = &feof;

    parse(&input);
    /*
        initialize_buffer(&token, 20);

        read_token(&token, &input);


        char* utf_string = "абв";

        //while(*utf_string)
        //    printf("%d ", read_next_UTF_8_character_from_string(&utf_string));

        printf("%d", compare_token(&token, utf_string));
    */
    return 0;
}
