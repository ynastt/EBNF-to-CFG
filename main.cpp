#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

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
    {"Operation()End", ")"}
};

map <string, string> parametrsCFG {
    {"Delim", "\n"},
    {"Arrow", "->"},
    {"Empty", ""},
    {"TermStart", ""},
    {"TermEnd", ""},
    {"NtermStart", "'"},
    {"NtermEnd", "'"}
};

// множество используемых обозначений операций
// использовать для проверки правильности грамматики 
set <string> grammarSymbols;

bool err = false;

struct Elem {
    string val;
    int num;    // [] - 0; {} - 1; () - 2; | - 3; , - 4; a-z -5; A-Z -6;
};

struct Rule {
    string left;
    vector<Elem> right;
};

// Грамматика хранится как вектор правил
// это позволит сохранить последовательность правил введенной грамматики и вывести КС грамматику аналогично.
// стартовый нетерминал грамматики всегда S (вот так условились)
// соотвественно vector[0] - первое правило грамматики, правило переписывания нетерминла S
vector<Rule> Grammar;

string Clear_str(string str){
    string v;
    for(char c : str) if (c != ' ') v += c;
    return v;
}

//Чисто пока нет парсера
// vector <string> string_rules;

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

void inputCFGsyntax(int n){
    string testName = "tests\\test" + to_string(n) + "\\CFGsyntax.txt";
    cout << testName << endl;
    ifstream file(testName);
    if (file.is_open()) {
        string str;
        while (getline(file,str)) {
            if (str.size()) {
                int pos = str.find("=");
                for(auto mypair: parametrsCFG) {
                    string key = mypair.first;
                    if (str.substr(0, pos) == key) {
                        if (str.substr(pos+2) != "") {
                            parametrsCFG[key] = str.substr(pos+2);
                            // cout << key << " - " << parametrsCFG[key] << endl;
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

/*
//input распарсивает наши правила по delim, то есть в string_rules мы храним по одному правилу. 
// Если там встречается какой-то корявый синтаксис [Delim], то мы это правило просто не добавляем
// добавить в парсинге при проверке arrow, чтобы в каждом правиле не было больше 1 arrow. Иначе удаляем это правило
int input_file(int n){
    ifstream in("tests\\test" + to_string(n) + "\\input.txt");
    string str;
    while (getline(in,str)){
        if (str.size()) {
            str=Clear_str(str);
            if (parametrs["Delim"] == "\n"){
                if (str.size()) string_rules.push_back(str);
            }
            else{
                while (str.size()){
                int k = str.find(parametrs["Delim"]);
                if (k != -1){
                    string dopstr = str.substr(0,k);
                    if (dopstr.size()) string_rules.push_back(dopstr);
                    str = str.erase(0,k+1);
                }
                else break;
            }
            
        }

    }
    if (!string_rules.size()) return 1;
    else return 0;
}
*/

// пока в процессе
int inputGrammar(int n) {
    bool errors = false;
    ifstream in("tests\\test" + to_string(n) + "\\input.txt");
    string str;
    while (getline(in,str)) {
        if (str.size()) {
            str=Clear_str(str);
            // я предлагаю проверять каждую строку без пробелов на корректность значков операций
            // т.к. если хотя бы в одной строке в грамматике ошибка, то ахтунг
            // bool ok = check_rule(str);
            // if (ok) {
                if (parametrs["Delim"]=="\n") {
                    if (str.size()) {
                        //string_rules.push_back(str);
                    }    
                }
                else {
                    while (str.size()) {
                        int k = str.find(parametrs["Delim"]);
                        if (k != -1){
                            string dopstr = str.substr(0,k);
                            if (dopstr.size()) //string_rules.push_back(dopstr);
                            str=str.erase(0,k+1);
                        }
                        else break;
                    }
                }
            // } else {
            //      errors = true;
            // }
        }

    }
    if (Grammar.size() == 0) return (true, errors);
    else return (false, errors);
}

int main() {
    int n;
    cout << "Enter test number" << endl;
    cin >> n;
    inputSyntax(n);
    inputCFGsyntax(n);

    cout << ">> PARAMETRS <<" << endl;
    for (auto mypair: parametrs) {  
        cout << mypair.first << " = " << mypair.second << endl;
        grammarSymbols.insert(mypair.second);
    } 

    // добавляем параметры для КС 
    cout << ">> CFG PARAMETRS <<" << endl;
    for (auto mypair: parametrsCFG) {  
        cout << mypair.first << " = " << mypair.second << endl;
        
    } 

    cout << "grammarSymbols" << endl;
    set <string> :: iterator it = grammarSymbols.begin();
    for (int i = 1; it != grammarSymbols.end(); i++, it++) {
        cout << *it << " ";
    }
    cout << endl;

    // парсим грамматику
    // если в грамматике не использованы значения по умолчанию (а параметры не были заполнены), 
    // то говорим об ошибке err
    bool empty;
    empty , err = inputGrammar(n);
    if (err) {   
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    } else {
        if (empty) {
            cout << "\nERROR - Grammar is empty";
            exit(0);
        } else {
            cout << "\nGrammar is fine! " << endl;
        }
    }

    cout << ">> PARSED GRAMMAR <<" << endl;
    

    

    // for (int i = 0; i < string_rules.size(); i++){
    //     cout << string_rules[i] << "\n";
    // }


    // TO DO
    // конвертируем в КС
    
    // выводим КС
    return 0;
}
