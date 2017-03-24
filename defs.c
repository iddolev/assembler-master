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
		case SYMBOL:
			s = "SYMBOL";
			break;	
		case ERROR:
			s = "ERROR";
			break;	
		default:
			s = "ILLEGAL OP_TYPE!";
			break;	

	};
	strcpy(buffer, s);
}
