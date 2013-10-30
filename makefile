default: compile

compile:
	#Language Compiler
	flex  src/LanguageCompiler.l
	bison src/LanguageCompiler.y
	gcc   lex.yy.c -DSYNTACTIC -o LanguageCompiler  src/common.c -ly -lfl
	rm -f lex.yy.c LanguageCompiler.tab.c 
	
	#Script Compiler
	flex  src/ScriptCompiler.l
	bison src/ScriptCompiler.y
	gcc   lex.yy.c -DSYNTACTIC -o ScriptCompiler    src/common.c -ly -lfl
	rm -f lex.yy.c ScriptCompiler.tab.c 
	
	#Script Decompiler
	gcc src/ScriptDecompiler.c -o ScriptDecompiler src/common.c

syntaxdebug:
	#Language Compiler
	flex        src/LanguageCompiler.l
	bison -vt   src/LanguageCompiler.y
	gcc         lex.yy.c -DNOTSYNTACTIC -o LanguageCompilerLex  src/common.c -ly -lfl -g
	gcc         lex.yy.c -DSYNTACTIC    -o LanguageCompiler     src/common.c -ly -lfl -g -ggdb -fno-inline -DDEBUG_TRACEALL
	
	#Script Compiler
	flex        src/ScriptCompiler.l
	bison -vt   src/ScriptCompiler.y
	gcc         lex.yy.c -DNOTSYNTACTIC -o ScriptCompilerLex    src/common.c -ly -lfl -g
	gcc         lex.yy.c -DSYNTACTIC    -o ScriptCompiler       src/common.c -ly -lfl -g -ggdb -fno-inline -DDEBUG_TRACEALL

debug:
	#Language Compiler
	flex        src/LanguageCompiler.l
	bison -vt   src/LanguageCompiler.y
	gcc         lex.yy.c -DSYNTACTIC    -o LanguageCompiler src/common.c -ly -lfl -g -ggdb -fno-inline
	rm -f lex.yy.c LanguageCompiler.tab.c 
	
	#Script Compiler
	flex        src/ScriptCompiler.l
	bison -vt   src/ScriptCompiler.y
	gcc         lex.yy.c -DSYNTACTIC    -o ScriptCompiler   src/common.c -ly -lfl -g -ggdb -fno-inline
	rm -f lex.yy.c src/ScriptCompiler.tab.c 
	
	#Script Decompiler
	gcc src/ScriptDecompiler.c -o ScriptDecompiler src/common.c -g -ggdb -fno-inline

clean:
    #Temporal files generated in debug
	rm -f LanguageCompiler.output ScriptCompiler.output
	rm -f lex.yy.c LanguageCompiler.tab.c ScriptCompiler.tab.c
    #On Unix systems
	rm -f ScriptCompiler LanguageCompiler LanguageCompilerLex ScriptCompilerLex
    #On windows systems
	rm -f ScriptCompiler.exe LanguageCompiler.exe LanguageCompilerLex.exe ScriptCompilerLex.exe


