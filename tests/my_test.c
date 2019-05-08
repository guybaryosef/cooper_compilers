char g() {
    char a;
    a = 'a';
    return a;
}

int main()
{
    // defined function
    if (g() == 'a')
        printf("test 1 passed\n");
    else
        printf("test 1 failed\n");

}
