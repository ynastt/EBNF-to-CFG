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
string Clear_str(string str){
    string v;
    for(char c:str) if (c != ' ') v += c;
    return v;
}

//Чисто пока нет парсера
vector <string> string_rules;

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

string Clear_str(string str){
    string v;
    for(char c:str) if (c != ' ') v += c;
    return v;
}

//input распарсивает наши правила по delim, то есть в string_rules мы храним по одному правилу. Если там встречается какой-то корявый синтаксис [Delim], то мы это правило просто не добавляем
int input_file(int n){
    ifstream in("tests\\test" + to_string(n) + "\\input.txt");
    string str;
    while (getline(in,str)){
        if (str.size()) {
            str=Clear_str(str);
            if (parametrs["Delim"]=="\n"){
                if (str.size()) string_rules.push_back(str);
            }
            else{
                while (str.size()){
                int k=str.find(parametrs["Delim"]);
                if (k!=-1){
                    string dopstr=str.substr(0,k);
                    if (dopstr.size()) string_rules.push_back(dopstr);
                    str=str.erase(0,k+1);
                }
                else break;
            }
            }
            
        }

    }
    if (!string_rules.size()) return 1;
    else return 0;
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
    
    cout<<"Please,input number of test for grammar\n";
    cin>>n;
    int error=input_file(n);
    if (error){
        cout<<"Grammar is empty.";
        return 0;
    }
    for (int i=0;i<string_rules.size();i++){
        cout<<string_rules[i]<<"\n";
    }

    return 0;
}
