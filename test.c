int FourWhile() {
    int a;
    a = 5;
    int b;
    int c;
    b = 6;
    c = 7;
    int d;
    d = 10;
    while (a < 20) {
        a = a + 3;
        while(b < 10){
            b = b + 1;
            if(b==10)
            {
                continue;
            }
            while(c == 7){
                c = c - 1;
                while(d < 20){
                    d = d + 3;
                    break;
                }
                d = d - 1;
                break;
            }
            c = c + 1;
        }
        b = b - 2;
    }

    return (a + (b + d) + c);
}

int main() {
    return FourWhile();
}

