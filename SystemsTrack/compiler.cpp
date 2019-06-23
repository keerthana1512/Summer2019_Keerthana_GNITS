#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "header.h"

struct Stack
{
	int top;
	int* array=(int *)calloc(10,sizeof(int *));
}stack;

int isEmpty(struct Stack* stack)
{
	return stack->top == -1;
}

void push(struct Stack* stack, int item)
{
	stack->array[++stack->top] = item;
	printf("%d pushed to stack\n", item);
}

int pop(struct Stack* stack)
{
	if (isEmpty(stack))
		return INT_MIN;
	return stack->array[stack->top--];
}

int get_reg_opcode(char reg[2])
{
	return(reg[0] - 'A');
}

int get_command_opcode(char command[10])
{
	if (!strcmp(command, "MOVR"))
		return 1;
	else if (!strcmp(command, "MOV"))
		return 2;
	else if (!strcmp(command, "ADD"))
		return 3;
	else if (!strcmp(command, "SUB"))
		return 4;
	else if (!strcmp(command, "MUL"))
		return 4;
	else if (!strcmp(command, "JMP"))
		return 6;
	else if (!strcmp(command, "IF"))
		return 7;
	else if (!strcmp(command, "EQ"))
		return 8;
	else if (!strcmp(command, "LT"))
		return 9;
	else if (!strcmp(command, "GT"))
		return 10;
	else if (!strcmp(command, "LTEQ"))
		return 11;
	else if (!strcmp(command, "GTEQ"))
		return 12;
	else if (!strcmp(command, "PRINT"))
		return 13;
	else if (!strcmp(command, "READ"))
		return 14;
}


int find_symbol_address(char variable[32], int *symbol_length, struct symbol symbol_table[20])
{
	int i = 0;
	for (i = 0; i < *symbol_length; i++)
		if (!strcmp(symbol_table[i].name, variable))
			return symbol_table[i].address;
}


void allocate_symbols(char var_name[20], int *total_size, int *arr_len,int *symbol_length,struct symbol symbol_table[32])
{
	strcpy(symbol_table[*symbol_length].name, var_name);
	symbol_table[*symbol_length].address = *total_size;
	symbol_table[*symbol_length].size = *arr_len;
	*total_size = *total_size + *arr_len;
	*symbol_length = *symbol_length + 1;
}

void data(char arguments[20],int *symbol_length,struct symbol symbol_table[32],int *total_size)
{
	int i = 0,is_array=0,arr_len=1;
	char var_name[20];
	for (i = 0; i < arguments[i] != '\0'; i++){
		if (arguments[i] == '['){
			is_array = 1;
			break;
		}
		var_name[i] = arguments[i];
	}
	var_name[i] = '\0';
	if (is_array == 1){
		arr_len = 0;
		for (i=i+1; arguments[i] != ']'; i++)
			arr_len = arr_len * 10 + (arguments[i] - '0');
	}
	allocate_symbols(var_name, total_size, &arr_len, symbol_length, symbol_table);
}


void mov_to_reg(char reg[20], char variable[32], int *inter_length, struct intermediate inter_table[20], int *symbol_length, struct symbol symbol_table[20])
{
	char var_name[10];
	inter_table[*inter_length].line_number = *inter_length+1;
	inter_table[*inter_length].opCode = get_command_opcode("MOV");
	inter_table[*inter_length].paramaters[0] = get_reg_opcode(reg); 
	if (is_array(variable,var_name))
		inter_table[*inter_length].paramaters[1] = find_symbol_address(var_name, symbol_length, symbol_table) + is_array(variable, var_name);
	else
	inter_table[*inter_length].paramaters[1] = find_symbol_address(variable, symbol_length, symbol_table);
	*inter_length = *inter_length + 1;
}

void mov_from_reg(char variable[32],char reg[3],int *inter_length,struct intermediate inter_table[20],int *symbol_length,struct symbol symbol_table[20])
{
	char var_name[10];
	inter_table[*inter_length].line_number = *inter_length+1;
	inter_table[*inter_length].opCode = get_command_opcode("MOVR");
	inter_table[*inter_length].paramaters[1] = get_reg_opcode(reg);
	if (is_array(variable, var_name))
		inter_table[*inter_length].paramaters[0] = find_symbol_address(var_name, symbol_length, symbol_table) + is_array(variable, var_name);
	else
		inter_table[*inter_length].paramaters[0] = find_symbol_address(variable, symbol_length, symbol_table);
	*inter_length = *inter_length + 1;
}

void mov(char arguments[20], struct symbol symbol_table[20], int *symbol_length, int *inter_length, struct intermediate inter_table[20])
{
	char argument1[32], argument2[32], arr_len = 0;
	int i = 0, k = 0;
	while (arguments[i] != ','){
		argument1[k] = arguments[i];
		i++;
		k++;
	}
	argument1[k] = '\0';
	k = 0;
	i += 1;
	while (arguments[i] != '\0'){
		argument2[k] = arguments[i];
		i++;
		k++;
	}
	argument2[k] = '\0';
	if (strlen(argument2) == 2)
		mov_from_reg(argument1, argument2, inter_length, inter_table, symbol_length, symbol_table);
	else
		mov_to_reg(argument1, argument2, inter_length, inter_table, symbol_length, symbol_table);
}

void arith_operation(char command[20],char arguments[20],int *inter_length,struct intermediate inter_table[20])
{
	int i = 0;
	inter_table[*inter_length].line_number = *inter_length+1;
	inter_table[*inter_length].opCode = get_command_opcode(command);	
	char* token = strtok(arguments, ",");
	while (token != NULL) {
		printf("%s\n", token);
		inter_table[*inter_length].paramaters[i] = get_reg_opcode(token);
		i += 1;
		token = strtok(NULL, ",");
	}
	*inter_length = *inter_length + 1;
}


void read(char arguments[20],int *inter_length,struct intermediate inter_table[20])
{
	inter_table[*inter_length].line_number = *inter_length+1;
	inter_table[*inter_length].opCode = get_command_opcode("READ");
	inter_table[*inter_length].paramaters[0] = get_reg_opcode(arguments);
	*inter_length = *inter_length + 1;
}

void getCommand(char input[50],char command[20],char arguments[20])
{
	int i ,pos=0;
	for (i = 0; input[i] != '\0';)
		if (input[i] != '\t' && input[i] != ' ')
		{
			break;
		}
		else{
			i++;
		}
	for (; input[i] != ' ' && input[i] != '\0' && input[i]!='\n'; i++){
		command[pos] = input[i];
		pos++;
	}
	command[pos] = '\0';
	pos = 0;
	if (!strcmp(command, "IF"))
	{
		for (i = i + 1; input[i] != '\0' && input[i] != '\n'; i++){
			arguments[pos] = input[i];
			pos += 1;
		}
	}
	else{
		for (i = i + 1; input[i] != '\0' && input[i] != '\n'; i++){
			if (input[i] == ' ')
				i += 1;
			arguments[pos] = input[i];
			pos += 1;
		}
	}
	arguments[pos] = '\0';
}

void constant(char arguments[32], struct memory memory_table, int *symbol_length, struct symbol symbol_table[20],int *table_size)
{
	int i = 0,number=0;
	char var_name[32];
	for (i = 0; arguments[i] != '='; i++)
		var_name[i] = arguments[i];
	var_name[i] = '\0';
	for (i = i + 1; arguments[i] != '\0' && arguments[i]!='\n'; i++)
		number = number * 10 + arguments[i] - '0';
	int len = 1,address;
	allocate_symbols(var_name, table_size,&len , symbol_length, symbol_table);
	address=find_symbol_address(var_name, symbol_length, symbol_table);
	memory_table.value[address] = number;
}

void label_allocation(char command[32],int *label_length, struct label label_table[20])
{
	int i = 0;
	char var_name[32];
	for (i = 0; command[i] != ':'; i++)
		var_name[i] = command[i];
	var_name[i] = '\0';
	strcpy(label_table[*label_length].block_name, var_name);
	label_table[*label_length].address = *label_length;
	*label_length = *label_length + 1;
}

void print(char arguments[32],int *symbol_length, struct symbol symbol_table[20],int *total_length)
{
	int i = 1;
	allocate_symbols(arguments, total_length, &i, symbol_length, symbol_table);
}


void jmp(char arguments[32], int *symbol_length, struct symbol symbol_table[20], int *total_length)
{
	int i = 1;
	allocate_symbols(arguments, total_length, &i, symbol_length, symbol_table);
}


int processif(struct Stack *stack, int *inter_length)
{
	push(stack, *inter_length);
	return (*stack).top;
}

int processelse(struct Stack *stack,int *instruction)
{
	push(stack, *instruction);
	return (*stack).top;
}

int processendif(struct Stack* stack, struct intermediate *inter_table, int *inter_length)
{
	int index;
	while ((*inter_table[(*stack).top]).get_command_opcode != 7)
	{
		index = (*stack.top)--;
		int i = 0;
		while (inter_length[index].parameters[i] != -1)
		{
			i++;
		}
		inter_length[index].parameters[i] = instruction;
		instruction = index;
	}
	index = stack[top--];
	int i = 0;
	while (inter_length[index].parameters[i] != 0)
	{
		i++;
	}
	inter_length[index].parameters[i] = instruction + 1;
	return top;
}


int main()
{
	int memory[20],label_length=0, total_size = 0,data_allowed=1,inter_length = 0, symbol_length = 0,i,j;
	char input[50],command[20], arguments[20];
	stack.top = -1;
	int prev = 0, backpatch_value;
	FILE *input_file = fopen("input.txt", "r");
	if (input != NULL){
		while (fgets(input,50,input_file)){
			if (isequal(command, "END"))
				break;
			getCommand(input,command,arguments);
			if (isequal(command, "DATA") && data_allowed==1)
				data(arguments,&symbol_length,symbol_table,&total_size);
			else if (isequal(command, "MOV"))
				mov(arguments, symbol_table, &symbol_length, &inter_length, inter_table);
			else if (isequal(command, "START:"))
				data_allowed = 0;
			else if (isequal(command, "ADD") || !strcmp(command, "SUB") || !strcmp(command, "MUL"))
				arith_operation(command, arguments, &inter_length, inter_table);
			else if (isequal(command, "READ"))
				read(arguments, &inter_length, inter_table);
			else if (isequal(command, "CONST"))
				constant(arguments, memory_table, &symbol_length, symbol_table, &total_size);
			else if (isequal(command, "PRINT"))
				print(arguments, &symbol_length, symbol_table,&total_size);
			else if (isequal(command, "JMP"))
				print(arguments, &symbol_length, symbol_table, &total_size);
			else if (isequal(command, "IF"))
				processif(&stack,&inter_length);
			//else if (isequal(command, "ELSE"))
			//	processelse(&stack,&inter_length);
			//else if (isequal(command, "ENDIF"))
			//	endif(&backpatch_value,&stack, &inter_length, inter_table, &label_length, label_table,&prev);
			else if (command[strlen(command - 1)] == ':')
				label_allocation(command,&label_length, label_table);
		}
	}

	if (!isEmpty(&stack))
	{
		while (!isEmpty(&stack))
		{
			prev = pop(&stack);
			for (i = 0; inter_table[inter_length - 1].paramaters[i]; i++);
			inter_table[inter_length - 1].paramaters[i] = prev;
		}
	}
	FILE *output_file = fopen("output.txt", "w");
	for (i = 0; i < inter_length; i++){
		fprintf(output_file, "%d ", inter_table[i].line_number);
		fprintf(output_file, "%d ", inter_table[i].opCode);
		for (j = 0; j < 6; j++)
			fprintf(output_file,"%d ", inter_table[i].paramaters[j]);
		fputc('\n',output_file);
	}
	fclose(output_file);
	fclose(input_file);
	return 0;
}
	