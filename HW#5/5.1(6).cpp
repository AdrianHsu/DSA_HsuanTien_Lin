#include <iostream>
#include <string>
using namespace std;
typedef unsigned long long int longint;

longint _hash(string s) //BONUS
{
    longint out = 0;
    for(int i = 0; i < s.size(); i++)
    {
        out *= 27;
        out += s[i] - 'a' + 1; //hash("a") == 1
    }
    return out % 32;
}

// consider hash("register"):
// 190329075127, still in the range of longint
// consider hash("volatile"):
// 236112196676, still in the range of longint
int main()
{
    string s;
    
    while(cin >> s)
        cout << _hash(s) << endl;

}

