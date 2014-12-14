
enum OPERATION_PRIORITY {
	PRIORITY_EQUAL = 3,
	PRIORITY_LOGIC = 5,
	PRIORITY_COMPARE = 4,
	PRIORITY_PLUS_MINUS = 6,
	PRIORITY_MUL_DIV = 7,
	PRIORITY_OPEN_PARENTHESIS = 1,
	PRIORITY_CLOSE_PARENTHESIS = 0
};
union VarData{
	long l;
	float f;
	char c[4];
	int i;
} ;
enum TOKEN_TYPES{
	TOKEN_VAR,
	TOKEN_NUMBER_INT,
	TOKEN_OPERATION,
	TOKEN_EMPTY,
	TOKEN_NUMBER_FLOAT
};
typedef struct Token{
	VarData data;
	TOKEN_TYPES type;
};
struct Operation{
	Token* token;
	char priority;
};
