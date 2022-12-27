#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
//#include <algorithm>

using namespace std;

map <string, string> parametrs {
    {"Delim", "\n"},
    {"Arrow", "->"},
    {"Empty", ""},
    {"TermStart", ""},
    {"TermEnd", ""},
    {"NtermStart", "'"},
    {"NtermEnd", "'"},
    {"Operation[]Start", "["},
    {"Operation[]End", "]"},
    {"Operation{}Start", "{"},
    {"Operation{}End", "}"},
    {"Operation()Start", "("},
    {"Operation()End", ")"},
    {"OperationConcat", ","}
};

bool err = false;

// string removeSpaces(string input) {
//   input.erase(std::remove(input.begin(),input.end(),' '),input.end());
//   return input;
// }

void inputSyntax(int n){
    string testName = "tests\\test" + to_string(n) + "\\syntax.txt";
    cout << testName << endl;
    ifstream file(testName);
    if (file.is_open()) {
        string str;
        while (getline(file,str)) {
            if (str.size()) {
                int pos = str.find("=");
                for(auto mypair: parametrs) {
                    string key = mypair.first;
                    if (str.substr(0, pos) == key) {
                        if (str.substr(pos+2) != "") {
                            parametrs[key] = str.substr(pos+2);
                            // cout << key << " - " << parametrs[key] << endl;
                        }
                    }
                }
            }
        }
        file.close();
    } else {
        cout << "file 'syntax.txt' does not exist. All parametrs are default" << endl;
    }
}

int main() {
    int n;
    cout << "Enter test number" << endl;
    cin >> n;
    inputSyntax(n);
    
    cout << ">> PARAMETRS <<" << endl;
    for (auto mypair: parametrs) {  
        cout << mypair.first << " = " << mypair.second << endl;
    } 
    // парсим грамматику
    // если в грамматике не использованы значения по умолчанию (а параметры не были заполнены), то говорим об ошибке 
    // bool err = inputGrammar(n);
    // if (!err) {
    //     cout << "ERROR - Grammar is incorrect";
    //     return 0;
    // }

    // конвертируем в КС

    // добавляем параметры для КС (опционально)

    // выводим КС

    return 0;
}
