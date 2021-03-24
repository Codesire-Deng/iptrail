#include <iostream>
using namespace std;

int main() {
    char a[50];
    memset(a, 'a', sizeof(a));
    for (int i=0; i<10; ++i)
        printf("%d ", a[i]);
    sprintf_s(a, "xx");
    for (int i=0; i<10; ++i)
        printf("%d ", a[i]);
    cout << endl;
    return 0;
}