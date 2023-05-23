#include <stdio.h>


int main(int argc, char **argv){
    int counter = 0;
    int i = 0;
    char* str = argv[1];
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