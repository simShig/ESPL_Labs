#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* copied from task 2 "base.c":*/

  char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
   /* TODO: Complete during task 2.a */

  for(int i=0; i<array_length; i++) {
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}

char my_get(char c) {
  return fgetc(stdin);
}


char cprt(char c) {
  if (c >= 0x20 && c <= 0x7E) {
    printf("%c\n", c);
  } else {
    printf(".\n");
  }
  return c;
}


char encrypt(char c) {
  if (c >= 0x20 && c <= 0x7E) {
    c = c + 1;
  }
  return c;
}

char decrypt(char c) {
  if (c >= 0x20 && c <= 0x7E) { //if its symetric decryption - it should be until 0x7E+1 (=0x7F)
    c = c - 1;
  }
  return c;
}

char xprt(char c) {
  printf("%02X\n", (unsigned char)c);
  return c;
}

struct fun_desc { 
  char *name; 
  char (*fun)(char); 
  };


int main(int argc, char **argv){
  char* carrey = (char*)(malloc(40));

  struct fun_desc menu[] = { 
    { "get string", my_get }, 
    { "print string", cprt }, 
    { "print hexa", xprt }, 
    { "encrypt", encrypt }, 
    { "decrypt", decrypt }, 
    { "exit", NULL }, 
    { NULL, NULL }
  };
  // for (size_t i = 0; i < 5; i++){
  //   printf("%02X\n", (unsigned char)carrey[i]); //for debug
  // }
  int bound = sizeof(menu)/8-1; //lower bound - 1,upper bound - 6
  printf ("%i\n",bound);

  while (1) {
    /* print menu iteritavly:*/
    printf("Select operation from the following menu:\n");
    for (int i = 0; i < sizeof(menu)/8-1; i++)
    {
      printf( "%i. %s\n", i+1,menu[i].name);
    }

    /* recieve user input:*/
    int menu_input = (int)fgetc(stdin);
    printf("%f\n \n", menu_input);
   
    if (menu_input==EOF) break;
    if (menu_input>=1&&menu_input<=bound)
    {
      printf ("within bounds!\n");
      /* code */
    }else printf("not within bounds!\n");
    

//     else{
//         printf("Invalid choice. Please enter a number between 1 and 6.\n");
//         break;
    }

  return 0;

}

