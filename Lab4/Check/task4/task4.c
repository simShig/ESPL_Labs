#include <stdio.h>


void digit_count(char* input){
    int counter = 0;
    int i = 0;
    char* str = input;
    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            counter++;
        }
        i++;
    }                   
    printf("%d\n",counter);
    return 0;
}

int main(int argc, char **argv){
    digit_count(argv[1]);
}