/* getenv()
       The  getenv()  function searches the environment list to find the envi‐
       ronment variable name, and returns a pointer to the corresponding value
       string.
*/

/* gethostname()

	#include <unistd.h>

       int gethostname(char *name, size_t len);

       gethostname() returns the null-terminated hostname in the character ar‐
       ray name, which has a length of  len  bytes.   If  the  null-terminated
       hostname  is too large to fit, then the name is truncated, and no error
       is returned (but see NOTES below).  POSIX.1 says that if  such  trunca‐
       tion  occurs,  then  it  is unspecified whether the returned buffer in‐
       cludes a terminating null byte.
*/

/* char * readline (const char *prompt

	Readline is Copyright (C) 1989-2020 Free Software Foundation,  Inc.

       readline will read a line from the terminal and return it, using prompt
       as a prompt.  If prompt is NULL or the empty string, no prompt  is  is‐
       sued.   The  line returned is allocated with malloc(3); the caller must
       free it when finished.  The line returned has  the  final  newline  re‐
       moved, so only the text of the line remains.
*/

#include <unistd.h>

int main(void)
{
	printf("%s", USER);
	while(1){
		// user@machine $ / #
		//printf("%d@
	}
	
	return 0;
}
