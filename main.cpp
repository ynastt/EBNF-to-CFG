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
    {"Operation()End", ")"},
    {"OperationConcat", ","}
};

// множество используемых обозначений операций 
set <string> grammarSymbols;

bool err = false;

// структура для описания правила переписывания нетерминала
struct Tree {
    char symbol;
    int num; // [] - 0; {} - 1; () - 2; | - 3; , - 4; a-z -5; A-Z -6;
    struct Tree* right;
    struct Tree* left;
};

// каждое правило имеет представление нетерминал left -> правило переписывания в виде дерева right
struct Rule {
    string left;
    struct Tree* right;
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

// bool check_rule(string str) {
//     for(int i =0; i < str.size(); i ++) {
//         if (grammarSymbols.find(str[i]) != grammarSymbols.end())
//     }
//     return false;
// }

//Чисто пока нет парсера
vector <string> string_rules;

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

//input распарсивает наши правила по delim, то есть в string_rules мы храним по одному правилу. Если там встречается какой-то корявый синтаксис [Delim], то мы это правило просто не добавляем
//добавить в парсинге при проверке arrow, чтобы в каждом правиле не было больше 1 arrow. Иначе удаляем это правило
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

int main() {
    int n;
    cout << "Enter test number" << endl;
    cin >> n;
    inputSyntax(n);
    
    cout << ">> PARAMETRS <<" << endl;
    for (auto mypair: parametrs) {  
        cout << mypair.first << " = " << mypair.second << endl;
        grammarSymbols.insert(mypair.second);
    } 

    cout << "grammarSymbols" << endl;
    set <string> :: iterator it = grammarSymbols.begin();
    for (int i = 1; it != grammarSymbols.end(); i++, it++) {
        cout << *it << " ";
    }
    cout << endl;

    // парсим грамматику
    // если в грамматике не использованы значения по умолчанию (а параметры не были заполнены), то говорим об ошибке 
    // bool err = inputGrammar(n);
    // if (!err) {
    //     cout << "ERROR - Grammar is incorrect";
    //     return 0;
    // }

    // зачем это, мы же уже прочитали параметры теста n, в той же папки тогда читаем и input
    // cout<<"Please,input number of test for grammar\n";
    // cin>>n;

    // int error = input_file(n);
    // if (error) {
    //     cout<<"Grammar is empty.";
    //     return 0;
    // }

    // for (int i = 0; i < string_rules.size(); i++){
    //     cout << string_rules[i] << "\n";
    // }


    // TO DO
    // конвертируем в КС
    // добавляем параметры для КС (опционально)
    // выводим КС
    return 0;
}
