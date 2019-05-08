

/**
 * A series of tests, specifically targeting function calls
 * and definitions. We will test calling both self-defined
 * functions and C standard library functions.
 */


char g() {
    char a;
    a = 'a';
    return a;
}


int main() {

    // defined function
    if (g() == 'a')
        printf("T2: test 1 passed\n");
    else
        printf("T2: test 1 failed\n");
    
    // c standard library functions
    char *p;
    p = malloc(sizeof(char)*15);
    strcpy(p, "hello world\n");

    if (!strcmp(p, "hello world\n"))
        printf("T2: test 2 passed\n");
    else
        printf("T2: test 2 failed\n");
}
