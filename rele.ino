#include "main.h"
#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include <SimpleTimer.h>

int clockInt = 0;            // digital pin 2 is now interrupt 0
int masterClock = 0;         // counts rising edge clock signals
long seconds = 0;             // variable
int minutes = 0;             // variable
int hours = 0;  
SimpleTimer timer;


char cfg[] = "d4 =  s <10 && s % 2;\
d5 = s >10 && s <20 && s % 2;\
d6 = s >20 && s <30 && s % 2;\
d7 = s >30 && s <40 && s % 2;\
d8 = s >40 && s <50 && s % 2;\
d9 = s >50 && s % 2;";
// % 2 || s==4";//d9 = !( h > 2 && h < 11 );d10 = !( h > 2 && h < 11 && m < 5 )";
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

/*Token[] tokenizer( char *expr ){
	
}*/

void setup()
{
	Serial.begin(57600);
	pinMode(5,OUTPUT);
	pinMode(30,OUTPUT);
	pinMode(32,OUTPUT);
	pinMode(34,OUTPUT);
	pinMode(36,OUTPUT);
	pinMode(38,INPUT);
	pinMode(40,INPUT);
	pinMode(42,INPUT);
	pinMode(44,INPUT);
	timer.setInterval(1000, repeatMeSecond);
	Serial.println("INIT");
	//timer.setInterval(60000, repeatMe);
}

void repeatMe()      // called by interrupt
{
	
	minutes++;
	if(minutes==60){
		minutes = 0;
		hours++;
		if(hours == 24){
			hours = 0;
		}
	}
	
	return;
}
char getPriority( VarData *data ){
	
	switch(data->i){
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
			return PRIORITY_LOGIC;
		case '=':
			return PRIORITY_EQUAL;
		case '|'+('|'<<8):
		case '&'+('&'<<8):
		case 'a'+('n'<<8)+('d'<<16):
		case 'o'+('r'<<8):
			return PRIORITY_COMPARE;
	};
	
	return 1;
}
Token resolveVar( VarData name ){
	Token out;
	out.type = TOKEN_NUMBER_INT;
	
	Serial.print("GET ");
	Serial.print(name.c);
	
	switch(name.l){
		case 's':
			out.data.l = seconds;
			break;
		default:
			out.data.l = name.l;
	}
	Serial.print('=');
	Serial.println(out.data.l);
	return out;
}
void setter(VarData to, Token t){
	
	//Serial.print(t.data.l);
	//Serial.print(to.c[0]);
	//Serial.print(to.c[1]);
	//Serial.println(" 66666 ");
	char leg = to.c[1]-'0';
	pinMode(leg,OUTPUT);
	digitalWrite(leg, t.data.l == 0 ? LOW : HIGH);
}
Token doIt(long operation, Token *data, TOKEN_TYPES type){
	
	
	Token out;
	out.type = type;
	VarData tmp;
	tmp.l = operation;
	
	switch(tmp.i){
		case '+':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l + data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[1].data.f + data[0].data.f;
			break;
		case '-':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l - data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[1].data.f - data[0].data.f;
			break;
		case '*':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l * data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[1].data.f * data[0].data.f;
			break;
		case '%':
			out.type = TOKEN_NUMBER_INT;
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l % data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.l = (long)data[1].data.f % (long)data[0].data.f;
			break;
		case '<':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l < data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[1].data.f < data[0].data.f;
			break;
		case '>':
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l > data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.f = data[1].data.f > data[0].data.f;
			break;
		case '|'+('|'<<8):
		case 'o'+('r'<<8):
			out.type = TOKEN_NUMBER_INT;
			
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l || data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.l = (long)data[1].data.f || (long)data[0].data.f;
			break;
		case '&'+('&'<<8):
		case 'a'+('n'<<8)+('d'<<16):
			out.type = TOKEN_NUMBER_INT;
			
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l && data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.l = (long)data[1].data.f && (long)data[0].data.f;
			break;	
		case '='+('='<<8):	
			out.type = TOKEN_NUMBER_INT;
			
			if( type == TOKEN_NUMBER_INT )
				out.data.l = data[1].data.l == data[0].data.l;
			else if( type == TOKEN_NUMBER_FLOAT )
				out.data.l = fabs(data[1].data.f - data[0].data.f)<0.0000001;
			break;	
		case '=':
		    Serial.println("ROVNO");
			setter(data[1].data, data[0]);
			out.data = data[0].data;
			break;
		default:
			out.data.l = operation;
			Serial.println("Not implemented");
			Serial.println(out.data.c[0]);
			//Serial.println(operation);
			/*Serial.println(out.data.c[0]);
			Serial.println(out.data.c[1]);
			Serial.println(out.data.c[2]);
			Serial.println(out.data.c[3]);*/
			break;
	};
	;
	Serial.print(data[1].data.l);
	 Serial.print((char)operation);
	 Serial.print(data[0].data.l);
	 Serial.print('=');
	 Serial.println(out.data.l);
	return out;
}

void step(long operation, Token *stack, int *pointer){
	Token token[3];
	char count, wereFloats = false, wereInts = false;
	int i;
	//cout << (char)operation << '|';
	
	switch(operation){
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		case '=':
		case '='+('='<<8):
			count = 2;
		default:
			count = 2;
		
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
	(*pointer) -= count;
	if( wereInts && wereFloats ){
		for( i = 0; i < count; i++ ){
			if( token[i].type == TOKEN_NUMBER_INT )
				token[i].data.f = (float)token[i].data.l;
		}
	}
	
	stack[*pointer] = doIt(operation, token, wereFloats ? TOKEN_NUMBER_FLOAT : TOKEN_NUMBER_INT );
	 *pointer+=1;
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
			Serial.print( programP );
			Serial.print( ',' );
			Serial.print( i );
			Serial.print( ' ');
			for( int j = 0; j < stackP; j++ ){
				Serial.print( stack[j].data.l );
				Serial.print( '>' );
			}
			step(ptrProgram[i].data.l, stack, &stackP);			
		}		
	}
	
	
}
void run(char *expr){
	Serial.println(expr);
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
			for( ; p <= start+4; p++)
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
//	cout << '~' << stP <<'\n';
	while( stP > 0 ){
		--stP;
		program[programP].type = TOKEN_OPERATION;
		program[programP].data = stack[stP].token->data;
		
		++programP;
	}
	Serial.println("LEXER");
	
	

	
	Serial.println( programP );
	for( i = 0; i < programP; i++)
		if(program[i].type==TOKEN_EMPTY || program[i].type == TOKEN_VAR || program[i].type == TOKEN_OPERATION){
			Serial.print( (int)program[i].type );
			Serial.print('|');
			Serial.println(program[i].data.c);
		}else if(program[i].type == TOKEN_NUMBER_INT){
			Serial.print( (int)program[i].type );
			Serial.print('|');
			Serial.println((long)tokens[i].data.l);
			
		}else if(program[i].type == TOKEN_NUMBER_FLOAT){
			Serial.print( (int)program[i].type );
			Serial.print('|');
			Serial.println((float)program[i].data.f);
			
		//	cout << (int)tokens[i].type <<'|'<< (float)tokens[i].data.f << '\n';
		}
			
	
	
	evalute(program, programP);
	
}
void repeatMeSecond(){
	Serial.print("Seconds: ");
	
	seconds++;
	if(seconds==60)seconds = 0;
	Serial.println(seconds);
	proceed(cfg);
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
void loop()
{
  timer.run();    	
}
void readKB(){
	int i;
  int l, m = 0, test = 4;
  for(i = 0; i < 4; i++){
	  
	  m = 0;
	  int arr[4] = {0,0,0,0};
	  do{
		  digitalWrite(30+i*2,LOW);
		  delay(1);
		  l = digitalRead(38)*8+digitalRead(40)*4+digitalRead(42)*2+digitalRead(44);
		  
		  digitalWrite(30+i*2,HIGH);
		  delay(1);
		  l = l ^ digitalRead(38)*8+digitalRead(40)*4+digitalRead(42)*2+digitalRead(44);
		  digitalWrite(30+i*2,LOW);
		  if( l > 0 ){
			  arr[0] += l & 1;
			  arr[1] += l & 2;
			  arr[2] += l & 4;
			  arr[3] += l & 8;
			  m++;
			  delay(1);
			  if(m == test)
				break;
		  }else
			break;
			
	  }while(1);
	  Serial.println( 16+
	    (arr[0]==test)+
		(arr[1]==test)*2+
		(arr[2]==test)*4+
		(arr[3]==test)*8,BIN);
	  delay(1);
	  
  }
  
  delay(300);
  
  
  //proceed(cfg);
  /*
  if((seconds % 2) == 0)
    digitalWrite(5, HIGH);
  else
	digitalWrite(5, LOW);*/
	
}
