#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*constants for true and false*/
#define FALSE 0
#define TRUE 1

/*enumerated types for token types*/
typedef enum
{
	ID, INTLITERAL, MAIN, READ, WRITE, IF, ELSE, WHILE, SCANEOF, 
	PLUSOP, MINUSOP, TIMESOP, DIVIDESOP, ASSIGNOP, LPAREN, RPAREN, LBRACKET, RBRACKET,
	GREATERTHAN, LESSTHAN, EQUALTO, GREATEREQUALTO, LESSEQUALTO, NOTEQUALTO, COMMA, SEMICOLON
} token;

/*functions declarations related to scanner*/
token scanner();
void clear_buffer();
void buffer_char(char c);
token check_reserved();
void lexical_error();

/*functions declarations related to parser*/
void parser();
void program();
void statement_list();
void statement();
void id_list();
void left_bracket();
void right_bracket();
void expression_list();
void expression();
void term();
void add_op();
void booleanExpr();
void operand();
void booleanOp();
void match(token tok);
void syntax_error();

/*global variables*/
FILE *fin; /*source file*/
FILE *fout; /*output file*/
token next_token; /*next token in source file*/
char token_buffer[100]; /*token buffer*/
int token_ptr; /*buffer pointer*/
int line_num = 1; /*line number in source file*/
int error = FALSE; /*flag to indicate error*/



/*********************************************************************/
/*returns next token from source file*/
token scanner()
{
	
	char c; /*current character in source file*/

	clear_buffer(); /*empty token buffer*/

	while(TRUE){ /*loop reads and returns next token*/
		c = getc(fin); /*read a character from source file*/ 
	
		if(c == EOF) /*end of file*/
			return SCANEOF;
	
		else if (isspace(c)) /*skip white spaces and count line number*/
		{
			if(c == '\n')
				line_num = line_num + 1;
		}
	
		else if (isalpha(c)) /*identifier or reversed word*/
			{
			buffer_char (c); /*buffer the first character*/
			c = getc (fin); 
			while (isalnum(c) || c == '_') /*read and buffer subsequent characters*/
			{
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c, fin); /*put back the last character read*/
			return check_reserved(); /*return identifier or reserved word*/
		}
	
		/*integer literal*/
		else if (isdigit(c))
		{
			buffer_char(c); /*buffer the first character*/
			c = getc(fin);
			while(isdigit(c)) /*read and buffer subsequent characters*/
			{
				buffer_char(c);
				c = getc(fin);
			}
			ungetc(c, fin); /*put back the last character read*/
			return INTLITERAL; /*return integer literal*/
		}
	
		else if (c == '(') /*left parentheses*/
			return LPAREN;
	
		else if (c == ')') /*right parentheses*/
			return RPAREN;
	
		else if (c == '{') /*left Bracket*/
			return LBRACKET;
	
		else if (c == '}') /*right Bracket*/
			return RBRACKET;
			
		else if (c == ',') /*comma*/
			return COMMA;
	
		else if (c == ';') /*semicolon*/
			return SEMICOLON;
	
		else if (c == '>'){
		
			c = getc(fin); //greater than or equal too
			if(c == '='){
				return GREATEREQUALTO;
			}else{
				ungetc(c, fin); //greater than
				return GREATERTHAN;
			}
		}
		else if (c == '<'){
		
			c = getc(fin); //less than or equal to
			if(c == '='){
				return LESSEQUALTO;
			}else{
				ungetc(c, fin); //less than
				return LESSTHAN;
			}
		}
			
		else if (c == '='){ //equal too
			c = getc(fin);
			if(c == '='){
				return EQUALTO;
				
			}else{/*error due to =*/
				ungetc(c, fin); 
				lexical_error();
			}
		}
		
		else if (c == '!'){
			c = getc(fin);
			if(c == '=') /*NOT equal to*/
				return NOTEQUALTO;
			else /*error due to !*/
			{
				ungetc(c, fin); 
				lexical_error();
			}
		}
				
		else if (c == '+') /*plus operator*/
			return PLUSOP;
	
		else if (c == '-') /*minus operator*/
			return MINUSOP;
			
		else if (c == '*') /*multiplication*/
			return TIMESOP;
	
		else if (c == '/'){
			c = getc(fin);
			if(c == '/') /*comment begins*/
			{
				 do /*read and discard until end of line*/
					c = getc(fin);
				while (c != '\n');
				line_num = line_num + 1;
			}
			else /*division operator*/
			{
				ungetc(c, fin); 
				return DIVIDESOP; 
			} 
		}
	
		else if(c == ':') /*possible assignment operator*/
		{
			c = getc(fin);
			if(c == '=') /*assignment operator*/
				return ASSIGNOP;
			else /*error due to :*/
			{
				ungetc(c, fin); 
				lexical_error();
			}
		}
	
		else /*invalid character*/
			lexical_error();
	}
}

/**********************************************************************/

/*clears the buffer*/
void clear_buffer()
{
	 token_ptr = 0; /*reset token pointer*/
	 token_buffer[token_ptr] = '\0'; /*add null character*/
}

/**********************************************************************/

/*appends the character to buffer*/
void buffer_char(char c)
{
	 token_buffer[token_ptr] = c; /*append current character*/
	 token_ptr = token_ptr + 1; /*move token pointer*/
	 token_buffer[token_ptr] = '\0'; /*move null character*/
}

/**********************************************************************/

/*checks whether buffer is reserved word or identifier*/
token check_reserved()
{
	if(strcmp(token_buffer, "main") == 0) /*Six reserved words*/
		return MAIN;
	else if(strcmp(token_buffer, "read") == 0)
		return READ;
	else if(strcmp(token_buffer, "write") == 0)
		return WRITE;
	else if(strcmp(token_buffer, "if") == 0)
		return IF;
	else if(strcmp(token_buffer, "else") == 0)
		return ELSE;
	else if(strcmp(token_buffer, "while") == 0)
		return WHILE;
	else /*identifier*/
		return ID;
}

/*********************************************************************/

/*reports lexical error and sets error flag*/
void lexical_error()
{
	printf("lexical error in line %d/n", line_num);
	error = TRUE;
}

/********************************************************************/

/*parses source file*/
void parser()
{
	next_token = scanner(); /*read the first token*/
	program(); /*parse the program*/
	match(SCANEOF); /*check end of file*/
}

/*********************************************************************/

/*parses a program*/
/* <program> --> main<l_brack> <stmtlist> <r_brack>*/
void program()
{
	match(MAIN); /*begin*/
	left_bracket();
	statement_list(); /*list of statements*/
	right_bracket(); /*end*/
}

/*********************************************************************/

/*parses list of statements*/
/* <stmtlist> --> <stmt> {<stmt>} */
void statement_list()
{
	statement(); /*first statement*/
	while (TRUE)
	{
	if(next_token == ID || next_token == READ || next_token == WRITE || 
	next_token == IF || next_token == ELSE || next_token == WHILE)
		statement(); /*subsequent statements*/
	else
		break;
	}
}

/********************************************************************/

/*parses one statement*/
/* 
 <stmt> --> id:=<expr>;
 <stmt> --> read(<idlist>);
 <stmt> --> write(<idlist>);
 <stmt> --> if( <booleanExp> ) <l_brack> <stmtlist> <r_brack> 
			else <l_brack> <stmtlist> <r_brack>
  <stmt> --> if(<booleanExp>)<l_brack> <stmtlist> <r_brack>
 <stmt> --> while(<booleanExp>)<l_brack> <stmtlist> <r_brack> */
void statement()
{
	if(next_token == ID) /*assignment statement*/
	{
		match(ID);
		match(ASSIGNOP);
		expression();
		match(SEMICOLON);
	}
	else if (next_token == READ) /*read statement*/
	{
		match(READ);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
	}
	else if (next_token == WRITE) /*write statement*/
	{
		match(WRITE);
		match(LPAREN);
		id_list();
		match(RPAREN);
		match(SEMICOLON);
	}
	else if (next_token == IF) /*if statement*/
	{
		match(IF);
		match(LPAREN);
		booleanExpr();
		match(RPAREN);
		match(LBRACKET);
		statement_list();
		match(RBRACKET);
	}
	else if (next_token == ELSE) /*else statement*/
	{
		match(ELSE);
		match(LBRACKET);
		statement_list();
		match(RBRACKET);
	}
	else if (next_token == WHILE) /*while statement*/
	{
		match(WHILE);
		match(LPAREN);
		booleanExpr();
		match(RPAREN);
		match(LBRACKET);
		statement_list();
		match(RBRACKET);
	}
	else /*invalid beginning of statement*/
		syntax_error();
}

/********************************************************************/

/*parses list of identifiers*/
/* <idlist> --> id {,id} */
void id_list()
{
	match(ID); /*first identifier*/
	while(next_token == COMMA) 
	{
		match(COMMA); 
		match(ID); /*subsequent identifiers*/
	}
}

/*******************************************************************/

/*<l_brack> --> { */
void left_bracket(){
	if(next_token == LBRACKET)
		match(next_token);
	else
		syntax_error();
}

/*******************************************************************/

/*<r_brack> --> { */
void right_bracket(){
	if(next_token == RBRACKET)
		match(next_token);
	else
		syntax_error();
}

/*******************************************************************/

/*parses list of expressions*/
/* <explist> --> <exp>{,<exp>} */
void expression_list()
{
	expression(); /*first expression*/
	while(next_token == COMMA)
	{
		match(COMMA); 
		expression(); /*susequent expressions*/
	}
}

/******************************************************************/

/*parses one expression*/
/* <expr> --> <term>{<adop><term>} */
void expression()
{
	term(); /*first term*/
	while(next_token == PLUSOP || next_token == MINUSOP || next_token == TIMESOP || next_token == DIVIDESOP)
	{
		add_op(); /*plus or minus or divide or multiple*/
		term(); /*subsequent terms*/
	}
}

/******************************************************************/

/*parses one term*/
/* <term> --> id | integer | (<expr>) */
void term()
{
	if(next_token == ID) /*identifier*/
		match(ID);
	else if(next_token == INTLITERAL) /*integer literal*/
		match(INTLITERAL);
	else if(next_token ==LPAREN) /*expression inside parentheses*/
	{
		match(LPAREN);
		expression();
		match(RPAREN);
	}
	else /*invalid term*/
		syntax_error();
}

/******************************************************************/

/*parses plus or minus or multiplication or division operator*/
/* <adop> --> ( +| - | * | / ) */
void add_op()
{
	if(next_token == PLUSOP || next_token == MINUSOP || next_token == TIMESOP || next_token == DIVIDESOP)
		match(next_token);
	else
		syntax_error();
}
/******************************************************************/

/*parses one boolean expression*/
/* <boolean_expr> --> <operand> <boolean_op> <operand>*/
void booleanExpr(){
	operand(); /*first term*/
	booleanOp();
	operand(); /*subsequent terms*/
}

/******************************************************************/
/*parses one operand*/
/* <operand> --> id | integer */

void operand(){
	if(next_token == ID) /*identifier*/
			match(ID);
	else 				/*integer literal*/
		match(INTLITERAL);
}

/******************************************************************/

/*parses greater than, less than, greater than or equal to, less than or equal to, equal to , not equal to operator*/
/* <boolean_op> --> ( > | < | >= | <= | == | != ) */
void booleanOp()
{
	if(next_token == GREATERTHAN || next_token == LESSTHAN || next_token == EQUALTO ||
		next_token == GREATEREQUALTO || next_token == LESSEQUALTO || next_token == NOTEQUALTO)
		match(next_token);
	else
		syntax_error();
}
/******************************************************************/

/*checks whether the expected token and the actual token match, 
and also reads the next token from source file*/ 
void match(token tok)
{
	if(tok == next_token) /*expected token and actual token match*/
		;
	else
		syntax_error(); /*expected token and actual token do not match*/

	next_token = scanner(); /*read the next token*/
}

/********************************************************************/
/*reports syntax error*/
void syntax_error()
{
	printf("syntax error in line %d\n", line_num);
	error = TRUE;
}
/********************************************************************/
//printing to the out file
void printIt(){
	
    fout = fopen("OutputFile.txt","w"); //open the file to write in it
    next_token = scanner(); //check next token using scanner()
    

    while(next_token != SCANEOF){//loop until EOF
    	
    	//loop through all the tokens and match them up and then print them out to the out file
        if(next_token == ID)
            fprintf(fout, "%s", "ID ");
            
        else if(next_token == INTLITERAL)
            fprintf(fout, "%s", "INTLITERAL ");
            
        else if(next_token == MAIN)
            fprintf(fout, "%s", "MAIN ");
            
        else if(next_token == READ)
            fprintf(fout, "%s", "READ ");
            
        else if(next_token == WRITE)
            fprintf(fout, "%s", "WRITE ");
            
        else if(next_token == IF)
            fprintf(fout, "%s", "IF ");
            
        else if(next_token == ELSE)
            fprintf(fout, "%s", "ELSE ");
            
        else if(next_token == WHILE)
            fprintf(fout, "%s", "WHILE ");
        
        else if(next_token == PLUSOP)
            fprintf(fout, "%s", "PLUSOP ");
            
        else if(next_token == MINUSOP)
            fprintf(fout, "%s", "MINUSOP ");
            
        else if(next_token == TIMESOP)
            fprintf(fout, "%s", "TIMESOP ");
            
        else if(next_token == DIVIDESOP)
            fprintf(fout, "%s", "DIVIDESOP ");
            
        else if(next_token == ASSIGNOP)
            fprintf(fout, "%s", "ASSIGNOP ");
            
        else if(next_token == LPAREN)
            fprintf(fout, "%s", "LPAREN ");
            
        else if(next_token == RPAREN)
            fprintf(fout, "%s", "RPAREN ");
            
        else if(next_token == LBRACKET)
            fprintf(fout, "%s", "LBRACKET ");
            
        else if(next_token == RBRACKET)
            fprintf(fout, "%s", "RBRACKET ");
            
        else if(next_token == GREATERTHAN)
            fprintf(fout, "%s", "GREATERTHAN ");
        
        else if(next_token == LESSTHAN)
            fprintf(fout, "%s", "LESSTHAN ");
            
        else if(next_token == EQUALTO)
            fprintf(fout, "%s", "EQUALTO ");
            
        else if(next_token == NOTEQUALTO)
            fprintf(fout, "%s", "NOTEQUALTO ");
            
        else if(next_token == GREATEREQUALTO)
            fprintf(fout, "%s", "GREATEREQUALTO ");
            
        else if(next_token == LESSEQUALTO)
            fprintf(fout, "%s", "LESSEQUALTO ");
            
        else if(next_token == COMMA)
            fprintf(fout, "%s", "COMMA ");
            
        else if(next_token == SEMICOLON)
            fprintf(fout, "%s", "SEMICOLON ");
            
        next_token = scanner(); //get next token everyloop
	}
	
    fprintf(fout, "%s", "SCANEOF "); //put SCANOF at the end of the file
            
    fclose(fout); //close file
        
}
/************************************************************************/
//main method
int main()
{
	char fileName[20]; //file name
	char token;
	int option;
	
	printf("Enter the file name you are reading from: ");
	gets(fileName); //user input for file name

	do{
		//menu options
		printf("\nOption 1: Write the sequence of tokens to another output file\n");
		printf("Option 2: Parse the source code\n");
		printf("Enter any other number to quit\n");
		printf("\nOption: ");
		scanf("%d", &option);
		
		//for option 1
		if(option == 1){
			
			fin= fopen(fileName,"r"); //read fill
			if(fin == NULL){ //error if the file is not found
				printf("[ERROR] Couldn't Open FIle");
				exit(0);
			}
			
			printIt(); //call printit method
			if(error != TRUE) //loop if there are no errors
				printf("Printing was successful\nPrinting can be found in output.txt\n");
			else
				printf("Printing was unsuccessful\n");
				
			fclose(fin); //close file
			
			
		}else{//option 2
			
			fin= fopen(fileName,"r"); //reading file
			parser(); //calling parser method
			if(error != TRUE) //loop if there are no errors
				printf("Parsing was successful\n");
			else
				printf("Parsing was unsuccessful\n");
			fclose(fin); //close file
			
		}
	}while(option ==1 || option == 2); //loop while option equals 1 or 2
}
/********************************************************************/
