script-to-bytecode by Alvaro Perez Alarcon
==========================================

Script-To-Bytecode is a toolkit of compilers with the purpose of creating easily maintainable script languages to be compiled into a simple bytecode, representing a series of commands in the format code-arguments. It is an auxiliary tool to be used in the development of other programs. An example of use case (the one in mind during development) would be creating the script language used in the events of an RPG. In a more abstract, general perspective, the scripts generated with these tools are an interface to call a number of functions from your program.

With Script-To-Bytecode, you can define your own script language, and use it to compile scripts into a format comprised of a sequence of commands represented as a code and a list of arguments. "raw" bytes or strings can also be inserted into a script if desired. The program that will then run the script must understand this format. To facilitate this task, the Language Compiler can also generate a generic C code that reads all the commands contained into the language.

Moreover, the design of the languages separates commands from instructions, where a command is what your program understands and has a rigid format, while instructions are a set of commands grouped into a single unit that allows a C-ish script style. The process of compiling a script identifies the instructions it contains, translates them into commands and then outputs the corresponding bytecode. In other words, instructions are a more user-friendly layer above commands. This way, the implementation of the instructions can change without affecting the scripts.

For instance, the following language...

    #lang example1:
    #let var:2; ##register identifiers
    #command add 0x5 {dst:var ; src1:var ; src2:var};
    #instruction $a:var = $b:var + $c:var   :{ add a b c; };
    
... applied to the following script ...

    #target 10: #lang example1;
    #let _v1 : 1;
    #let _v2 : 2;
    #let _v3 : 3;
    _v3 = _v1 + _v2;

Generates the following bytecode in the binary file 10.example1:

    0x5 0x0003 0x0001 0x0002
    
Then, when a program reads 10.example1, it will understand the command and do the corresponding action, presumably assign the sum of variables 1 and 2 to variable 3. Note that since this is not a programming language per se, it doesn't manage variables. That task is up to your progran, which acts as an interpreter of the compiled script.

In adition to this, Script-To-Bytecode allows the declaration of labels or using command blocks, allowing a script to reference different parts of the same script, or a different script, which can be used to control the execution flow.

Script-To-Bytecode is made of three tools:
* The Language Compiler, which generates the info of your language
* The Script Compiler, which uses your language to compile the scripts.
* The Script Decompiler, which reads a compiled script and outputs all the commands it contains, sequentially.
    
All tools work in command line, reading input text files that comply to the syntax of each tool (except the decompiler, which reads any file and identifies any incongruences between the script and the language).
