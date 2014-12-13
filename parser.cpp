#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
using namespace std;
long seconds = 0;
union VarData {
		long l;
		float f;
		char c[4];
		int i;
	};
enum TOKEN_TYPES{
	TOKEN_VAR,
	TOKEN_NUMBER_INT,
	TOKEN_OPERATION,
	TOKEN_EMPTY,
	TOKEN_NUMBER_FLOAT
};
struct Token{
	VarData data;
	TOKEN_TYPES type;
};
struct Operation{
	Token* token;
	char priority;
};

char cfg[] = "x=(1-2)*(3+5)*(3+5)*(3+5)";//d5 = s % 2 || s == 4;";//d9 = !( h > 2 && h < 11 );d10 = !( h > 2 && h < 11 && m < 5 )";
bool isNum(char *c){
	return (*c >= '0' && *c <= '9');
}
bool isNumDot(char *c){
	return isNum(c) || *c == '.';
}
bool isChar(char *c){
	return (*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z');
}
bool isEmpty(char *c){
	return *c == ' ' || *c == '\n' || *c == '\t';
}
bool isVar(char *c){
	return isChar(c) || isNum(c);
}

enum OPERATION_PRIORITY {
	PRIORITY_EQUAL = 3,
	PRIORITY_LOGIC = 4,
	PRIORITY_PLUS_MINUS = 5,
	PRIORITY_MUL_DIV = 6,
	PRIORITY_OPEN_PARENTHESIS = 1,
	PRIORITY_CLOSE_PARENTHESIS = 0
};
/*Token[] tokenizer( char *expr ){
	
}*/
char getPriority( VarData *data ){
	
	switch(data->l){
		case '+':
		case '-':
			return PRIORITY_PLUS_MINUS;
		case '*':
		case '/':
		case '%':
			return PRIORITY_MUL_DIV;
		case '(':
			return PRIORITY_OPEN_PARENTHESIS;
		case ')':
			return PRIORITY_CLOSE_PARENTHESIS;
		case '<':
		case '>':
		case '>'+('='<<8):
		case '<'+('='<<8):
		case 'a'+('n'<<8)+('d'<<16):
		case 'o'+('r'<<8):
			return PRIORITY_LOGIC;
		case '=':
			return PRIORITY_EQUAL;
	};
	
	return 1;
}
Token resolveVar( VarData name ){
	Token out;
	out.type = TOKEN_NUMBER_INT;
	switch(name.l){
		case 's':
			out.data.l = seconds;
			break;
	}
	return out;
}

Token doIt(long operation, Token *data, TOKEN_TYPES type){
	Token out;
	out.type = type;
	switch(operation){
		case '+':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[0].data.l + data[1].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[0].data.f + data[1].data.f;
		break;
	};
	return out;
}

void step(long operation, Token *stack, int *pointer){
	Token token[3];
	char count, wereFloats = false, wereInts = false;
	int i;
	
	switch(operation){
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
			count = 2;
		;
	};
	
	for( i = 0; i < count; i++ ){
		token[i] = stack[*pointer - i - 1];
		if( token[i].type == TOKEN_VAR )
			token[i] = resolveVar( token[i].data );
		if( token[i].type == TOKEN_NUMBER_FLOAT )
			wereFloats = true;
		else if( token[i].type == TOKEN_NUMBER_INT )
			wereInts = true;
	}
	pointer -= count;
	if( wereInts && wereFloats ){
		for( i = 0; i < count; i++ ){
			if( token[i].type == TOKEN_NUMBER_INT )
				token[i].data.f = (float)token[i].data.l;
		}
	}
	
	cout << pointer << "@\n";
	stack[*pointer] = doIt(operation, token, wereFloats ? TOKEN_NUMBER_FLOAT : TOKEN_NUMBER_INT );
	pointer++;
}


void evalute(Token *ptrProgram, int programP){
	/*Token program[] = *ptrProgram;
	cout << programP << '\n';
	for( int i = 0; i < programP; i++)
	if(program[i].type==TOKEN_EMPTY || program[i].type == TOKEN_VAR || program[i].type == TOKEN_OPERATION)
		cout << (int)program[i].type <<'|'<< program[i].data.c << '\n';
	/*else if(*program[i].type == TOKEN_NUMBER_INT)
	cout << (int)&program[i].type <<'|'<< (long)program[i].data.l << '\n';
	else if(program[i].type == TOKEN_NUMBER_FLOAT)
	cout << (int)program[i].type <<'|'<< (float)program[i].data.f << '\n';*/

	Token stack[64];
	int stackP = 0;
	TOKEN_TYPES tokenType;
	for( int i = 0; i < programP; i++ ){
		tokenType = ptrProgram[i].type;
		
		if( tokenType == TOKEN_NUMBER_INT || tokenType == TOKEN_NUMBER_FLOAT || tokenType == TOKEN_VAR ){
			stack[stackP].type = tokenType;
			stack[stackP].data = ptrProgram[i].data;
			stackP++;
		}else if( tokenType == TOKEN_OPERATION ){
			step(ptrProgram[i].data.l, stack, &stackP);
		}
	}
	cout << '\n';
}
void run(char *expr){
	cout << expr << '\n';
	Token tokens[256];
	int tokenP = 0, i;
	char *p, tmpChar;
	char *c = expr;
	char *start;
	bool wasDot;
	while(1){
		if( isChar(c) ){
			start = c;
			while(isVar(++c));
			--c;
			for( p = start; p <= c; p++)
				tokens[tokenP].data.c[p-start] = *p;
			if( p-start < 4 )
				tokens[tokenP].data.c[p-start] = 0;
			tokens[tokenP].type = TOKEN_VAR;
			tokenP++;
		}else if( isNum(c) ){
			start = c;
			wasDot = false;
			++c;
			while(1){
				if(isNum(c)){
					
				}else if( *c == '.' ){
					wasDot = true;
				}else{
					break;
				}
				++c;
			};
			
			tmpChar = *c;
			*c = 0;
			if(wasDot){
				tokens[tokenP].data.f = atof(start);
				tokens[tokenP].type = TOKEN_NUMBER_FLOAT;
			}else{
				tokens[tokenP].data.l = atol(start);
				tokens[tokenP].type = TOKEN_NUMBER_INT;
			}
			*c = tmpChar;
			--c;
			tokenP++;
		}else if( isEmpty(c) ){
			start = c;
			while(isEmpty(++c));
			--c;
			for( p = start; p <= c; p++)
				tokens[tokenP].data.c[p-start] = *p;
			if( p-start < 4 )
				tokens[tokenP].data.c[p-start] = 0;
			tokens[tokenP].type = TOKEN_EMPTY;
			tokenP++;
		}else if( *c == 0 ){
			break;
		}else{
			i = 0;
			if( tokenP > 0 && tokens[tokenP-1].type == TOKEN_OPERATION ){
				p = &tokens[tokenP-1].data.c[0];
				if( 
					((*p == '>' || *p == '<' || *p == '=' || *p == '!') && *c == '=') ||
					(*p == '|' && *c == '|') ||
					(*p == '&' && *c == '&')
				){
					++i;
					tokenP--;
				}
			}
			
			tokens[tokenP].data.c[i] = *c;
			tokens[tokenP].data.c[i+1] = 0;
			tokens[tokenP].type = TOKEN_OPERATION;
			tokenP++;
		}
		
			
		
		
		c++;
	}
	// LEXER
	Token program[256];
	int programP = 0;
	Operation stack[64];
	int stP = 0, push;
	char priority, tmpPriority;
	
	for( i = 0; i < tokenP; i++){
		if( tokens[i].type == TOKEN_OPERATION ){
			priority = getPriority(&tokens[i].data);
			
			if( priority == PRIORITY_CLOSE_PARENTHESIS ){
				while( stP > 0 ){
					--stP;
					if( stack[stP].priority != PRIORITY_OPEN_PARENTHESIS ){
						program[programP].type = TOKEN_OPERATION;
						program[programP].data = stack[stP].token->data;
						++programP;
					}else
						break;
				}
								
				continue; // we don't want to push close PARENTHESIS
			}else if( !(				
				stP == 0 || // if stack is empty
				priority > stack[stP - 1].priority || // if priority is higher than previous
				priority == PRIORITY_OPEN_PARENTHESIS // if it's open PARENTHESIS
			) ){// then do not roll stack back
				while( stP > 0 ){
					--stP;
					tmpPriority = stack[stP].priority;
					if( priority <= tmpPriority){
						if( tmpPriority != PRIORITY_OPEN_PARENTHESIS ){
							program[programP].type = TOKEN_OPERATION;
							program[programP].data = stack[stP].token->data;
							++programP;
						}
					}else{
						stP++;
						break;
					}
				}
			} // just push
			stack[stP].priority = priority;				
			stack[stP].token = &tokens[i];
			++stP;
			
		}else if( tokens[i].type == TOKEN_VAR || tokens[i].type == TOKEN_NUMBER_INT || tokens[i].type == TOKEN_NUMBER_FLOAT ){
			program[programP].type = tokens[i].type;
			program[programP].data = tokens[i].data;
			++programP;
		}
	}
	cout << '~' << stP <<'\n';
	while( stP > 0 ){
		--stP;
		program[programP].type = TOKEN_OPERATION;
		program[programP].data = stack[stP].token->data;
		
		++programP;
	}
	evalute(program, programP);
/*
	
	cout << tokenP << '\n';
	for( i = 0; i < tokenP; i++)
		if(tokens[i].type==TOKEN_EMPTY || tokens[i].type == TOKEN_VAR || tokens[i].type == TOKEN_OPERATION)
			cout << (int)tokens[i].type <<'|'<< tokens[i].data.c << '\n';
		else if(tokens[i].type == TOKEN_NUMBER_INT)
			cout << (int)tokens[i].type <<'|'<< (long)tokens[i].data.l << '\n';
		else if(tokens[i].type == TOKEN_NUMBER_FLOAT)
			cout << (int)tokens[i].type <<'|'<< (float)tokens[i].data.f << '\n';
			
	cout << '\n';
	*/
	
}
void proceed(char *cfg){
	int i = 0, eLen;
	char expr[256];
	char c;
	i = 0;
	eLen = 0;
	while(1){
		c = cfg[i++];
		if( c == ';'){
			expr[eLen] = 0;
			run(expr);
			eLen = 0;
		}else if( c == 0 ){
			expr[eLen] = 0;
			run(expr);
			break;
		}else{
			expr[eLen++] = c;
		}
	}
	
	
}
int main()
{
	proceed(cfg);
	return 0;
}