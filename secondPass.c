/* I just started writing the pseodocode */

void secondPass()
{
  int lineNum = 0, len;
  char * linep;
  IC = 0
  
  while (linep = fgets(line, MAX_LINE+1, fd))
	{
    		lineNum++;
		len = strlen(line);
		if (line[len-1] == EOF) /* so we don't have to deal with EOF all the time */
		{
			line[len-1] = '\n';
		}
		
		linep = getNextToken(linep, nextWord);
		
		if (!linep)
			continue;
		
		if (nextWord[0] ==';')
			continue; /*need to ignore comments or blank lines*/
		
		type = is_label(nextWord, lineNum);
    
		if(type == SYMBOL && *linep == ':') /*maybe need to check if isSymbol == error*/
		{
      			linep = getNextToken(linep, nextWord);
    		}
    		if (!linep)
			continue;
      
		type = is_instruction(linep, lineNum);
    
	    	if (type == DATA || type == STRING)
		{
	      		continue;
	   	 }

	    	if (type == EXTERN || TYPE == ENTRY)
		{
			if (type == EXTERN)
			{
				/*do stuff for extern*/
			}
		      	else
		      	{
				/*do stuff for entery*/
		      	}
		      	continue;
		 }

		    /* do lines 7-9 in the pseodocode*/
	}
  /* do line 11 in the pseodocode */
}
