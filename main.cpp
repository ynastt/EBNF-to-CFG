#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>

using namespace std;
const string EMPTY = "#";

map <string, string> parametrs {
    {"Delim", "\n"},
    {"Arrow", "->"},
    {"TermStart", ""},
    {"TermEnd", ""},
    {"NtermStart", "'"},
    {"NtermEnd", "'"}
    // {"Operation[]Start", "["},
    // {"Operation[]End", "]"},
    // {"Operation{}Start", "{"},
    // {"Operation{}End", "}"},
    // {"Operation()Start", "("},
    // {"Operation()End", ")"}
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
bool errSyntax = false;

// struct Elem {
//     string val;
//     int num;    // [] - 0; {} - 1; () - 2; | - 3; concat - 4; a-z -5; A-Z -6, empty - 7;
// };

// struct Rule {
//     string left;
//     vector<Elem> right;
// };

// структура для описания правила переписывания нетерминала
struct Tree {
    string str;
    int num; // [] - 0; {} - 1; () - 2; | - 3; concat - 4; a-z -5; A-Z -6, empty - 7;
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

// функция находит индекс первой внешней альтернативы
// если -1 - правило не имеет вид альтернативы, значит это конкатенация чего-то
// если не -1, то дан индекс первой альтенативы 
// (типа правило имеет вид `S -> что-то | что-то ...` ), возможно есть еще альтернативы
int getFirstAltIndex (string str) {
    int outBracketsCount1 = 0;
    int outBracketsCount2 = 0;
    int outBracketsCount3 = 0;
	for (int i =0; i < str.size(); i++) {
		if (outBracketsCount1 == outBracketsCount2 == outBracketsCount3 == 0 && 
		str[i] == '|') {
			return i;
		} else if (str[i] == '(') {
			outBracketsCount1++;
		} else if (str[i] == ')') {
			outBracketsCount1--;
		} else if (str[i] == '{') {
			outBracketsCount2++;
		} else if (str[i] == '}') {
			outBracketsCount2--;
		} else if (str[i] == '[') {
			outBracketsCount3++;
		} else if (str[i] == ']') {
			outBracketsCount3--;
		}
	}
	return -1;
}

// // функция ищет все альтернативные правила (те самые что-то, про которые речь в функции выше)
// vector<string> getListOfAltSubstrings(string str) {
// 	vector<string> v;
// 	int k = str.length();
// 	int i = 0;
// 	while (i <= k) {
// 		int alt = getFirstAltIndex(str);
// 		if (alt == -1) {
//             v.push_back(str);
// 			return v;
// 		} else {
//             v.push_back(str.substr(0, alt));
// 			str = str.substr(alt+1);
// 			k = str.length();
// 			i = 0;
// 		}
// 		i++;
// 	}
// 	return v;
// }

Tree parseConcat(string str) {
    Tree t = {"zero addr", -1, nullptr, nullptr};
    if (str.length() == 1) {
        if (islower(str[0])) {
            t.str = str;
            t.num = 5;
            t.left = nullptr;
            t.right = nullptr;
            return t;
        }
        if (isupper(str[0])) {
            t.str = str;
            t.num = 6;
            t.left = nullptr;
            t.right = nullptr;
            return t;
        }
        if (str == EMPTY) {
            t.str = str;
            t.num = 7;
            t.left = nullptr;
            t.right = nullptr;
            return t;
        }
    }
    if (str.length() == 3) {
        if (str[0] == '[' && str[0] == ']') {
            string s = {str[1]};
            t.str = str;
            t.num = 0;
            t.left = &parseConcat(s);;
            t.right = nullptr;
            return t;
        }
        if (str[0] == '{' && str[0] == '}') {
            string s = {str[1]};
            t.str = str;
            t.num = 1;
            t.left = &parseConcat(s);
            t.right = nullptr;
            return t;
        }
        if (str[0] == '(' && str[0] == ')') {   // такого случая наверное не будет в норм грамматике пользователя
            string s = {str[1]};
            t.str = str;
            t.num = 2;
            t.left = &parseConcat(s);
            t.right = nullptr;
            return t;
        }
    }
    // идея в том, чтобы убрать обрамления терминалов и нетерминалов,
    // потому что мы можем разлечить a-z и A-Z по юникоду
    int pos = 0;
    string sub = parametrs["NtermStart"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    string sub = parametrs["NtermEnd"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    string sub = parametrs["TermStart"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    string sub = parametrs["TermEnd"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    // парсим
    if (islower(str[0]) || isupper(str[0])) {
        string s = {str[0]};
        t.str = str;
        t.num = 4;
        t.left = &parseConcat(s);
        t.right = &parseConcat(str.substr(1));
        return t;
    }
    if (str[0] == '[') {
        int pos = str.find(']');
        if (pos != str.size() - 1) {
            t.str = str;
            t.num = 4;
            t.left = &parseAlt(str.substr(0, pos));
            t.right = &parseConcat(str.substr(pos + 1));
            return t;
        } else {
            t.str = str;
            t.num = 0;
            t.left = &parseAlt(str);
            t.right = nullptr;
            return t;
        }    
    }
    if (str[0] == '{') {
        int pos = str.find('}');
        if (pos != str.size() - 1) {
            t.str = str;
            t.num = 4;
            t.left = &parseAlt(str.substr(0, pos));
            t.right = &parseConcat(str.substr(pos + 1));
            return t;
        } else {
            t.str = str;
            t.num = 1;
            t.left = &parseAlt(str);
            t.right = nullptr;
            return t;
        }    
    }
    if (str[0] == '(') {
        int pos = str.find(')');
        if (pos != str.size() - 1) {
            t.str = str;
            t.num = 4;
            t.left = &parseAlt(str.substr(0, pos));
            t.right = &parseConcat(str.substr(pos + 1));
            return t;
        } else {
            t.str = str;
            t.num = 2;
            t.left = &parseAlt(str);
            t.right = nullptr;
            return t;
        }    
    }
    return t;
}

Tree parseAlt(string str) {
    Tree t;
    int alt = getFirstAltIndex(str);
    if (alt == -1) {
        t = parseConcat(str);  
	} else {
        t.str = str;
        t.num = 3;
        t.left = &parseAlt(str.substr(0, alt));
        t.right = &parseAlt(str.substr(alt+1));	
	}
    return t;
}

Rule parseRule(string str) {
    Rule rule;
    string param = parametrs["NtermStart"];
    int len = param.length();
    int i = 0;
    if (str.substr(0, len) == param) {
        rule.left = str.substr(len, 1);
        i = len + 1;    // теперь i указывает на следующий символ после нетерминала 
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    param = parametrs["NtermEnd"];
    int len = param.length();
    if (str.substr(i, len) == param) {
        i += len; // теперь i указывает на следующий символ после NtermEnd
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    param = parametrs["Arrow"];
    int len = param.length();
    if (str.substr(i, len) == param) {
        i += len; // теперь i указывает на следующий символ после Arrow
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    // здесь начинаем парсить правую часть правила
    Tree rightPart = parseAlt(str.substr(i));
    rule.right = &rightPart;
    return rule;
}

int inputGrammar(int n) {
    
    ifstream in("tests\\test" + to_string(n) + "\\input.txt");
    string str;
    while (getline(in,str)) {
        if (str.size()) {
            str=Clear_str(str);
            if (parametrs["Delim"]=="\n") {
                if (str.size()) {
                    //string_rules.push_back(str);
                    Rule r = parseRule(str);
                    Grammar.push_back(r);
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
        }
    }
    if (Grammar.size() == 0) return (true, errSyntax);
    else return (false, errSyntax);
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
