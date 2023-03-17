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
  int menu_input;
  struct fun_desc menu[] = { 
    { "Get string", my_get }, 
    { "Print string", cprt }, 
    { "Print hexa", xprt }, 
    { "Encrypt", encrypt }, 
    { "Decrypt", decrypt }, 
    { NULL, NULL }
  };
  // for (size_t i = 0; i < 5; i++){
  //   printf("%02X\n", (unsigned char)carrey[i]); //for debug
  // }
  int bound = sizeof(menu)/8-2; //lower bound - 1,upper bound - 6
  // printf ("%i\n",bound);
  int flag = 1;   //all flags to deal with double print of menu because of 0A garbage values
  
  while (1) {
    /* print menu iteritavly:*/
    if (flag !=0){
      printf("Select operation from the following menu:\n");
      for (int i = 0; i < sizeof(menu)/8-1; i++)
      {
        printf( "%i)  %s\n", i,menu[i].name);
      }
    }
    flag = 1; 

    // /* recieve user input:*/
    char input[256];
    if (fgets(input, 256, stdin) == NULL) {
      break; // if EOF - exit while loop
    }
    menu_input = input[0];
    printf("input is:%i\n",(int)menu_input);
    if (menu_input==10) flag = 0;   //ascii - hxa garbage 0A
    // if (menu_input==EOF) break;

    menu_input=menu_input-48;
    if (menu_input>=0&&menu_input<=bound)
    {
      printf ("within bounds!\n");
      /* code */
    }else {
      printf("not within bounds!\n");
      break;
    }
    
  /* T3b - 5:*/
    carrey = map(carrey,5,menu[menu_input].fun);
   
    }

  return 0;

}

