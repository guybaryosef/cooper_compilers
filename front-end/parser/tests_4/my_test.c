
int g();

int d() {
    int i; 
    ++i;
}

int z() {
    d();
    g();
    z();
    s();
}