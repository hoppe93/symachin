# symachin — the Symbol Manipulator for speCHial Individuals
`symachin` is a small symbol manipulator, designed to help with accounting during
long calculations. While the language is rather limited (only supporting addition,
subtraction and multiplication in expressions) it can greatly help in tedious
calculations which mostly include substitutions of relations on many terms.

Note that `symachin` was implemented in a short amount of time as a simple helper
tool (initially just as a C++ library) for a particular problem, and thus uses
very simple internal logic and has not been extensively optimized.

*For those curious: symachin was designed to help in deriving an expression for
the angular distribution of synchrotron radiation emitted by a charged particle
moving according to the first order guiding-center equations of motion. This
problem is relevant to the study of runaway electron physics in tokamak fusion
plasmas.* http://ft.nephy.chalmers.se/retools

Table of Contents
=================
<!--ts-->
   * [Compiling and running](#compiling-and-running)
      * [Preliminaries](#preliminaries)
      * [Compiling](#compiling)
      * [Running](#running)
   * [Basic concepts](#basic-concepts)
      * [Expressions and Rules](#expressions-and-rules)
      * [Comments](#comments)
      * [Labels and References](#labels-and-references)
      * [Numbers, mathematical operators and negation](#zero,-mathematical-operators-and-negation)
      * [Words and Strings](#words-and-strings)
      * [Equality](#equality)
      * [Sourcing scripts](#sourcing-scripts)
   * [Commands](#commands)
      * [assign](#--assign-expression-to-label)
      * [apply](#apply--apply-a-rule-to-an-expression)
      * [assert](#assert--assert-that-two-expressions-are-equal)
      * [define](#define--define-a-replacement-rule)
      * [eval](#eval--evaluate-expression-numerically)
      * [group](#group--group-expression-by-factor)
      * [include](#include--include-script)
      * [print](#print--print-a-series-of-terms)
      * [printf](#printf--print-formatted-expression)
      * [printn](#printn--print-the-number-of-terms-in-an-expression)
      * [replace](#replace--replace-term-in-expression)
<!--te-->

Compiling and running
=====================

Preliminaries
-------------
In order to compile `symachin` you will need the following:
- CMake version 3.1 or later (https://cmake.org/)
- A C++11 compliant compiler (only GCC has been tested)

Compiling
---------
To compile `symachin`, do the following (starting from the root `symachin`
directory):
```bash
$ mkdir build
$ cd build
$ cmake ../
$ make
```
If all steps complete successfully the `symachin` library will be named
`build/lib/libsymachin.a` and the interpreter `build/interpreter/isymachin`.

Running
-------
Currently, there is no support for running `symachin` interactively. To run
a `symachin` script, pass it as an argument to the `isymachin` executable.

Basic concepts
==============

Expressions and rules
---------------------
The two fundamental objects operated on/with in `symachin` are *expressions*
and *rules*. **Expressions** are basically the algebraic expressions that are
processed by `symachin`, i.e. they consists of symbols that are added, subtracted
and multiplied together. Formally, an expression in `symachin` is a list of
*terms*, and each term in turn consists of a list of *factors*. Factors are the
smallest units `symachin` works with and has two properties assigned to it:
a *sign* and a *name*. The sign is either positive or negative, and determines
how the factor behaves during addition and subtraction. The name is used to
identify the factor, and any two factors with the same name are considered
equal (in absolute value). The name may consist of any characters that doesn't
hold special meaning to `symachin`.

**Rules**, or more complete, ***replacement*** **rules**, are tell `symachin` how
to replace certain factors in a given expression. When applied, `symachin` will
go through the given expression and replace all occurences of a list of given
factors by the expressions defined in the rule. Thus rules are able to emulate
the behaviour of scalar products, cross products, or other more exotic
operators (such as derivatives, integrals and beyond) when acting on
expressions, while allowing the user to define symbolic relations.

`symachin` keeps one list of expressions and one list of rules during execution.
Both are labeled in the same way, but a label is either associated with an
expression or a replacement rule. While the language syntax would not prevent
an expression and a rule to have the same label, `symachin` enforces this syntax
rule to avoid confusion.

Comments
--------
There are two types of comments in `symachin`. Line comments are started by `#`
and ended by a newline character. Multiline comments are also allowed and have
the same syntax as in the C family of languages. They are started with `/*` and
ended by `*/`.

Labels and references
---------------------
Expressions can be labeled and later used by referencing their labels. A label
is always declared at the start of the line and must be surrounded by square
brackets, `[` and `]`. The result of the following command is connected to the
label, and most commands therefore require a label to be declared.

Expressions can later be referenced in other expressions by prefixing the label
with a dollar sign, `$`. Label references may occur within, or in place of, any
*expression*.

Numbers, mathematical operators and negation
-----------------------------------------
`symachin` is able to work with integer factors and treat them as numeric. This
allows `symachin` to combine and cancel proportional and equal expressions,
thus simplifying results. Two numeric factors have special meaning, and these
are zero (`0`) and one (`1`). Zeros are automatically eliminated, along with any
term they multiply, but are used to indicate empty expressions or as helpers to
negate expressions. Ones are also eliminated if they are not the only factor of
a term, and they do not eliminate any other terms in an expression.

Three different mathematical operations are implemented in `symachin`:
addition (`+`), subtraction (`-`) and multiplication (`*`). The latter is used
to combine factors into terms, while the two former are used to combine terms
into expressions.

There is no negation operation in `symachin`, but negation of an expression can
still be achieved using `0`. By subtracting an expression from zero, it will
become negative.

Words and strings
-----------------
Unrecognized, contiguous (not separated by spaces or operators) symbols in a
`symachin` script are called *words*. A word is converted into a *factor* when
it is placed as part of an expression. While most commands operate on
expressions, some (such as the *replace* command) take words as operands.

Words are automatically separated by any blank space, or operator (mathematical,
comparison or assignment), but it is still possible to include such characters
in a word. To do so, simply surround the word by citation marks (`"`). This is
primarily useful when sourcing other `symachin` scripts.

Equality
--------
Equality is defined as follows in `symachin`:

1. Two expressions are equal if their difference is zero.
2. Two terms are equal if all factors of term 1 also occur the same number of times in term 2.
3. Two factors are equal if they have the same name (or symbol).

The first point connects to another important concept in `symachin`, namely
*proportionality*. Two terms are said to be proportional to each other if they
are equal when they're numerical factors are not considered. This becomes
important when `symachin` tries to simplify expressions, as any two terms that
are proportional to each other can be written as a single term with another
numerical factor.

Sourcing scripts
----------------
Scripts can be split across multiple source files, and using the command
`include` the contents of other source files can be executed. When the
interpreter reaches an `include` command, the contents of the sourced script
is executed directly within the current environment, and any expressions or
replacement rules defined by the script are accessible to any code placed
after the `include`.

Commands
========
The commands of `symachin` are best illustrated with the following example:
```
# Other symachin scripts can be included
include "otherfile.sm"

# Assign expression to label
[1]: (a+b) * (c+d) + q;

[some_operator] define      # Define rule
    a -> e+f;
    c -> q-m;
end

# Apply the rule to [1]
[2] apply $some_operator to $1;

# Group the resulting expression
[3] group $2 by
    [a] a;
    [b] c;
    [c] other;
end

# Print the grouped expression
print $3.a;
print $3.b;
print $3.c;
```

: — Assign expression to label
-------------------------------
Syntax: `[label] : <expression>;`

The colon operator `:` assigns an expression to a label and allows you to later
access the expression be referencing the label.

apply — Apply a rule to an expression
--------------------------------------
Syntax: `[label] apply <rule> to <expression>;`

Replaces factors in the expression according to the given rule. The result is
assigned to the label at the start of the line.

assert — Assert that two expressions are equal
-----------------------------------------------
Syntax: `assert <expr> = <expr>;`

Asserts that two expressions are equal. If the assertion fails, the interpreter
throws an error and exits.

define — Define a replacement rule
-----------------------------------
Syntax: `[label] define <factor> -> <expression>; <factor> -> <expression>; ... end`

Defines a set of replacement rules which replace the given factors by the given
expression. The rule is assigned to the label at the start of the line and can
be accessed by referencing it later. Each rule can contain several independent
replacement statements of the form `factor -> expression;`.

eval — Evaluate expression numerically
--------------------------------------
Syntax: `eval <expression> with <factor> = <value>; ... end` or
`eval <expression> with <factor> = <value>; ... assert <value>;`

Evaluates the given expression numerically by replacing all symbolic factors
with corresponding numeric values. The numeric values are given after the `with`
keyword in the format `<factor> = <value>;`. Any symbolic factor not explicitly
listed is automatically assigned the value `1`. To change this default value,
assign the desired value to `other`, i.e. `other = 2.0;`.

It is possible to let `<value>` be a factor which has previously been assigned
a numeric value in the same list.

If the command is ended with `assert <value>;` instead of `end`, `symachin` will
ensure that the given expression evaluates to the given value, and exit with an
error message and non-zero exit code if not.

An example of how to use this command is:
```
[1]: a*a + b*b + c*c;

eval $1 with
    a = 3;
    b = 4;
    c = a;
end
```
which will result in the output
```
34
```
Instead of the `end`, we could also request that `symachin` exit in case the
expression does not evaluate to the expected value:
```
[1]: a*a + b*b + c*c;

eval $1 with
    a = 3;
    b = 4;
    c = a;
assert 34;
```
which should run without generating any output, and exit with code `0`.

group — Group expression by factor
-----------------------------------
Syntax: `[opt. label] group <expression> by [label] <factor>; [label] <factor>; ... [label] other; end`

Groups the given expression into a number of sub-expressions based on which
factors they are multiplied by. The resulting expression corresponding to a
specific factor is assigned to the label `label`, or if an optional global
label is given at the start of the line, to the label `global-label.label`.
An optional last line assigning `other` (which is a reserved word) to a label
assigns all remaining terms to the given label.

As an example, consider the grouping rule
```
[1]: a * (aa + aaa) + b * (bb + bbb) + c;

[2] group $1 by
    [a] a;
    [b] b;
    [c] other;
end

print $2.a;
print $2.b;
print $2.c;
```
which results in the output
```
aa + aaa
bb + bbb
c
```

include — Include script
-------------------------
Syntax: `include <word>`

Includes the given file in the present location of the file.

print — Print a series of terms
---------------------------------
Syntax: `print <term> <term> ...;`

Prints the sequence of objects to `stdout` with spaces in between, as well as
a final newline character.

printf — Print formatted expression
-----------------------------------
Syntax: `printf <expr>;`

Similar to `print`, but formats the given expression slightly before printing
to make easier to read. Primarily, multiple factors occuring in an expression
are replaced with an exponent.

printn — Print the number of terms in an expression
-----------------------------------------------------
Syntax: `printn <expr>;`

Prints the number of terms in the given expression. For example, the code
```
[1]: a + b + c;
printn $1;
```
would result in `3` being printed to the terminal.

replace — Replace term in expression
--------------------------------------
Syntax: `[label] replace <term> -> <expr> in <expr>;`

Replaces all occurences of the given term in the second expression by the
first expression. This command is a short form of the more complete `define`
command.

Example:
```
[1]: a + b;
[2] replace b -> c-a in $1;

print "a + b =" $2;
```
The above results in the output
```
a + b = c
```

