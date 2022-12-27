# EBNF-to-CFG
Программа реализует преобразование РБНФ в пользовательском синтаксисе в классическую КС-грамматику.
## Формат входных данных
Пользовательская грамматика должна соответствовать правилам общей грамматики для описания РБНФ, приведенным ниже:
- Грамматика имеет вид
 ```
[Grammar] -> [Rules]
[Rules] -> [Rule] | [Rule][Delim][Rules]
[Rule] -> [Nterm][Arrow][RightPart]  
[RightPart] -> [Operation[]Start][Term][ConcOrAlt][Rightpart][Operation[]End][ConcOrAlt][Rightpart] | 
               [Operation[]Start][Nterm][ConcOrAlt][Rightpart][Operation[]End][ConcOrAlt][Rightpart] |
               [Operation[]Start][Rightpart][Operation[]End][ConcOrAlt][Rightpart] |
               [Operation()Start][Term][ConcOrAlt][Rightpart][Operation()End][ConcOrAlt][Rightpart] |
               [Operation()Start][Nterm][ConcOrAlt][Rightpart][Operation()End][ConcOrAlt][Rightpart] |
               [Operation()Start][Rightpart][Operation()End][ConcOrAlt][Rightpart] |
               [Operation{}Start][Term][ConcOrAlt][Rightpart][Operation{}End][ConcOrAlt][Rightpart] |
               [Operation{}Start][Nterm][ConcOrAlt][Rightpart][Operation{}End][ConcOrAlt][Rightpart] |
               [Operation{}Start][Rightpart][Operation{}End][ConcOrAlt][Rightpart] |
               [Term] [Concatoralternative][Rightpart] |
               [Nterm][Concatoralternative] [Rightpart] | 
               [Empty] 

[ConcOrAlt] -> [OperationConcat] | [OperationAlternative]
[OperationAlternative] -> '|'
[Term] -> [TermStart] [TermStr] [TermEnd]  
[Nterm] -> [NtermStart] [NtermStr] [NtermEnd] 
[NtermStr] -> [A-Z][NtermStr] | [A-Z]
[TermStr] -> [a-z]
 ```  
- Пользовательская грамматика имеет следующие ограничения (при несоблюдении программа аварийно завершится):
1. Если пользователь не указывает значение каких-либо параметров, то будут подставлены значения по умолчанию. Соответсвенно пользователь должен вводить грамматику, учитывая значения по умолчанию для тех параметров, значения которых он оставил пустыми.  
(Например,  в классической грамматике РБНФ в качестве повторения используются символы фигурных скобок ```{}```, если пользователь не задаст параметры ```[Operation{}Start]=``` и ```[Operation{}End]=```, то программа подставит значения по умолчанию, т.е.  
```[Operation{}Start]= {``` и ```[Operation{}End]= }```  
Если пользователь далее введет грамматику без учета значений параметров по умолчанию, то программа выдаст ошибку и завершится)
2. В качестве символов операций, символов начала и конца нетерминалов и терминалов, символа пустоты не могут быть использованы буквы и буквенные последовательности  
(Например, НЕЛЬЗЯ: [NtermStart] = AA, [Empty] = eof, НО МОЖНО: [NtermStart] = AA_, [Empty] = '#')
3. Терминалы и нетерминалы в любом случае должны иметь символы начала и конца  
(‘a’, ‘A’ - по умолчанию, если пользователь не выберет обрамляющие символы сам)
4. Пробел запрещен для обозначения каких-либо параметров пользовательской грамматики
5. Символы разных операций (итераций) должны быть различными.  
(Например, повторение и группировка не могут задаваться одинаковыми скобками) 
6. Начальным мы считаем нетерминал S (т.е. ```[NtermStart] S [NtermEnd]```, если его не будет программа не сработает.  

- Параметры пользовательской грамматики

| Название параметра  | Применение      | Значение по умолчанию |
|:-------------------: |:---------------:| :---------------------:|  
| [Delim]  | разделитель правил грамматики  | '\n - перевод строки' |
| [Arrow]  | разделитель левой и правой части правила  | '->' |
| [Empty]  | символ пустоты (обозначение для ε)  | '' - пустая строка |
| [TermStart] | символ начала терминала  | '' - пустая строка |
| [TermEnd]  | символ конца терминала  | '' - пустая строка |
| [NtermStart]  | символ начала нетерминала  | ' - одинарная кавычка |
| [NtermEnd]  | символ конца нетерминала  | ' - одинарная кавычка |  
| [Operation[]Start]  | символ начала операции условное вхождение  | [ |
| [Operation[]End]  | символ конца операции условное вхождение  | ] |    
| [Operation{}Start]]  | символ начала операции повторение  | { |
| [Operation{}End]  | символ конца операции повторение  | } | 
| [Operation()Start]  | символ начала операции группировка  | ( |
| [Operation()End]  | символ конца операции группировка  | ) | 
| [OperationConcat]  | символ конкатенации   | , | 
    
## Формат выходных данных 
КС грамматика на выходе имеет следующий вид 
 ```
[Grammar] -> [Rules]
[Rules] -> [Rule] | [Rule][Delim][Rules]
[Rule] -> [Nterm][Arrow][RightPart]  
[RightPart] -> [Term][RighPart]|[NTerm][RightPart]|[Empty]
[Term] -> [TermStart] [TermStr] [TermEnd]  
[Nterm] -> [NtermStart] [NtermStr] [NtermEnd] 
[NtermStr] -> [A-Z][NtermStr] | [A-Z]
[TermStr] -> [a-z]
 ```  
если будем делать доп, то добавляем параметры и говорим, что параметры выходной грамматики можно задать в текстовом файле, например CFGsyntax.txt 

Все параметры, которые встречаются в грамматике, но не имеют правил раскрытия, пользователь может задать самостоятельно.
По умолчанию программа задаёт эти параметры, опираясь на встроенный синстаксис. Ниже вы можете ознакомиться со значениями по умолчанию. 

| Название параметра  | Применение      | Значение по умолчанию |
|:-------------------: |:---------------:| :---------------------:|  
| [Delim]  | разделитель правил грамматики  | '\n - перевод строки' |
| [Arrow]  | разделитель левой и правой части правила  | '->' |
| [Empty]  | символ пустоты (обозначение для ε)  | '' - пустая строка |
| [TermStart] | символ начала терминала  | '' - пустая строка |
| [TermEnd]  | символ конца терминала  | '' - пустая строка |
| [NtermStart]  | символ начала нетерминала  | ' - одинарная кавычка |
| [NtermEnd]  | символ конца нетерминала  | ' - одинарная кавычка |  


## Пример использования
syntax.txt  
```данные из файла syntax.txt```

input.txt  
```данные из файла input.txt```

CFGsyntax.txt (опционально)  
```данные из файла VFGsyntax.txt```

Результат или output.txt (если результат записываем в файл)  
```выходные данные или данные из файла output.txt```

### место для высказывания эмоций 
какой ужас.....  
жжжужас 🐝  
мы аморалы мы конкатенируем запятой ! 
