---
title: "Compiladores — Trabalho Prático 1"
subtitle: "Analisador léxico e sintático para um subconjunto de Pascal"
author:
  - "Gonçalo Sousa (up202403669)"
  - "Matilde Alves (up202403407)"
date: "Faculdade de Ciências da Universidade do Porto — Outubro 2026"
lang: pt-PT
toc: true
---

# 1 Introdução

Este trabalho consiste num analisador léxico e sintático para um pequeno subconjunto da linguagem Pascal. O subconjunto inclui:

- a declaração de variáveis inteiras e booleanas;
- atribuições;
- condições (`if-then-else`) e ciclos (`while-do`);
- blocos `begin ... end`;
- operações de leitura e escrita;
- expressões aritméticas, relacionais e booleanas.

O analisador lexical (Flex) divide o programa em tokens. O analisador sintático (Bison) verifica se esses tokens respeitam a gramática e, ao mesmo tempo, constrói a árvore sintática abstrata (AST) do programa, que no fim é impressa no ecrã. Se o programa tiver um erro, é indicada a linha onde foi encontrado. O código foi escrito em C e segue a estrutura do código de apoio à geração de ASTs fornecido nas aulas práticas.

**Nota:** o exemplo usado neste documento é o ficheiro `tests/exemplo3.pas`.

# 2 Ferramentas

## 2.1 Requisitos

Para compilar e correr o trabalho é preciso ter instalado o `make`, o `flex`, o `bison` (versão 3) e o `gcc`.

## 2.2 Compilação

A forma mais simples de compilar é usar o Makefile:

```
make
```

Este comando gera os ficheiros `parser.c`, `parser.h` e `scanner.c`, compila cada ficheiro C para um objeto (`.o`) e cria o executável `pascal`. Também é possível compilar manualmente:

```
bison --defines=parser.h -o parser.c parser.bison
flex -o scanner.c scanner.flex
gcc -o pascal scanner.c parser.c ast.c printAbsTree.c main.c
```

Para apagar os ficheiros gerados usa-se `make clean`.

## 2.3 Execução

O programa recebe como argumento o ficheiro Pascal a analisar e imprime a AST:

```
./pascal caminho/para/ficheiro.pas
```

Se não for dado nenhum ficheiro, o programa lê do *standard input*. Para correr todos os exemplos da pasta `tests/` de uma vez, mostrando também o código de saída de cada um:

```
make test
```

O programa devolve 0 se a análise correr bem e 1 se houver um erro.

## 2.4 Estrutura de diretórios

```
|-- COMP_TP1/
    |-- scanner.flex
    |-- parser.bison
    |-- ast.h
    |-- ast.c
    |-- printAbsTree.c
    |-- main.c
    |-- Makefile
    |-- README.pdf
    |-- tests/
        |-- exemplo1.pas
        |-- exemplo2.pas
        |-- exemplo3.pas
        |-- exemplo4.pas
        |-- erro.pas
        |-- erro_lexico.pas
        |-- erro_relacional.pas
```

| Ficheiro | Conteúdo |
|---|---|
| `scanner.flex` | analisador lexical |
| `parser.bison` | gramática e construção da AST nas ações semânticas |
| `ast.h`, `ast.c` | tipos da AST e funções construtoras |
| `printAbsTree.c` | impressão da AST |
| `main.c` | abre o ficheiro, chama o parser e imprime a árvore |

Os ficheiros `exemplo*.pas` são programas corretos:

- `exemplo4.pas` testa palavras reservadas em maiúsculas, plicas dentro de strings, comentários `(* *)`, `writeln()` e o *dangling else*;
- os ficheiros `erro*.pas` têm, respetivamente, um erro de sintaxe, um erro lexical e uma comparação encadeada (`a < b < c`), que não é permitida.

# 3 Frontend

## 3.1 Análise lexical e sintática

### 3.1.1 Tokens

O analisador lexical reconhece:

- **Palavras reservadas:** `program var integer boolean begin end if then else while do write writeln read readln true false div mod and or not`;
- **Operadores e símbolos:** `:= <> <= >= + - * / = < > ( ) ; : , .`;
- **Números inteiros, strings e identificadores:** as strings ficam entre plicas (`'texto'`), e duas plicas seguidas (`''`) representam uma plica dentro da string;
- **Comentários e espaços:** os comentários `{ ... }`, `(* ... *)` e `//` são ignorados, tal como os espaços.

Cada token tem um nome (por exemplo `PLUS`, `ASSIGN`, `BEGIN_T`), declarado no `parser.bison`. O token `begin` chama-se `BEGIN_T` porque `BEGIN` já é uma macro do Flex.

As regras de desambiguação do Flex funcionam assim:

- **First match:** as palavras reservadas estão escritas antes da regra dos identificadores. Quando há empate (por exemplo `if`), ganha a primeira regra.
- **Longest match:** nos operadores de dois caracteres, como `<=`, ganha a regra que lê mais caracteres.
- **Maiúsculas e minúsculas:** com `%option caseless`, o Pascal não distingue maiúsculas de minúsculas (`BEGIN` é igual a `begin`). Os identificadores guardam o texto original.

Nos tokens com valor, o número é convertido com `atoi` e o texto dos identificadores e strings é copiado com `strdup`, porque o `yytext` é reescrito a cada token. Um caracter que não pertença à linguagem, ou um comentário que não seja fechado, dá erro.

### 3.1.2 Gramática

A gramática deste subconjunto é a seguinte:

- **Programa:** `program {nome}; {declarações} begin {comandos} end.`
- **Declarações:** opcionais; começam por `var` e têm uma ou mais linhas `{ids} : {tipo};`, com tipo `integer` ou `boolean`
- **Comandos:** zero ou mais, separados por `;` (o comando vazio permite um `;` antes de `end`)
  - Atribuição: `{id} := {expressão}`
  - If-then: `if {expressão} then {comando}`
  - If-then-else: `if {expressão} then {comando} else {comando}`
  - Ciclo while: `while {expressão} do {comando}`
  - Bloco: `begin {comandos} end`
  - Escrita: `write({args})`, `writeln({args})`, `writeln()` ou `writeln`, onde os argumentos são expressões ou strings
  - Leitura: `read({ids})` ou `readln({ids})`
- **Expressões:**
  - Números inteiros, variáveis, `true` e `false`
  - Menos unário (`-`) e negação (`not`)
  - Operações aritméticas: `+ - * / div mod`
  - Operações booleanas: `and or`
  - Operações relacionais: `= <> < <= > >=`
  - `(expressão)`

As listas (identificadores, argumentos, linhas de declarações e comandos) usam recursividade à direita, por exemplo `ids: ID | ID COMMA ids`. Assim, cada lista é construída diretamente pela ordem em que aparece no programa.

### 3.1.3 Precedência de operadores

A regra das expressões é ambígua: por exemplo, `2+3*4` tem duas árvores possíveis. Para o Bison escolher a árvore correta, a precedência e a associatividade dos operadores são declaradas com `%left`, `%nonassoc` e `%precedence`, e cada linha tem mais prioridade do que a anterior. Seguindo o Pascal, da precedência mais alta para a mais baixa:

- Menos unário e `not` (estão na regra `factor`, por isso aplicam-se só ao operando que vem a seguir)
- Multiplicativos: `* / div mod and` (associativos à esquerda)
- Aditivos: `+ - or` (associativos à esquerda)
- Relacionais: `= <> < <= > >=` (não associativos: `a < b < c` é erro)

Como em Pascal, o `and` tem mais prioridade do que os relacionais. Por isso `a > b and c` é lido como `a > (b and c)`, e as comparações combinadas com `and`/`or` precisam de parênteses, como em `(a > b) and not (a = 0)`.

O mesmo mecanismo resolve o *dangling else*: o `ELSE` tem mais precedência do que o `THEN`, por isso o `else` fica sempre ligado ao `if` mais próximo. Com estas declarações, o Bison não reporta conflitos nem avisos (`bison -Wall`).

## 3.2 Árvore sintática abstrata (AST)

A AST segue a estrutura apresentada nas aulas. Cada categoria sintática é uma estrutura com:

- uma etiqueta (`kind`);
- uma união de alternativas (`attr`);
- funções construtoras `ast_*`, que reservam o nó com `malloc` e inicializam todos os campos.

As expressões são representadas por:

```c
struct _Expr {
  enum { E_INTEGER, E_BOOLEAN, E_VARIABLE, E_STRING,
         E_OPERATION, E_UNARY } kind;
  union {
    int value;                 // inteiros e booleanos
    char* name;                // variáveis
    char* text;                // strings
    struct { int operator; struct _Expr *left, *right; } op;
    struct { int operator; struct _Expr *child; } unary;
  } attr;
};
typedef struct _Expr Expr;
```

Os comandos são representados por:

```c
struct _Cmd {
  enum { C_ASSIGN, C_IF, C_WHILE, C_SEQ, C_WRITE, C_READ } kind;
  union { ... } attr;
};
typedef struct _Cmd Cmd;
```

Existem ainda as listas `ExprList` (argumentos do `write`) e `IdList` (variáveis do `read` e das declarações), as declarações `Decl` e o programa `Program`, que é a raiz da árvore.

Algumas escolhas de representação:

- **Operadores:** são guardados pelo código do token (`PLUS`, `LT`, ...), como em `ast_operation(PLUS, $1, $3)`.
- **Sequências:** `c1; c2` é representada por um nó `C_SEQ` (`ast_seq`). Um bloco `begin ... end` não precisa de nó próprio: é apenas a sequência dos seus comandos. Os comandos vazios não entram na árvore.
- **Booleanos:** as expressões aritméticas e booleanas usam o mesmo tipo `Expr`. Numa atribuição como `x := a`, o parser não consegue saber se `a` é inteira ou booleana. Separar os dois tipos de expressão na gramática criaria conflitos, e essa verificação é feita na análise semântica (verificação de tipos).

A AST é impressa pelas funções `printExpr`, `printCmd` e `printProgram` (`printAbsTree.c`), em pré-ordem, com 2 espaços de indentação por nível. As etiquetas usadas são:

- `Program`, `Declaracoes`, `Decl`, `Var`, `Corpo`;
- `Assign`, `If`, `While`, `Write`, `Writeln`, `Read`, `Readln`;
- `Int`, `String`, `true`, `false` e o símbolo de cada operador.

Um `If` tem 2 filhos (condição e *then*) ou 3 (com *else*). O menos unário aparece como `-` com um só filho.

Para o programa `tests/exemplo3.pas`:

```
// ciclo while: soma de 1 a n
program Soma;
var n, i, s : integer;
begin
  readln(n);
  i := 1;
  s := 0;
  while i <= n do
  begin
    s := s + i;
    i := i + 1
  end;
  writeln('soma = ', s)
end.
```

a saída é:

```
Program Soma
  Declaracoes
    Decl integer
      Var n
      Var i
      Var s
  Corpo
    Readln
      Var n
    Assign i
      Int 1
    Assign s
      Int 0
    While
      <=
        Var i
        Var n
      Assign s
        +
          Var s
          Var i
      Assign i
        +
          Var i
          Int 1
    Writeln
      String 'soma = '
      Var s
```

## 3.3 Erros

Os erros são escritos no *standard error* e o programa termina com código de saída 1. Há dois tipos:

- **Erros lexicais:** um caracter inválido ou um comentário não fechado. São detetados no Flex, que indica a linha e o texto.
- **Erros de sintaxe:** o Bison chama a função `yyerror`. Com `%define parse.error verbose`, a mensagem indica o token inesperado. A linha vem da `%option yylineno` do Flex.

| Ficheiro | Saída |
|---|---|
| `tests/erro.pas` (falta o segundo operando de uma soma) | `Linha 4: syntax error, unexpected end - 'end'` |
| `tests/erro_lexico.pas` | `Linha 4: caracter invalido - '@'` |
| `tests/erro_relacional.pas` | `Linha 4: syntax error, unexpected < - '<'` |

Em `erro.pas` o operando falta na linha 3, mas o erro é indicado na linha 4. O parser só deteta o erro quando lê o token seguinte, o `end`.

# 4 Conclusão

Neste trabalho foi desenvolvido um analisador léxico e sintático para um subconjunto de Pascal, usando o Flex e o Bison. O programa reconhece os tokens da linguagem, verifica a sintaxe, constrói a AST e imprime-a, indicando a linha onde ocorrem erros.

Foi possível pôr em prática a matéria das aulas:

- expressões regulares e as regras de desambiguação do Flex;
- gramáticas independentes de contexto;
- a resolução de ambiguidades com precedências e associatividade;
- a construção da AST nas ações semânticas, com a estrutura de etiqueta, união e construtores vista nas aulas.

O trabalho tem algumas limitações:

- não faz análise semântica: não verifica se as variáveis foram declaradas nem se os tipos estão certos (por exemplo, `/` é tratado como os restantes operadores, sem distinguir a divisão real);
- as strings só podem aparecer como argumentos do `write`;
- a memória da AST não é libertada, porque a árvore é usada até ao fim do programa.

# 5 Referências

- Slides das aulas teóricas e folhas das aulas práticas da unidade curricular, incluindo o código C de apoio à geração de ASTs
- Manual do GNU Bison: <https://www.gnu.org/software/bison/manual/>
- Manual do Flex: <https://westes.github.io/flex/manual/>
- Pascal (programming language): <https://en.wikipedia.org/wiki/Pascal_(programming_language)>
- Documentação do Free Pascal: <https://www.freepascal.org/docs.html>
