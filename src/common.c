#include "common.h"

size_t mallocd = 0;

// Malloc with error control
void* malloc2 (size_t size)
{
if (size == 0)
    return NULL;
void* buffer = (void*) malloc (size);
if (buffer == NULL) {
	perror ("malloc");
	exit(-1);
	}
return buffer;
}

// Realloc with error control
void* realloc2 (void* alo, size_t size)
{
if (size == 0)
    return NULL;
void* buffer = (void*) realloc (alo, size);
if (buffer == NULL) {
	perror ("realloc");
	exit(-1);
	}
return buffer;
}

// Reads a line and returns a malloc'd copy without the '\n'
char* avanzalinea (FILE* file)
{
char s[1000];
fgets (s, 1000, file);
s[strlen(s) - 1] = 0; // remove eol
char* buffer = (char*) malloc2(strlen(s)+1);
sprintf(buffer, "%s", s);
return buffer;
}

// atoi+free
int atoi2 (char* buffer)
{
int num = atoi (buffer);
free (buffer);
return num;
}

//Returns the number of characters in c parsed to read the character and store it in "to"
char read_char (char* c, char* to)
{
    switch (c[0]) {
        case '\\' :
            switch (c[1]) {
                case 'a':  *to = '\a'; return 2; break;
                case 'b':  *to = '\b'; return 2; break;
                case 'f':  *to = '\f'; return 2; break;
                case 'n':  *to = '\n'; return 2; break;
                case 'r':  *to = '\r'; return 2; break;
                case 't':  *to = '\t'; return 2; break;
                case 'v':  *to = '\v'; return 2; break;
                case '\'': *to = '\''; return 2; break;
                case '\"': *to = '\"'; return 2; break;
                case '\\': *to = '\\'; return 2; break;
                case '?':  *to = '\?'; return 2; break;
                default: break;
                }
            break;
        default: *to = c[0]; return 1; break;
        }
}

//Reads a character
char read_lit_char (char* c)
{
    switch (c[1]) {
        case '\\' :
            switch (c[2]) {
                case 'a':  return '\a'; break;
                case 'b':  return '\b'; break;
                case 'f':  return '\f'; break;
                case 'n':  return '\n'; break;
                case 'r':  return '\r'; break;
                case 't':  return '\t'; break;
                case 'v':  return '\v'; break;
                case '\'': return '\''; break;
                case '\"': return '\"'; break;
                case '\\': return '\\'; break;
                case '?':  return '\?'; break;
                default: break;
                }
            break;
        default: return c[1]; break;
        }
}


//Searches differences between two instruction parameters.
//If local_general is true, when checking labels checks if it's a local label only if "a" is local (permisive pattern matching)
bool param_diff (instruction_param a, instruction_param b, bool local_general)
{
    if (a.type != b.type) return true;
    switch(a.type) {
        case TYPE_ATOM:
            if ( !strcmp(a.name, b.name) ) // equal if identical atom
                return false;
            else return true;
            break;
        case TYPE_LABEL:
            return (!local_general && a.extra && !b.extra) || (!a.extra && b.extra);
            break;
        case TYPE_VAL:   return false; break; //Widths are not checked
        case TYPE_BLOCK: //One is a block and the other is a string
            return (a.name[0] == '\"' && b.name[0] != '\"')
            ||     (a.name[0] != '\"' && b.name[0] == '\"');
            break; 
        default: return false;
        }
}

//Printing functions
extern int line, column;
extern bool verbose_mode;
void yyerror (const char *s)
{
    fprintf (stderr, "%d.%d: %s\n", line, column, s);
    exit(-1);
}

void err_lex(char* fmt, ...)
{
    fprintf(stderr, "Lexical error: ");
    va_list ap; va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    yyerror(msg);
}
void err_sin(char* fmt, ...)
{
    fprintf(stderr, "Syntax error: ");
    va_list ap; va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    yyerror(msg);
}
void err_sem(char* fmt, ...)
{
    fprintf(stderr, "Semantic error: ");
    va_list ap; va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    yyerror(msg);
}
void err_other(char* fmt, ...)
{
    fprintf(stderr, "Error: ");
    va_list ap; va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    yyerror(msg);
}
void warn_sem(char* fmt, ...)
{   
    fprintf(stderr, "Warning: ");
    va_list ap; va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    fprintf (stderr, "%d.%d: %s\n", line, column, msg);
}

void verbose(char* fmt, ...)
{
    if (!verbose_mode) return;
    va_list ap;
    va_start(ap, fmt);
    char msg[1000];
    vsprintf(msg, fmt, ap); va_end(ap);
    fprintf (stdout, "%d.%d:\t%s\n", line, column, msg);
}

//Write a translation to a given, open file
void writetranslation(FILE* f, translation tr)
{
    unsigned int j, len;
    for (j=0; j<tr.size; j++) {
        Translation_item t = tr.items[j];
        enum translation_type type = t->type;
        fwrite(&t->type, sizeof(enum translation_type), 1, f);
        switch(type) {
            case TRANS_COMMAND:
                len = strlen(t->value.command.name);
                fwrite(&len, sizeof(unsigned int), 1, f);
                fprintf(f, "%s", t->value.command.name);
                fwrite(&t->value.command.code, sizeof(int), 1, f);
                break;
            case TRANS_LIT:
                fwrite(&t->value.number.val, sizeof(int), 1, f);
                fwrite(&t->value.number.width, sizeof(char), 1, f);
                break;
            case TRANS_CURTARGET:
                break;
            default:
                len = strlen(t->value.str);
                fwrite(&len, sizeof(unsigned int), 1, f);
                fprintf(f, "%s", t->value.str);
                break;
            }
        }
}

// Writes a language to file
void writelanguage(FILE* f)
{
    unsigned int len, i, j, k, l;

    //Options
    len = strlen(lang.options.name);
    fwrite(&len, sizeof(unsigned int), 1, f);
    fprintf(f, "%s", lang.options.name);
    fwrite(&lang.options.codeWidth, sizeof(char), 1, f);
    fwrite(&lang.options.targetWidth, sizeof(char), 1, f);
    fwrite(&lang.options.offsetWidth, sizeof(char), 1, f);
    fwrite(&lang.options.endian, sizeof(endian_t), 1, f);
    fwrite(&lang.options.maxfilesize, sizeof(size_t), 1, f);
    
    //Number of instructions and commands
    fwrite(&lang.commands.numCommands, sizeof(int), 1, f);
    fwrite(&lang.instructions.numInstructions, sizeof(int), 1, f);
    
    //Commands
    for(i=0; i<lang.commands.numCommands; i++) {
        command c = lang.commands.commands[i];
        len = strlen(c.name);
        fwrite(&len, sizeof(unsigned int), 1, f);
        fprintf(f, "%s", c.name);
        fwrite(&c.code, sizeof(unsigned int), 1, f);
        fwrite(&c.params.numParams, sizeof(char), 1, f);
        //Parameters
        for (j=0; j<c.params.numParams; j++) {
            command_param p = c.params.params[j];
            len = strlen(p.name);
            fwrite(&len, sizeof(unsigned int), 1, f);
            fprintf(f, "%s", p.name);
            fwrite(&p.width, sizeof(unsigned char), 1, f);
            }
        }
    //Instructions
    for(i=0; i<lang.instructions.numInstructions; i++) {
        instruction ins = lang.instructions.instructions[i];
        fwrite(&ins.params.mask, sizeof(unsigned int), 1, f);
        fwrite(&ins.params.numParams, sizeof(char), 1, f);
        fwrite(&ins.translation.size, sizeof(int), 1, f);
        //Parameters
        for (j=0; j<ins.params.numParams; j++) {
            instruction_param p = ins.params.params[j];
            len = strlen(p.name);
            fwrite(&len, sizeof(unsigned int), 1, f);
            fprintf(f, "%s", p.name);
            fwrite(&p.type, sizeof(param_type), 1, f);
            fwrite(&p.extra, sizeof(unsigned int), 1, f);
            //Modifications
            fwrite(&p.modify.numMods, sizeof(unsigned int), 1, f);
            for (k=0; k<p.modify.numMods; k++) {
                fwrite(&p.modify.mods[k].type, sizeof(enum mod_type), 1, f);
                switch(p.modify.mods[k].type) {
                    case MOD_BEGIN:
                    case MOD_END:
                        fwrite(&p.modify.mods[k].data.translation.size, sizeof(int), 1, f);
                        writetranslation(f, p.modify.mods[k].data.translation);
                        break;
                    default: break;
                    }
                }
            }
        //Translation
        writetranslation(f, ins.translation);
        }
}

void readtranslation(FILE* f, translation * trans)
{
    int len, j;
    char* str;

    trans->items = malloc2(sizeof(Translation_item) * trans->size); //Pointer to each content
    struct translation_item* tritems = malloc2(sizeof(struct translation_item) * trans->size); //Contents
    for (j=0; j<trans->size; j++) {
        Translation_item t = &tritems[j];
        trans->items[j] = t;
        fread(&t->type, sizeof(enum translation_type), 1, f);
        enum translation_type type = t->type;
        switch(type) {
            case TRANS_COMMAND:
                fread(&len, sizeof(unsigned int), 1, f);
                str = malloc2(len + 1); str[len] = '\0';
                fread(str, sizeof(char), len, f);
                t->value.command.name = str;
                fread(&t->value.command.code, sizeof(int), 1, f);
                break;
            case TRANS_LIT:
                fread(&t->value.number.val, sizeof(int), 1, f);
                fread(&t->value.number.width, sizeof(char), 1, f);
                break;
            case TRANS_CURTARGET:
                t->value.str = NULL;
                break;
            default:
                fread(&len, sizeof(unsigned int), 1, f);
                str = malloc2(len + 1); str[len] = '\0';
                fread(str, sizeof(char), len, f);
                t->value.str = str;
                break;
            }
        }
}

//Read language from file
void readlanguage(FILE* f)
{
    unsigned int len, i, j, k;
    char* str;

    //Options
    fread(&len, sizeof(unsigned int), 1, f);
    str = malloc2(len + 1); str[len] = '\0';
    fread(str, sizeof(char), len, f);
    lang.options.name = str;
    fread(&lang.options.codeWidth, sizeof(char), 1, f);
    fread(&lang.options.targetWidth, sizeof(char), 1, f);
    fread(&lang.options.offsetWidth, sizeof(char), 1, f);
    fread(&lang.options.endian, sizeof(endian_t), 1, f);
    fread(&lang.options.maxfilesize, sizeof(size_t), 1, f);
    //Number of instructions and commands
    fread(&lang.commands.numCommands, sizeof(int), 1, f);
    fread(&lang.instructions.numInstructions, sizeof(int), 1, f);
    lang.commands.commands = malloc2(sizeof(command) * lang.commands.numCommands);
    lang.instructions.instructions = malloc2(sizeof(instruction) * lang.instructions.numInstructions);
    
    //Commands
    for(i=0; i<lang.commands.numCommands; i++) {
        command* c = &(lang.commands.commands[i]);
        fread(&len, sizeof(unsigned int), 1, f);
        str = malloc2(len + 1); str[len] = '\0';
        fread(str, sizeof(char), len, f);
        c->name = str;
        fread(&c->code, sizeof(unsigned int), 1, f);
        fread(&c->params.numParams, sizeof(char), 1, f);
        c->params.params = malloc2(sizeof(command_param) * c->params.numParams);
        //Parameters
        for (j=0; j<c->params.numParams; j++) {
            command_param* p = &c->params.params[j];
            fread(&len, sizeof(unsigned int), 1, f);
            str = malloc2(len + 1); str[len] = '\0';
            fread(str, sizeof(char), len, f);
            p->name = str;
            fread(&p->width, sizeof(unsigned char), 1, f);
            }
        }
    //Instructions
    for(i=0; i<lang.instructions.numInstructions; i++) {
        instruction* ins = &(lang.instructions.instructions[i]);
        fread(&ins->params.mask, sizeof(unsigned int), 1, f);
        fread(&ins->params.numParams, sizeof(char), 1, f);
        fread(&ins->translation.size, sizeof(int), 1, f);
        ins->params.params = malloc2(sizeof(instruction_param) * ins->params.numParams);
        //Parameters
        for (j=0; j<ins->params.numParams; j++) {
            instruction_param* p = &ins->params.params[j];
            fread(&len, sizeof(unsigned int), 1, f);
            str = malloc2(len + 1); str[len] = '\0';
            fread(str, sizeof(char), len, f);
            p->name = str;
            fread(&p->type, sizeof(param_type), 1, f);
            fread(&p->extra, sizeof(unsigned int), 1, f);
            //Modifications
            fread(&p->modify.numMods, sizeof(unsigned int), 1, f);
            p->modify.mods = (modification*) malloc2(p->modify.numMods * sizeof(modification));
            for (k=0; k<p->modify.numMods; k++) {
                fread(&p->modify.mods[k].type, sizeof(enum mod_type), 1, f);
                switch(p->modify.mods[k].type) {
                    case MOD_BEGIN:
                    case MOD_END:
                        fread(&p->modify.mods[k].data.translation.size, sizeof(int), 1, f);
                        readtranslation(f, &p->modify.mods[k].data.translation);
                        break;
                    default: break;
                    }
                }
            }
        //Translation
        readtranslation(f, &ins->translation);
        }
}
