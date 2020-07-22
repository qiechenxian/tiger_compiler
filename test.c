int a;
int b = 1;
int main(){
    b = a;
    while (a){
        int b = 2;
        a = a + b;
    }
    return 0;
}