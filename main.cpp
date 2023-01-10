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
    {"NtermEnd", "'"},
    {"Empty", "#"}
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
    {"NtermEnd", "'"},
    {"Concat",""},
    {"Alternative","|"}
};

// множество используемых обозначений операций
// использовать для проверки правильности грамматики 
set <string> grammarSymbols;

bool err = false;
bool errSyntax = false;
int lock = false;
string saveInputEmpty = "";

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
vector<Rule*> Grammar;

string Clear_str(string str){
    string v;
    for(char c : str) if (c != ' ') v += c;
    return v;
}

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

// функция находит индекс первой внешней альтернативы
// если -1 - правило не имеет вид альтернативы, значит это конкатенация чего-то
// если не -1, то дан индекс первой альтенативы 
// (типа правило имеет вид `S -> что-то | что-то ...` ), возможно есть еще альтернативы
int getFirstAltIndex (string str) {
    int outBracketsCount1 = 0;
    int outBracketsCount2 = 0;
    int outBracketsCount3 = 0;
	for (int i = 0; i < str.size(); i++) {
        //printf("counters are\n(): %d\n{}: %d\n[]: %d\n", outBracketsCount1, outBracketsCount2, outBracketsCount3);
        //cout << "letter: " << str[i] << endl;
		if (outBracketsCount1 == 0 && outBracketsCount2 == 0 &&
             outBracketsCount3 == 0 && str[i] == '|') {
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
		} else {
            continue;
        }
	}
	return -1;
}

int findClosingParanthesis( string str, char pstart, char pend) {
    int c = 0;
	for (int i = 0; i < str.size(); i++) {
		if (str[i] == pstart) {
			c++;
		} else if (str[i] == pend) {
			c--;
			if (c == 0) {
				return i;
			}
		}
	}
    return -1;
}

struct Tree* parseAlt(string str);

struct Tree* parseConcat(string str) { 
    struct Tree* tr = new Tree;
    // cout << "CON: " << str << endl;
    if (str.length() == 1) {
        if (islower(str[0])) {
            tr->str = str;
            tr->num = 5;
            tr->left = NULL;
            tr->right = NULL;
            return tr;
        }
        if (isupper(str[0])) {
            tr->str = str;
            tr->num = 6;
            tr->left = NULL;
            tr->right = NULL;
            return tr;
        }
        if (str == parametrs["Empty"]) {
            tr->str = parametrs["Empty"];
            tr->num = 7;
            tr->left = NULL;
            tr->right = NULL;
            return tr;
        }
    }
    if (str.length() == 3) {
        if (str[0] == '[' && str[2] == ']') {
            string s = {str[1]};
            tr->str = str;
            tr->num = 0;
            tr->left = parseConcat(s);;
            tr->right = NULL;
            return tr;
        }
        if (str[0] == '{' && str[2] == '}') {
            string s = {str[1]};
            tr->str = str;
            tr->num = 1;
            tr->left = parseConcat(s);
            tr->right = NULL;
            return tr;
        }
        if (str[0] == '(' && str[2] == ')') {   // такого случая наверное не будет в норм грамматике пользователя
            string s = {str[1]};
            tr->str = str;
            tr->num = 2;
            tr->left = parseConcat(s);
            tr->right = NULL;
            return tr;
        }
    }    
    // парсим
    if (islower(str[0]) || isupper(str[0])) {
        string s = {str[0]};
        tr->str = str;
        tr->num = 4;
        tr->left = parseConcat(s);
        tr->right = parseConcat(str.substr(1));
        return tr;
    }
    if (str[0] == '[') {
        int pos = findClosingParanthesis(str, '[', ']');
        if (pos != -1) {
            if (pos != str.size() - 1) {
                tr->str = str;
                tr->num = 4;
                tr->left = parseAlt(str.substr(0, pos + 1));
                tr->right = parseConcat(str.substr(pos + 1));
                return tr;
            } else if (pos == str.size() - 1) {
                //cout << "HERE []" << endl;
                tr->str = str;
                tr->num = 0;
                tr->left = parseAlt(str.substr(1,pos - 1));
                tr->right = NULL;
                return tr;
            }
        }    
    }
    if (str[0] == '{') {
        // здесь поиск закрывающей соответствующей скобки
        int pos = findClosingParanthesis(str, '{', '}');
        if (pos != -1) {
            if (pos < str.size() - 1) {
                //cout << "HERE 1 {}" << endl;
                tr->str = str;
                tr->num = 4;
                tr->left = parseAlt(str.substr(0, pos + 1));
                tr->right = parseConcat(str.substr(pos + 1));
                return tr;
            } else if (pos == str.size() - 1){
                //cout << "HERE {}" << endl;
                tr->str = str;
                tr->num = 1;
                tr->left = parseAlt(str.substr(1, pos - 1));
                tr->right = NULL;
                return tr;
            }
        }    
    }
    if (str[0] == '(') {
        //cout << "debug - str starts with ("<< endl;
        int pos = findClosingParanthesis(str, '(', ')');
        //cout << "pos of ): " << pos << endl;
        if (pos != -1) {
            if (pos != str.size() - 1) {
                tr->str = str;
                tr->num = 4;
                tr->left = parseAlt(str.substr(0, pos + 1));
                tr->right = parseConcat(str.substr(pos + 1));
                return tr;
            } else {
                 tr->str = str;
                tr->num = 2;
                tr->left = parseAlt(str.substr(1,pos - 1));
                tr->right = NULL;
                return tr;
            }    
        }
    }
    return tr;
}

struct Tree* parseAlt(string str) {
    // cout << "ALT: " << str << endl;
    struct Tree* t = new Tree;
    int alt = getFirstAltIndex(str);
    if (alt == -1) {
        t = parseConcat(str);  
	} else {
        t->str = str;
        t->num = 3;
        t->left = parseAlt(str.substr(0, alt));
        t->right = parseAlt(str.substr(alt+1));	
	}
    return t;
}

// идея в том, чтобы убрать обрамления терминалов и нетерминалов,
// потому что мы можем разлечить a-z и A-Z по юникоду
string removeNtermTermBrackets(string str) {
    int pos = 0;
    string sub = parametrs["NtermStart"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    sub = parametrs["NtermEnd"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    sub = parametrs["TermStart"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    pos = 0;
    sub = parametrs["TermEnd"];
    if (sub != "") {
        while((pos = str.find(sub, pos)) != string::npos) {
            str.replace(pos, sub.length(), "");
        }
    }
    return str;
}

string magicReplaceEmpty(string str) {
    int pos = 0;
    lock = false;
    string sub = parametrs["Empty"];
    if (sub != "#" && !lock) {
        saveInputEmpty = sub;
        lock = true;
    }
    parametrs["Empty"] = "#";
    if (saveInputEmpty != "") {
        while((pos = str.find(saveInputEmpty, pos)) != string::npos) {
            str.replace(pos, saveInputEmpty.length(), "#");
        }
    }
    return str;
}

Rule* parseRule(string str) {
    Rule* rule = new Rule;
    string param = parametrs["NtermStart"];
    int len = param.length();
    int i = 0;
    if (str.substr(0, len) == param) {
        rule->left = str.substr(len, 1);
        i = len + 1;    // теперь i указывает на следующий символ после нетерминала 
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    param = parametrs["NtermEnd"];
    len = param.length();
    if (str.substr(i, len) == param) {
        i += len; // теперь i указывает на следующий символ после NtermEnd
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    param = parametrs["Arrow"];
    len = param.length();
    if (str.substr(i, len) == param) {
        i += len; // теперь i указывает на следующий символ после Arrow
    } else {
        errSyntax = true;
        cout << "\nERROR - Grammar is incorrect";
        exit(0);
    }
    // здесь начинаем парсить правую часть правила
    struct Tree * rightPart = new struct Tree;
    string str_upd = magicReplaceEmpty(str.substr(i));
    str_upd = removeNtermTermBrackets(str_upd);
    rightPart = parseAlt(str_upd);
    rule->right = rightPart;
    return rule;
}

bool inputGrammar(int n) {
    ifstream in("tests\\test" + to_string(n) + "\\input.txt");
    string str;
    while (getline(in,str)) {
        if (str.size()) {
            str=Clear_str(str);
            if (parametrs["Delim"]=="\n") {
                if (str.size()) {
                    Rule* r = parseRule(str);
                    Grammar.push_back(r);
                }    
            }
            else {
                while (str.size()) {
                    int k = str.find(parametrs["Delim"]);
                    if (k != -1){
                        string dopstr = str.substr(0,k);
                        if (dopstr.size())  {
                            Rule* r = parseRule(dopstr);
                            Grammar.push_back(r);
                        }
                        str=str.erase(0,k+1);
                    }
                    else break;
                }
            }
        }
    }
    if (Grammar.size() == 0) return (true);
    else return (false);
}

void printTree(struct Tree* t) {    
    cout << t -> str <<" "<<t->num<< endl;;
    if (t->left != NULL) {
        printTree(t->left); 
    } 
    if (t->right != NULL) {
        printTree(t->right);
    } 
    
} 

void printGrammar() {
    int i = 0;
    if (Grammar.size() == 0) {
        cout << "CHECK IF INPUT GRAMMAR IS CORRECT, HAS CORRECT PARAMETRS!\nRESTART THE PROGRAM\n";
        exit(0);
    }
    for (auto rule : Grammar) {
        cout << "== RULE " << i++ << " ==" << endl;
        cout << "NTERM: " << rule->left << endl;
        cout << "RIGHT PART TREE:" << endl;
        printTree(rule->right);
    }
    
}

struct cfg_rule{
    string leftpart;
    string rightpart;
};

vector <cfg_rule> cfg_answer;

vector <cfg_rule> onerule;
 
vector <string> forcheck;
//Функция для генерации разных нетерминалов
string GeneratorNewNeterm(){
    int num=0;
    string newneterm="A";
    while (true){
        int flag=0;
        for (int i=0;i<forcheck.size();i++){
            if (forcheck[i].find(to_string(num))!=-1){
                flag=1;
                break;
            }
        }
        if (!flag) break;
        else num++;
    }
    return newneterm+to_string(num);
}

/*
map <string, string> parametrsCFG {
    {"Delim", "\n"},
    {"Arrow", "->"},
    {"Empty", ""},
    {"TermStart", ""},
    {"TermEnd", ""},
    {"NtermStart", "'"},
    {"NtermEnd", "'"}
};
*/
// [] - 0; {} - 1; () - 2; | - 3; concat - 4; a-z -5; A-Z -6, empty - 7;
string ConvertertoCFG_1(string leftpart,struct Tree* p,int flag){
    string str;
   if (p!=NULL){
    if (p->num==4){
        if ((p->left!=NULL)&&(p->right!=NULL))str=ConvertertoCFG_1(leftpart,p->left,0)+parametrsCFG["Concat"]+ConvertertoCFG_1(leftpart,p->right,0);
    }
    else if (p->num==5){
        str=parametrsCFG["TermStart"]+p->str+parametrsCFG["TermEnd"];
    }
    else if (p->num==6){
        str=parametrsCFG["NtermStart"]+p->str+parametrsCFG["NtermEnd"];
    }
    else if (p->num==7){
        str=parametrsCFG["Empty"];
    }
    else if ((p->num==3)&&(!flag)){
        string newneterm=GeneratorNewNeterm();
        str=parametrsCFG["NtermStart"]+newneterm+parametrsCFG["NtermEnd"];
        forcheck.push_back(newneterm);
        ConvertertoCFG_1(newneterm,p,1);
        //str=ConvertertoCFG_1(newneterm,p->left)+"|"+ConvertertoCFG_1(newneterm,p->right);
    }
    else if ((p->num==3)&&(flag)){
        if ((p->left!=NULL)&&(p->right!=NULL)) str=ConvertertoCFG_1(leftpart,p->left,0)+parametrsCFG["Alternative"]+ConvertertoCFG_1(leftpart,p->right,0);
    }
    else if ((p->num==0)&&(!flag)){
        string newneterm=GeneratorNewNeterm();
        str=parametrsCFG["NtermStart"]+newneterm+parametrsCFG["NtermEnd"];
        forcheck.push_back(newneterm);
        ConvertertoCFG_1(newneterm,p,1);
    }
    else if ((p->num==0)&&(flag)){
        if ((p->left!=NULL)) str=ConvertertoCFG_1(leftpart,p->left,0)+parametrsCFG["Alternative"]+parametrsCFG["Empty"];
    }
    else if ((p->num==1)&&(!flag)){
        string newneterm=GeneratorNewNeterm();
        str=parametrsCFG["NtermStart"]+newneterm+parametrsCFG["NtermEnd"];
        forcheck.push_back(newneterm);
        ConvertertoCFG_1(newneterm,p,1);
        //str=ConvertertoCFG_1(newneterm,p->left)+"|"+ConvertertoCFG_1(newneterm,p->right);
    }
    else if ((p->num==1)&&(flag)){
        if ((p->left!=NULL)) str=ConvertertoCFG_1(leftpart,p->left,0)+parametrsCFG["NtermStart"]+leftpart+parametrsCFG["NtermEnd"]+parametrsCFG["Alternative"]+parametrsCFG["Empty"];
    }
    else if ((p->num==2)){
        if ((p->left!=NULL)) str=ConvertertoCFG_1(leftpart,p->left,0);
    }
   }
    if (onerule.empty()){
        struct cfg_rule dop;
        dop.leftpart=leftpart;
        dop.rightpart=str;
        onerule.push_back(dop);
    }
    else{
        int find=0;
        for (int i=0;i<onerule.size();i++){
            if (onerule[i].leftpart==leftpart){
                onerule[i].rightpart=str;
                find=1;
            }
        }
        if (!find){
            struct cfg_rule dop;
            dop.leftpart=leftpart;
            dop.rightpart=str;
            onerule.push_back(dop);
        }
    }
    //cout<<leftpart<<"\n";
    return str;
} 

//Чекаем правую сторону, 
//vector <cfg_rule> cfg_answer;

void NormOrder(){
    for (int i=0;i<onerule.size();i++){
        if (onerule[i].leftpart.size()==1){
            onerule[i].leftpart=parametrsCFG["NtermStart"]+onerule[i].leftpart+parametrsCFG["NtermEnd"];
            cfg_answer.push_back(onerule[i]);
            break;
        }
    }
    string neterm="A";
    int num=0;
    string str;
    str=neterm+to_string(num);
    if (onerule.size()>1){
        while (true){
        int flag=0;
        for (int i=0;i<forcheck.size();i++){
            if (forcheck[i]==str){
                flag=1;
                break;
            }
        }
        if (!flag) break;
        for (int i=0;i<onerule.size();i++){
            if (onerule[i].leftpart==str){
                onerule[i].leftpart=parametrsCFG["NtermStart"]+onerule[i].leftpart+parametrsCFG["NtermEnd"];
                cfg_answer.push_back(onerule[i]);
                break;
            }
        }
        num++;
        str=neterm+to_string(num);
        }
    }
    
}

void ConvertertoCFG(){
    for (int i=0;i<Grammar.size();i++){
        onerule.clear();
        ConvertertoCFG_1(Grammar[i]->left,Grammar[i]->right,0);
        if (!onerule.empty()) NormOrder();
    }
}

void PrintAnswer(int n){
    ofstream f1;
    f1.open("tests\\test" + to_string(n) + "\\output.txt");
    for (int i=0;i<cfg_answer.size();i++){
        string str=cfg_answer[i].leftpart+parametrsCFG["Arrow"]+cfg_answer[i].rightpart+parametrsCFG["Delim"];
        cout<<cfg_answer[i].leftpart<<parametrsCFG["Arrow"]+cfg_answer[i].rightpart<<parametrsCFG["Delim"];
        f1<<str;
    }
}

void inputSyntax_2(int n){
    string testName = "tests\\test" + to_string(n) + "\\CFGsyntax.txt";
    // cout << testName << endl;
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
                            // cout << key << " - " << parametrs[key] << endl;
                        }
                    }
                }
            }
        }
        file.close();
    } else {
        cout << "\nfile 'CFGsyntax.txt' does not exist. All parametrs are default" << endl;
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
    // если в грамматике не использованы значения по умолчанию (а параметры не были заполнены), 
    // то говорим об ошибке err
    bool empty;
    empty  = inputGrammar(n);
    if (empty) {
        cout << "\nERROR - Grammar is empty";
        exit(0);
    } else {
        cout << "\nGrammar is fine! " << endl;
    }
    inputSyntax_2(n);
    cout << ">> PARAMETRS FOR CFG <<" << endl;
    for (auto mypair: parametrsCFG) {  
        cout << mypair.first << " = " << mypair.second << endl;
    } 
    cout << "\n>> PARSED GRAMMAR <<" << endl;
    printGrammar();
    cout<<"\n\n";
    ConvertertoCFG();
    cout<<"\n.....CFG.....\n";
    PrintAnswer(n);
    return 0;
}