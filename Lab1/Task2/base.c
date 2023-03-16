#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
   /* TODO: Complete during task 2.a */

  for(int i=0; i<array_length; i++) {
    mapped_array[i] = f(array[i]);
  }
  return mapped_array;
}

/* T2b:*/
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

int main(int argc, char **argv){
   /* TODO: Test your code - example copied from the instuctions*/
  int base_len = 5;
  char arr1[base_len];
  
  char* arr2 = map(arr1, base_len, my_get); 
  printf("\n");
  

  char* arr3 = map(arr2, base_len, cprt); 
  printf("\n");
  
  char* arr4 = map(arr3, base_len, xprt); 
  printf("\n");
  
  char* arr5 = map(arr4, base_len, encrypt); 
  printf("\n");
  
  char* arr6 = map(arr5, base_len, decrypt); 
  printf("\n");
  
  free(arr2); 
  free(arr3); 
  free(arr4); 
  free(arr5); 
  free(arr6);
}


 