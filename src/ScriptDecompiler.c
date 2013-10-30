#include <stdlib.h>
#include "common.h"

//required by some common.c functions. Cleaner code would not need these dummy lines
unsigned line, column;
bool verbose_mode;


size_t fread_endian(unsigned* to, unsigned char width, endian_t endian, FILE* f)
{
	unsigned ret = 0, i, sizeread = 0;
    unsigned char byte;
    if (endian == ENDIAN_LITTLE)
        //little endian
        for (i=0; i<width; i++) {
            if (fread(&byte, 1, 1, f) != 1) return sizeread; else sizeread++;
            ret += byte << (i<<3);
            }
    else
        //big endian
        for (i=0; i<width; i++) {
            if (fread(&byte, 1, 1, f) != 1) return sizeread; else sizeread++;
            ret += byte << ((width-i-1)<<3);
            }
            
	*to = ret;
    return sizeread;
}

command* getCommand(unsigned code)
{
    int i;
    for (i=0; i<lang.commands.numCommands; i++) {
        command* c = &lang.commands.commands[i];
        if (c->code == code) return c;
        }
    return NULL;
}

//Decompile a script from an open file according to language lang (from common.h)
//out must be open for writing
bool decompile(FILE* script, FILE* out)
{
    fprintf(out, "## Script is printed in commands ##\n\n");
    unsigned commandCode, paramValue, sizeread;
    bool raw = false, failure = false;
    unsigned offset = 0;
    while (!failure) {
        //read command code
        sizeread = fread_endian(&commandCode, lang.options.codeWidth, lang.options.endian, script);
        if (sizeread != lang.options.codeWidth) {
            if (sizeread) {fprintf(out, "\nScript ended with %d extra bytes.\n", sizeread); failure = true;}
            break; //while
            }
            
        //get command with that code
        command* c = getCommand(commandCode);
        if (c != NULL) {
            //if a raw line ends, print the eol
            if (raw) { fprintf(out, "\n"); raw = false;}
            
            //print command and parameters
            fprintf(out, "0x%08x\t\t%s", offset, c->name);
            offset += lang.options.codeWidth;
            int i;
            for (i=0; i< c->params.numParams; i++) {
                command_param param = c->params.params[i];
                //read parameter
                sizeread = fread_endian(&paramValue, param.width, lang.options.endian, script);
                if (sizeread != param.width) {
                    failure = true;
                    if (sizeread) fprintf(out, "\n\nScript ended with %d extra bytes.\n", sizeread);
                    fprintf(out, "Command incomplete!\n", sizeread);
                    break; //for
                    }                
                
                //print parameter
                fprintf(out, " 0x%x", paramValue);
                offset += param.width;
                }
            if (failure) break; //while
            fprintf(out, ";\n");
            }
        else  {
            if (!raw) {
                raw = true;
                fprintf(out, "0x%08x\t\t#raw:%d", offset, lang.options.codeWidth);
                }
            fprintf(out, " 0x%x", commandCode);
           
            offset += lang.options.codeWidth;
            }
        }
    
    return failure;
}

bool open_lang (char* name)
{
    char buffer [strlen(name) + 6];
    sprintf (buffer, "%s.lang", name);
    FILE* f = fopen (buffer, "r");
    if (f == NULL) return false;
    readlanguage(f);
    fclose (f);
    return true;
}

int main(int argc, char* argv[])
{
    int i;
    
    if (argc != 3) {
        printf("USAGE: %s <language_name> <script_file>", argv[0]);
        exit(0);
        }
    
    if (!open_lang(argv[1])) {
        perror("Unable to open language file");
        exit(-1);
        }
    
    FILE* script = fopen(argv[2], "r");
    
    if (script == NULL) {
        perror("Unable to open script file");
        exit(-1);
        }
    
    decompile(script, stdout);
}