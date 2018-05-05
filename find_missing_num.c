/* strtok example */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int find_missing_num(char* str_numbers) {
      printf ("Splitting string \"%s\" into tokens:\n",str_numbers);
      str_numbers = strtok (str_numbers,",");
	int sum = 0;
      while (str_numbers != NULL)
      {
	sum += atoi(str_numbers);
      
        printf ("%s\n",str_numbers);
        str_numbers = strtok (NULL, ",");
	
	}

	return (45 - sum);
}

int main ()
{
  char str[] ="4,9,2,3,8,1,5,7";
  
	int num = find_missing_num(str);
	printf(" missing number = %d ", num);

  return 0;
}
