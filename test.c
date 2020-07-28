int test(int a,int b,int c,int d,int e)
{
    a=a+b+c+d+e;
    return a;
}
int main(){
    int a=5;
    int b=12;
    a=b/a;
    test(a,b,b,a,a);
    putf("this is a string","this is a string","this is a string","this is a string","this is a string","this is a string");
    test(a,b,b,a,a);
    return 0;
}