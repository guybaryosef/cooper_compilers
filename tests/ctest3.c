
/**
 * A series of tests, specifically targeting pointer
 * arithmetic, arrays, and expressions.
 */

int *p;
int a[10];

int main() {

    // some array indexing with some for loop action
    int i,j;
    for (i = 0; i < 10; ++i) {
        if (i==0) {
            a[i] = 0;
            continue;
        }
        else {
            a[i] = 2;
            for(j=1;j<i; ++j)
                a[i] *= 2;
        }
    }
    if (a[0]==0 && a[1]==2 && a[2]==4 && a[3]==8 && a[4]==16 && a[5]==32 && a[6]==64)
        printf("T3: test 1 passed\n");
    else
        printf("T3: test 1 failed\n");


    // address of operator and dereferencing
    p = &i;
    if (*p == i)
        printf("T3: test 2 passed\n");
    else
        printf("T3: test 2 failed\n");   

    // a sizeof test
    if (sizeof(char *) == sizeof(int *) && sizeof(int *) == sizeof(void *) && sizeof(char *) == 4)
        printf("T3: test 3 passed\n");
    else
        printf("T3: test 3 failed\n");


    // pointer arithmetic 
    int *p2;
    p = a;
    p2 = &(*a)+ 3*sizeof(char *);
    // printf("%ld\n", &a[3]);
    // printf("%ld\n", &p[3]);
    // printf("%ld\n", p2);
    if (&a[3] == p2 && &p[3] == p2)
        printf("T3: test 4 passed\n");
    else
        printf("T3: test 4 failed\n");

}