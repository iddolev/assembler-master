#include <string.h>
#include "defs.h"

void op_type_to_string(OP_TYPE op_type, char buffer[])
{
	char *s;

	switch(op_type)
	{
		case NOT:
			s = "NOT";
			break;	
		case NEW_LINE:
			s = "NEW_LINE";
			break;	
		case SYMBOL:
			s = "SYMBOL";
			break;	
		case METHOD:
			s = "METHOD";
			break;	
		case OPERANT:
			s = "OPERANT";
			break;	
		case REGISTER:
			s = "REGISTER";
			break;	
		case ERROR:
			s = "ERROR";
			break;	
		case COMMENT:
			s = "COMMENT";
			break;	
		default:
			s = "ILLEGAL!!!";
			break;	

	};
	strcpy(buffer, s);
}
