%error-verbose
%{
#include "src/common.h" //Since flex always outputs at cwd, we won't bother to do that on bison. This is the route relative to common.h from the root

//Program options
bool
        verbose_mode        = false,   //Print everything you're doing in stdout except translations
        verbose_trans_mode  = false,   //Print everything you're doing in stdout including translations
        scriptH             = false,   //Output script  to h file
        exportsH            = false    //Output exports to h file
    ;
char* source_name;
    
//__TRACE is used in debug mode to print bison information during the compilation. That's why it's in every rule.
#ifdef DEBUG_TRACEALL
#define __TRACE ({printf("\n"); yydebug=1;})
#else
#define __TRACE
#endif

int line, column;
bool generate_code;

//Reference to an offset, (as a position within a context)
typedef struct offset_reference {
    int tag;
    unsigned int offset;
    unsigned int offwrite, contextwrite;
    unsigned int*  data;
} offset_reference;

//Forms of describing labels
typedef struct label {
    char* name;
    unsigned int target, offset;
} label;

typedef struct undefined_label {
    label* label;
    bool declared;
    unsigned int context, offset; //defined as an offset within a context
} undefined_label;

//Reference to a label that must be declared later on
typedef struct label_reference {
    undefined_label* label;
    int line, column;
    offset_reference* reference;
} label_reference;

//Dynamic arrays. Available is the amount of unused values that have been malloc'd
#define BLOCK_PAGE 32
typedef struct exec_block{
    unsigned int size, available;
    unsigned char* data;
    
    //If references in this block have been created, they must be registered
    //If it is concatenated, the offset of that reference aren't correct, so they must be updated
    struct {
        unsigned int size, available;
        offset_reference* * item;
        //Innerlabel references
        struct {
            unsigned int size, available;
            undefined_label* * item;
        } innerlabel;
    } referenced;
    
} exec_block;

//Add a reference to write in this block, at a certain offset inside it, that might change if the block is concatenated.
void add_concat_reference(exec_block* b, offset_reference* ref)
{
    if (b->referenced.available == 0) {
        #define CONCAT_REFERENCE_PAGE 16
        b->referenced.item =
            (offset_reference**) realloc2(b->referenced.item, sizeof(offset_reference*) * (b->referenced.size + CONCAT_REFERENCE_PAGE));
        b->referenced.available = CONCAT_REFERENCE_PAGE;
        }
    int i = (b->referenced.size)++;
    b->referenced.item[i] = ref;
    (b->referenced.available)--;
}
//Add a reference to push an inner label created within this block if it is ever concatenated
void add_innerlabel_concat_reference(exec_block* b, undefined_label* ul)
{
    // We won't add a label twice. If we do, it'll be pushed twice and the offset will be wrong.
    int i;
    for (i=0; i<b->referenced.innerlabel.size; i++) {
        if (ul == b->referenced.innerlabel.item[i])
            return;
        }

    // Add the innerlabel
    if (b->referenced.innerlabel.available == 0) {
        #define CONCAT_INNERLABEL_REFERENCE_PAGE 16
        b->referenced.innerlabel.item =
            (undefined_label**) realloc2(
                b->referenced.innerlabel.item,
                sizeof(undefined_label*) * (b->referenced.innerlabel.size + CONCAT_INNERLABEL_REFERENCE_PAGE)
                );
        b->referenced.innerlabel.available = CONCAT_INNERLABEL_REFERENCE_PAGE;
        }
    i = (b->referenced.innerlabel.size)++;
    b->referenced.innerlabel.item[i] = ul;
    (b->referenced.innerlabel.available)--;
}

typedef struct array_int{
    unsigned int size, available;
    int* data;
} array_int;

typedef struct array_str{
    unsigned int size, available;
    char** items;
} array_str;

typedef struct iparamp_array {
    unsigned int mask;
    unsigned int amount;
    instruction_param** items;
} iparamp_array;


//Default values
const char* emptystr = "";
const char* stringstring = "\"";
const exec_block empty_block = {0, 0, NULL, {0,0,NULL}};

/* Variables and structures used during compilation */

unsigned int target; //Script identifier
FILE* fout;          //File where the restults will be written

//Used to extract a fresh, new context
unsigned int newest_context;

//Contexts (each block of code, including the main block)
struct context_array {
    struct context {
        unsigned int tag;
        exec_block block;
    } * context;
    unsigned int size, available;
    
    size_t totalscriptsize;
    
    struct context_stack {
        unsigned int size, available;
        unsigned int * data;
    } stack;
} contexts;


/* Management of context stack */
unsigned int current_context()
{
    if (contexts.stack.size <= 0) return 0; //safety check
    else
        return contexts.stack.data[contexts.stack.size -1];
}
void push_context()
{
    if (contexts.stack.available == 0) {
        #define CONTEXT_PAGE 16
        contexts.stack.data = (unsigned int*) realloc2(contexts.stack.data, sizeof(unsigned int)* (contexts.stack.size + CONTEXT_PAGE));
        contexts.stack.available = CONTEXT_PAGE;
        }
    contexts.stack.data[(contexts.stack.size)++] = ++newest_context;
    (contexts.stack.available)--;
}
void pop_context()
{
    (contexts.stack.size)--;
    (contexts.stack.available)++;
}

//Array of defined vlues
struct values {
    unsigned int size, available;
    char** name;
    int* value;
} values;

//Array of labels
struct labels {
    struct{
        unsigned int size, available;
        label ** label;
    } defined;
    struct{
        unsigned int size, available;
        undefined_label* pending;
    } undefined;
} labels;

//Arrays of references
struct references {
    struct offset_references {
        unsigned int size, available;
        offset_reference* * item;
    } offsets;
    struct label_references {
        unsigned int size, available;
        label_reference* * item;
    } labels;
} references;

//Array of exports
#define EXPORTABLES_PAGE 16
struct exportables {
   unsigned int size, available;
   struct exports {
        char* filename;
        array_str data;
  } * data;
} exportables;

//Inner label renaming management
struct innerLabelMgmt {
    bool used;
    int  n;
} innerLabelMgmt;

//Set defaults
void init(void)
{
    fout = NULL;
    line = 1; column = 1;
    
    //Init context stack
    contexts.stack.data = (unsigned int*) malloc2(sizeof(unsigned int)*CONTEXT_PAGE);
    contexts.stack.available = 7;
    contexts.stack.size = 1;
    contexts.stack.data[0] = 0; //First context is 0, the main context (the one that is not nested in any instruction)
    newest_context = 0;
    
    //Init all dynamic arrays as empty arrays
    contexts.available = 0; contexts.size = 0; contexts.context = NULL;
    contexts.totalscriptsize = 0;
    values.available = 0; values.size = 0; values.name = NULL; values.value = NULL;
    exportables.available = 0; exportables.size = 0; exportables.data = NULL;
    labels.defined.available   = 0; labels.defined.size   = 0; labels.defined.label     = NULL;
    labels.undefined.available = 0; labels.undefined.size = 0; labels.undefined.pending = NULL;
    references.offsets.available = 0; references.offsets.size = 0; references.offsets.item = NULL; 
    references.labels.available  = 0; references.labels.size  = 0; references.labels.item  = NULL;
    
    //Other
    generate_code = true;
    innerLabelMgmt.n = 0;
    innerLabelMgmt.used = false;
}

/* Auxiliary functions */

//Add this block to the block list
//Returns the number of block, which serves as a 'ticket'
int add_block(exec_block);

//Add a label to the list
//Returns false if and only if a label with that name already exists
//declaring == false when label is just being menctioned (i.e: not declared)
//Returns false if and only if a label with that name already exists
//declaring = false when it's referenced, true when it's being declared
bool add_undefined_label(char* name, unsigned context, unsigned int offset, bool declaring, undefined_label ** ret);

//Retrieves a label by name
label* fetch_label(char*);

//Adds a value to the list
//Returns false if and only if a value with that name already exists
bool add_value(char*, int);

// Finds the value defined as id and stores in v if it exists and is not NULL
// Returns false if and only if it doesn't exist, and true otherwise
bool value_of(char* id, int* v);

//Opens a language from a file and sets it as the global variable
//Returns false if the language doesn't exist
bool open_lang(char*);

//Opens a file with definitions and loads them as names (labels and values)
//Returns false if the file can't be found
bool import(char*);

//Prepares a list of names to be exported to a file (if filename is NULL, to a default file)
//Returns NULL if success, or the repeated name, if any (and therefore the operation can't be completed)
//Repetitions in the very array aren't checked (this is assumed to be already done)
char* set_exportables(array_str exportables, char* filename);


//Writes all code blocks, starting by the main one, solving all the references
void output_all_blocks(void);

//Generates code from an instruction
exec_block block_instruction(iparamp_array params);

//Creates a block of code from an array of integers of a given width
exec_block block(int width, int* data, int n);

//Gets a string from a raw chunk of data
char * chunk_to_string(int width, int* data, int n)
{
    int _printval(char* str, int val, int w) {
        switch (w) {
            case 1 :
                sprintf(str, "0x%02X", val);
                break;
            case 2 :
                sprintf(str, "0x%04X", val);
                break;
            case 3 :
                sprintf(str, "0x%06X", val);
                break;
            case 4 :
            default:
                sprintf(str, "0x%08X", val);
                break;
            }
        return 2+(w<<1);
        }
    
    char* ret = malloc2( sizeof(char) * (
              ((width*2) + 2) * n //Size of each value (0xBYTES) * number of values
            + n //Number of spaces and the final \0 character
            )
        );
    unsigned int i, j;
    j = _printval(ret, data[0], width);
    for(i=1; i<n; i++) {
        sprintf(ret + j, " "); j++;
        j += _printval(ret + j, data[i], width);
        }
    ret[j] = 0;
    return ret;
}

//Creates a string from a parameter list
char* params_toString(char buffer[], iparamp_array p, unsigned int context)
{
    int pos = sprintf(buffer, "[%d] Instruction:", context);
    int i;
    for (i=0; i< p.amount; i++) {
        instruction_param* ip = p.items[i];
        switch (ip->type) {
            case TYPE_ATOM:
                pos += sprintf(buffer+pos, "\n\t\t%s", ip->name);
                break;
            case TYPE_VAL:
                pos += sprintf(buffer+pos, "\n\t\t%s := %d", ip->name, ip->extra);
                break;
            case TYPE_LABEL:
                pos += sprintf(buffer+pos, "\n\t\tLabel \"%s\"", ip->name);
                break;
            case TYPE_BLOCK:
                pos += sprintf(buffer+pos, "\n\t\t%s := [%d]", ip->name, ip->extra);
                break;
            default: break;
            }
        }
    return buffer;
}

//Creates a block of code from an array of characters
exec_block block_string(char*);

//Concat two blocks of code
//frees the memory used by the source blocks
exec_block concat_block(exec_block, exec_block);

%}

%union {
    int                 int_t;
    char*               str;
    array_int           array_int;
    array_str           array_str;
    
    instruction_param*  iparamp;
    iparamp_array       iparamp_array;
    exec_block          exec_block; //bloque ejecutable
}

%token<int_t> LITERAL   "<literal>"
%token<int_t> WIDTH     "<constant width>"

%token<str> ID          "<identifier>"
%token<str> ATOM        "<atom>"
%token<str> VALUE       "<value>"
%token<str> LABEL       "<label>"
%token<str> STRING      "<string>"

%token DECLARE          "#let"
%token EXPORT           "#export"
%token IMPORT           "#import"
%token RAW              "#raw"
%token LANGUAGE         "#lang"
%token TARGET           "#target"
%token RESOURCE         "#resource"
%token RAW_STRING       "#string"

//%token ':' ';' '{' '}'

%type<str> atom exportable label_decl
%type<int_t> valued

%type<array_str> exportable_list

%type<iparamp> param
%type<iparamp_array> param_list

%type<array_int> data_chunk

%type<exec_block> exec_block instruction body code_unit

%start s

%{
#define YYPRINT(file, type, value)   yyprint (file, type, value)
void yyprint (FILE *file, int type, YYSTYPE value)
{
  if (type == ID || type == ATOM || type == STRING || type == VALUE || type == LABEL
  ||  type == 41 || type == 33  || type == 29)
    fprintf (file, "\"%s\"", value.str);
  else if (type == LITERAL || type == WIDTH || type == 38)
    fprintf (file, "%d", value.int_t);
}
%}


%%
/* SCRIPT DEFINITION */
s               : header body {__TRACE; add_block($2); output_all_blocks();} 
                //Syntax error
                | header {__TRACE; err_sin("Script must contain at least a declaration or an instruction.");} 
                ;
//Header of an script #target/#resource and #lang are obligatory, and they may be followed by #export and #import
header          : target_decl lang_get opt_decls {__TRACE;} 
                //Syntax error
                | lang_get opt_decls{__TRACE; err_sin("#target or #resource clause is obligatory.");} 
                | target_decl opt_decls{__TRACE; err_sin("#lang clause is obligatory.");} 
                ;
//Body of the script
//(Terms in these rules aren't factorized because otherwise it's complicated to follow the offset of the labels)
body            : label_decl instruction       {__TRACE; $$ = $2;
                    if (!generate_code)
                        err_sem("Resource files can't contain code.");
                    if ( !add_undefined_label($1, current_context(), 0, true, NULL)  )
                        err_sem("Label \"%s\" redeclared.", $1);
                    }
                |            instruction       {__TRACE;
                    if (!generate_code)
                        err_sem("Resource files can't contain code.");
                    $$ = $1;
                    } 
                | value_decl                   {__TRACE; $$ = empty_block;}
                | body label_decl instruction  {__TRACE;
                    if (!generate_code)
                        err_sem("Resource files can't contain code.");
                    if ( !add_undefined_label($2, current_context(), $1.size, true, NULL)  )
                        err_sem("Label \"%s\" redeclared.", $2);
                    $$ = concat_block($1, $3);
                    } 
                | body instruction             {__TRACE;
                    if (!generate_code)
                        err_sem("Resource files can't contain code.");
                    $$ = concat_block($1, $2);
                    } 
                | body value_decl              {__TRACE; $$ = $1;} 
                ;

/* DECLARATIONS */

//List of declarations
opt_decls       : /*vacia*/ {__TRACE;}
                | decl_list {__TRACE;} 
                ;
decl_list       : declaration ';' {__TRACE;} 
                | decl_list declaration ';' {__TRACE;}
                //Syntax error
                | declaration {__TRACE; err_sin("Imports and exports must end in \";\".");} 
                | decl_list declaration {__TRACE; err_sin("Imports and exports must end in \";\".");}
                ;
//Declarations that may go in the header
declaration     : export_decl {__TRACE;} 
                | import_decl {__TRACE;} 
                ;
//Name the script
target_decl     : TARGET LITERAL ':' {__TRACE;
                    target = (unsigned int) $2;
                    verbose("*** Parsing script number %d ***", target);
                    }
                //Only definitions, no code
                | RESOURCE ':' {__TRACE;
                    generate_code = false;
                    verbose("*** Parsing a resource file ***");
                    }
                //Syntax error
                | TARGET ID ':' {__TRACE; free($2); err_sin("Target must be a number or character.");}
                | TARGET LITERAL {__TRACE; err_sin("Clause #target must end in \":\".");}
                | RESOURCE       {__TRACE; err_sin("Clause #resource must end in \":\".");}
                ;
//Declare this position with a label name
label_decl      : LABEL ':' {__TRACE; $$ = $1;}
                ;
//Declare a value name, with a value
value_decl      : DECLARE VALUE ':' valued ';' {__TRACE;
                    if( !add_value($2, $4))
                        err_sem("Value \"%s\" redeclared.", $2);
                    verbose("%s := 0x%0x", $2, $4);
                        }
                //Syntax error
                | DECLARE VALUE ':' valued {__TRACE; err_sin("Value declarations must end in \";\".");} 
                ;
//Anounce the exportation a list of names
//  All names must be declared. Can't export the same name twice to the same file.
export_decl     : //default
                  EXPORT exportable_list {__TRACE;
                    char * c = set_exportables( $2, NULL);
                    if ( c != NULL )
                        err_sem("Can't export \"%s\" twice to the same file.", c);
                    verbose("Exporting names to %s", "<default>");
                    } 
                | //given name
                  EXPORT exportable_list ':' ID {__TRACE;
                    char * c = set_exportables( $2, $4);
                    if ( c != NULL )
                        err_sem("Can't export \"%s\" twice to the same file.", c);
                    verbose("Exporting names to %s", $4);
                    }
                //Syntax error
                |  EXPORT exportable_list ID {__TRACE;
                    err_sin("After the list of names to be exported, name the file by separating it with \":\".");
                    }
                ;
//Names to export
exportable_list : exportable {__TRACE;
                    $$.size = 1;
                    $$.items = (char**) malloc2(sizeof(char*) * EXPORTABLES_PAGE);
                    $$.items[0] = $1;
                    $$.available = 3;
                    }
                | exportable_list exportable {__TRACE;
                    $$ = $1;
                    //Check the name is not repeated
                    int i;
                    for(i=0; i<$$.size; i++)
                        if ( !strcmp($2, $$.items[i]) )
                            err_sem("Can't export \"%s\" twice to the same file.", $2);
                    //Insert new data
                    if ($$.available == 0) {
                        $$.items = (char**) realloc2($$.items, sizeof(char*) * ($$.size + EXPORTABLES_PAGE));
                        $$.available = EXPORTABLES_PAGE;
                        }
                    $$.items[($$.size)++] = $2;
                    ($$.available)--;
                    } 
                ;
//Exportable names
exportable      : LABEL {__TRACE; $$ = $1;}
                | VALUE {__TRACE; $$ = $1;}
                ;
//Retrieve exported names from a file
import_decl     : IMPORT ID {__TRACE; char* id = $2;
                    if( !import(id) )
                        err_sem("Definition file \"%s.def\" not found.", id);
                    verbose("Importing names from %s", id);
                    free($2);
                    } 
                ;
//Name the language we're using
lang_get        : LANGUAGE ID ';' {__TRACE; char* id = $2;
                    if ( !open_lang(id) )
                        err_sem("Language \"%s\" not found.", id);
                    verbose("Language %s loaded.", id);
                    free($2);
                    }
                //Syntax error
                | LANGUAGE ID {__TRACE; free($2); err_sin("#lang clause must end in \";\".");}
                ;

/* Instruction code */
//Instructions
instruction     : code_unit ';' {__TRACE; $$ = $1}

//Smallest compilable unit
code_unit     :   param_list {__TRACE;
                    $$ = block_instruction($1); //Generate a block of code
                    char buffer[200] = {0};
                    verbose("%s", params_toString(buffer, $1, current_context()));
                    //free memory
                    int i;
                    for (i=0; i<$1.amount; i++)
                        switch($1.items[i]->type) {
                            case TYPE_ATOM:
                            case TYPE_LABEL:
                                free($1.items[i]->name);
                                break;
                            default: break;
                        }
                    free($1.items);
                    }     
                | RAW WIDTH data_chunk {__TRACE;
                    $$ = block($2, $3.data, $3.size);
                    char * verboseStr = chunk_to_string($2, $3.data, $3.size);
                    verbose("[%d] Raw data (%d*%d bytes):\n\t\t%s", current_context(), $2, $3.size, verboseStr);
                    free($3.data);
                    free(verboseStr);
                    } 
                | RAW_STRING STRING {__TRACE;
                    $$ = block_string($2);
                    verbose("[%d] String:\n\t\t%s", current_context(), $2);
                    free($2);
                    } 
                //Syntax error
                | RAW data_chunk {__TRACE; err_sin("Instruction #raw requires a width.");}
                ;
//Array of #raw values
data_chunk      : valued {__TRACE;
                    $$.data = (int*)malloc2(sizeof(int) * BLOCK_PAGE);
                    $$.available = 7;
                    $$.size = 1;
                    $$.data[0]= $1;
                    } 
                | data_chunk valued {__TRACE;
                    $$ = $1;
                    if ($$.available == 0) {
                        $$.data = (int*)realloc2($$.data, sizeof(int) * ($$.size + BLOCK_PAGE));
                        $$.available = BLOCK_PAGE;
                        }
                    $$.data[($$.size)++]= $2;
                    ($$.available)--;
                    } 
                ;
valued          : // Directly a number (or a character)
                  LITERAL {__TRACE; $$ = $1;} 
                | // The name of a known value
                  VALUE {__TRACE;
                    if ( !value_of($1, &$$) )
                        err_sem("Value \"%s\" not defined.", $1);
                    free($1);
                    } 
                ;               
//Parameter list
param_list      : param            {__TRACE;
                    //Allocate room for 16 parameters - the max
                    $$.items = (instruction_param**) malloc2( sizeof(struct instruction_param*) * 16);
                    $$.items[0] = $1;
                    $$.amount = 1;
                    $$.mask = $1->type;
                    }  
                | param_list param {__TRACE;
                    $$ = $1;
                    if ($$.amount == 16)
                        err_sem("Instructions are limited to 16 elements");
                    $$.items[($$.amount)++] = $2;
                    $$.mask = ($1.mask<<2) | $2->type;
                    } 
                ;
// Available types of parameters
param           : // Atoms (Strings that are not any other type of parameter)
                  atom {__TRACE;
                    $$ = (instruction_param*) malloc2( sizeof(struct instruction_param));
                    $$->type = TYPE_ATOM;
                    $$->name = $1;
                    }
                | // Valued parameter
                  valued {__TRACE;
                    $$ = (instruction_param*) malloc2( sizeof(struct instruction_param));
                    $$->type = TYPE_VAL;
                    $$->name = (char*) emptystr;
                    $$->extra = $1;
                    } 
                | // Reference to a position in a script
                  LABEL {__TRACE;
                    $$ = (instruction_param*) malloc2( sizeof(struct instruction_param));
                    $$->type = TYPE_LABEL;
                    $$->name = $1;
                    //Find if the label is global
                    label* l = fetch_label($1);
                    if (l == NULL) {
                        //Not declared, so we asume it'll be declared later
                        if ( ! add_undefined_label($1, current_context(), 0xFFFFFFFF, false, NULL) )
                            err_other("Fatal error: label considered redeclared, even though it's not being declared.");
                        //Thus, the label is local
                        $$->extra = 0;
                        }
                    else $$->extra = (l->target != target) && generate_code;
                    }  
                | // Executable block
                  '{' exec_block '}' {__TRACE;
                    $$ = (instruction_param*) malloc2( sizeof(struct instruction_param));
                    $$->name = (char*) emptystr;
                    $$->type = TYPE_BLOCK;
                    $$->extra = add_block($2); //Add block to block list, and keep block number
                    pop_context();
                    }
                | // String
                  STRING {__TRACE;
                    push_context();
                    $$ = (instruction_param*) malloc2( sizeof(struct instruction_param));
                    $$->name = (char*) stringstring;
                    $$->type = TYPE_BLOCK;
                    $$->extra = add_block(block_string($1)); //Add string as a block to block list, and keep block number
                    free($1);
                    pop_context();
                    }
                ;
//Lexical definition of atoms forces to check syntactically for atoms like this  (ID are a subset of atoms)
atom            : ATOM {__TRACE; $$ = $1;} 
                | ID   {__TRACE; $$ = $1;} 
                ;
//Contents of a executable block parameter
exec_block      :            instruction {__TRACE; $$=$1;}
                | label_decl instruction {__TRACE;
                    $$=$2;
                    if ( !add_undefined_label($1, current_context(), 0, true, NULL) )
                        err_sem("Label \"%s\" redeclared.", $1);
                    }
                | exec_block            instruction {__TRACE; $$ = concat_block($1, $2);} 
                | exec_block label_decl instruction {__TRACE;
                    if ( !add_undefined_label($2, current_context(), $1.size, true, NULL)  )
                        err_sem("Label \"%s\" redeclared.", $2);
                    $$ = concat_block($1, $3);} 
                ;

%%

//Creates a reference to an offset to write some data when it is known
//if offwrite != 0xFFFFFFFF, the data will be written, following the language's rules, in the given context, pushed offwrite bytes ahead
//The data will be written to p if p is not NULL
//returns the created reference
offset_reference* create_offset_reference(int tag, unsigned int offset, unsigned tagwrite, unsigned int offwrite, unsigned int * p)
{
    offset_reference* r = (offset_reference*) malloc2(sizeof(offset_reference));
    r->tag = tag;
    r->contextwrite = tagwrite;
    r->offwrite = offwrite;
    r->offset = offset;
    r->data = p;
    
    
    if (references.offsets.available == 0) {
        #define OFFSET_REFERENCE_PAGE 16
        references.offsets.item =
            (offset_reference**) realloc2(references.offsets.item, sizeof(offset_reference*) * (references.offsets.size + OFFSET_REFERENCE_PAGE));
        references.offsets.available = OFFSET_REFERENCE_PAGE;
        }
    int i = (references.offsets.size)++;
    references.offsets.item[i] = r;
    (references.offsets.available)--;
    return references.offsets.item[i];
}

//Creates a reference to an undeclared undefined_label that will be written in the corresponding context when it is declared
//The associated context and offset will be written in ref
//The reference is taken from name, the name of the label
label_reference* create_label_reference(undefined_label* label, offset_reference* ref)
{
    label_reference * r = (label_reference*) malloc2(sizeof(label_reference));
    r->label = label;
    r->reference = ref;
    r->line = line; r->column = column;
    
    if (references.labels.available == 0) {
        #define LABELS_REFERENCE_PAGE 16
        references.labels.item =
            (label_reference**) realloc2(references.labels.item, sizeof(label_reference*) * (references.labels.size + LABELS_REFERENCE_PAGE));
        references.labels.available = LABELS_REFERENCE_PAGE;
        }
    int i = (references.labels.size)++;
    references.labels.item[i] = r;
    (references.labels.available)--;
    return references.labels.item[i];
}

//Add this block to the block list
//Returns the number of block, which serves as a 'ticket'
int add_block(exec_block b)
{
    //Check the script is not too big
    contexts.totalscriptsize += b.size;
    if ( lang.options.maxfilesize == FILESIZE_MAX ?
         !val_fits_in_bytes(contexts.totalscriptsize, lang.options.offsetWidth)
    :    contexts.totalscriptsize > lang.options.maxfilesize )
        err_sem("Script doesn't adjust to size restriction imposed by the language.");
    
    //Add new context
    struct context c;
    c.tag = current_context();
    c.block = b;
    
    if (contexts.available == 0) {
        contexts.context = (struct context*) realloc2(contexts.context, sizeof(struct context) * (contexts.size + CONTEXT_PAGE));
        contexts.available = CONTEXT_PAGE;
        }
    contexts.context[(contexts.size)++] = c;
    (contexts.available)--;     
    return c.tag;
}

//Retrieves an undefined label, returning NULL if it can't be found (because it is either unexistant or defined)
undefined_label* fetch_undefined_label(char* name)
{
    int i;
    for (i=0; i < labels.undefined.size; i++)
        if ( !strcmp(labels.undefined.pending[i].label->name, name) )
            return &( labels.undefined.pending[i] );
    return NULL;
}

//Solves the associated label references, with the known tag and offset
bool solve_label_reference(label_reference* lref)
{
    if (!lref->label->declared)
        return false;
    else {
        lref->reference->offset  = lref->label->offset;
        lref->reference->tag     = lref->label->context;
        return true;
        }
}

//Adds a label to the defined label list
//No checks are made on whether the label already exists
label* add_label(char* name, unsigned int tg, unsigned int offset)
{
    label * l = (label*) malloc2(sizeof(label));
    l->name   = strdup(name);
    l->target = tg;
    l->offset = offset;
    
    if (labels.defined.available == 0) {
        #define LABELS_DEFINED_PAGE 32
        labels.defined.label = (label**) realloc2(labels.defined.label, sizeof(label*) * (labels.defined.size + LABELS_DEFINED_PAGE));
        labels.defined.available = LABELS_DEFINED_PAGE;
        }
    labels.defined.label[(labels.defined.size)++] = l;  
    (labels.defined.available)--;
    
    return l;
}

//Add a label to the list
//Returns false if and only if a label with that name already exists
//declaring == false when label is just being menctioned (i.e: not declared)
bool add_undefined_label(char* name, unsigned context, unsigned int offset, bool declaring,undefined_label ** ret)
{
    undefined_label ul, *ulp = fetch_undefined_label(name);
    bool declaring_menctioned = false; //If when we're done declaring, we must solve a reference
    if ( ulp != NULL) { //Check it exists
        if (ret != NULL) *ret = ulp;
        //If it exists, it can't be repeated. That happens if we're trying to declare it again
        if (declaring) {
            if (ulp->declared) return false;
            else declaring_menctioned = true;
            }
        else {
            return true; // Already registered, but not declared. This menction adds nothing new
            }
        }
    else {
        if (declaring && (fetch_label(name) != NULL))
            return false;
        }
    //Create a label
    ul.context = context;
    ul.offset = offset;
    ul.label = add_label(name, target, 0xFFFFFFFF); //Add to defined label list
    offset_reference* ref =
        create_offset_reference(ul.context, ul.offset, 0xFFFFFFFF, 0xFFFFFFFF, &(ul.label->offset)); //Require the offset when it's known
    
    bool add = false;
    if(!declaring) {
        //If we aren't declaring, actually we don't know the tag and offset
        //We create a reference that must be solved later
        ul.declared = false;
        add = true;
        }
    else {
        ul.declared = true;
        if (declaring_menctioned) {
            *ulp = ul;
            }
        else add = true;
        }
    if (add) {      
        //Add label to array
        if (labels.undefined.available == 0) {
            #define LABELS_UNDEFINED_PAGE 16
            labels.undefined.pending = (undefined_label*)
                realloc2(labels.undefined.pending, sizeof(undefined_label) * (labels.undefined.size + LABELS_UNDEFINED_PAGE));
            labels.undefined.available = LABELS_UNDEFINED_PAGE;
            }
        labels.undefined.pending[labels.undefined.size] = ul;  
        if (ret != NULL) *ret = &labels.undefined.pending[labels.undefined.size];
        (labels.undefined.size)++;
        (labels.undefined.available)--;
        }

    
    return true;
}

//Retrieves a label by name
label* fetch_label(char* name)
{
    int i;
    for (i=0; i < labels.defined.size; i++) {
        if ( !strcmp(labels.defined.label[i]->name, name) )
            return labels.defined.label[i];
        }
    return NULL;
}

//Adds a value to the list
//Returns false if and only if a value with that name already exists
bool add_value(char* id, int value)
{
    if (value_of(id, NULL)) return false;
    else {
        if (values.available == 0) {
            #define VALUES_PAGE 32
            values.name = (char**) realloc2(values.name, sizeof(char*) * (values.size + VALUES_PAGE));
            values.value = (int*) realloc2(values.value, sizeof(int) * (values.size + VALUES_PAGE));
            values.available = VALUES_PAGE;
            }
        int i = (values.size)++;
        values.name[i] = id;  
        values.value[i] = value;  
        (values.available)--;        
        }
    return true;
}

// Finds the value defined as id and stores in v if it exists and is not NULL
// Returns false if and only if it doesn't exist, and true otherwise
bool value_of(char* id, int* v)
{
    int i;
    for (i=0; i<values.size; i++)
        if ( !strcmp(values.name[i], id) ) {
            if (v != NULL) *v = values.value[i];
            return true;
            }
    return false;
}


// Read exports from file
void readexport (FILE* file)
{
int size = atoi2 (avanzalinea (file));
int i = 0;
for (i = 0; i < size; i++) {
	int islabel = atoi2 (avanzalinea (file)); 
	// Label
	if (islabel)
		{
		char* name = strdup (avanzalinea (file));
		int target = atoi2 (avanzalinea (file));
		int offset = atoi2 (avanzalinea (file));
		if ( fetch_label(name) != NULL) err_sem ("Label \"%s\" redeclared (while importing).", name);
		add_label (name, target, offset);
		}
	// Value
	else {
		char* name = strdup (avanzalinea (file));
		int value = atoi2 (avanzalinea (file));
		if (! add_value (name, value)) err_sem ("Value \"%s\" redeclared (while importing).", name);
		}
	}
}

// Writes an export to file
void writeexport (array_str e, FILE* file)
{
fprintf (file, "%d\n", e.size);
int i = 0;
for (i = 0; i < e.size; i++) {
	bool islabel = (e.items[i][0] == '@');
	fprintf (file, "%d\n", islabel);
	// Label
	if (islabel) {
		label* l;
		if ( (l = fetch_label(e.items[i])) == NULL) err_sem ("Label \"%s\" not defined.", e.items[i]);
		fprintf (file, "%s\n", l->name);
		fprintf (file, "%d\n", l->target);
		fprintf (file, "%d\n", l->offset);
		}
	// Value
	else {
		int value;
		if (!value_of (e.items[i], &value)) err_sem ("Value \"%s\" not defined.", e.items[i]);
		fprintf (file, "%s\n", e.items[i]);
		fprintf (file, "%d\n", value);
		}
    free(e.items[i]);
	}
}

// writeexport, outputting a C header file
void writeexportH (array_str e, FILE* file)
{
fprintf (file,
"/******************************************************\n"        
" Export from script in language %s \n"      
" Generated by Script-to-Bytecode's ScriptCompiler \n"      
"******************************************************/\n\n",
lang.options.name);
int i = 0;
for (i = 0; i < e.size; i++) {
	bool islabel = (e.items[i][0] == '@');
	// Label
	if (islabel) {
		label* l;
		if ( (l = fetch_label(e.items[i])) == NULL) err_sem ("Label \"%s\" not defined.", e.items[i]);
		fprintf (file, "#define %s_%s_TARGET\t%d\n"
                       "#define %s_%s_OFFSET\t%d\n"
                       "\n"
                       , lang.options.name, l->name, l->target
                       , lang.options.name, l->name, l->offset
                       );
		}
	// Value
	else {
		int value;
		if (!value_of (e.items[i], &value)) err_sem ("Value \"%s\" not defined.", e.items[i]);
		fprintf (file, "#define %s_%s\n\t%d\n", lang.options.name, target, e.items[i], value);
		}
	}
}

//Opens a language from a file and sets it as the global variable
//Returns false if the language doesn't exist
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

//Opens a file with definitions and loads them as names (labels and values)
//Returns false if the file can't be found
bool import (char* name)
{
    char buffer [strlen(name) + 5];
    sprintf (buffer, "%s.def", name);
    FILE* file = fopen (buffer, "r");
    if (file == NULL) return false;
    readexport (file);
    fclose (file);
    return true;
}

//Prepares a list of names to be exported to a file (if filename is NULL, to a default file)
//Returns NULL if success, or the repeated name, if any (and therefore the operation can't be completed)
//Repetitions in the very array aren't checked (this is assumed to be already done)
char* set_exportables(array_str exports, char* filename)
{    
    //Look for a previous list to write in the same file
    int i, j, k;
    for (i=0; i<exportables.size; i++) {
        if ( !strcmp(exportables.data[i].filename, filename) ) {
            //Check that names aren't repeated
            for(j=0; j < exports.size; j++) {
                char* jname = exports.items[j];
                for(k=0; k < exportables.data[i].data.size; k++) {
                    char* kname =  exportables.data[i].data.items[k];
                    if ( !strcmp(jname, kname) )
                        return jname;
                    }
                }
            //Add to this file's list
            int old_size = exportables.data[i].data.size;
            exportables.data[i].data.size += exports.size;
            exportables.data[i].data.items = (char**) realloc2(exportables.data[i].data.items, sizeof(char*) * exportables.data[i].data.size);
            for(j=0; j < exports.size; j++) {
                char* jname = exports.items[j];
                exportables.data[i].data.items[old_size + j] = jname;
                }
            exportables.data[i].data.available = 0;
            return NULL;
            }
        }
    //If there werent previous exportations for this file, create a new exporting entry
    struct exports e;
    e.filename = filename;
    e.data = exports;
    if (contexts.available == 0) {
        exportables.data = (struct exports*) realloc2(exportables.data, sizeof(struct exports) * (exportables.size + EXPORTABLES_PAGE));
        exportables.available = EXPORTABLES_PAGE;
        }
    exportables.data[(exportables.size)++] = e;
    (exportables.available)--;    
    return NULL;
}


//Writes to an array of bytes, following the rules of the language, a value of a known width
void lang_putvalue (int width, int data, char* array)
{
    int i;
    if (lang.options.endian == ENDIAN_BIG)
        for (i=0; i<width; i++)
            array[i] = ( data & (0xFF << (8*(width-1-i))) ) >> (8*(width-1-i));
    else 
        for (i=0; i<width; i++)
            array[i] = ( data & (0xFF << (8*(i))) ) >> (8*(i));
}

//Looks for the array of a block with a certain tag
unsigned char* get_block_array(unsigned int tag)
{
    int i;
    for (i=0; i<contexts.size; i++) {
        if (contexts.context[i].tag == tag) {
            return contexts.context[i].block.data;
            }
        }
}

//Solves the requested references to offset, knowing the value of the offset that was unknown before
void solve_offset_references(unsigned int tag, unsigned int offset)
{
    int i, final;
    for (i=0; i < references.offsets.size; i++) {
        offset_reference * r = references.offsets.item[i];
        if ( r->tag == tag) {
            final = offset + r->offset;
            if (r->offwrite != 0xFFFFFFFF) {
                unsigned char* array = get_block_array(r->contextwrite);
                if (array == NULL)
                    err_other("Error fatal: context %d doesn't exist.", r->contextwrite);
                lang_putvalue(lang.options.offsetWidth, final, &(array[r->offwrite]));
                }
            if (r->data  != NULL) *(r->data) = final;
            }
        }
}

//Writes a block of code to file
void output_block(exec_block b)
{
    if ( fwrite(b.data, 1, b.size, fout) < b.size )
        err_other("Error fatal: error when writing in the output file.");
}


//Writes a block of code to file as text
void output_blockH(exec_block b)
{
    int i;
    for (i=0; i<b.size-1; i++)
        fprintf(fout, "%u, ", b.data[i]);
    fprintf(fout, "%u", b.data[b.size-1]);
}

/*
void debug_printAllBlocks()
{
    printf("BLOCKS\n\n");
    int i, j, l;
    for(i=contexts.size-1; i>=0; i--) {
        exec_block b = contexts.context[i].block;
        l=0;
        for(j=0; j<b.size; j++) {
            printf("%02x|", b.data[j]);
            if (++l >= 16) {printf("\n"); l = 0;}
            }
        printf("\n\n");
        }
    printf("-------------------\n\n");
}
*/

//Writes all code blocks, starting by the main one, solving all the references
void output_all_blocks()
{
                
    int i, finalLine = line;
            
    //Solve all label references
    for(i=0; i<references.labels.size; i++) {
        line = references.labels.item[i]->line;
        column = references.labels.item[i]->column;
        if (!solve_label_reference(references.labels.item[i]))
            err_sem("Label \"%s\" used but not defined.", references.labels.item[i]->label->label->name);
        }
    line = finalLine;
    
    unsigned int offset = 0;
    
    //Solve all offset references
    for(i=contexts.size-1; i>=0; i--) {
        struct context c = contexts.context[i];
        solve_offset_references(c.tag, offset);
        verbose("[%d] = offset 0x%x", c.tag, offset);
        offset += c.block.size;
        }
    
    //Write to file
    if (generate_code) {
        
        if (!scriptH) {
            //Create output file
            char filename[11+strlen(lang.options.name)+1];
            sprintf(filename, "%d.%s", target, lang.options.name);
            fout = fopen(filename, "wb");
            if (fout == NULL) err_other("Error when trying to open \"%s\" for writing.", filename);
            
            //Write to file
            for(i=contexts.size-1; i>=0; i--) {
                exec_block b = contexts.context[i].block;
                output_block(b);
                free(b.data);
                }
            }
        else { //Write as C header file
            //Create output file
            char filename[13+strlen(lang.options.name)+1];
            sprintf(filename, "%s_%d.h",  lang.options.name, target);
            fout = fopen(filename, "wb");
            if (fout == NULL) err_other("Error when trying to open \"%s\" for writing.", filename);
            
            //Write to file
            fprintf (fout,
"/******************************************************\n"        
" Script %d in language %s \n"      
" Generated by Script-to-Bytecode's ScriptCompiler \n"      
"******************************************************/\n"
"#include <stdint.h>\n"
"\n"
"const uint8_t %s_SCR_%d = {\n\t",
                target, lang.options.name, lang.options.name, target
                );
            exec_block b;
            for(i=contexts.size-1; i>0; i--) {
                b = contexts.context[i].block;
                output_blockH(b);
                fprintf(fout, ",\n\t");
                free(b.data);
                }
            b = contexts.context[0].block;
            output_blockH(b);
            free(b.data);
            fprintf (fout, "\n\t};\n");
            }
    }
    //Free some memory
    free(contexts.context); contexts.size=0; contexts.available=0;
    free(contexts.stack.data); contexts.stack.size=0; contexts.stack.available=0;
}

//Find the described instruction
bool instruction_patternMatching(iparamp_array params, instruction* ret)
{
int i, j;
bool equivalent;
char* size_problems;
//For each instruction
for (i=0; i<lang.instructions.numInstructions; i++) {
    //Check number of tokens and mask
    if (params.amount == lang.instructions.instructions[i].params.numParams
    &&  params.mask   == lang.instructions.instructions[i].params.mask) {
        size_problems = NULL;
        //If mask matches, test each token
        equivalent = true;
        for (j=0; j<params.amount; j++) {
            instruction_param ptest   = lang.instructions.instructions[i].params.params[j];
            instruction_param pscript = *params.items[j];
            if ( param_diff(ptest, pscript, true) ) //Find differences
                equivalent = false;
            else if ((ptest.type == TYPE_VAL) && !val_fits_in_bytes(pscript.extra, ptest.extra))
                size_problems = ptest.name;
            }
        if (equivalent) {
            if (size_problems != NULL)
                err_sem("Value too big for parameter \"%s\".", size_problems);
            if (ret != NULL) *ret = lang.instructions.instructions[i];
            return true;
            }
        }
    }
return false;
}

//Searches the parameter of a given name and stores it in p if p is not NULL
//Returns the position of the parameter (first is 0)
//If the parameter doesn't exist, the number of paramters is returned
int get_param_from_name(instruction ins, char* name, instruction_param* p)
{
    int i;
    for(i=0; i<ins.params.numParams; i++)
        if ( !strcmp(ins.params.params[i].name, name) ) {
            if (p != NULL) *p = ins.params.params[i];
            return i;
            }
    return i;
}

//Reserves more space for a exec_block if needed
void exec_block_need(unsigned int need, exec_block* e)
{
    if (e->available < need) {
        e->data = (unsigned char*) realloc2( e->data, sizeof(unsigned char) * (e->size + need + BLOCK_PAGE) );
        e->available = need + BLOCK_PAGE;
        }
    e->size += need;
    e->available -= need;
}

//Retrieve a command from code, and store it in c if c!= NULL
//Returns false only if the command doesn't exist
bool get_command_from_code(int code, command* c)
{
    int i;
    for  (i=0; i<lang.commands.numCommands; i++) {
        if (lang.commands.commands[i].code == code) {
            if (c != NULL) *c = lang.commands.commands[i];
            return true;
            }
        }
    return false;
}

unsigned getUIntPrintLength(unsigned n)
{
    int i, tens = 10;
    for(i=1; tens < n; i++, tens*=10);
    return i;
}

char* getInnerLabelNameFor(char* original)
{
    //Get a new prefix if we didn't get it already
    if (!innerLabelMgmt.used) {
        innerLabelMgmt.used = true;
        innerLabelMgmt.n ++;
        }
    //Make the new label name
    char* ret = (char*) malloc2(sizeof(char)* (strlen(original) + 3 + getUIntPrintLength(innerLabelMgmt.n)) );
    sprintf(ret, "#i%u%s", innerLabelMgmt.n, original);
    return ret;
}

void resetInnerLabelContext(void)
{
    innerLabelMgmt.used = false;
}

//Builds a block from an instruction in the language, an instruction in the script and a translation
exec_block build_block_instruction(instruction ins, iparamp_array params, translation trans, unsigned context)
{
    //Create a blank block
    exec_block ret = empty_block; offset_reference * ref;
    ret.size = 0;
    ret.data = (unsigned char*) malloc2(sizeof(unsigned char) * 16);
    ret.available = 16;
    ret.referenced.size = 0;
    ret.referenced.available = 0;
    ret.referenced.item = NULL;
    ret.referenced.innerlabel.size = 0;
    ret.referenced.innerlabel.available = 0;
    ret.referenced.innerlabel.item = NULL;
    
    
    
    //Build the translation of the instruction
    int i, j = 0, n, value, width, k;
    instruction_param p;
    command com; int nparams;
    label* l; undefined_label* ul;
    char* innerLabelName;
    if (verbose_trans_mode) verbose("\tTRANSLATION");
    
    for (i=0; i < trans.size; i++) {
        Translation_item t = trans.items[i];
        switch (t->type) {
            case TRANS_COMMAND:
                exec_block_need(lang.options.codeWidth, &ret);
                lang_putvalue(lang.options.codeWidth, t->value.command.code, &(ret.data[j]));
                j += lang.options.codeWidth;
                
                //Change com so parameters (and their width) can be known
                if ( !get_command_from_code(t->value.command.code, &com) )
                    err_other("Error in language: command not found: \"%s\" (%d).",
                                t->value.command.name, t->value.command.code);
                if (verbose_trans_mode) verbose("\t\t\tcommand %s (%d)", com.name, com.code);
                nparams = 0;
                break;
            case TRANS_LIT:
                exec_block_need(t->value.number.width, &ret);
                lang_putvalue(t->value.number.width, t->value.number.val, &(ret.data[j]));
                j += t->value.number.width;
                if (verbose_trans_mode) verbose("\t\t\tliteral 0x%x", t->value.number.val);
                break;
            case TRANS_ID: // => TYPE_VAL
                n = get_param_from_name(ins, t->value.str, &p);
                //Extract width of current command (com)
                width = com.params.params[nparams-1].width;
                value = params.items[n]->extra;
                exec_block_need(width, &ret);
                lang_putvalue(width, value, &(ret.data[j]));
                j += width;
                if (verbose_trans_mode) verbose("\t\t\tparameter %s = 0x%x", params.items[n]->name, value);
                break;
            case TRANS_BLOCK: // => TYPE_BLOCK
                n = get_param_from_name(ins, t->value.str, &p);
                value = params.items[n]->extra;
                exec_block_need(lang.options.offsetWidth, &ret);
                ref = create_offset_reference(value, 0, context, j, NULL);
                lang_putvalue(lang.options.offsetWidth, 0xFFFFFFFF, &(ret.data[j]));
                //The resulting block can be concatenated, so this reference must be registered
                add_concat_reference(&ret, ref);
                j += lang.options.offsetWidth;
                if (verbose_trans_mode) verbose("\t\t\tparameter %s = [%d]", params.items[n]->name, value);
                break;
            case TRANS_LTARGET:
                n = get_param_from_name(ins, t->value.str, &p);
                l = fetch_label(params.items[n]->name);
                exec_block_need(lang.options.targetWidth, &ret);
                lang_putvalue(lang.options.targetWidth, l->target, &(ret.data[j]));
                j += lang.options.targetWidth;
                if (verbose_trans_mode) verbose("\t\t\tparameter %s#target = %d", params.items[n]->name, l->target);
                break;
            case TRANS_LOFFSET:
                n = get_param_from_name(ins, t->value.str, &p);
                ul = fetch_undefined_label(params.items[n]->name);
                exec_block_need(lang.options.offsetWidth, &ret);
                if (ul != NULL) {
                    ref = create_offset_reference(ul->context, ul->offset, context, j, NULL);
                    lang_putvalue(lang.options.offsetWidth, 0xFFFFFFFF, &(ret.data[j]));
                    //If the label wasn't declared, we have to create a reference to this reference,
                    //because the context and offset of ul aren't valid
                    if (!ul->declared) create_label_reference(ul, ref);
                    //The resulting block can be concatenated, so this reference must be registered
                    add_concat_reference(&ret, ref);
                    if (verbose_trans_mode) verbose("\t\t\tparameter %s#offset", params.items[n]->name);
                    }
                else {
                    l = fetch_label(params.items[n]->name);
                    if (l == NULL)  //Impossible, because when reading paramters,
                                    //if the label doesn't exist, it is created with a reference
                        err_other("Fatal error: Program logic should have forced the label to exist at this point.");
                    lang_putvalue(lang.options.offsetWidth, l->offset, &(ret.data[j]));
                    if (verbose_trans_mode) verbose("\t\tparameter %s#offset = 0x%x", params.items[n]->name, l->offset);
                    }
                j += lang.options.offsetWidth;
                break;
            case TRANS_STRING:
                exec_block_need(strlen(t->value.str) + 1, &ret);
                j += sprintf(&(ret.data[j]), "%s", t->value.str) + 1;
                if (verbose_trans_mode) verbose("\t\t\tstring \"%s\"", t->value.str);
                break;
            case TRANS_CURTARGET:
                exec_block_need(lang.options.targetWidth, &ret);
                lang_putvalue(lang.options.targetWidth, target, &(ret.data[j]));
                j += lang.options.targetWidth;
                if (verbose_trans_mode) verbose("\t\t\tcurrent target (%d)", target);
                break;
            case TRANS_INNERLABEL:
                innerLabelName = getInnerLabelNameFor(t->value.str);
                if ( !add_undefined_label(innerLabelName, context, j, true, &ul)  )
                        err_other("Fatal error: Translation scope label \"%s\" redeclared.", t->value.str);
                else add_innerlabel_concat_reference(&ret, ul); //The resulting block may be concatenated
                    
                
                free(innerLabelName);
                if (verbose_trans_mode) verbose("\t\t%s:", t->value.str);
                break;
            case TRANS_LABEL:
                innerLabelName = getInnerLabelNameFor(t->value.str);
                //If the language compiler is correct, this isn't a parameter label
                ul = fetch_undefined_label(innerLabelName);
                if (ul == NULL) {
                    if ( !add_undefined_label(innerLabelName, context, 0xFFFFFFFF, false, &ul)  )
                        err_other("Fatal error: Translation scope label \"%s\" redeclared.", t->value.str);
                    else add_innerlabel_concat_reference(&ret, ul); //The resulting block may be concatenated
                    }
                ref = create_offset_reference(ul->context, ul->offset, context, j, NULL);
                //If the label wasn't declared, we have to create a reference to this reference,
                //because the context and offset of ul aren't valid
                if (!ul->declared)
                    create_label_reference(ul, ref);//And here's why we need to rename these inner labels
                free(innerLabelName);
                //The resulting block can be concatenated, so this reference must be registered
                add_concat_reference(&ret, ref);
                exec_block_need(lang.options.offsetWidth, &ret);
                lang_putvalue(lang.options.offsetWidth, 0xFFFFFFFF, &(ret.data[j]));
                j += lang.options.offsetWidth;
                if (verbose_trans_mode) verbose("\t\t\tinner label %s", t->value.str);
                break;
            default: err_other("Error in language: translation type \"%d\".", t->type); break;
            }
        nparams++;
        }
    if (verbose_trans_mode) verbose("\tFOR:");
    return ret;
}

//Gets the pointer to block from the list
exec_block* get_block(unsigned int number)
{
int i;
for (i=0; i< contexts.size; i++)
    if (contexts.context[i].tag == number)
        return &(contexts.context[i].block);
}

//Generates code from an instruction
exec_block block_instruction(iparamp_array params)
{
    //Find the instruction used
    instruction ins;
    if ( !instruction_patternMatching(params, &ins) ) {
        err_sem("Unknown instruction.");
        }
    int i,j;
        
    //Unify names
    for (i=0; i<ins.params.numParams; i++)
        if (params.items[i]->type == TYPE_VAL
        ||  params.items[i]->type == TYPE_BLOCK)
            params.items[i]->name = ins.params.params[i].name;
    
    //Insert modifications
    exec_block b, *b_old;
    for (i=0; i<ins.params.numParams; i++) {
        struct modify m = ins.params.params[i].modify;
        for (j=0; j<m.numMods; j++)
            switch (m.mods[j].type) {
                case MOD_BEGIN:
                    b_old = get_block(params.items[i]->extra);
                    b = build_block_instruction(ins, params, m.mods[j].data.translation, params.items[i]->extra);
                    *b_old = concat_block(b, *b_old);
                    break;
                case MOD_END:
                    b_old = get_block(params.items[i]->extra);
                    b = build_block_instruction(ins, params, m.mods[j].data.translation, params.items[i]->extra);
                    *b_old = concat_block(*b_old, b);
                    break;
                default:break;
                }
        }
    
    //Create into the current block
    exec_block ret = build_block_instruction(ins, params, ins.translation, current_context());
    resetInnerLabelContext();
    return ret;
}

//Creates a block of code from an array of integers of a given width
exec_block block(int width, int* data, int n)
{
    int i; exec_block ret = empty_block;
    ret.size = width * n;
    ret.available = 0;
    ret.data = (unsigned char*)malloc2(sizeof(unsigned char) * ret.size);
    for (i=0; i < n; i++)
        lang_putvalue(width, data[i], &(ret.data[i*width]));
    return ret;
}

//Creates a block of code from an array of characters
exec_block block_string(char* string)
{
    if (!string) return empty_block;
    exec_block ret = empty_block;
    ret.size = strlen(string)+1;
    ret.available = 0;
    ret.data = (unsigned char*)malloc2(sizeof(char) * ret.size);
    sprintf(ret.data, "%s", string);
    ret.data[ret.size-1] = 0;
    return ret;
}

//pushes the innerlabels when concatenating blocks
//dest can be either a or b or neither
void push_innerlabel_concat(exec_block* dest, exec_block a, exec_block b)
{
    int i;
    //We'll work in separated variables in case dest==a or dest==b
    unsigned size = a.referenced.innerlabel.size + b.referenced.innerlabel.size;
    unsigned available = 0;
    undefined_label** item = (undefined_label**)malloc2(sizeof(undefined_label*) * size);
    
    for (i=0; i<a.referenced.innerlabel.size; i++)
        item[i] = a.referenced.innerlabel.item[i];
        
    for (; i<size; i++) {
        item[i] = b.referenced.innerlabel.item[i-a.referenced.innerlabel.size];
        undefined_label * ul = b.referenced.innerlabel.item[i-a.referenced.innerlabel.size];
        if (ul->offset != 0xFFFFFFFF) {
            (ul->offset) += a.size;
            }
        }
    //discards a and b's related arrays
    if (a.referenced.innerlabel.item != NULL) free(a.referenced.innerlabel.item);
    if (b.referenced.innerlabel.item != NULL) free(b.referenced.innerlabel.item);
    
    dest->referenced.innerlabel.size = size;
    dest->referenced.innerlabel.available = available;
    dest->referenced.innerlabel.item = item;
}

//Concat two blocks of code
//frees the memory used by the source blocks
exec_block concat_block(exec_block a, exec_block b)
{
   
    // Particular cases: empty blocks
    if (a.size == 0) {
        push_innerlabel_concat(&b, a, b);
        return b;
        }
    if (b.size == 0) {
        push_innerlabel_concat(&a, a, b);
        return a;
        }
    
    // General case
    exec_block ret = empty_block;
    ret.size = a.size + b.size;
    ret.available = 0;
    ret.data = (unsigned char*)malloc2(sizeof(unsigned char) * ret.size);
    int i;
    for (i=0; i<a.size; i++)
        ret.data[i] = a.data[i];
    for (; i<ret.size; i++)
        ret.data[i] = b.data[i-a.size];
    free(a.data);
    free(b.data);
    
    //Adjust concat references: those in b are now in a (ret), and have been pushed a.size bytes.
    ret.referenced.size = a.referenced.size + b.referenced.size;
    ret.referenced.available = 0;
    ret.referenced.item = (offset_reference**)malloc2(sizeof(offset_reference*) * ret.referenced.size);
    for (i=0; i<a.referenced.size; i++)
        ret.referenced.item[i] = a.referenced.item[i];
    for (; i<ret.referenced.size; i++) {
        ret.referenced.item[i] = b.referenced.item[i-a.referenced.size];
        offset_reference * r = b.referenced.item[i-a.referenced.size];
        if (r->offwrite != 0xFFFFFFFF) {
            r->offwrite += a.size;
            }
        }
    if (a.referenced.item != NULL) free(a.referenced.item);
    if (b.referenced.item != NULL) free(b.referenced.item);
    
    push_innerlabel_concat(&ret, a, b);
    return ret;
}

// Save exports
void save_export()
{
int i;
for (i = 0; i < exportables.size; i++) {
    int len = exportables.data[i].filename != NULL ? strlen(exportables.data[i].filename) + 5 + 2 : 25;

	char buffer [len], bufferH [len];
    if (exportables.data[i].filename != NULL) {
        sprintf (buffer, "%s.def", exportables.data[i].filename);
        free(exportables.data[i].filename);
        }
    else {
        if (generate_code)
            sprintf (buffer, "defs%d.def", target);
        else
            sprintf (buffer, "%s.def", source_name);
        }
    FILE* file;
    if (exportsH) {
        //Also export to a C (h) file
        sprintf (bufferH, "%s.h", buffer);
        file = fopen (bufferH, "w");
        writeexportH (exportables.data[i].data, file);
        fclose(file);
        verbose("Writing exports to %s as C definitions.", bufferH);
        }
	file = fopen (buffer, "w");
	writeexport (exportables.data[i].data, file);
    verbose("Writing exports to %s", buffer);
	fclose (file);
	}
free(exportables.data); exportables.size = 0; exportables.available=0;
}

//Gets options from a string of -options
int getOptions(char* argv)
{
    int i, len = strlen(argv);
    for (i=1; i<len; i++)
        switch (argv[i]) {
            case 'v':  verbose_mode = true; break;
            case 'V':  verbose_trans_mode = verbose_mode = true; break;
            case 's':  scriptH = true; break;
            case 'e':  exportsH = true; break;
            default: err_other("Option \"%c\" unknown.", argv[i]); break;
            }

}

int main(int argc, char* argv[])
{
    int i;
    
    //Get options
    for (i=1; i<argc; i++)
        if (argv[i][0] == '-')
            getOptions(argv[i]);
    
    //Parse
    for (i=1; i<argc; i++) {
        if (argv[i][0] != '-') {
            init();
            yyin = fopen(argv[i], "rb");
            if (yyin == NULL)
                err_other("\"%s\" not found.", argv[i]);
            verbose("Reading %s...", argv[i]);
            if (yyparse() == 0) {
                save_export();
                verbose("...done\n");
                }
            YY_FLUSH_BUFFER;
            }
        }
}