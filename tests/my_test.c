

int a,i,j;

int main()
{
    int i, a;
    i=1;
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
}
