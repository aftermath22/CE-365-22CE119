#include <bits/stdc++.h>
#define ll long long int
#define li long int
#define pb push_back
#define all(x) x.begin(), x.end()
#define Y cout << "Valid String\n"
#define N cout << "Invalid String\n"
using namespace std;

int charCheck(char c) // returns if a given char is a digit(1),alphabet(0) or special(2)
{
    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
        return 0;
    else if (c >= 48 && c <= 57)
        return 1;
    else
        return 2;
}

void err() // func. for common error msg
{
    cout << "Invalid format of transition input!\n";
    exit(1);
}

bool checkForInputString(unordered_map<char, bool> &m, string s) // checks if the input string is made solely of input alphabets or not
{
    if (s.empty())
        return 1;
    for (char x : s)
    {
        if (!m[x])
            return 0;
    }
    return 1;
}

void solve()
{
    //------------------------------------input start-------------------------------------------------
    ll n;
    cout << "Number of input symbols : ";
    cin >> n;
    unordered_map<char, bool> ip_symbols(n);
    for (int i = 0; i < n; i++)
    {
        char x;
        cin >> x;
        ip_symbols[x] = 1;
    }
    ll n_st;
    cout << "Enter number of states : ";
    cin >> n_st;
    ll ini_st;
    cout << "Initial state : ";
    cin >> ini_st;
    if (ini_st < 1 && ini_st > n_st)
    {
        cout << "Entered an Invalid Initial State!\n";
        exit(1);
    }
    ll n_ac;
    cout << "Number of accepting states : ";
    cin >> n_ac;
    if (n_ac < 1 && n_ac > n_st)
    {
        cout << "Entered an Invalid number of Accepting State(s)!\n";
        exit(1);
    }
    map<ll, bool> ac_states;
    for (int i = 0; i < n_ac; i++)
    {
        ll x;
        cin >> x;
        ac_states[x] = 1;
    }
    cout << "Transition table :" << endl;

    vector<string> lines; // to store all the remaining inputs 1 string per line
    string temp;
    while (getline(cin >> ws, temp))
    {
        if (temp.empty())
            break;
        lines.pb(temp);
    }

    int size = (int)lines.size();
    string inp = lines[size - 1];
    if (inp.find("->") != std::string::npos) //checks if "->" exists in the strings indicating there is no input string(i.e. empty string)
    //"->" exists
        inp = "";
    else
        size--;	//input string is present so visit till the second last index of 'lines' in order to get the transitions

    vector<pair<ll, pair<ll, char>>> transitions;
    char delimiter = ' '; //seperation by whitespace
    string t;
    int j = 0;
    while (j < size)
    {
        if (temp.empty())
            break;
        stringstream ss(lines[j++]); // passing every line and delimit on every whitspace
        ll num1, num2;
        char c;
        int fl = 0;
        while (getline(ss, t, delimiter))
        {
            if (!fl)
            {
                for (char x : t)
                {
                    int status = charCheck(x);
                    if (status == 0 || status == 2)
                        err();
                }
                num1 = stoi(t); // converting string to integer
                fl++;
            }
            else if (fl == 1)
            {
                if (t != "to")
                    err();
                fl++;
            }
            else if (fl == 2)
            {
                if (t.size() != 1)
                    err();
                c = t[0];
                if (!ip_symbols[c])
                {
                    cout << "Entered an Invalid Input symbol!";
                    exit(1);
                }
                fl++;
            }
            else if (fl == 3)
            {
                if (t != "->")
                    err();
                fl++;
            }
            else
            {
                for (char x : t)
                {
                    int status = charCheck(x);
                    if (status == 0 || status == 2)
                        err();
                }
                num2 = stoi(t); // converting string to integer
                break;
            }
        }
        transitions.pb({num1, {num2, c}}); // stateA --(INPUT SYMBOL)--> stateB
    }

    if (transitions.size() == 0)
    {
        cout << "NO DFA found!\n";
        return;
    }
    //------------------------------------input finish------------------------------------------------

    //------------------------------------making dfa--------------------------------------------------
    map<ll, bool> null_states; // for storing null states

    map<ll, map<char, ll>> dfa;
    // state1 ---{any input symbol}--> state2
    //        ---{any input symbol}--> state3
    //        ...
    //        and so on for other states...

    for (auto &x : transitions)
    {
        // cout<<x.first<<" : "<<x.second.first<<" : "<<x.second.second<<endl;
        if (dfa[x.first][x.second.second] != 0)
        {
            cout << "Not a valid DFA!\n";
            exit(1);
        }
        else
        {
            dfa[x.first][x.second.second] = x.second.first;
        }
    }
    for (auto &x : dfa)
    {
        bool ok = 0;
        map<char, ll> temp_map = x.second;
        for (auto &y : temp_map)
        {
            if (x.first != y.second)
            {
                ok = 1;
                break;
            }
        }
        if (!ok)
        {
            null_states[x.first] = 1; // finding out null states in dfa
        }
    }

    //------------------------------------dfa made----------------------------------------------------

    //------------------------------------string validation-------------------------------------------

    if (!checkForInputString(ip_symbols, inp))
    {
        cout << "Entered input string with invalid input alphabet(s)!\n";
        return;
    }
    cout << "Input String : " << inp << endl;
    ll c_st = ini_st;
    for (char &x : inp)
    {
        if (null_states[c_st])
        {
            cout << "Went into a null state\n";
            N;
            return;
        }
        if (dfa[c_st][x] == 0)
        {
            N;
            return;
        }
        c_st = dfa[c_st][x];
    }

    if (ac_states[c_st])
        Y;
    else
        N;
    //------------------------------------validation completed----------------------------------------
    return;
}
int main()
{
    // ios_base::sync_with_stdio(false);
    // cin.tie(nullptr);
    li tc = 1;
    while (tc--)
    {
        solve();
    }
    printf("This code is performed by 22CE119-Keval Shah\n");
    return 0;
}