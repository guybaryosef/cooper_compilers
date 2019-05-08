
/**
 * A series of tests, specifically targeting if-else
 * conditional statements as well as loops. These two
 * constructs together for the bulk of the comparison
 * jump op codes in assembly language, so in essence
 * we are testing to make sure that these opcodes 
 * work as intended. 
 */
int main() {
    int a,i;
    a = 0;
    i = 1;


    if (a < i)
        printf("T1: test 1 passed\n");
    

    // if else stmt
    if (4 > 7)
        printf("T1: test 2 failed\n");
    else
        printf("T1: test 2 passed\n");
    

    // if else(if) stmt
    if (!i) {
        printf("T1: test 3 failed\n");
    }
    else {
        if ( a < i)
            printf("T1: test 3 passed\n");
    }


    // if else(if else) stmt
    if (a) {
        printf("T1: test 4 failed\n");
    }
    else {
        if (!i)
            printf("T1: test 4 failed\n");
        else
            printf("T1: test 4 passed\n");
    }


    // if else(if else(if)) stmt
    if (1) {
        if ( a < -2) {
            printf("T1: test 5 failed\n");
        }
        else {
            if (i > 0)
                printf("T1: test 5 passed\n");
            else
                printf("T1: test 5 failed\n");
        }
    }
    else
        printf("T1: test 5 failed\n");


    // if then(for loop) stmt
    if (i) {
        i = 0;
        for (a=0; a < 10; ++a)
            i+=2;

        if (i == 20)
            printf("T1: test 6 passed\n");
        else
            printf("T1: test 6 failed\n");
    }
    else
        printf("T1: test 6 failed\n");


    // for loop (if stmt)
    for (i=0; i<10; ++i) {
        if (i<4) {
            continue;
        }
        else {
            a = 17;
            break;
        }
    }
    if (i!=4 || a!= 17)
        printf("T1: test 7 failed\n");
    else
        printf("T1: test 7 passed\n");
        

    // for loop (if else stmt)
    a=0;
    for (i=0; i<10; ++i) {
        if (i < 5)
            a+=2;
        else
            a+=1;
    }
    if (i!=10 || a!=15)
        printf("T1: test 8 failed\n");
    else
        printf("T1: test 8 passed\n");


    // while loop(if stmt)
    i=0;
    a=0;
    while(i<10) {
        if (i<5)
            a+=2;
        else
            a+=1;
        ++i;
    }
    if (i!=10 || a!=15)
        printf("T1: test 9 failed\n");
    else
        printf("T1: test 9 passed\n");
}