Script-To-Bytecode included documentation
=========================================

BNF folder
----------
The BNF syntax of the compilers that form this toolkit.


example folder
--------------
Contains an example language and scripts for visual reference. This language has a number of simple commands, partially based on an assembly language, and instructions that refer to them. Language and scripts demonstrate most of the features of the toolkit.

It also contains the C source code of a program that reads scripts written in the example language. This program was created from the code generated in the LanguageCompiler using the option -c. This code uses the c header file test_lang.h, generated using the options -lwh during language compilation.

To compile the scripts properly, you have to compile the language first, then the scripts in this order: resources.stbs first, then routines.stbs and finally test.stbs.

LanguageCompiler -lwhc example.stbl
ScriptCompiler -se resources.stbs routines.stbs test.stbs 


highlight folder
----------------
Contains highlighting info that can be used to highlight different kinds of tokens in notepad++. It's very simple, but helps reading the files, specially language definitions, as they can be messier. They recognize extension stbl for language definitions and stbs for scripts.