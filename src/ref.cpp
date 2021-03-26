#include <iostream>
#include <cstring>
using namespace std;

int main() {
    char st[10];
    memset(st, 0xff, sizeof(st));
    sprintf(st,"abc");
    cout << st << endl;
    for (char c : st)
        cout << int(c) << ' ';
    cout << endl;

    return 0;
}