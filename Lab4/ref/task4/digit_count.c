#include <stdio.h>
int digit_count(char* str){
    int counter = 0;
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            counter++;
        }
        i++;
    }
    return counter;
}

int main(int argc, char **argv){
    printf("%d\n",digit_count(argv[1]));
    return 0;
}