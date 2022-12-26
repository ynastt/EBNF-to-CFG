# EBNF-to-CFG
Программа реализует преобразование РБНФ в пользовательском синтаксисе в классическую КС-грамматику.
## Формат входных данных
Грамматика, введенная пользователем, должна соответствовать правилам грамматики для описания РБНФ, приведенным ниже:
> место для правил и ```[Grammar] -> [Rules]
>                       [Rules] -> [Rule] | [Rule][Delim] [Rules]
                        [Delim] -> ’\n’ | ’;’ | ‘/ ’ 
                        [Rule] -> [Nterm] [Arrow] [RightPart]  
                        [RightPart] -> [Operation[]Start] [Term] [Concatoralternative] [Rightpart] [Operation[]End] [Concatoralternative]  [Rightpart]|                                         [Operation[]Start]  [Nterm] [Concatoralternative][Rightpart] [Operation[]End] [Concatoralternative][Rightpart] |
                        [Operation[]Start] [Rightpart] [Operation[]End] [Concatoralternative] [Rightpart] |
                        [Term] [Concatoralternative][Rightpart] | [Nterm][Concatoralternative] [Rightpart] | [Empty] |
                        [Operation()Start] [Term] [Concatoralternative] [Rightpart] [Operation()End] [Concatoralternative]  [Rightpart] |
                        [Operation()Start]  [Nterm] [Concatoralternative][Rightpart] [Operation()End] [Concatoralternative][Rightpart]|
                        [Operation{}Start] [Term] [Concatoralternative] [Rightpart] [Operation{}End] [Concatoralternative]  [Rightpart] |
                        [Operation{}Start]  [Nterm] [Concatoralternative][Rightpart] [Operation{}End] [Concatoralternative][Rightpart]  
                        [Concatoralternative]->[OperationConcat]|[OperationAlternative]
                        [Term] ->[TermStart] [TermStr] [TermEnd]  
                        [Nterm] -> [NtermStart] [NtermStr] [NtermEnd] 
                        [NtermStr] -> [A-Z] [NtermStr] | [A-Z]
                        [TermStr] -> [a-z]
```
## Формат выходных данных
> место для описания классической КС-грамматики, которая будет на выходе, и если будем делать доп, то говорим, что параметры выходной грамматики можно задать в текстовом файле, например CFGsyntax.txt 
## Пример использования
syntax.txt
```
данные из файла syntax.txt
```

input.txt
```
данные из файла input.txt
```

CFGsyntax.txt (опционально)
```
данные из файла VFGsyntax.txt
```

Результат или output.txt (если результат записываем в файл)
```
выходные данные или данные из файла output.txt
```
### место для высказывания эмоций 
какой ужас.....
жжжужас 🐝
