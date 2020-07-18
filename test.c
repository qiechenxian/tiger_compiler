const int arr[2][2] = {{1,2},{3,4}};
int main (){
    const int a = arr[0][0] + arr[0][1] + arr[1][1];
    return a + arr[1][0];
}