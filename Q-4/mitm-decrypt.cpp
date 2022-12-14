#include<bits/stdc++.h>
using namespace std;
long long int g, A, B, x, a;
string key;
vector<int> RC4(vector<int> &ip, vector<int>&K, int &K_len)
{
    int S[256], T[256];
    // Initialization
    int i;
    for(i = 0; i < 256; i++){
        S[i] = i;
        T[i] = K[i%K_len];
    }
    int j = 0;
    for(i = 0;i < 256; i++){
        j = (j + S[i] + T[i])%256;
        int temp = S[i];
        S[i]  = S[j];
        S[j] = temp;
    }

    // Stream generation
    i = 0, j = 0;
    vector<int>K1;
    for(int t = 0; t < ip.size(); t++){
        i = (i + 1)%256;
        j = (j + S[i])%256;
        int temp = S[i];
        S[i] = S[j];
        S[j] = temp;
        temp = (S[i] + S[j])%256;
        K1.push_back(S[temp]);
    }
    // cout << "\n";s
    // Encrypting using XOR
    vector<int>op;
    for(int i=0;i<ip.size();i++){
        int p = K1[i]^ip[i];
        op.push_back(p);
    }
    return op;
}
string RC4_Cipher(string& buf)
{
	vector<int>ip, K;
	int K_len, i;
	for(int i = 0; buf[i] != '\0'; i++){
		ip.push_back((int)buf[i]);
	}
	for(int i = 0; i < key.size(); i++){
		K.push_back((int)key[i]);
	}
	K_len = K.size();
	vector<int>op = RC4(ip, K, K_len);
    string ans;
	for(i = 0; i<op.size(); i++){
        ans.push_back(op[i]);
		// buf[i] = (char)op[i];
	}
	// buf[i] = '\0';
    buf = ans;
    return ans;
}
vector<string> removeWhiteSpaces(string &s)
{
    vector<string>ans;
    string temp;
    for(int i = 0; i < s.size(); i++){
        if(s[i] == ' ') {
            if(temp != ""){
                ans.push_back(temp);
                temp = "";
            }
        }
        else temp.push_back(s[i]); 
    }
    if(temp != "") ans.push_back(temp);
    return ans;
}
long long int power(long long int B, long long int x, long long int a)
{
	long long int res = 1;
	
	    // Check till the number becomes zero
	    while (x > 0) {
		
	        // If y is odd, multiply x with result
	        if (x % 2 == 1)
	            res = (res * B) % a;
	
	        // y = y/2
	        x = x >> 1;
	
	        // Change x to x^2
	        B = (B * B) % a;
	    }
	    return res % a;
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
void skipLines(fstream &file, fstream &ofile, int count)
{
    string temp;
    while(count--){
        getline(file, temp);
        ofile << temp << "\n";
    } 
}
string getLine(fstream &ifile, fstream &ofile)
{
    string temp;
    getline(ifile, temp);
    ofile << temp << "\n";
    return temp;
}
void parseFile(int c1, int c2)
{
    string key_temp;
    fstream file, ofile;
    string end(4, '-'), filename = "Log-" + to_string(min(c1, c2)) + "-" + to_string(max(c1, c2)) + ".txt";
    string s_c1 = "Client-" + to_string(c1) + ":", s_c2 = "Client-" + to_string(c2) + ":";
    file.open(filename, ios::out | ios::app);
    file << "\n----";
    file.close();
    file.open(filename, ios::in);
    ofile.open("decrypted-" + filename, ios::out);
    ofile.close();
    ofile.open("decrypted-" + filename, ios::out | ios::app);
    while(file.is_open()){
        string temp;
        vector<string>curr, prev;
        while(1){
            temp = getLine(file, ofile);
            if(temp != "") break;
        }
        if(temp == end) return;
        // temp = getLine(file, ofile);
        if(s_c2 == temp) swap(s_c1, s_c2);
        skipLines(file, ofile, 1);
        temp = getLine(file, ofile);
        A = stoll(temp);
        x = findKey(g, A, a);
        while(1){
            getline(file, temp);
            curr = removeWhiteSpaces(temp);
            if(curr.size() == 1 && curr[0] == s_c2) break;
            ofile << temp << "\n";
        }
        temp = getLine(file, ofile);
        if(temp != "yes"){
            skipLines(file, ofile, 2);
            continue;
        }
        temp = getLine(file, ofile);
        B = stoll(temp);
        key = to_string(power(B, x, a));
        key_temp = key;
        cout << key_temp << "\n";
        skipLines(file, ofile, 4);
        while(1){
            skipLines(file, ofile, 1);
            getline(file, temp);
            if(temp == "good bye" || temp == ""){
                ofile << temp << "\n";
                break;
            }
            temp = RC4_Cipher(temp);
            string temp1 = temp;
            cout<<temp1<<endl;
            ofile << temp1 << endl;
        }
    }
}
int main()
{
    int c1, c2;
    g = 39, a = 79;
    cout<<"Enter the value of g : ";
    cin>>g;
    cout<<"Enter the value of a : ";
    cin>>a; 
    cout << "Enter the client id's by giving space between them\n";
    cin >> c1 >> c2;
    parseFile(c1, c2);

    return 0;
}