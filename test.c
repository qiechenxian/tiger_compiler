
int func(int p,int m,int n){
    p=5;
    m=4;
    n=3;
    return p+m+n;
}
int main(){
    int a;
    int b;
    int c;
    func(a,b,c);
    a=b;
    return a;
}
