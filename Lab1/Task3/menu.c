#include <stdio.h>

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

int main(int argc, char **argv){

 while (1) {
    printf("Select operation from the following menu:\n");
    printf("1. Get String\n");
    printf("2. Print String (crpt)\n");
    printf("3. Print Hex (xprt)\n");
    printf("4. Encrypt\n");
    printf("5. Decrypt\n");
    printf("6. Exit\n");
    
    
    char input[256];
    if (fgets(input, 256, stdin) == NULL) {
      break; // if EOF - exit loop
    }
    int choice = atoi(input); // convert input to integer

    if (input=='1'){
        char* str = get_string();
        printf("You entered: %s\n", str);
        free(str);
        break;
    }
    else if (input=='2'){
        char* str = get_string();
        char* mapped_str = map(str, strlen(str), &cprt);
        printf("Mapped string: %s\n", mapped_str);
        free(str);
        free(mapped_str);
        break;
    }
    else if (input=='3'){
        char* str = get_string();
        char* mapped_str = map(str, strlen(str), &xprt);
        printf("Mapped string: %s\n", mapped_str);
        free(str);
        free(mapped_str);
        break;
    }
    else if (input=='4'){
        char* str = get_string();
        char* mapped_str = map(str, strlen(str), &encrypt);
        printf("Mapped string: %s\n", mapped_str);
        free(str);
        free(mapped_str);
        break;
    }
    else if (input=='5'){
        char* str = get_string();
        char* mapped_str = map(str, strlen(str), &decrypt);
        printf("Mapped string: %s\n", mapped_str);
        free(str);
        free(mapped_str);
        break;
    }
    else if (input=='6'){
        printf("Exiting program.\n");
        return 0;
    }
    else{
        printf("Invalid choice. Please enter a number between 1 and 6.\n");
        break;
    }

  return 0;
}
}

