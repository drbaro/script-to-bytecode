#include <stdlib.h>
#include <string.h>
#include "common.h"

//required by some common.c functions. Cleaner code would not need these dummy lines. Whatever.
unsigned line, column;
bool verbose_mode;

//Get a new File
FILE* outFile()
{
    const char* firstName = "decLang";
    const char* extension = "stbl";
    char filename[strlen(firstName) + 5 + strlen(extension)];
    
    sprintf(filename, "%s.%s", firstName, extension);
    FILE* ret = fopen(filename, "rb");
    if (ret == NULL) return fopen(filename, "wb");
    else {
        unsigned short i;
        for(i=0; i<0xFFFF; i++) {
            sprintf(filename, "%s_%d.%s", firstName, i, extension);
            FILE* ret = fopen(filename, "rb");
            if (ret == NULL) return fopen(filename, "wb");
            }
        printf("You should really erase previous decompilations...\n");
        exit(-1);
        }
}

void fprintCommand(FILE* out, command c)
{

}


void fprintInstruction(FILE* out, instruction i)
{

}

void decompile(char* filename)
{
    FILE* out = outFile();
    
    fprintf(out,
        "#################################\n"
        "## Language decompiled from %s ##\n"
        "#################################\n"
        "\n"
        "#lang %s:\n"
        "\n"
        "#code %d;  #target %d;  #offset %d;\n"
        "#filesize %d;\n"
        "#endian %d;\n"
        "\n"
        "\n"
        "## COMMANDS\n"
        , filename
        , lang.options.name
        , lang.options.codeWidth, lang.options.targetWidth, lang.options.offsetWidth
        , lang.options.maxfilesize
        , (lang.options.endian == ENDIAN_LITTLE) ? "little" : "big");
    
    int i;
    for (i=0; i<lang.commands.numCommands; i++) {
        fprintCommand(out, lang.commands.commands[i]);
        fprintf(out, "\n");
        }
    for (i=0; i<lang.instructions.numInstructions; i++) {
        fprintInstruction(out, lang.instructions.instructions[i]);
        fprintf(out, "\n");
        }

}

bool open_lang (char* name)
{
    FILE* f = fopen (name, "r");
    if (f == NULL) return false;
    readlanguage(f);
    fclose (f);
    return true;
}

int main(int argc, char* argv[])
{
    int i;
    
    if (argc != 2) {
        printf("USAGE: %s <language_file>", argv[0]);
        exit(0);
        }
    
    if (!open_lang(argv[1])) {
        perror("Unable to open language file");
        exit(-1);
        }
    
    decompile(argv[1]);
}