#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int addr5;
int addr6;

int foo()
{
    return -1;
}
void point_at(void *p);
void foo1();
char g = 'g';
void foo2();

int main(int argc, char **argv)
{ 
    int addr2;
    int addr3;
    char *yos = "ree";
    int *addr4 = (int *)(malloc(50));
    
    printf("Print addresses:\n");
    printf("local vars on stack:\n");   //i added, not sure if true...
    printf("- &addr2: %p\n", &addr2);
    printf("- &addr3: %p\n", &addr3);

    printf("global func - defined on text-segment?:\n");   //i added, not sure if true...
    printf("- foo: %p\n", &foo);

    printf("global vars - defined on text-segment?:\n");   //i added, not sure if true...
    printf("- &addr5: %p\n", &addr5);

    printf("funcs' argument - defined on stack?:\n");   //i added, not sure if true...
    printf("- argc %p\n", &argc);
    printf("- argv %p\n", argv);

    printf("funcs' argument- pointer which is first cell of arrey - defined on heap?:\n");   //i added, not sure if true...
    printf("- &argv %p\n", &argv);
    
    printf("Print distances:\n");
    point_at(&addr5);


    printf("\nlong integer should be enough to represent the address - 4bytes=32bits as needed for\n");   //i added, not sure if true...

    printf("Print more addresses:\n");
    printf("- &addr6: %p\n", &addr6);
    printf("- yos: %p\n", yos);
    printf("- gg: %p\n", &g);
    printf("- addr4: %p\n", addr4);
    printf("- &addr4: %p\n", &addr4);

    printf("- &foo1: %p\n", &foo1);
    printf("- &foo1: %p\n", &foo2);
    
    printf("Print another distance:\n");
    printf("- &foo2 - &foo1: %ld\n", (long) (&foo2 - &foo1));

   
    printf("Arrays Mem Layout (T1b):\n");
    int iarray[3];
    float farray[3];
    double darray[3];
    char carray[3]; 
    printf("- &iarrey:   %p\n- &iarray+1: %p\n- dist: %ld\n \n", &iarray, &iarray+1,(size_t)&iarray - (size_t)(&iarray+1));
    printf("- &farrey:   %p\n- &farray+1: %p\n- dist: %ld\n \n", &farray, &farray+1,(size_t)&farray - (size_t)(&farray+1));
    printf("- &darrey:   %p\n- &darray+1: %p\n- dist: %ld\n \n", &darray, &darray+1,(size_t)&darray - (size_t)(&darray+1));
    printf("- &carrey:   %p\n- &carray+1: %p\n- dist: %ld\n \n", &carray, &carray+1,(size_t)&carray - (size_t)(&carray+1));
//conclusion:
    printf("we can notice that the +1 moves to next \"free\" cell (after the arrey) neither to the next cell\n \n");



    /* task 1 b here */
    
    printf("Pointers and arrays (T1d): ");
    int iarray2[] = {1,2,3};
    char carray2[] = {'a','b','c'};
    int* iarray2Ptr = iarray2;
    char* carray2Ptr = carray2;
    int* p;
    /* task 1 d here */

  printf("Values of iarray2 using iarray2Ptr:\n");
  for(int i = 0; i < 3; i++) {
    printf("%d\n", *(iarray2Ptr+i));
  }

  printf("Values of carray2 using carray2Ptr:\n");
  for(int i = 0; i < 3; i++) {
    printf("%c\n", *(carray2Ptr+i));
  }

  printf("uninitialized pointer p: %p\n \n", p);        //we notice it is nullptr


    
    printf("Command line arg addresses (T1e):\n");
    /* task 1 e here */

    printf("argc = %d\n", argc);
    printf("&argc = %p\n", (void*)&argc);
    printf("&argv = %p\n", (void*)&argv);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %p, %s\n", i, (void*)&argv[i], argv[i]);
    }

    return 0;
}

//we can see each arg is 4 bytes and its on the stack.

void point_at(void *p)
{
    int local;          //text segment? (because is part of the code)
    static int addr0 = 2;   //BSS? because initialized
    static int addr1;   //BSS? because global (static)
    //p is a global var, here passed on as a pointer (argument on stack)
    long dist1 = (size_t)&addr6 - (size_t)p;    //dist between two global vars on the stack (p holds the adress of the argument, therefore the dist is same as checked before (between addr5 and addr6))
    long dist2 = (size_t)&local - (size_t)p;        //dist between var on text-segment (local) to var on stack (p)
    long dist3 = (size_t)&foo - (size_t)p;      //dist between func on text-segment (foo) to car on stack (p)
    // something doesnt adds up with the actual distance between "foo" and "p" - perhaps foo is not at textsegment but on heap?
    printf("\ndist between two global vars:\n");
    printf("- dist1: (size_t)&addr6 - (size_t)p: %ld\n", dist1);

    printf("dist between stack -local var-  and text-segment -  global var:\n");

    printf("- dist2: (size_t)&local - (size_t)p: %ld\n", dist2);

    printf("dist between textsegment -global func-  and text-segment -  global var:\n");

    printf("- dist3: (size_t)&foo - (size_t)p:  %ld\n", dist3);
    
    printf("Check long type mem size (T1a):\n");
    /* part of task 1 a here */

    printf("- addr0: %p\n", &addr0);
    printf("- addr1: %p\n", &addr1);
    printf("- dist: %ld\n \n", (size_t)&addr1-(size_t)&addr0);

}

void foo1()
{
    printf("foo1\n");
}

void foo2()
{
    printf("foo2\n");
}
