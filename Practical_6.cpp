//rdp
#include <bits/stdc++.h>
#define ll long long int
#define li long int
#define pb push_back
#define all(x) x.begin(), x.end()
#define Y cout<<"Valid String\n"
#define N cout<<"Invalid String\n"
using namespace std;
void S();
void L();
void Ldsh();
// S → ( L ) | a
// L → S L’
// L’ → , S L’ | ϵ
string s;
int i=0;
bool fl=0;
void S(){
    if(s[i]=='('){
        i++;
        L();
        if(s[i]==')'){
            i++;
            return;
        }
        else{
            // cout<<"he";
            fl=1;
        }
    }
    else if(s[i]=='a'){
        i++;
        return;
    }
    else{
        // cout<<"he1";
        fl=1;
    }
}
void L(){
    S();
    Ldsh();
}
void Ldsh(){
    if(s[i]==','){
        i++;
        S();
        Ldsh();
    }
    else{
        return;
    }
}
void code()
{
    cout<<"Enter a string: ";
    cin>>s;
    S(); //start sym.
    if(!fl && i==s.size()) Y;
    else N;
    i=0;
    fl=0;
    return;
}
int main()
{
    ios_base::sync_with_stdio(false);
    // cin.tie(nullptr);
    li t = 1;
    // cin >> t;
    while (t--)
    {
        code();
    }
    return 0;
}