#include <bits/stdc++.h>
using namespace std;

long long int power(long long int x, long long int y, long long int p)
{

    long long int res = 1;

    x = x % p;

    while (y > 0)
    {

        if (y & 1)
            res = (res * x) % p;

        y = y >> 1;
        x = (x * x) % p;
    }
    return res;
}

int findKey(long long int g, long long int B, long long int p)
{

    int m = (int)sqrt(p) + 1;

    unordered_map<long long int, long long int> value;

    for (int i = m; i >= 1; --i)
        value[power(g, i * m, p)] = i;

    for (int j = 0; j < m; ++j)
    {

        long long int cur = (power(g, j, p) * B) % p;

        if (value[cur])
        {
            int ans = value[cur] * m - j;
            if (ans < p)
                return ans;
        }
    }
    return -1;
}

int main()
{

    long long int g = 12;
    long long int n = 97;
    long long int x = 13;
    long long int y = 71;

    long long int A = power(g, x, n); // g^x mod n
    cout << A << endl;

    long long int B = power(g, y, n); // g^y mod n
    cout << B << endl;

    long long int key = power(B, x, n);

    cout << key << endl;

    int newy = findKey(g, B, n);
    long long int newkey = power(A, newy, n);

    cout << newkey << endl;
}