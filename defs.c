#include <string.h>
#include "defs.h"

char * op_type_to_string(OP_TYPE op_type)
{
	switch(op_type)
	{
		case NOT:
			return "NOT";
		case SYMBOL:
			return "SYMBOL";
		case ERROR:
			return "ERROR";

	};
	return "ILLEGAL OP_TYPE!";
}

