#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

struct ucell
{
	int flag=0;
	int data=0;
	int formflag = 0;
	char formula[10];// = (char *)malloc(10 * sizeof(char *));
};
typedef ucell cell;


void op_push(char ch, char *stack, int top)
{
	top = top + 1;
	stack[top] = ch;
}

void var_push(int ch, int *stack, int top)
{
	top++;
	stack[top] = ch;
}


int ifOperator(char ch)
{
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/')
		return 1;
	else
		return 0;
}

int Prec(char ch)
{
	switch (ch)
	{
	case '+':
	case '-':
		return 1;

	case '*':
	case '/':
		return 2;

	case '^':
		return 3;
	}
	return -1;
}

char op_peek(char *stack, int op_top)
{
	return stack[op_top];
}


int var_peek(int *stack, int top)
{
	return stack[top];
}


int operation(int a, int b, char op)
{
	int val = 0;
	switch (op)
	{
	case '*':
		val = b * a;
		break;

	case '/':
		val = b / a;
		break;
	case '+':
		val = b + a;
		break;
	case '-':
		val = b - a;
		break;
	}
	return val;
}


int var_pop(int *stack, int top)
{
	int item;
	item = stack[top];
	return item;
}
char op_pop(char *stack, int top)
{
	return stack[top];
}



int areequal(char *s1, char *s2)
{
	int l1 = strlen(s1), l2 = strlen(s2);
	int i;
	if (l1 != l2)
		return 0;
	for (i = 0; i < strlen(s1); i++)
		if (tolower(s1[i]) != tolower(s2[i]))
			return 0;
	return 1;
}

int computeExpr(int evalue1, char op, int evalue2)
{
	if (op == '+')
		return evalue1 + evalue2;
	else if (op == '-')
		return evalue1 - evalue2;
	else if (op == '/')
		return evalue1 / evalue2;
	else if (op == '*')
		return evalue1*evalue2;
}

int eval(char formula[5],struct ucell **cell)
{
	int row, col;
	int i;
	if (strlen(formula) == 1)
		return formula[0] - '0';
	if (strlen(formula) == 2)
	{
		if (cell[formula[1] - '1'][formula[0] - 'A'].formflag != 1)
			return cell[formula[1] - '1'][formula[0] - 'A'].data;
		else
			return eval(cell[formula[1] - '1'][formula[0] - 'A'].formula, cell);
	}
	else 
	{
		
		char expr1[5];// = (char *)malloc(5 * sizeof(char *));
		char op;
		char expr2[5];// = (char *)malloc(5 * sizeof(char *));
		int evalue1, evalue2;
		int i = 0, j = 0;
		for (i = 0; formula[i] != '+' && formula[i] != '-' && formula[i] != '*' && formula[i] != '/'; i++)
		{
			expr1[j++] = formula[i];
		}
		expr1[i] = '\0';
		evalue1 = eval(expr1, cell);
		//i++;
		op= formula[i];
		int k = i;
		j = 0;
		for (i=k+1; formula[i]!='\0'; i++)
			expr2[j++] = formula[i];
		expr2[j] = '\0';
		evalue2 = eval(expr2, cell);
		return computeExpr(evalue1, op, evalue2);
	}
}


int getNumber(char*str, int start, int end)
{
	int i, n = 0;
	for (i = start; i <=end; i++)
	{
		if (str[i] == ' ')
			continue;
		if (isdigit(str[i]))
			n = (n * 10) + (str[i] - '0');
	}
	return n;
}



int evalinfix(char *infix, int *var_stack, int var_top, char op_stack[20], int op_top, struct ucell **cell)
{
	int result = 0;
	int i = 0, a, b;
	for (i = 0; i < strlen(infix);)
	{
		if (ifOperator(infix[i]))
		{
			while (op_top >= 0 && Prec(infix[i]) <= Prec(op_peek(op_stack, op_top)))
			{
				a = var_pop(var_stack, var_top);
				var_top--;
				b = var_pop(var_stack, var_top);
				var_top--;
				var_push(operation(a, b, op_pop(op_stack, op_top)), var_stack, var_top);
				op_top--;
				var_top++;
			}
			op_push(infix[i], op_stack, op_top);
			op_top++;
			i++;
		}
		else if (infix[i] == '(')
		{
			op_push('(', op_stack, op_top);
			op_top++;
			i++;
		}
		else if (infix[i] == ')')
		{
			while (op_top>0 && op_peek(op_stack, op_top) != '(')
			{
				a = var_pop(var_stack, var_top);
				var_top--;
				b = var_pop(var_stack, var_top);
				var_top--;
				var_push(operation(a, b, op_pop(op_stack, op_top)), var_stack, var_top);
				op_top--;
				var_top++;//exp[++k] = pop(stack);
			}
			if (op_top > 0 && op_peek(op_stack, op_top) != '(')
				return -1;
			else
			{
				op_pop(op_stack, op_top);
				op_top--;
			}i++;
		}
		else
		{
			char pos = 0, s[10];
			while (!ifOperator(infix[i]) && infix[i] != '\0' && infix[i] != '(' && infix[i] != ')')
			{
				s[pos++] = infix[i];
				i++;
				s[pos] = '\0';
			}
			
			//var_push(evalinfix(s, cell), var_stack, var_top);
			//var_top++;

			int row, col;
			int k;
			for (k = 0; k < strlen(s); k++)
			{
				if (!isdigit(s[k]))
					break;
			}
			if (k == strlen(s))
			{
				var_push(getNumber(s, 0, strlen(s) - 1),var_stack,var_top);
				var_top++;
			}
			else if (strlen(s) == 1)
				return s[0] - '0';
			else if (strlen(s) == 2)
			{
				if (cell[s[1] - '1'][s[0] - 'A'].formflag != 1){
					var_push(cell[s[1] - '1'][s[0] - 'A'].data, var_stack, var_top);// , op_stack, op_top, cell);
					var_top++;
				}
				else
				{
					var_push(evalinfix(cell[s[1] - '1'][s[0] - 'A'].formula, var_stack, var_top, op_stack, op_top, cell),var_stack,var_top);
					var_top++;
				}
			}
		}
	}
	while (op_top > -1)
	{
		a = var_pop(var_stack, var_top);
		var_top--;
		b = var_pop(var_stack, var_top);
		var_top--;
		var_push(operation(a, b, op_pop(op_stack, op_top)), var_stack, var_top);
		op_top--;
		var_top++;
	}
	return var_stack[0];
}




int get(char *attr,struct ucell **cell,int printflag)
{
	int i = 0, j, res = 0;
	int row=0,col = (attr[0] - 'A');
	row = attr[1]-'1';
	if (cell[row][col].formflag != 1)
	{
		if (cell[row][col].flag != 1)
		{
			if (printflag)
				printf("0 ");
			return 0;
		}
		else
		{
			if (printflag)
				printf("%d ", (cell[row][col].data));
			return cell[row][col].data;
		}
	}
	else
	{
		int var_top = -1;
		int op_top = -1;
		int var_stack[20];
		char op_stack[20];
		//i = eval(cell[row][col].formula, cell);
		i = evalinfix(attr, var_stack, var_top, op_stack, op_top, cell);
		printf("%d ", i);
		return i;
	}
}

/*
int getValue(char *s,struct ucell **cell)
{
	return get(s,cell,0);

	int i = 0;
	//GETTING NAME OR NUMBER FOR POSTFIX EVALUATION
	for (i = 0; i < strlen(s); i++)
	{
		if (!isdigit(s[i]))
		{
			break;
		}
	}

	//getting number from string
	if (i == strlen(attr))
	{
		for (j = 0; j < strlen(attr); j++)
		{
			res = (res * 10) + attr[j] - '0';
		}
		return res;
	}
	
	/*
	int i = 0, j, res = 0;


	//GETTING NAME OR NUMBER FOR POSTFIX evalinfixUATION
	//for (i = 0; i < strlen(s); i++)
	{
	if (!isdigit(s[i]))
	{
	break;
	}
	}
	//getting number from string
	if (i == strlen(s))
	{
	for (j = 0; j < strlen(s); j++)
	{
	res = (res * 10) + s[j] - '0';
	}
	return res;
	}

}

*/

void set(char *attr, struct ucell **cell)
{
	int i = 0,j, row = 0, d = 0,value=0;
	int col = (attr[0] - 'A');
	for (i = 0; i < strlen(attr); i++)
	{
		if (attr[i] == '=')
			break;
		if (isdigit(attr[i]))
			row = (row * 10) + (attr[i] - '1');
	}
	i++;
	//Getting expression after '=' 
	for (j = i; j < strlen(attr); j++)
	{
		if (attr[j] == ' ')
			continue;
		if (!isdigit(attr[j]))
			break;
	}
	int pos=0;

	//Attribute contains only number
	if (j == strlen(attr))
	{
		cell[row][col].data = getNumber(attr, i, strlen(attr));
		cell[row][col].formflag = 0;
		cell[row][col].flag = 1;
	}
	else
	{
		cell[row][col].flag = 0;
		cell[row][col].formflag = 1;
		for (j = i; j < strlen(attr); j++)
			(cell[row][col].formula)[pos++] = attr[j];
		(cell[row][col].formula)[pos] = '\0';
	}
}

void print(struct ucell **cell)
{
	char *buffer = (char *)calloc(5, sizeof(char));
	int i,j;
	for (i = 0; i < 10; i++)
	{
		for (j = 1; j <=10; j++)
		{
			sprintf(buffer, "%c%c", i + 'A', j + '0');
			get(buffer, cell,1);
		}
		printf("\n");
	}
}

char *export(char *attr,struct ucell **cell)
{
	char *buffer = (char *)calloc(5, sizeof(char));
	FILE *fp;
	fp = fopen(attr, "a");
	int i, j;
	rewind(fp);// fseek(fp, 0, SEEK_SET);
	for (i = 0; i < 10; i++)
	{
		for (j = 1; j < 10; j++)
		{
			sprintf(buffer, "%c%c", i + 'A', j + '0');
			if (cell[i][j].formflag != 1){
				fprintf(fp, "%d,", get(buffer,cell,0));
			}
			else{
				fprintf(fp, "%d,", get(buffer,cell,0));
			}
		}	fputs("\n", fp);
	}
	return attr;
}

/*
void import(char* file_name,cell** cell1)
{
	char *buff = (char *)calloc(1000, sizeof(char));
	char *token, *next;
	FILE *fp;
	int res, i = 0, j = 0, k = 0, flag = 0, len = strlen(file_name);
	for (; file_name[k] != '\0'; k++)
	{
		if (file_name[k] == '.')
		{
			flag = 1;
			break;
		}
	}
	if (!flag)
		strcat_s(file_name, len + 5, ".csv");
	res = fopen_s(&fp, file_name, "r");
	if (res)
	{
		printf("file doesnt exist");
		return;
	}
	while (i < 10)
	{
		fgets(buff, 1000, fp);
		token = strtok_s(buff, ",", &next);
		j = 0;
		while (token != NULL)
		{
			cell1[i][j].data = atoi(token);
			cell1[i][j].flag = 1;
			cell1[i][j].formflag = 0;
			token = strtok_s(NULL, ",", &next);
			j++;
		}
		i++;
	}
	fclose(fp);
}
*/

int main()
{
	int n = 10;
	struct ucell **cell = (struct ucell **)malloc(n*sizeof(struct ucell **));
	int i = 0, com_len, attr_len = 0, f = 0, j = 0;
	int var_top = -1;
	int op_top = -1;
	int var_stack[20];
	char op_stack[20];
	char *infix = (char *)malloc(10 * sizeof(char *));

	for (i = 0; i < n; i++)
		cell[i] = (struct ucell *)malloc(10*sizeof(struct ucell *));
	char *input = (char *)malloc(10 * sizeof(char *));
	char *command = (char *)malloc(7 * sizeof(char *));
	char *attr = (char *)malloc(10 * sizeof(char *));
	char *openedFile=(char *)malloc(10*sizeof(char *));
	int flagopen = 0;
	while (1)
	{
		printf("\n>");
		gets(input);
		com_len = 0;
		i = 0;
		attr_len = 0;
		for (i = 0; i < strlen(input); i++)
		{
			if (input[i] == ' ')
				break;
			command[com_len++] = input[i];
		}
		command[com_len] = '\0';
	
		if (areequal(command, "exit"))
			break;
		if (!areequal("PRINT", command) && !areequal("SAVE", command))
		{
			if (i == strlen(input))
			{
				printf("Invalid command\n");
				continue;
			}

			for (i = com_len; i < strlen(input);)
			{
				while (input[i] == ' ')
					i++;
				if (input[i] != ' ')
					attr[attr_len++] = input[i++];
			}
			attr[attr_len] = '\0';
		}

		if (areequal(command,"GET"))
			get(attr, cell,1);

		else if (areequal(command, "SET"))
			set(attr,cell);	

		else if (areequal(command, "PRINT"))
			print(cell);

		else if (areequal(command, "EXPORT"))
		{
			openedFile=export(attr, cell);
			flagopen = 1;
		}

		else if (areequal(command, "SAVE"))
		{
			if (flagopen == 1)
				export(openedFile, cell);
			else
				printf("No FILE EXISTS");
		}
		
		else if (areequal(command, "IMPORT"))
		{
			import(attr, cell);
		}
		else if (areequal(command, "EVAL"))
			printf("%d",evalinfix(attr, var_stack, var_top, op_stack, op_top, cell));
	}
	return 0;
}