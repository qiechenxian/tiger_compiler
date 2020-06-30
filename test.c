const int a = 00007;
const int c = 0x001a2B3C4D5E6F;

int test(int a, int b[][a*10]){
    b[1][1] = 1;
    return 1;
}

int main(){
    int d[4][4] = {1,2,3,{{}}};
    int _a = 10;
    d[1][1] = d[1][2];
    test(1);
    return 1;
}

