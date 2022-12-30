# EBNF-to-CFG
Программа реализует преобразование РБНФ в пользовательском синтаксисе в классическую КС-грамматику.
## Формат входных данных  
Для работы с программой необходимо создать следующие файлы в директории ```tests/test<номер теста>/```:  
```syntax.txt``` - файл с пользовательскими параметрами (если файла нет, используются значения параметров по умолчанию);  
```input.txt``` - файл с пользовательской грамматикой;  
```CFGsyntax.txt``` - файл с пользовательскими параметрами для КС-грамматики (если файла нет, используются значения параметров по умолчанию);  

**Пользовательская грамматика должна соответствовать правилам общей грамматики для описания РБНФ, приведенным ниже:**
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
[Term] -> [TermStart] [TermStr] [TermEnd]  
[Nterm] -> [NtermStart] [NtermStr] [NtermEnd] 
[OperationAlternative] -> '|'  
[OperationConcat] ->  
[NtermStr] -> [A-Z]
[TermStr] -> [a-z]
[Operation[]Start] -> '['  
[Operation[]End] -> ']'  
[Operation{}Start] -> '{'  
[Operation{}End] -> '}'  
[Operation()Start] -> '('  
[Operation()End] -> ')'  
 ```  
Все параметры, которые встречаются в грамматике, но не имеют правил раскрытия, пользователь может задать самостоятельно.
По умолчанию программа задаёт эти параметры, опираясь на встроенный синстаксис. Ниже вы можете ознакомиться со значениями по умолчанию.  

**Параметры пользовательской РБНФ грамматики**
| Название параметра  | Применение      | Значение по умолчанию |
|:-------------------: |:---------------:| :---------------------:|  
| [Delim]  | разделитель правил грамматики  | '\n - перевод строки' |
| [Arrow]  | разделитель левой и правой части правила  | '->' |
| [TermStart] | символ начала терминала  | '' - пустая строка |
| [TermEnd]  | символ конца терминала  | '' - пустая строка |
| [NtermStart]  | символ начала нетерминала  | ' - одинарная кавычка |
| [NtermEnd]  | символ конца нетерминала  | ' - одинарная кавычка |  
| [Empty]  | символ пустоты (обозначение для ε)  | '#' - пустая строка |

- Пользовательская РБНФ грамматика имеет следующие ограничения (при несоблюдении программа аварийно завершится):
1. Пользователь должен вводить грамматику в ```input.txt```, учитывая значения по умолчанию для тех параметров, значения которых он оставил пустыми в файле ```syntax.txt```.  
(Например, в РБНФ грамматике по умолчанию в качестве обрамляющих символов для нетерминалов используются одинарные кавычки ```'S'```, если пользователь не задаст параметры (или оставит пустыми) ```NtermStart=``` и ```NtermEnd=```, то программа подставит значения по умолчанию, т.е.  
```NtermStart= '``` и ```NtermEnd= '}```  
Если пользователь далее введет грамматику без учета значений параметров по умолчанию, то программа выдаст ошибку и завершится.)
2. Пробел запрещен для обозначения каких-либо параметров пользовательской грамматики.
3. Начальным считается нетерминал, правило переписывания которого введено первым в ```input.txt```.  
4. Символ пустоты имеет обозначение ```#```.   
    
## Формат выходных данных 
В результате выполнения программы в директории ```tests/test<номер теста>/``` будет создан файл ```output.txt``` с построенной КС-грамматикой.  
**КС-грамматика на выходе имеет следующий вид:**  
 ```
[Grammar] -> [Rules]
[Rules] -> [Rule] | [Rule][Delim][Rules]
[Rule] -> [Nterm][Arrow][RightPart]  
[RightPart] -> [RightPart1]|[Empty]
[RightPart1] -> [Term]|[Nterm]|[Nterm][ConcatorAlternative][RightPart1]|[Term][ConcatorAlternative][RightPart1]
[ConcatorAlternative]->[Concat]|[Alternative]
[Term] -> [TermStart] [TermStr] [TermEnd]  
[Nterm] -> [NtermStart] [NtermStr] [NtermEnd] 
[NtermStr] -> [A-Z]
[TermStr] -> [a-z]
 ```  
Все параметры, которые встречаются в грамматике, но не имеют правил раскрытия, пользователь может задать самостоятельно.
По умолчанию программа задаёт эти параметры, опираясь на встроенный синстаксис. Ниже вы можете ознакомиться со значениями по умолчанию. 

**Параметры КС-грамматики**
| Название параметра  | Применение      | Значение по умолчанию |
|:-------------------: |:---------------:| :---------------------:|  
| [Delim]  | разделитель правил грамматики  | '\n - перевод строки' |
| [Arrow]  | разделитель левой и правой части правила  | '->' |
| [Empty]  | символ пустоты (обозначение для ε)  | '' - пустая строка |
| [TermStart] | символ начала терминала  | '' - пустая строка |
| [TermEnd]  | символ конца терминала  | '' - пустая строка |
| [NtermStart]  | символ начала нетерминала  | ' - одинарная кавычка |
| [NtermEnd]  | символ конца нетерминала  | ' - одинарная кавычка |  
| [Concat] | символ для конкатенации |' - пустая строка'|
|[Alternative] | символ для альтернативы | '\|'|

## Пример использования
syntax.txt  
```
Delim= 
Arrow= :=
NtermStart= SS
NtermEnd= SS
```

input.txt  
```
SSKSS := [a {SSSSS}] | b | # 
SSSSS := c | #
```

CFGsyntax.txt   
```
Arrow= =>
Empty= @
Alternative= \\
```

output.txt (и в терминале)   
```
'K'=>'A0'
'A0'=>'A2'\\'A1'
'A1'=>b\\@
'A2'=>a'A3'\\@
'A3'=>'S''A3'\\@
'S'=>'A4'
'A4'=>c\\@
```
