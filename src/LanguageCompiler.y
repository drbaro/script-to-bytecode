%error-verbose

%{
#include "src/common.h" //Since flex always outputs at cwd, we won't bother to do that on bison. This is the route relative to common.h from the root

//Program options
bool
        verbose_mode = false,   //Print everything you're doing in stdout
        langH = false,          //Write data about the language in the <lang>.h file
        widthH = false,         //Write defined widths in the <lang>.h file
        commandH = false,       //Write command names in the <lang>.h file
        codeStub = false       //Write an imperative script interpreter stub
    ;

//__TRACE is used in debug mode to print bison information during the compilation. That's why it's in every rule.
#ifdef DEBUG_TRACEALL
#define __TRACE ({printf("\n"); yydebug=1;})
#else
#define __TRACE
#endif

int line, column;

//References to codes that haven't been defined as literals
typedef struct code_reference {
    char* id;                    //Unique name of the command
    unsigned int number;         //Number of references to this command
    unsigned int available;      //Reserved space
    int** references;            //Array of pointers where the final code is requested.

} code_reference;

#define COMMAND_CODEUNKNOWN 0xFFFFFFFF


//Modifications as read from the source
typedef struct mod_list {
    unsigned int size, available;
    char* paramname;
    modification * mods;
} mod_list;

//List of diferent sets of modifications
typedef struct mod_list_list {
    unsigned int size, available;
    mod_list * mods;
} mod_list_list;

//Adds an instruction to the language
//Calls err_sem if the instruction already exists
void add_instruction(instruction);

//Adds a command to the language
//Calls err_sem if a command of the same name already exists
void add_command(command*);

//Generate an instruction from a command
instruction instruction_from_command(command*);

//Creates a reference to command
void create_reference(char* id, int* place);

//Creates an entry for the new width
//Returns the pointer to the value that width will take
//Returns NULL if there's already a width using that name
unsigned char* width_definition_new(char* name);

//Retrieve the value of a defined width
//0 => not defined
unsigned char get_width(char*);

//Retrieve a command from an id, and store in c if c is not NULL
//Returns false if and only if the command doesn't exist
bool get_command_from_id(char* id, command* c);

//Unify instruction and its translations
//Blocks are identified, incongruences found
//Modifications are asociated with the corresponding parameter.
void unifyTranslation(instruction*, translation, mod_list_list);

//Save instructions and commands read in the language
void put_instructions_and_commands(void);


/* Structures used during compilation */

unsigned char mask_set; //Mask to check if an option has been defined
#define MASK_CODEWDTH   0x1
#define MASK_TARGETWDTH 0x2
#define MASK_OFFSETWDTH 0x4
#define MASK_ENDIAN     0x8
#define MASK_FILESIZE   0x10
#define MASK_ALL (MASK_CODEWDTH|MASK_TARGETWDTH|MASK_OFFSETWDTH|MASK_ENDIAN|MASK_FILESIZE)

void init_lang(void) //Initialize lang
{
    lang.options.name = NULL;
    lang.options.codeWidth = 1; lang.options.targetWidth = 1; lang.options.offsetWidth = 1;
    lang.options.endian = ENDIAN_BIG;
    lang.options.maxfilesize = FILESIZE_MAX; //Maximum posible (depends on offset size)

    lang.commands.numCommands = 0; lang.commands.commands = NULL;
    lang.instructions.numInstructions = 0; lang.instructions.instructions = NULL;
}

//References to code
struct {
    unsigned int size;      //How many in use
    unsigned int available; //How many have been reserved for future use
    code_reference* items;
} references;

//Widths
struct {
    unsigned int size;      //How many in use
    unsigned int available; //How many have been reserved for future use
    char** id;              //Identifiers
    char* width;            //widths (id[i] => width[i])
} widths;


//Commands
struct {
    unsigned int size;      //How many in use
    unsigned int available; //How many have been reserved for future use
    command** items;
} commands;

//Instructions
static struct {
    unsigned int size;      //How many in use
    unsigned int available; //How many have been reserved for future use
    instruction* items;
} instructions;

//Init everything
void init(void)
{
    line=1; column=1;
    mask_set = 0;
    init_lang();
    references.size = 0;    references.available = 0;   references.items = NULL;
    commands.size = 0;      commands.available = 0;     commands.items = NULL;
    instructions.size = 0;  instructions.available = 0; instructions.items = NULL;
    widths.size = 0; widths.available = 0; widths.id = NULL; widths.width = NULL;
}

%}

%union {
    char*   str;
    int     int_t;
    bool    bool_t;
    
    struct widthdef {
        unsigned char* ptr;
        char* name;
        } widthdef;
    
    command*                commandp;
    command_params          comm_params;
    
    instruction             instruction;
    instruction_param       instruction_param;
    instruction_params      instruction_params;
    
    translation             translation;
    Translation_item        Translation_item;
    
    modification            modification;
    mod_list                mod_list;
    mod_list_list           mod_list_list;
}


%token CODE             ":code"
%token TARGET           ":target"
%token OFFSET           ":offset"

%token CODE_FRESH       "#fresh"
%token LOCAL            "#local"
%token RAW              "#raw"
%token RAW_STRING       "#string"
%token LANGUAGE         "#language" 
%token INSTRUCTION      "#instruction" 
%token COMMAND          "#command"
%token DECLARE          "#let"
%token ENDIAN           "#endian"
%token FILESIZE         "#filesize"
%token CODE_TYPE        "#code"
%token TARGET_TYPE      "#target"
%token OFFSET_TYPE      "#offset"

%token<str> STRING      "<string>"
%token<str> WIDTH_ID    "<named width>"
%token<str> ID          "<identifier>"
%token<str> ATOM        "<atom>"
%token<str> LABEL       "<label>"

%token<int_t> WIDTH     "<constant width>"
%token<int_t> LITERAL   "<literal>"

%token MODIFY           "#modify"
%token M_BEGIN          "#begin"
%token M_END            "#end"

%token<str> STRINGID    "<stringid>"

//Options
%type<widthdef> width_name

//Commands
%type<bool_t> command_auto
%type<commandp> command
%type<comm_params> comm_param_list
%type<int_t> width_value code

//Instructions
%type<instruction> instruction
%type<str> atom
%type<instruction_param> param
%type<instruction_params> param_list

//Instruction meaning
%type<translation> instr_meaning instr_command data_chunk trans_param_lst trans_param_lst2
%type<Translation_item> trans_param

//Modifications
%type<mod_list_list> modify_list
%type<mod_list> modify_clauses
%type<modification> modify_clause

%type<str> string

%start s

%{
#define YYPRINT(file, type, value)   yyprint (file, type, value)
void yyprint (FILE *file, int type, YYSTYPE value)
{
  if (type == ID || type == ATOM || type == STRING || type == WIDTH_ID || type == LABEL
  ||  type == 47)
    fprintf (file, "\"%s\"", value.str);
  else if (type == LITERAL || type == WIDTH || type == 41 || type == 42)
    fprintf (file, "%d", value.int_t);
  else if (type == 43)
    fprintf (file, "%s", value.bool_t ? "true" : "false");
  else if (type == 37)
    fprintf (file, "%s(%d)", value.widthdef.name, value.widthdef.ptr != NULL ? *value.widthdef.ptr : -1);
}
%}

%%

s               : lang_decl opt_option_list def_list {__TRACE; put_instructions_and_commands();}
                ;

/* OPTIONS */

//Language name
lang_decl       : LANGUAGE ID ':' {__TRACE; lang.options.name = $2;
                    verbose("*** Parsing language %s ***", $2);
                    }
                ;
//Language options
opt_option_list : option_list {__TRACE; mask_set = MASK_ALL;} //Se marca todo como definido, con los valores por defecto si no se han declarado
                |             {__TRACE; mask_set = MASK_ALL;}
                ;
option_list     : option ';'{__TRACE;}
                | option_list option ';'{__TRACE;}
                //syntax error
                | option             {__TRACE; err_sin("Options must end with \";\".");}
                | option_list option {__TRACE; err_sin("Options must end with \";\".");}
                ;
//Available options
option          : width_name width_value {__TRACE; *($1.ptr) = $2;
                    verbose("> %s is %d bytes long", $1.name, $2);
                    }
                | ENDIAN ID  {__TRACE; char* id = $2;
                    if(!strcmp(id, "big")) lang.options.endian = ENDIAN_BIG;
                    else if(!strcmp(id, "little")) lang.options.endian = ENDIAN_LITTLE;
                    else err_sem("#endian can't be \"%s\" (Only \"little\" or \"big\").", id);
                    verbose("> %s endian", lang.options.endian == ENDIAN_BIG ? "big" : "little");
                    }
                | FILESIZE ID {__TRACE; char* id = $2;
                    if(!strcmp(id, "max") || !strcmp(id, "full"))
                        lang.options.maxfilesize = FILESIZE_MAX;
                    else err_sem("#filesize can't be \"%s\" (Only \"max\", \"full\" or a positive integer).", id);
                    verbose("> Compiled file size limited by address space.");
                    }
                | FILESIZE LITERAL {__TRACE;
                    int val = $2;
                    if (val > 0) {
                        if ( (mask_set & MASK_OFFSETWDTH) && (! val_fits_in_bytes(val, lang.options.offsetWidth)) ) 
                            err_sem("The declared file size (%d) is not directionable by the specified offset width (%d bytes).", val, lang.options.offsetWidth);
                        else lang.options.maxfilesize = val;
                        }
                    else err_sem("#filesize can't be less or equal to 0 (Only \"max\", \"full\" or a positive integer).");
                    verbose("> Compiled files can't be larger than %d.", lang.options.maxfilesize);
                    }
                ;
//Things that can be given a width
width_name      : DECLARE ID  {__TRACE;
                    char* id = $2;
                    //Check if it's not code, offset or target
                    if (!strcmp(id, "code"))
                        err_sem("The name \"code\" is already associated to command code width (change it with #code).");
                    else if (!strcmp(id, "target"))
                        err_sem("The name \"target\" is already associated to script id width (change it with #target).");
                    else if (!strcmp(id, "offset"))
                        err_sem("The name \"offset\" is already associated to the width of positions in script (change it with #offset).");
                    
                    unsigned char* a = width_definition_new(id);
                    if (a==NULL)
                        err_sem("Width of \"%s\" already defined.", id);
                    else $$.ptr = a;
                    $$.name = id;
                    }
                | CODE_TYPE   {__TRACE; //Width of command code
                    if (mask_set & MASK_CODEWDTH) err_sem("Code width already defined.");
                    else {
                        mask_set = mask_set | MASK_CODEWDTH;
                        static char* txtcode = "Language code";
                        $$.ptr = &(lang.options.codeWidth);
                        $$.name = txtcode;
                        }
                    }
                | TARGET_TYPE {__TRACE; //Width of target (script id.)
                    if (mask_set & MASK_TARGETWDTH) err_sem("Target width already defined.");
                    else {
                        mask_set = mask_set | MASK_TARGETWDTH;
                        static char* txttarget = "Language target";
                        $$.ptr = &(lang.options.targetWidth);
                        $$.name = txttarget;
                        }
                    }
                | OFFSET_TYPE {__TRACE; //Width of offset
                    if (mask_set & MASK_OFFSETWDTH) err_sem("Offset width already defined.");
                    else {
                        mask_set = mask_set | MASK_OFFSETWDTH;
                        static char* txtoffset = "Language offset";
                        $$.ptr = &(lang.options.offsetWidth);
                        $$.name = txtoffset;
                        }
                    }
                ;
//Values a width can take
width_value     : //Constant
                  WIDTH {__TRACE; $$=$1;}
                | //Width of command codes
                  CODE {__TRACE;
                    if (mask_set & MASK_CODEWDTH) $$ = lang.options.codeWidth;
                    else err_sem("Code width undefined.");
                    }
                | //Width of target (id. of an script)
                  TARGET {__TRACE;
                    if (mask_set & MASK_TARGETWDTH) $$ = lang.options.targetWidth;
                    else err_sem("Target width undefined.");
                    }
                | //Width of offsets
                  OFFSET {__TRACE;
                    if (mask_set & MASK_OFFSETWDTH) $$ = lang.options.offsetWidth;
                    else err_sem("Offset width undefined.");
                    }
                | //Width of an identifier
                  WIDTH_ID {__TRACE;
                    unsigned char width = get_width($1+1);
                    if (width) $$ = width;
                    else err_sem("Width of \"%s\" undefined.", $1);
                    }
                ;

/* Definitions */

//Definition list
def_list        : definition ';'{__TRACE;}
                | def_list definition ';'{__TRACE;}
                //syntax error
                | option            {__TRACE; err_sin("Options can't be declared in the definitions block.");}
                | def_list option   {__TRACE; err_sin("Options can't be declared in the definitions block.");}
                | definition          {__TRACE; err_sin("Definitions must end in \";\".");}
                | def_list definition {__TRACE; err_sin("Definitions must end in \";\".");}
                ;
//Definitions that may appear in the body of the description of the language
definition      : command     {__TRACE;     add_command($1);}
                | instruction {__TRACE; add_instruction($1);}
                ;


/* Command declaration */

//Command declaration format
command         : COMMAND ID code '{' comm_param_list '}' command_auto {__TRACE;
                    $$ = (command*) malloc2(sizeof(command));
                    $$->name = $2; $$->code = $3; $$->params = $5;
                    if ($7) add_instruction( instruction_from_command($$) );
                    if ($$->code == COMMAND_CODEUNKNOWN) create_reference($$->name, &($$->code));
                    }
                | //No parameters
                  COMMAND ID code command_auto { __TRACE;
                    $$ = (command*) malloc2(sizeof(command));
                    $$->name = $2; $$->code = $3; $$->params.params = NULL; $$->params.numParams = 0;
                    if ($4) add_instruction( instruction_from_command($$) );
                    if ($$->code == COMMAND_CODEUNKNOWN) create_reference($$->name, &($$->code));
                    }
                //syntax error
                | COMMAND ID code comm_param_list command_auto {__TRACE;
                    err_sin("Parameter list must go between brackets.");
                    }
                | COMMAND ID code '{' comm_param_list command_auto {__TRACE;
                    err_sin("Closing bracket missing.");
                    }
                | COMMAND ID code comm_param_list '}' command_auto {__TRACE;
                    err_sin("Opening bracket missing.");
                    }
                ;
//Command parameters
comm_param_list : ID width_value {__TRACE;
                    command_param p; p.name = $1; p.width = $2;
                    $$.params = (command_param*) malloc2(sizeof(command_param) * 16);
                    $$.params[0] = p; $$.numParams = 1;
                    }
                | comm_param_list ';' ID width_value {__TRACE;
                    if ($1.numParams >= 15)
                        err_sem("A command must not have more than 15 parameters.");
                    command_param p; p.name = $3; p.width = $4;
                    $$ = $1; $$.params[($$.numParams)++] = p;
                    }
                //syntax error
                | comm_param_list ID width_value {__TRACE; err_sin("Parameters must be separated by \";\".");}
                | ID {__TRACE; err_sin("Parameter width must be included");}
                | comm_param_list ';' ID {__TRACE; err_sin("Parameter width must be included");}
                ;
//Code of a command
code            : //Chosen by the compiler (implicit)
                 {__TRACE; $$ = COMMAND_CODEUNKNOWN}
                | //Chosen by the compiler (explicit)
                  ':' CODE_FRESH {__TRACE; $$ = COMMAND_CODEUNKNOWN}
                | //Chosen by user
                  ':' LITERAL   { __TRACE;
                    int code = $2;
                    if (code == COMMAND_CODEUNKNOWN) { //Se reserva un código para indicar que lo seleccionará el sistema
                        err_sem("Code %x is reserved by the tool.", COMMAND_CODEUNKNOWN);
                        }
                    else if ( !val_fits_in_bytes(code, lang.options.codeWidth) ) {
                        err_sem("Code %x is too big for the specified code width.", COMMAND_CODEUNKNOWN);
                        }
                    else $$ = code;
                    }
                | //ERROR/WARNING: when putting ':' after a number the lex can return a WIDTG (solution: insert a separation)
                  //This could result in a syntax error, but let's make a more robust extracting the value, which is known
                  //Anyway, a warning will be shown, because future versions of the program may drop this fact
                  WIDTH { __TRACE; warn_sem("Separating the code from the character \":\" is recommended."); $$=$1;}
                ;
//Command becomes an usable instruction per se
command_auto    : INSTRUCTION   {__TRACE; $$=true; }
                |               {__TRACE; $$=false;}
                ;
                

/* Instruction format */

instruction     : INSTRUCTION param_list ':' '{' instr_meaning '}'
                    {__TRACE;
                    mod_list_list emptymodlstlst;
                    emptymodlstlst.size=0; emptymodlstlst.mods = NULL;
                    $$.params = $2; unifyTranslation(&$$, $5, emptymodlstlst); 
                    }
                | INSTRUCTION param_list ':' '{' instr_meaning '}' MODIFY ':' modify_list
                    {__TRACE; $$.params = $2; unifyTranslation(&$$, $5, $9); }
                ;
//List of modifications
modify_list     : ID '{' modify_clauses '}' {__TRACE;
                    $$.mods = (mod_list*) malloc2(sizeof(mod_list) * 4);
                    $$.available = 3;
                    $$.size = 1;
                    $3.paramname = $1;
                    $$.mods[0] = $3;
                    }
                | modify_list ID '{' modify_clauses '}' {__TRACE;
                    $$ = $1;
                    if ($$.available == 0) {
                        $$.mods = (mod_list*) realloc2($$.mods, sizeof(mod_list) * ($$.size + 4));
                        $$.available = 4;
                        }
                    $4.paramname = $2;
                    $$.mods[($$.size)++] = $4;
                    ($$.available)--;
                    }
                ;
//List of modification clauses
modify_clauses  : modify_clause {__TRACE;
                    $$.paramname = NULL;
                    $$.mods = (modification*) malloc2(sizeof(modification) * 2);
                    $$.available = 1;
                    $$.size = 1;
                    $$.mods[0] = $1;
                    }
                | modify_clauses modify_clause {__TRACE;
                    $$ = $1;
                    int i;
                    for (i=0; i<$$.size; i++)
                        if ($$.mods[i].type == $2.type)
                            err_sem("Can't modify twice the same parameter in the same way");
                    if ($$.available == 0) {
                        $$.mods = (modification*) realloc2($$.mods, sizeof(modification) * ($$.size + 2));
                        $$.available = 2;
                        }
                    $$.mods[($$.size)++] = $2;
                    ($$.available)--;
                    }
                ;
modify_clause   : M_BEGIN '{' instr_meaning '}' {__TRACE; $$.type = MOD_BEGIN; $$.data.translation = $3;}
                | M_END   '{' instr_meaning '}' {__TRACE; $$.type = MOD_END;   $$.data.translation = $3;}
                ;
//Format of an instruction
param_list      : param {__TRACE;
                    instruction_param p = $1;
                    $$.params = (instruction_param*) malloc2(sizeof(instruction_param) * 16);
                    $$.params[0] = p; $$.numParams = 1;
                    $$.mask = p.type;
                    }
                | param_list param {__TRACE;
                    instruction_param p = $2;
                    
                    //Check the parameter name is not repeated
                    //Irrelevant in commands, but necessary in instructions
                    int i;
                    for (i=0; i< $$.numParams; i++)
                        if(!strcmp(p.name, $$.params[i].name) && (p.type != TYPE_ATOM || $$.params[i].type != TYPE_ATOM))
                            err_sem("Token \"%s\" is already defined in this instruction.", p.name);
                    
                    $$ = $1; $$.params[($$.numParams)++] = p;
                    $$.mask = ($$.mask << 2) | p.type;
                    }
                ;
//Kinds of parameters
param           : // A constant string (needed to recognize the command)
                  atom {__TRACE; $$.type = TYPE_ATOM; $$.name = $1; $$.modify.numMods = 0; $$.modify.mods = NULL; }
                | // A number of a given width in bytes
                  ID width_value {__TRACE; $$.type = TYPE_VAL; $$.name = $1; $$.extra = $2; $$.modify.numMods = 0; $$.modify.mods = NULL;}
                | // A reference to a position
                  LABEL       {__TRACE; $$.type = TYPE_LABEL; $$.name = $1; $$.extra = 1; $$.modify.numMods = 0; $$.modify.mods = NULL;}
                | // A reference to a position, with the restriction of being defined in the script where it's used
                  LABEL LOCAL {__TRACE; $$.type = TYPE_LABEL; $$.name = $1; $$.extra = 0; $$.modify.numMods = 0; $$.modify.mods = NULL;}
                | // A block of code
                  '{' ID '}'  {__TRACE; $$.type = TYPE_BLOCK; $$.name = $2; $$.modify.numMods = 0; $$.modify.mods = NULL;}
                | //A string parameter
                  STRINGID    {__TRACE; $$.type = TYPE_BLOCK; $$.name = $1; $$.modify.numMods = 0; $$.modify.mods = NULL;}
                //syntax errors
                | '{' ID {__TRACE; err_sin("Block parameter unended.");}
                | ID '}' {__TRACE; err_sin("Block parameter never started (stray \"}\").");}
                ;
//The definition of atom is ambiguous, hence this rule
atom            : ATOM {__TRACE; $$ = $1;}
                | ID {__TRACE;   $$ = $1;}
                ;

//The definition of atom is ambiguous, hence this rule
string          : STRINGID {__TRACE; $$ = $1;}
                | STRING {__TRACE;   $$ = $1;}
                ;

/* Meaning of instructions */
instr_meaning   : instr_command {__TRACE; $$ = $1;}
                | instr_meaning instr_command {__TRACE; 
                    translation a = $1; translation b = $2;
                    int size = a.size + b.size, i;
                    if (size) {
                        $$.items = (Translation_item*) malloc2(sizeof(Translation_item) * size);
                        $$.size = size;
                        for(i=0; i<a.size; i++)
                            $$.items[i] = a.items[i];
                        for(; i<size; i++)
                            $$.items[i] = b.items[i-a.size];
                        free(a.items); free(b.items);
                        }
                    int numInnerLabels = a.numInnerLabels + b.numInnerLabels;
                    if (numInnerLabels) {
                        $$.innerLabels = (char**) malloc2(sizeof(char*) * numInnerLabels);
                        $$.numInnerLabels = numInnerLabels;
                        for(i=0; i<a.numInnerLabels; i++)
                            $$.innerLabels[i] = a.innerLabels[i];
                        for(; i<numInnerLabels; i++)
                            $$.innerLabels[i] = b.innerLabels[i-a.numInnerLabels];
                        free(a.innerLabels); free(b.innerLabels);
                        }
                    }
//Each command, raw, string or modification in the translation.
//Modifications are read separately.
instr_command   :
                  ID trans_param_lst ';' {__TRACE;
                    $$ = $2;
                    //Add the command's code
                    command c; char* id = $1;
                    if ( !get_command_from_id(id, &c) )
                        err_sem("Command \"%s\" undefined.", id);
                    else {
                        $$.items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                        $$.items[0]->type = TRANS_COMMAND;
                        $$.items[0]->value.command.name = id;
                        $$.items[0]->value.command.code = c.code;
                        //If the code wasn't defined explicitly, we have to create a reference and write it later on
                        if (c.code == COMMAND_CODEUNKNOWN)
                            create_reference(id, &($$.items[0]->value.command.code));
                        }
                    //Check number of parameters
                    if ($$.size-1 != c.params.numParams)
                        err_sem("Command %s expects %d parameters, but %d provided.", id, c.params.numParams, $$.size-1);
                    //Unify parameters and command (parameter width)
                    int i;
                    for (i=1; i < $$.size; i++) {
                        command_param p = c.params.params[i-1];
                        if ($$.items[i]->type == TRANS_LIT) {
                            //Check if the value verifies the width restriction
                            if (val_fits_in_bytes((unsigned int)$$.items[i]->value.number.val, p.width))
                                $$.items[i]->value.number.width = p.width;
                            else err_sem("Parameter %s.%s cant be %d : must fit in %d bytes.",
                                            id, p.name, (unsigned int)$$.items[i]->value.number.val, p.width);
                            }
                        }
                    $$.innerLabels = NULL; $$.numInnerLabels = 0;
                    }
                | LABEL ':' {__TRACE;
                    $$.size = 1;
                    $$.items = (Translation_item*) malloc2( sizeof(Translation_item) );
                    $$.items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$.items[0]->type = TRANS_INNERLABEL;
                    $$.items[0]->value.str = $1;
                    $$.innerLabels = (char**) malloc2(sizeof(char*));
                    $$.innerLabels[0] = $1; $$.numInnerLabels = 1;
                    }
                | RAW width_value data_chunk ';' {__TRACE;
                    $$ = $3;
                    //We need to unify: we know the width of the data, so it must be asigned to the values
                    //(which may fit in a smaller width, but must be adapted to the required width)
                    int i;
                    for (i=0; i<$$.size; i++) {
                        int w = $2, v = $$.items[i]->value.number.val;
                        //Check if the value verifies the width restriction
                        if (val_fits_in_bytes((unsigned int)v, w))
                            $$.items[i]->value.number.width = w;
                        else err_sem("Width %d specified, but %d needs a larger width.", v, w);
                        }
                    $$.innerLabels = NULL; $$.numInnerLabels = 0;
                    }
                | RAW_STRING string ';' {__TRACE;
                    $$.items = (Translation_item*) malloc2( sizeof(Translation_item) );
                    $$.size = 1;
                    $$.items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$.items[0]->type = TRANS_STRING;
                    $$.items[0]->value.str = $2;
                    $$.innerLabels = NULL; $$.numInnerLabels = 0;
                    }
                //syntax errors
                | ID trans_param_lst            {__TRACE; err_sin("Commands in the translation must end in \";\".");}
                | LABEL                         {__TRACE; err_sin("Labels in the translation must be followed by \":\".");}
                | LABEL ';'                     {__TRACE; err_sin("Labels in the translation must be followed by \":\".");}
                | RAW width_value data_chunk    {__TRACE; err_sin("Raw data statements in the translation must end in \";\".");}
                | RAW_STRING string             {__TRACE; err_sin("Raw string statements in the translation must end in \";\".");}
                ;
//Chunk of bytes.
data_chunk      : LITERAL {__TRACE;
                    $$.items = (Translation_item*) malloc2(sizeof(Translation_item) );
                    $$.items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$.items[0]->type = TRANS_LIT;
                    $$.items[0]->value.number.val = $1;
                    $$.items[0]->value.number.width = 0; //Mark it 0. Unification will give the actual width.
                    }
                | data_chunk LITERAL {__TRACE;
                    translation a = $1;
                    $$.items = (Translation_item*) realloc2(a.items, sizeof(Translation_item) * (a.size + 1));
                    $$.size = a.size + 1;
                    //Add new element
                    $$.items[a.size] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$.items[a.size]->type = TRANS_LIT;
                    $$.items[a.size]->value.number.val = $2;
                    $$.items[a.size]->value.number.width = 0; //Mark it 0. Unification will give the actual width.
                    }
                ;
//Parameters of the command that has been translated
trans_param_lst : trans_param_lst2 {__TRACE; $$ = $1;}
                | {//No parameters
                    $$.items = (Translation_item*) malloc2(sizeof(Translation_item) * 1);
                    $$.size = 1;
                  }
                ;
trans_param_lst2: trans_param {__TRACE;
                    $$.items = (Translation_item*) malloc2(sizeof(Translation_item) * 17);
                    $$.size = 2;
                    $$.items[1] = $1; // Position 0 is for command code
                    }
                | trans_param_lst2 trans_param {__TRACE;
                    $$ = $1;
                    if ($$.size >= 17) err_sem("Instruction translation to commands is too big.");
                    else $$.items[($$.size)++] = $2;
                    }
                ;
//Each one of the translated parameters
trans_param     : // Number
                  LITERAL {__TRACE;
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_LIT; $$->value.number.val = $1;
                    $$->value.number.width = 0; //Marked as 0 because it's unknown until unification.
                    }
                | // keyword #target
                  TARGET_TYPE {__TRACE;
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_CURTARGET;
                    $$->value.str = NULL;
                    }
                | // Must be an inner label (will be check at unification)
                  LABEL {__TRACE;
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_LABEL;
                    $$->value.str = $1;
                    }
                | // Name of one of the instruction parameters (blocks and values)
                  ID {__TRACE;
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_ID; //Unification will tell blocks from values
                    $$->value.str = $1;
                    }
                | // String
                  STRINGID {__TRACE;
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_BLOCK;
                    $$->value.str = $1;
                    }
                | // Extract offset from label
                  LABEL OFFSET {__TRACE; 
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_LOFFSET; $$->value.str = $1;
                    }
                | // Extract target from label
                  LABEL TARGET {__TRACE; 
                    $$ = (Translation_item) malloc2( sizeof(struct translation_item) );
                    $$->type = TRANS_LTARGET; $$->value.str = $1;
                    }
                ;
%%

//Test if an instruction is duplicated
bool instruction_exists(instruction instr)
{
int i, j;
bool equivalent;
//For each instruction
for (i=0; i<instructions.size; i++)
    //First we check the number of tokens and the mask
    if (instr.params.numParams == instructions.items[i].params.numParams
    &&  instr.params.mask      == instructions.items[i].params.mask) {
        //If same mask, compare token by token
        equivalent = true;
        for (j=0; j<instr.params.numParams; j++)
            if ( param_diff(instr.params.params[j], instructions.items[i].params.params[j], false) ) //Search differences
                {equivalent = false; break;}
        if (equivalent) return true; //No differences between this instruction and instr
        }
return false;
}


//Generates a string from an instruction
char* instruction_toString(char buffer[], instruction i)
{
    int pos = 0;
    pos += sprintf(buffer+pos, "INSTRUCTION");
    int n;
    for (n=0; n<i.params.numParams; n++) {
        pos += sprintf(buffer+pos, "\n\t %s", i.params.params[n].name);
        switch (i.params.params[n].type) {
            case TYPE_ATOM: break;
            case TYPE_VAL: pos += sprintf(buffer+pos, ": value, %d bytes", i.params.params[n].extra); break;
            case TYPE_LABEL: pos += sprintf(buffer+pos, ": %slabel", (i.params.params[n].extra == 0) ? "local " : ""); break;
            case TYPE_BLOCK: pos += sprintf(buffer+pos, ": block"); break;
            default: pos += sprintf(buffer+pos, "???"); break;
            
            }
        }
    return buffer;
}

//Adds an instruction to the language
//Calls err_sem if the instruction already exists
void add_instruction(instruction i)
{
    if ( instruction_exists(i) ) 
        err_sem("Instruction already exists.");
    else {
        if (instructions.available == 0) {
            //Realloc structure
            instructions.items = (instruction*) realloc2(instructions.items, sizeof(instruction) * (instructions.size + 16));
            instructions.available = 16;
            }
        (instructions.available)--;
        instructions.items[(instructions.size)++] = i;
        char txtinstruction[500] = {0};
        verbose("%s\n", instruction_toString(txtinstruction, i));
        }      
}
 

//Retrieve a command from its code, and store it in c if c != NULL
//Returns false if and only if the command doesn't exist
bool get_command_from_code(int code, command* c)
{
    int i;
    for  (i=0; i<commands.size; i++) {
        if (commands.items[i]->code == code) {
            if (c != NULL) *c = *commands.items[i];
            return true;
            }
        }
    return false;
}

//Generates an string from a command
char* command_toString(char buffer[], command* c)
{
    int pos = 0;
    pos += sprintf(buffer+pos, "COMMAND %s", c->name);
    if (c->code != COMMAND_CODEUNKNOWN)
        pos += sprintf(buffer+pos, " (0x%x)", c->code);
    int i;
    for (i=0; i<c->params.numParams; i++)
        pos += sprintf(buffer+pos, "\n\t %s: %d bytes", c->params.params[i].name, c->params.params[i].width);
    return buffer;
}

//Adds a command to the language
//Calls err_sem if a command of the same name already exists
void add_command(command* c)
{
    command c2;
    if ( get_command_from_id(c->name, NULL) ) 
        err_sem("Command \"%s\" already defined.", c->name);
    else if ( c->code != COMMAND_CODEUNKNOWN && get_command_from_code(c->code, &c2) ) 
        err_sem("There's already a command using code \"%d\" (%s).", c->code, c2.name);
    else {
        if (commands.available == 0) {
            //Realojar la estructuras
            commands.items = (command**) realloc2(commands.items, sizeof(command*) * (commands.size + 16));
            commands.available = 16;
            }
        (commands.available)--;
        commands.items[(commands.size)++] = c;
        char txtcommand[500] = {0};
        verbose("%s\n", command_toString(txtcommand, c));
        }
}
//Generate an instruction from a command
instruction instruction_from_command(command* c)
{
    instruction instr;
    int i;
    //Create arrays for parameters and translation
    instr.params.numParams = c->params.numParams + 1;
    instr.params.params = malloc2(sizeof(instruction_param) * (c->params.numParams +1));
    instr.translation.size = c->params.numParams + 1;
    instr.translation.items = malloc2(sizeof(Translation_item) * (c->params.numParams +1));
    
    //Add command code
    instr.params.params[0].type = TYPE_ATOM;
    instr.params.params[0].name = c->name;
    instr.params.params[0].extra = 0;
    instr.params.params[0].modify.numMods = 0;
    instr.params.params[0].modify.mods = NULL;
    //...also in the translation
    instr.translation.items[0] = malloc2(sizeof(struct translation_item));
    instr.translation.items[0]->type = TRANS_COMMAND;
    instr.translation.items[0]->value.command.name = c->name;
    instr.translation.items[0]->value.command.code = c->code;
    //If command code is not known, create a reference
    if (c->code == COMMAND_CODEUNKNOWN) create_reference(c->name, &(instr.translation.items[0]->value.command.code));

    instr.params.mask = TYPE_ATOM;
    for (i=0; i<c->params.numParams; i++) {
        //Create an instruction parameter
        instr.params.params[i+1].type = TYPE_VAL;
        instr.params.params[i+1].name = c->params.params[i].name;
        instr.params.params[i+1].extra = c->params.params[i].width;
        instr.params.params[i+1].modify.numMods = 0;
        instr.params.params[i+1].modify.mods = NULL;
        //Generate translation
        instr.translation.items[i+1] = malloc2(sizeof(struct translation_item));
        instr.translation.items[i+1]->type = TRANS_ID;
        instr.translation.items[i+1]->value.str = c->params.params[i].name;
        //Mask
        instr.params.mask = (instr.params.mask << 2) | TYPE_VAL;
        }
    return instr;
}

//Creates a refference to command
void create_reference(char* id, int* place)
{
    //See if there's a previous reference
    int i;
    for(i=0; i<references.size; i++) {
        code_reference* ref = &references.items[i];
        if ( !strcmp(id, ref->id) ) {
            //Add new entry
            if (ref->available == 0) {
                ref->references = realloc2(ref->references, sizeof(int*) * (ref->number + 4));
                ref->available = 4;
                }
            ref->references[(ref->number)++] = place;
            ref->available--;
            return;
            }
        }
    //Add new entry
    if (references.available == 0) { //If there's not enough room, realloc the array.
        references.items = realloc2(references.items, sizeof(code_reference)*(references.size + 16));
        references.available = 16;
        }
    code_reference* ref = &references.items[(references.size)++];
    (references.available)--;
    
    //Fill fields in the new entry
    ref->id = id;
    ref->references = (int**) malloc2(sizeof(int*) * 4);
    ref->number = 1; ref->available = 3;
    ref->references[0] = place;
}
    
//Creates an entry for the new width
//Returns the pointer to the value that width will take
//Returns NULL if there's already a width using that name
unsigned char* width_definition_new(char* name)
{
    if ( get_width(name) != 0 ) return NULL;
    else {
        if (widths.available == 0) {
            //Realloc structure
            widths.id    = (char**)realloc2(widths.id,    sizeof(char*) * (widths.size + 8));
            widths.width = (char* )realloc2(widths.width, sizeof(char)  * (widths.size + 8));
            widths.available = 8;
            }
        int i = (widths.size)++;
        widths.id[i] = name;
        (widths.available)--;
        return &(widths.width[i]);
        }
}

//Retrieve the value of a defined width
//0 => not defined
unsigned char get_width(char* id)
{
    int i;
    for(i=0; i<widths.size; i++) {
        if ( !strcmp(id, widths.id[i]) )
            return widths.width[i];
        }
    return 0;
}

//Retrieve a command from an id, and store in c if c is not NULL
//Returns false if and only if the command doesn't exist
bool get_command_from_id(char* id, command* c)
{
    int i;
    for  (i=0; i<commands.size; i++) {
        if ( !strcmp(commands.items[i]->name, id) ) {
            if (c != NULL) *c = *commands.items[i];
            return true;
            }
        }
    return false;
}

//Retrieve the type of parameter in an instruction
instruction_param get_param(instruction inst, char* name)
{
    int i;
    for (i=0; i<inst.params.numParams; i++)
        if ( !strcmp(name, inst.params.params[i].name) )
            return inst.params.params[i];
    const instruction_param err = {NULL, TYPE_ERR, -1, {0, NULL}};
    return err;
}

//Same as get_param, but returns a pointer.
instruction_param * get_param_ptr(instruction * inst, char* name)
{
    int i;
    for (i=0; i<inst->params.numParams; i++)
        if ( !strcmp(name, inst->params.params[i].name) )
            return &(inst->params.params[i]);
    return NULL;
}

//Work with a translation related to some instruction
void unifyOneTranslation(instruction inst, translation tr)
{
    command c; command_param cp; instruction_param ip;
    unsigned nparam;
    int i, j; bool found;
    for(i=0; i<tr.size; i++) {
        switch(tr.items[i]->type) {
            case TRANS_COMMAND:
                //Change the command we're dealing with
                if ( !get_command_from_id(tr.items[i]->value.command.name, &c) )
                    err_other("Fatal error: Program logic should have made it so at this point the command does exist");
                nparam = 0;
                break;
            case TRANS_ID:
                ip = get_param(inst, tr.items[i]->value.str);
                //Check if it's there and tell blocks from values.
                switch(ip.type) {
                    case TYPE_VAL:
                        cp = c.params.params[nparam-1];
                        if (ip.extra > cp.width)
                            err_sem("Width of parameter \"%s\" is bigger than that of \"%s.%s\" .",
                                    tr.items[i]->value.str, c.name, cp.name, nparam, c.params.numParams);
                        break;
                    case TYPE_BLOCK:
                        tr.items[i]->type = TRANS_BLOCK;
                        break;
                    case TYPE_ERR:
                        err_sem("Definition of parameter \"%s\" unknown.", tr.items[i]->value.str);
                        break;
                    default:
                        err_sem("Parameter \"%s\" not found.", tr.items[i]->value.str);
                        break;
                    }
                break;
            case TRANS_LOFFSET:
            case TRANS_LTARGET:
                //Check if it's there
                ip = get_param(inst, tr.items[i]->value.str);
                if ( ip.type != TYPE_LABEL)
                        err_sem("Parameter \"%s\" not found as a label.", tr.items[i]->value.str);
                break;
            case TRANS_LABEL:
                //Check if it's not a parameter
                ip = get_param(inst, tr.items[i]->value.str);
                if ( ip.type == TYPE_LABEL)
                        err_sem("Inner label \"%s\" found as a label.", tr.items[i]->value.str);
                //Check it's an inner label instead
                found = false;
                for(j=0; j<tr.numInnerLabels; j++)
                    if(!strcmp(tr.innerLabels[j], tr.items[i]->value.str))
                        found = true;
                if (found == false)
                    err_sem("Inner label \"%s\" not declared in translation scope.", tr.items[i]->value.str);
                break;
            case TRANS_INNERLABEL:
                break;
            default: break;
            }
        nparam++;
        }
}

//Unify instruction and its translations
//Blocks are identified, incongruences found
//Modifications are asociated with the corresponding parameter.
void unifyTranslation(instruction* inst, translation tr, mod_list_list mods)
{
    int i, j, k, l=0, m;
    inst->translation = tr;
    int numInnerLabels = tr.numInnerLabels;
    
    //Kind of a for loop hell but modifications aren't usually numerous so we'll let this be
    for (i=0; i<mods.size; i++)
        for (j=0; j<mods.mods[i].size; j++)
            numInnerLabels += mods.mods[i].mods[j].data.translation.numInnerLabels;
        
    char** innerLabels = (char**) malloc2(sizeof(char*) * numInnerLabels);
    for(i=0; i<numInnerLabels; i++) {
        innerLabels[l] = tr.innerLabels[i]; l++;
        }
    for (i=0; i<mods.size; i++)
        for (j=0; j<mods.mods[i].size; j++)
            for(k=0; k<mods.mods[i].mods[j].data.translation.numInnerLabels; k++) {
                innerLabels[l] = mods.mods[i].mods[j].data.translation.innerLabels[k]; l++;
                }
                
    //Unification rule: inner labels must not share name with a parameter
    //Or with another inner label!
    for (i=0; i<numInnerLabels; i++) {
        for (j=0; j<inst->params.numParams; j++)
            if(!strcmp(innerLabels[i], inst->params.params[j].name))
                err_sem("Translation redeclares label %s from parameter list.", innerLabels[i]);
        for (j=0; j<numInnerLabels; j++)
            if(i!=j && !strcmp(innerLabels[i], innerLabels[j]))
                err_sem("Translation declares label %s twice.", innerLabels[i]);
        }
    
    translation t = tr; t.numInnerLabels = numInnerLabels; t.innerLabels = innerLabels;
    
    //Main translation
    unifyOneTranslation(*inst, t);
    
    //Parameter modifications (#begin and #end)
    instruction_param * ip;
    //For each modified parameter
    for (i=0; i<mods.size; i++) {
        //Check it's valid and unmodified
        ip = get_param_ptr(inst, mods.mods[i].paramname);
        if (ip == NULL) 
            err_sem("Parameter \"%s\" not found, and thus can't be modified.", mods.mods[i].paramname);
        if (ip -> modify.numMods != 0)
            err_sem("Parameter \"%s\" can't be modified twice.", mods.mods[i].paramname);
        ip->modify.numMods = mods.mods[i].size;
        ip->modify.mods = (modification*) malloc2(sizeof(modification)*mods.mods[i].size);
        
        //For each modification
        for (j=0; j<mods.mods[i].size; j++) {
            switch (mods.mods[i].mods[j].type) {
                case MOD_BEGIN:
                case MOD_END:
                    if (ip->type != TYPE_BLOCK)
                        err_sem("Parameter %s can't have #begin or #end modifications.", ip->name);
                    ip->modify.mods[j] = mods.mods[i].mods[j];
                    t = mods.mods[i].mods[j].data.translation;
                    t.numInnerLabels = numInnerLabels; t.innerLabels = innerLabels;
                    unifyOneTranslation(*inst, t);
                    break;
                default: break;
                }
            }
        }
}

//Compare two commands by code
int on_code_number(const void* a, const void* b)
{
    int aux(const command* ca, const command* cb) {
        if (ca->code > cb->code) return 1;
        if (ca->code < cb->code) return -1;
        else return 0;
    }
    
    return aux(*((command**) a), *((command**) b));
}

//Solve the references (unassigned command codes)
void solve_references()
{
    qsort(commands.items, commands.size, sizeof(command*), on_code_number);
    int i,      //Different codes
        j,      //References for the same code
        k = 0,  //command checked
        fresh = 0;
    for (i=0; i<references.size; i++) {
        //Obtain a fresh (unused) code
        while (fresh == commands.items[k]->code) {
            fresh++; k++;
            if ( !val_fits_in_bytes(fresh, lang.options.codeWidth) ) {
                err_sem("Amount of commands exceeds those indexable by specified command code width.");
                }
            }
        //Assing this code in every place requested.
        for (j=0; j<references.items[i].number; j++) {
            *(references.items[i].references[j]) = fresh;
            }
        verbose("Code for command %s resolved to be 0x%x", references.items[i].id, fresh);
        fresh ++;
        }
}


//Save the instructions and commands read in the language
void put_instructions_and_commands(void)
{
    int i;
    
    //Commands
    solve_references();
    qsort(commands.items, commands.size, sizeof(command*), on_code_number);
    
    //Insert commands
    lang.commands.numCommands = commands.size;
    lang.commands.commands = (command*) malloc2(sizeof(command) * commands.size);
    for (i=0; i< commands.size; i++)
        lang.commands.commands[i] = *commands.items[i];

    //Insert instructions
    lang.instructions.numInstructions = instructions.size;
    lang.instructions.instructions = (instruction*) malloc2(sizeof(instruction) * instructions.size);
    for (i=0; i< instructions.size; i++)
        lang.instructions.instructions[i] = instructions.items[i];
    
}


//Write info on the language to a C header file
void writeHlanginfo(FILE* h)
{
    fprintf(h,
"/* Language info */\n\n"        
"#define %s_NAME            \"%s\"\n"   
"#define %s_ID              %s   /*For macro magic*/\n"   
"#define %s_ENDIAN          %d\n"   
"#define %s_FILESIZE        %d\n"   
"#define %s_CODEWIDTH       %d\n"   
"#define %s_TARGETWIDTH     %d\n"   
"#define %s_OFFSETWIDTH     %d\n"   
"#define %s_NUMCOMMANDS     %d\n"
"\n\n"
    ,
    lang.options.name, lang.options.name,
    lang.options.name, lang.options.name,
    lang.options.name, lang.options.endian,
    lang.options.name, lang.options.maxfilesize == FILESIZE_MAX ?
                              directionable_in_bytes(lang.options.offsetWidth)
                            : lang.options.maxfilesize,
    lang.options.name, lang.options.codeWidth,
    lang.options.name, lang.options.targetWidth,
    lang.options.name, lang.options.offsetWidth,
    lang.options.name, lang.commands.numCommands
            );
    verbose("Writing C header language info.");
}

/*Writes the declared widths in a C header file*/
void writeHwidths(FILE* h)
{
    fprintf(h, "/* Data widths */\n\n");
    int i;
    for(i=0; i< widths.size; i++)
        fprintf(h, "#define %s_WIDTH_%s\t%d\n", lang.options.name, widths.id[i], widths.width[i]);
    fprintf(h, "\n\n");
    verbose("Writing C header declared widths.");
}


/* Writes the command codes and parameter length sum in a C header file */
/* Command code width is not added because it's a value to be used when you identify the command */
void writeHcommands(FILE* h)
{
    fprintf(h, "/* Command codes and parameter length*/\n\n");
    int i, j, paramlen, maxNumParams = 0;
    for(i=0; i< lang.commands.numCommands; i++) {
        fprintf(h, "#define %s_CMD_%s\t\t0x%x\n", lang.options.name, lang.commands.commands[i].name, lang.commands.commands[i].code);
        int numParams = lang.commands.commands[i].params.numParams;
        if (numParams > maxNumParams) maxNumParams = numParams;
        for ( ({j=0; paramlen = 0;}) ; j< numParams; j++) {
            command_param p = lang.commands.commands[i].params.params[j];
            int thisParamLen = p.width;
            fprintf(h, "#define %s_WID_%s_%s\t%d\n"
                       "#define %s_WID_%s_%d\t%d\n"
                       "#define %s_POS_%s_%s\t%d\n"
                       "#define %s_POS_%s_%d\t%d\n",
                       lang.options.name, lang.commands.commands[i].name, p.name, p.width,
                       lang.options.name, lang.commands.commands[i].name,      j, p.width,
                       lang.options.name, lang.commands.commands[i].name, p.name, paramlen,
                       lang.options.name, lang.commands.commands[i].name,      j, paramlen
                       );
            paramlen += thisParamLen;
            }
        fprintf(h, "#define %s_LEN_%s\t\t%d\n\n", lang.options.name, lang.commands.commands[i].name, paramlen);
        }
    fprintf(h, "\n#define %s_MAXNUMPARAMS\t%d\n\n", lang.options.name, maxNumParams);
    verbose("Writing C header command info.");
}

void writeCodeStub(FILE* f)
{
//stuffs
fprintf(f,
    "typedef void* %s_script_t;  /*TODO: redefine %s_script_t*/\n"
    "\n"
    "%s_script_t %s_getScript(unsigned target, unsigned offset)\n{\n\t/*TODO*/\n}\n"
    "\n"
    "unsigned char %s_readScriptByteAt(%s_script_t script, unsigned offset)\n{\n\t/*TODO*/\n}\n"
    "\n"
    "unsigned %s_readScriptAt(%s_script_t script, unsigned offset, unsigned char width)\n"
    "{\n"
        "\t%s\n"
        "\tunsigned ret = 0, i;\n"
        "\tfor (i=0; i<width; i++)\n"
        "\t\tret += %s_readScriptByteAt(script, offset+i) << (%s<<3);\n"
        "\treturn ret;\n"
    "}\n"
    "\n"
    "unsigned %s_runScript(unsigned target, unsigned offset)\n"
    "{\n"
    "\t%s_script_t script = %s_getScript(target, offset);\n"
    "\tunsigned char continueRunning = 1;\n"
    "\tunsigned parameter[%s_MAXNUMPARAMS];\n"
    "\twhile(continueRunning) {\n"
        "\t\tunsigned commandCode = %s_readScriptAt(script, offset, %s_CODEWIDTH);\n"
        "\t\toffset += %s_CODEWIDTH;\n"
        "\t\tswitch (commandCode) {\n"
    , lang.options.name, lang.options.name, lang.options.name, lang.options.name
    , lang.options.name, lang.options.name, lang.options.name, lang.options.name
    , lang.options.endian == ENDIAN_LITTLE ? "//little endian" : "//big endian"
    , lang.options.name
    , lang.options.endian == ENDIAN_LITTLE ? "i" : "(width-i-1)"
    , lang.options.name, lang.options.name, lang.options.name, lang.options.name
    , lang.options.name, lang.options.name, lang.options.name
    );

//command cases
int i, j;
for(i=0; i<lang.commands.numCommands; i++) {
    //open case
    fprintf(f, "\t\t\tcase %s_CMD_%s:\n", lang.options.name, lang.commands.commands[i].name);
    //read params
    for (j=0; j<lang.commands.commands[i].params.numParams; j++)
        fprintf(f, 
            "\t\t\t\tparameter[%d] = %s_readScriptAt(script, offset + %s_POS_%s_%d, %s_WID_%s_%d);\n"
            , j , lang.options.name
            , lang.options.name, lang.commands.commands[i].name, j
            , lang.options.name, lang.commands.commands[i].name, j
            );
    //Do stuff, then update offset break
    fprintf(f, "\t\t\t\t/*TODO: do something*/\n");
    if (lang.commands.commands[i].params.numParams > 0)
        fprintf(f, "\t\t\t\toffset += %s_LEN_%s;\n" , lang.options.name, lang.commands.commands[i].name);
    fprintf(f, "\t\t\t\tbreak; //%s\n\n" , lang.commands.commands[i].name);
    }
//close
fprintf(f,
            "\t\t\tdefault: break;\n"
        "\t\t} //end switch (commandCode)\n"
    "\t}\n"
    "\treturn offset;\n"
    "}\n"
    );
    verbose("Writing C command reader stub.");
}

//Writes a C header file if the corresponding options were set
void writeExtrafiles(void)
{
    if (!(langH || widthH || commandH)) return;
    else {
        char filename[strlen(lang.options.name) + 8] ;
        sprintf(filename, "%s_lang.h", lang.options.name);
        FILE* h = fopen(filename, "w");
        fprintf(h,
"/******************************************************\n"        
" Language information for script language %s \n"      
" Generated by Script-to-Bytecode's LanguageCompiler \n"      
"******************************************************/\n\n"
        , lang.options.name
        );
        if(langH) writeHlanginfo(h);
        if(widthH) writeHwidths(h);
        if(commandH) writeHcommands(h);
        }
    if (codeStub) {
        char filename[strlen(lang.options.name) + 8] ;
        sprintf(filename, "%s_runScript.c", lang.options.name);
        FILE* c = fopen(filename, "w");
        fprintf(c,
"/******************************************************\n"        
" Script interpreter code for language %s \n"      
" Generated by Script-to-Bytecode's LanguageCompiler \n"      
"******************************************************/\n"
"#include \"%s_lang.h\"\n\n"
        , lang.options.name, lang.options.name
        );
        writeCodeStub(c);
        }
}

//Gets options from a string of -options
int getOptions(char* argv)
{
    int i, len = strlen(argv);
    for (i=1; i<len; i++)
        switch (argv[i]) {
            case 'v':  verbose_mode = true; break;
            case 'l':  langH = true; break;
            case 'w':  widthH = true; break;
            case 'h':  commandH = true; break;
            case 'c':  codeStub = true; break;
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
    verbose("Warning: Verbose mode doesn't tell about translations and modifications yet.\n");
    
    //Parse
    for (i=1; i<argc; i++) {
        if (argv[i][0] != '-') {
            init();
            yyin = fopen(argv[i], "rb");
            if (yyin == NULL)
                err_other("\"%s\" not found.", argv[i]);
            verbose("Reading %s...", argv[i]);
            if (yyparse() == 0) {
                char filename[strlen(lang.options.name) + 6];
                sprintf(filename, "%s.lang", lang.options.name);
                FILE* fout = fopen(filename, "wb");
                writelanguage(fout);
                writeExtrafiles();
                verbose("...done\n");
                }
            YY_FLUSH_BUFFER;
            }
        }
}
