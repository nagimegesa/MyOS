#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;
void init(unordered_map<string, char>& ump);
int main() {
    ifstream s("./hankaku.txt", ios::in | ios::binary);
    ofstream out("./font.txt", ios::out | ios::trunc);
    if (!s.is_open() || !out.is_open()) {
        cout << "open failed";
        exit(1);
    }
    unordered_map<string, char> ump;
    init(ump);
    out << "char font[4096] = {";
    string buf, buf2, buf3, buf4(", 0x");
    int j = 0;
    getline(s, buf);
    getline(s, buf);
    while (getline(s, buf)) {
        if (buf.size() > 1 && buf.find("char") == string::npos) {
            buf2.clear();
            buf3.clear();
            for (int i = 0; i < 4; ++i) {
                char num = buf[i] == '*' ? '1' : '0';
                buf2.push_back(num);
            }
            for (int i = 4; i < 8; ++i) {
                char num = buf[i] == '*' ? '1' : '0';
                buf3.push_back(num);
            }
            out << buf4 + ump[buf2] + ump[buf3];
            ++j;
            if (j % 128 == 0) out << endl;
        }
    }
    out << "};";
    s.close();
    return 0;
}

void init(unordered_map<string, char>& ump) {
    ump.emplace("0000", '0');
    ump.emplace("0001", '1');
    ump.emplace("0010", '2');
    ump.emplace("0011", '3');
    ump.emplace("0100", '4');
    ump.emplace("0101", '5');
    ump.emplace("0110", '6');
    ump.emplace("0111", '7');
    ump.emplace("1000", '8');
    ump.emplace("1001", '9');
    ump.emplace("1010", 'a');
    ump.emplace("1011", 'b');
    ump.emplace("1100", 'c');
    ump.emplace("1101", 'd');
    ump.emplace("1110", 'e');
    ump.emplace("1111", 'f');
}