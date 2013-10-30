#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

typedef enum bool{false=0,true=1} bool;
typedef enum endian {ENDIAN_LITTLE, ENDIAN_BIG} endian_t;
#define FILESIZE_MAX 0

//Reads a character
char read_lit_char (char* c);

//Returns the number of characters in c parsed to read the character and store it in "to"
char read_char (char* c, char* to);

//Return the highest value that fits in a given number of bytes
static inline unsigned int directionable_in_bytes(unsigned int width)
{
    switch(width) {
        case 1: return 0xFF;
        case 2: return 0xFFFF;
        case 3: return 0xFFFFFF;
        case 4: return 0xFFFFFFFF;
        default: return 0;
    }
}

//Can 'val' be represented in 'width' bytes?
static inline bool val_fits_in_bytes(unsigned int val, unsigned int width)
{
    return val <= directionable_in_bytes(width);
}

//Remove escape sequences
static inline char* remove_escape_sequences (char* s)
{
    int i, j = 0;
    for (i=0; i< strlen(s); i+= read_char(&(s[i]), &(s[j++])));
    s[j] = 0;
    return s;
}

/*
OVERVIEW OF THE TYPES DESCRIBED IN THIS FILE

There are two main elements in the languages: instructions and commands

-Commands
Executable parts of the code, which are read by the running function in the user program.
Identified by a operation name and an array of constant values for parameters.
Could be compared to each instruction in [simple] assembly languages.

-Instructions
Identified by its structure (amount of elements and their type)
Used by the scripter (i.e: used in script files).
Script contain instructions, but they don't contain commands.
Each instruction is translated into a small piece of code made of commands.
Could be compared to each sentence in a high level language which, when compiled, generates code in some lower level language.

-Language
Holds commands and instructions, as well as other variables

In adition to this, both compilers use something that I've called "references"
A reference is a value that is mentioned but its actual value is not known yet
Solving a reference means writing a value in the places it was menctioned, when we know its value
*/




//Parameter types
typedef enum param_type {
    TYPE_ATOM  = 0,
    TYPE_VAL   = 1,
    TYPE_LABEL = 2,
    TYPE_BLOCK = 3,
    TYPE_ERR   = 0xFF //If parameter doesn't exist
} param_type;

/* Translation of an instruction to a sequence of commands (and/or #raw data) */
typedef struct translation_item {
    enum translation_type{
        TRANS_COMMAND,      //Command code
        TRANS_LIT,          //Literal value (parameters and #raw)
        TRANS_ID,           //Value names (during unification, also includes blocks)
        TRANS_BLOCK,        //Offset in which a block will be output
        TRANS_LTARGET,      //Script where a label was declared
        TRANS_LOFFSET,      //Position where the label was declared
        TRANS_STRING,       //Array of chars
        TRANS_CURTARGET,    //Value of current target
        TRANS_INNERLABEL,   //Label declared on scope of this translation.
        TRANS_LABEL         //Use of an inner label
    } type;
    union {
        char* str;  //TRANS_STRING, TRANS_ID, TRANS_LTARGET, TRANS_LOFFSET, TRANS_ID, TRANS_BLOCK, TRANS_INNERLABEL, TRANS_LABEL
                    //TRANS_CURTARGET also uses str but only to set it to NULL
        struct{int val; char width;} number;//TRANS_LIT
        struct{char* name; int code;} command;//TRANS_COMMAND
    } value;
} * Translation_item;

//Array of translation_item
typedef struct translation {
    int size, numInnerLabels;
    Translation_item* items;
    char* *innerLabels;
} translation;

//Parameter modification (currently only blocks)
typedef struct modification {
    enum mod_type {MOD_NONE = 0, MOD_BEGIN, MOD_END} type;
    union {
        translation translation;
    } data;
} modification;

/* Instructions */
//Definiton of each element in an instruction
typedef struct instruction_param {
    char* name;              //Name of the parameter (for atoms, the atom itself)
                             //For a string, starts with '\"'
    param_type type;         //Patameter type
    unsigned int extra; /* TYPE_LABEL: zero if local, nonzero otherwise
                           TYPE_VAL  : width
                           TYPE_BLOCK: during script compilation, holds the block context id
                        */
    struct modify {
        unsigned int numMods; //Number of modifications
        modification * mods;  //Array of modifications
        }
        modify;
} instruction_param;


//Definition of the instruction itself
typedef struct instruction {
    struct instruction_params {
        unsigned int mask;          //Mask to recognize its structure
        char numParams;             //Number of parameters (16 max)
        instruction_param* params;  //Parameters
    } params;
    translation translation;
} instruction;
typedef struct instruction_params instruction_params;

/* Commands */
//Parameters
typedef struct command_param {
    char* name;                 //Name
    unsigned char width;        //bytes reserved for this parameter
} command_param;

//Command
typedef struct command {
    unsigned int code;          //Command code
    char* name;                 //Name of command
    struct command_params {
        char numParams;         //Number of parameters
        command_param* params;  //Parameters
    } params;
} command;
typedef struct command_params command_params;

/* Language */
typedef struct language {
    struct language_options { //Options
        char* name;                                     //Name of the language
        char codeWidth;                                 //Width of codes
        char targetWidth;                               //Width of script identifiers
        char offsetWidth;                               //Width of offsets
        endian_t endian;                                //Endian
        size_t  maxfilesize;                            //Maximum file size (limit on compiled code)
    } options;
    struct language_commands {
        int numCommands;                                //Number of commands
        command * commands;                             //Commands
    } commands;
    struct language_instructions {
        int numInstructions;                            //Number of instructions
        instruction * instructions;                     //Instructions
    } instructions;
} language;


language lang;


// Reads a language from a file
void readlanguage(FILE* f);
// Writes a language to file
void writelanguage(FILE* f);


// Malloc with error control
void* malloc2 (size_t size);
// Realloc with error control
void* realloc2 (void* alo, size_t size);

//Searches differences between two instruction parameters.
//If local_general is true, when checking labels checks if it's a local label only if "a" is local (permisive pattern matching)
bool param_diff (instruction_param a, instruction_param b, bool local_general);


// Reads a line and returns a malloc'd copy without the '\n'
char* avanzalinea (FILE* file);
// atoi+free
int atoi2 (char* buffer);


/* Printing */
void yyerror (const char* s);
void err_other(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void err_lex(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void err_sin(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void err_sem(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void warn_sem(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void verbose(char* fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));

#endif
