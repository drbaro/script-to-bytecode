
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 11 "src/LanguageCompiler.y"

#include "common.h"

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



/* Line 189 of yacc.c  */
#line 221 "src/LanguageCompiler.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     CODE = 258,
     TARGET = 259,
     OFFSET = 260,
     CODE_FRESH = 261,
     LOCAL = 262,
     RAW = 263,
     RAW_STRING = 264,
     LANGUAGE = 265,
     INSTRUCTION = 266,
     COMMAND = 267,
     DECLARE = 268,
     ENDIAN = 269,
     FILESIZE = 270,
     CODE_TYPE = 271,
     TARGET_TYPE = 272,
     OFFSET_TYPE = 273,
     STRING = 274,
     WIDTH_ID = 275,
     ID = 276,
     ATOM = 277,
     LABEL = 278,
     WIDTH = 279,
     LITERAL = 280,
     MODIFY = 281,
     M_BEGIN = 282,
     M_END = 283,
     STRINGID = 284
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 158 "src/LanguageCompiler.y"

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



/* Line 214 of yacc.c  */
#line 313 "src/LanguageCompiler.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 245 "src/LanguageCompiler.y"

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


/* Line 264 of yacc.c  */
#line 342 "src/LanguageCompiler.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   146

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  34
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  133

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   284

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    30,    31,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    32,     2,    33,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     7,    11,    13,    14,    17,    21,    23,
      26,    29,    32,    35,    38,    41,    43,    45,    47,    49,
      51,    53,    55,    57,    60,    64,    66,    69,    71,    74,
      76,    78,    86,    91,    97,   104,   111,   114,   119,   123,
     125,   129,   130,   133,   136,   138,   140,   141,   148,   158,
     163,   169,   171,   174,   179,   184,   186,   189,   191,   194,
     196,   199,   203,   205,   208,   211,   213,   215,   217,   219,
     221,   224,   228,   231,   236,   240,   243,   245,   248,   252,
     255,   257,   260,   262,   263,   265,   268,   270,   272,   274,
     276,   278,   281
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      35,     0,    -1,    36,    37,    42,    -1,    10,    21,    30,
      -1,    38,    -1,    -1,    39,    31,    -1,    38,    39,    31,
      -1,    39,    -1,    38,    39,    -1,    40,    41,    -1,    14,
      21,    -1,    15,    21,    -1,    15,    25,    -1,    13,    21,
      -1,    16,    -1,    17,    -1,    18,    -1,    24,    -1,     3,
      -1,     4,    -1,     5,    -1,    20,    -1,    43,    31,    -1,
      42,    43,    31,    -1,    39,    -1,    42,    39,    -1,    43,
      -1,    42,    43,    -1,    44,    -1,    48,    -1,    12,    21,
      46,    32,    45,    33,    47,    -1,    12,    21,    46,    47,
      -1,    12,    21,    46,    45,    47,    -1,    12,    21,    46,
      32,    45,    47,    -1,    12,    21,    46,    45,    33,    47,
      -1,    21,    41,    -1,    45,    31,    21,    41,    -1,    45,
      21,    41,    -1,    21,    -1,    45,    31,    21,    -1,    -1,
      30,     6,    -1,    30,    25,    -1,    24,    -1,    11,    -1,
      -1,    11,    52,    30,    32,    56,    33,    -1,    11,    52,
      30,    32,    56,    33,    26,    30,    49,    -1,    21,    32,
      50,    33,    -1,    49,    21,    32,    50,    33,    -1,    51,
      -1,    50,    51,    -1,    27,    32,    56,    33,    -1,    28,
      32,    56,    33,    -1,    53,    -1,    52,    53,    -1,    54,
      -1,    21,    41,    -1,    23,    -1,    23,     7,    -1,    32,
      21,    33,    -1,    29,    -1,    32,    21,    -1,    21,    33,
      -1,    22,    -1,    21,    -1,    29,    -1,    19,    -1,    57,
      -1,    56,    57,    -1,    21,    59,    31,    -1,    23,    30,
      -1,     8,    41,    58,    31,    -1,     9,    55,    31,    -1,
      21,    59,    -1,    23,    -1,    23,    31,    -1,     8,    41,
      58,    -1,     9,    55,    -1,    25,    -1,    58,    25,    -1,
      60,    -1,    -1,    61,    -1,    60,    61,    -1,    25,    -1,
      17,    -1,    23,    -1,    21,    -1,    29,    -1,    23,     5,
      -1,    23,     4,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   263,   263,   269,   274,   275,   277,   278,   280,   281,
     284,   287,   293,   299,   311,   327,   336,   345,   357,   359,
     364,   369,   374,   384,   385,   387,   388,   389,   390,   393,
     394,   401,   408,   415,   418,   421,   426,   431,   438,   439,
     440,   444,   446,   448,   461,   464,   465,   471,   477,   481,
     488,   500,   507,   521,   522,   525,   531,   547,   549,   551,
     553,   555,   557,   559,   560,   563,   564,   568,   569,   573,
     574,   600,   632,   641,   655,   664,   665,   666,   667,   668,
     671,   678,   690,   691,   696,   701,   709,   715,   721,   727,
     733,   739,   744
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "\":code\"", "\":target\"",
  "\":offset\"", "\"#fresh\"", "\"#local\"", "\"#raw\"", "\"#string\"",
  "\"#language\"", "\"#instruction\"", "\"#command\"", "\"#let\"",
  "\"#endian\"", "\"#filesize\"", "\"#code\"", "\"#target\"",
  "\"#offset\"", "\"<string>\"", "\"<named width>\"", "\"<identifier>\"",
  "\"<atom>\"", "\"<label>\"", "\"<constant width>\"", "\"<literal>\"",
  "\"#modify\"", "\"#begin\"", "\"#end\"", "\"<stringid>\"", "':'", "';'",
  "'{'", "'}'", "$accept", "s", "lang_decl", "opt_option_list",
  "option_list", "option", "width_name", "width_value", "def_list",
  "definition", "command", "comm_param_list", "code", "command_auto",
  "instruction", "modify_list", "modify_clauses", "modify_clause",
  "param_list", "param", "atom", "string", "instr_meaning",
  "instr_command", "data_chunk", "trans_param_lst", "trans_param_lst2",
  "trans_param", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
      58,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    34,    35,    36,    37,    37,    38,    38,    38,    38,
      39,    39,    39,    39,    40,    40,    40,    40,    41,    41,
      41,    41,    41,    42,    42,    42,    42,    42,    42,    43,
      43,    44,    44,    44,    44,    44,    45,    45,    45,    45,
      45,    46,    46,    46,    46,    47,    47,    48,    48,    49,
      49,    50,    50,    51,    51,    52,    52,    53,    53,    53,
      53,    53,    53,    53,    53,    54,    54,    55,    55,    56,
      56,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      58,    58,    59,    59,    60,    60,    61,    61,    61,    61,
      61,    61,    61
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     3,     3,     1,     0,     2,     3,     1,     2,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     1,     2,     1,     2,     1,
       1,     7,     4,     5,     6,     6,     2,     4,     3,     1,
       3,     0,     2,     2,     1,     1,     0,     6,     9,     4,
       5,     1,     2,     4,     4,     1,     2,     1,     2,     1,
       2,     3,     1,     2,     2,     1,     1,     1,     1,     1,
       2,     3,     2,     4,     3,     2,     1,     2,     3,     2,
       1,     2,     1,     0,     1,     2,     1,     1,     1,     1,
       1,     2,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     5,     0,     1,     0,     0,     0,    15,
      16,    17,     0,     4,     8,     0,     3,    14,    11,    12,
      13,     0,     0,    25,     2,    27,    29,    30,     9,     6,
      19,    20,    21,    22,    18,    10,    66,    65,    59,    62,
       0,     0,    55,    57,    41,    26,    28,    23,     7,    64,
      58,    60,    63,     0,    56,    44,     0,    46,    24,    61,
       0,    42,    43,    45,    39,     0,    46,    32,     0,     0,
      83,    76,     0,    69,    36,    46,     0,     0,    46,    33,
       0,    68,    67,    79,    87,    89,    88,    86,    90,    75,
      82,    84,    72,    77,    47,    70,    46,    34,    38,    40,
      35,    80,    78,    74,    92,    91,    71,    85,     0,    31,
      37,    81,    73,     0,     0,    48,     0,     0,     0,     0,
       0,    51,     0,     0,     0,    49,    52,     0,     0,     0,
      50,    53,    54
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,    12,    13,    14,    15,    35,    24,    25,
      26,    66,    57,    67,    27,   115,   120,   121,    41,    42,
      43,    83,    72,    73,   102,    89,    90,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -114
static const yytype_int8 yypact[] =
{
       3,   -13,    55,    97,    30,  -114,    48,    68,    53,  -114,
    -114,  -114,    91,    97,    62,    61,  -114,  -114,  -114,  -114,
    -114,    69,    75,  -114,    91,    89,  -114,  -114,    90,  -114,
    -114,  -114,  -114,  -114,  -114,  -114,     0,  -114,   115,  -114,
     102,    65,  -114,  -114,    46,  -114,    93,  -114,  -114,  -114,
    -114,  -114,    92,    94,  -114,  -114,    -4,    -2,  -114,  -114,
      59,  -114,  -114,  -114,    61,   106,    23,  -114,    61,    -7,
      54,    86,     2,  -114,  -114,    28,    61,   107,   118,  -114,
     105,  -114,  -114,   100,  -114,  -114,    95,  -114,  -114,   101,
      54,  -114,  -114,  -114,   108,  -114,   118,  -114,  -114,    61,
    -114,  -114,    22,  -114,  -114,  -114,  -114,  -114,   103,  -114,
    -114,  -114,  -114,   114,   104,   116,    -1,   109,   110,   111,
      15,  -114,    -1,    59,    59,  -114,  -114,    18,     8,    29,
    -114,  -114,  -114
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -114,  -114,  -114,  -114,  -114,    60,  -114,   -36,  -114,   120,
    -114,    73,  -114,   -60,  -114,  -114,    17,  -113,  -114,    99,
    -114,  -114,    -5,   -71,  -114,  -114,  -114,    56
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      50,    95,    61,    30,    31,    32,    79,   126,     4,    63,
      68,    69,    81,     1,   126,    97,    68,    69,   100,    64,
      33,    62,    82,    70,    34,    71,   118,   119,    74,    70,
      65,    71,    80,    49,    63,    94,   109,    68,    69,    63,
      98,   131,   118,   119,    76,   118,   119,   111,   125,    76,
      70,   130,    71,   112,    77,     5,    78,    95,    95,    77,
      16,    96,   132,   110,    30,    31,    32,    68,    69,    17,
      55,    84,    23,    28,    19,    85,    56,    86,    20,    87,
      70,    33,    71,    88,    45,    34,    36,    37,    38,    18,
      36,    37,    38,    29,    39,    53,    44,    40,    39,   104,
     105,    40,    21,    22,     6,     7,     8,     9,    10,    11,
       6,     7,     8,     9,    10,    11,    92,    93,   128,   129,
      47,    48,    51,    52,    58,    59,    60,    64,    99,    63,
     101,   103,   106,   113,   108,   114,   116,   117,    75,   127,
      54,   122,   123,   124,    46,     0,   107
};

static const yytype_int16 yycheck[] =
{
      36,    72,     6,     3,     4,     5,    66,   120,    21,    11,
       8,     9,    19,    10,   127,    75,     8,     9,    78,    21,
      20,    25,    29,    21,    24,    23,    27,    28,    64,    21,
      32,    23,    68,    33,    11,    33,    96,     8,     9,    11,
      76,    33,    27,    28,    21,    27,    28,    25,    33,    21,
      21,    33,    23,    31,    31,     0,    33,   128,   129,    31,
      30,    33,    33,    99,     3,     4,     5,     8,     9,    21,
      24,    17,    12,    13,    21,    21,    30,    23,    25,    25,
      21,    20,    23,    29,    24,    24,    21,    22,    23,    21,
      21,    22,    23,    31,    29,    30,    21,    32,    29,     4,
       5,    32,    11,    12,    13,    14,    15,    16,    17,    18,
      13,    14,    15,    16,    17,    18,    30,    31,   123,   124,
      31,    31,     7,    21,    31,    33,    32,    21,    21,    11,
      25,    31,    31,    30,    26,    21,    32,    21,    65,   122,
      41,    32,    32,    32,    24,    -1,    90
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    10,    35,    36,    21,     0,    13,    14,    15,    16,
      17,    18,    37,    38,    39,    40,    30,    21,    21,    21,
      25,    11,    12,    39,    42,    43,    44,    48,    39,    31,
       3,     4,     5,    20,    24,    41,    21,    22,    23,    29,
      32,    52,    53,    54,    21,    39,    43,    31,    31,    33,
      41,     7,    21,    30,    53,    24,    30,    46,    31,    33,
      32,     6,    25,    11,    21,    32,    45,    47,     8,     9,
      21,    23,    56,    57,    41,    45,    21,    31,    33,    47,
      41,    19,    29,    55,    17,    21,    23,    25,    29,    59,
      60,    61,    30,    31,    33,    57,    33,    47,    41,    21,
      47,    25,    58,    31,     4,     5,    31,    61,    26,    47,
      41,    25,    31,    30,    21,    49,    32,    21,    27,    28,
      50,    51,    32,    32,    32,    33,    51,    50,    56,    56,
      33,    33,    33
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 263 "src/LanguageCompiler.y"
    {__TRACE; put_instructions_and_commands();;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 269 "src/LanguageCompiler.y"
    {__TRACE; lang.options.name = (yyvsp[(2) - (3)].str);
                    verbose("*** Parsing language %s ***", (yyvsp[(2) - (3)].str));
                    ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 274 "src/LanguageCompiler.y"
    {__TRACE; mask_set = MASK_ALL;;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 275 "src/LanguageCompiler.y"
    {__TRACE; mask_set = MASK_ALL;;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 277 "src/LanguageCompiler.y"
    {__TRACE;;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 278 "src/LanguageCompiler.y"
    {__TRACE;;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 280 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Options must end with \";\".");;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 281 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Options must end with \";\".");;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 284 "src/LanguageCompiler.y"
    {__TRACE; *((yyvsp[(1) - (2)].widthdef).ptr) = (yyvsp[(2) - (2)].int_t);
                    verbose("> %s is %d bytes long", (yyvsp[(1) - (2)].widthdef).name, (yyvsp[(2) - (2)].int_t));
                    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 287 "src/LanguageCompiler.y"
    {__TRACE; char* id = (yyvsp[(2) - (2)].str);
                    if(!strcmp(id, "big")) lang.options.endian = ENDIAN_BIG;
                    else if(!strcmp(id, "little")) lang.options.endian = ENDIAN_LITTLE;
                    else err_sem("#endian can't be \"%s\" (Only \"little\" or \"big\").", id);
                    verbose("> %s endian", lang.options.endian == ENDIAN_BIG ? "big" : "little");
                    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 293 "src/LanguageCompiler.y"
    {__TRACE; char* id = (yyvsp[(2) - (2)].str);
                    if(!strcmp(id, "max") || !strcmp(id, "full"))
                        lang.options.maxfilesize = FILESIZE_MAX;
                    else err_sem("#filesize can't be \"%s\" (Only \"max\", \"full\" or a positive integer).", id);
                    verbose("> Compiled file size limited by address space.");
                    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 299 "src/LanguageCompiler.y"
    {__TRACE;
                    int val = (yyvsp[(2) - (2)].int_t);
                    if (val > 0) {
                        if ( (mask_set & MASK_OFFSETWDTH) && (! val_fits_in_bytes(val, lang.options.offsetWidth)) ) 
                            err_sem("The declared file size (%d) is not directionable by the specified offset width (%d bytes).", val, lang.options.offsetWidth);
                        else lang.options.maxfilesize = val;
                        }
                    else err_sem("#filesize can't be less or equal to 0 (Only \"max\", \"full\" or a positive integer).");
                    verbose("> Compiled files can't be larger than %d.", lang.options.maxfilesize);
                    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 311 "src/LanguageCompiler.y"
    {__TRACE;
                    char* id = (yyvsp[(2) - (2)].str);
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
                    else (yyval.widthdef).ptr = a;
                    (yyval.widthdef).name = id;
                    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 327 "src/LanguageCompiler.y"
    {__TRACE; //Width of command code
                    if (mask_set & MASK_CODEWDTH) err_sem("Code width already defined.");
                    else {
                        mask_set = mask_set | MASK_CODEWDTH;
                        static char* txtcode = "Language code";
                        (yyval.widthdef).ptr = &(lang.options.codeWidth);
                        (yyval.widthdef).name = txtcode;
                        }
                    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 336 "src/LanguageCompiler.y"
    {__TRACE; //Width of target (script id.)
                    if (mask_set & MASK_TARGETWDTH) err_sem("Target width already defined.");
                    else {
                        mask_set = mask_set | MASK_TARGETWDTH;
                        static char* txttarget = "Language target";
                        (yyval.widthdef).ptr = &(lang.options.targetWidth);
                        (yyval.widthdef).name = txttarget;
                        }
                    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 345 "src/LanguageCompiler.y"
    {__TRACE; //Width of offset
                    if (mask_set & MASK_OFFSETWDTH) err_sem("Offset width already defined.");
                    else {
                        mask_set = mask_set | MASK_OFFSETWDTH;
                        static char* txtoffset = "Language offset";
                        (yyval.widthdef).ptr = &(lang.options.offsetWidth);
                        (yyval.widthdef).name = txtoffset;
                        }
                    ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 357 "src/LanguageCompiler.y"
    {__TRACE; (yyval.int_t)=(yyvsp[(1) - (1)].int_t);;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 359 "src/LanguageCompiler.y"
    {__TRACE;
                    if (mask_set & MASK_CODEWDTH) (yyval.int_t) = lang.options.codeWidth;
                    else err_sem("Code width undefined.");
                    ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 364 "src/LanguageCompiler.y"
    {__TRACE;
                    if (mask_set & MASK_TARGETWDTH) (yyval.int_t) = lang.options.targetWidth;
                    else err_sem("Target width undefined.");
                    ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 369 "src/LanguageCompiler.y"
    {__TRACE;
                    if (mask_set & MASK_OFFSETWDTH) (yyval.int_t) = lang.options.offsetWidth;
                    else err_sem("Offset width undefined.");
                    ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 374 "src/LanguageCompiler.y"
    {__TRACE;
                    unsigned char width = get_width((yyvsp[(1) - (1)].str)+1);
                    if (width) (yyval.int_t) = width;
                    else err_sem("Width of \"%s\" undefined.", (yyvsp[(1) - (1)].str));
                    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 384 "src/LanguageCompiler.y"
    {__TRACE;;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 385 "src/LanguageCompiler.y"
    {__TRACE;;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 387 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Options can't be declared in the definitions block.");;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 388 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Options can't be declared in the definitions block.");;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 389 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Definitions must end in \";\".");;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 390 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Definitions must end in \";\".");;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 393 "src/LanguageCompiler.y"
    {__TRACE;     add_command((yyvsp[(1) - (1)].commandp));;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 394 "src/LanguageCompiler.y"
    {__TRACE; add_instruction((yyvsp[(1) - (1)].instruction));;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 401 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.commandp) = (command*) malloc2(sizeof(command));
                    (yyval.commandp)->name = (yyvsp[(2) - (7)].str); (yyval.commandp)->code = (yyvsp[(3) - (7)].int_t); (yyval.commandp)->params = (yyvsp[(5) - (7)].comm_params);
                    if ((yyvsp[(7) - (7)].bool_t)) add_instruction( instruction_from_command((yyval.commandp)) );
                    if ((yyval.commandp)->code == COMMAND_CODEUNKNOWN) create_reference((yyval.commandp)->name, &((yyval.commandp)->code));
                    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 408 "src/LanguageCompiler.y"
    { __TRACE;
                    (yyval.commandp) = (command*) malloc2(sizeof(command));
                    (yyval.commandp)->name = (yyvsp[(2) - (4)].str); (yyval.commandp)->code = (yyvsp[(3) - (4)].int_t); (yyval.commandp)->params.params = NULL; (yyval.commandp)->params.numParams = 0;
                    if ((yyvsp[(4) - (4)].bool_t)) add_instruction( instruction_from_command((yyval.commandp)) );
                    if ((yyval.commandp)->code == COMMAND_CODEUNKNOWN) create_reference((yyval.commandp)->name, &((yyval.commandp)->code));
                    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 415 "src/LanguageCompiler.y"
    {__TRACE;
                    err_sin("Parameter list must go between brackets.");
                    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 418 "src/LanguageCompiler.y"
    {__TRACE;
                    err_sin("Closing bracket missing.");
                    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 421 "src/LanguageCompiler.y"
    {__TRACE;
                    err_sin("Opening bracket missing.");
                    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 426 "src/LanguageCompiler.y"
    {__TRACE;
                    command_param p; p.name = (yyvsp[(1) - (2)].str); p.width = (yyvsp[(2) - (2)].int_t);
                    (yyval.comm_params).params = (command_param*) malloc2(sizeof(command_param) * 16);
                    (yyval.comm_params).params[0] = p; (yyval.comm_params).numParams = 1;
                    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 431 "src/LanguageCompiler.y"
    {__TRACE;
                    if ((yyvsp[(1) - (4)].comm_params).numParams >= 15)
                        err_sem("A command must not have more than 15 parameters.");
                    command_param p; p.name = (yyvsp[(3) - (4)].str); p.width = (yyvsp[(4) - (4)].int_t);
                    (yyval.comm_params) = (yyvsp[(1) - (4)].comm_params); (yyval.comm_params).params[((yyval.comm_params).numParams)++] = p;
                    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 438 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Parameters must be separated by \";\".");;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 439 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Parameter width must be included");;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 440 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Parameter width must be included");;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 444 "src/LanguageCompiler.y"
    {__TRACE; (yyval.int_t) = COMMAND_CODEUNKNOWN;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 446 "src/LanguageCompiler.y"
    {__TRACE; (yyval.int_t) = COMMAND_CODEUNKNOWN;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 448 "src/LanguageCompiler.y"
    { __TRACE;
                    int code = (yyvsp[(2) - (2)].int_t);
                    if (code == COMMAND_CODEUNKNOWN) { //Se reserva un código para indicar que lo seleccionará el sistema
                        err_sem("Code %x is reserved by the tool.", COMMAND_CODEUNKNOWN);
                        }
                    else if ( !val_fits_in_bytes(code, lang.options.codeWidth) ) {
                        err_sem("Code %x is too big for the specified code width.", COMMAND_CODEUNKNOWN);
                        }
                    else (yyval.int_t) = code;
                    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 461 "src/LanguageCompiler.y"
    { __TRACE; warn_sem("Separating the code from the character \":\" is recommended."); (yyval.int_t)=(yyvsp[(1) - (1)].int_t);;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 464 "src/LanguageCompiler.y"
    {__TRACE; (yyval.bool_t)=true; ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 465 "src/LanguageCompiler.y"
    {__TRACE; (yyval.bool_t)=false;;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 472 "src/LanguageCompiler.y"
    {__TRACE;
                    mod_list_list emptymodlstlst;
                    emptymodlstlst.size=0; emptymodlstlst.mods = NULL;
                    (yyval.instruction).params = (yyvsp[(2) - (6)].instruction_params); unifyTranslation(&(yyval.instruction), (yyvsp[(5) - (6)].translation), emptymodlstlst); 
                    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 478 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction).params = (yyvsp[(2) - (9)].instruction_params); unifyTranslation(&(yyval.instruction), (yyvsp[(5) - (9)].translation), (yyvsp[(9) - (9)].mod_list_list)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 481 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.mod_list_list).mods = (mod_list*) malloc2(sizeof(mod_list) * 4);
                    (yyval.mod_list_list).available = 3;
                    (yyval.mod_list_list).size = 1;
                    (yyvsp[(3) - (4)].mod_list).paramname = (yyvsp[(1) - (4)].str);
                    (yyval.mod_list_list).mods[0] = (yyvsp[(3) - (4)].mod_list);
                    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 488 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.mod_list_list) = (yyvsp[(1) - (5)].mod_list_list);
                    if ((yyval.mod_list_list).available == 0) {
                        (yyval.mod_list_list).mods = (mod_list*) realloc2((yyval.mod_list_list).mods, sizeof(mod_list) * ((yyval.mod_list_list).size + 4));
                        (yyval.mod_list_list).available = 4;
                        }
                    (yyvsp[(4) - (5)].mod_list).paramname = (yyvsp[(2) - (5)].str);
                    (yyval.mod_list_list).mods[((yyval.mod_list_list).size)++] = (yyvsp[(4) - (5)].mod_list);
                    ((yyval.mod_list_list).available)--;
                    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 500 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.mod_list).paramname = NULL;
                    (yyval.mod_list).mods = (modification*) malloc2(sizeof(modification) * 2);
                    (yyval.mod_list).available = 1;
                    (yyval.mod_list).size = 1;
                    (yyval.mod_list).mods[0] = (yyvsp[(1) - (1)].modification);
                    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 507 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.mod_list) = (yyvsp[(1) - (2)].mod_list);
                    int i;
                    for (i=0; i<(yyval.mod_list).size; i++)
                        if ((yyval.mod_list).mods[i].type == (yyvsp[(2) - (2)].modification).type)
                            err_sem("Can't modify twice the same parameter in the same way");
                    if ((yyval.mod_list).available == 0) {
                        (yyval.mod_list).mods = (modification*) realloc2((yyval.mod_list).mods, sizeof(modification) * ((yyval.mod_list).size + 2));
                        (yyval.mod_list).available = 2;
                        }
                    (yyval.mod_list).mods[((yyval.mod_list).size)++] = (yyvsp[(2) - (2)].modification);
                    ((yyval.mod_list).available)--;
                    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 521 "src/LanguageCompiler.y"
    {__TRACE; (yyval.modification).type = MOD_BEGIN; (yyval.modification).data.translation = (yyvsp[(3) - (4)].translation);;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 522 "src/LanguageCompiler.y"
    {__TRACE; (yyval.modification).type = MOD_END;   (yyval.modification).data.translation = (yyvsp[(3) - (4)].translation);;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 525 "src/LanguageCompiler.y"
    {__TRACE;
                    instruction_param p = (yyvsp[(1) - (1)].instruction_param);
                    (yyval.instruction_params).params = (instruction_param*) malloc2(sizeof(instruction_param) * 16);
                    (yyval.instruction_params).params[0] = p; (yyval.instruction_params).numParams = 1;
                    (yyval.instruction_params).mask = p.type;
                    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 531 "src/LanguageCompiler.y"
    {__TRACE;
                    instruction_param p = (yyvsp[(2) - (2)].instruction_param);
                    
                    //Check the parameter name is not repeated
                    //Irrelevant in commands, but necessary in instructions
                    int i;
                    for (i=0; i< (yyval.instruction_params).numParams; i++)
                        if(!strcmp(p.name, (yyval.instruction_params).params[i].name) && (p.type != TYPE_ATOM || (yyval.instruction_params).params[i].type != TYPE_ATOM))
                            err_sem("Token \"%s\" is already defined in this instruction.", p.name);
                    
                    (yyval.instruction_params) = (yyvsp[(1) - (2)].instruction_params); (yyval.instruction_params).params[((yyval.instruction_params).numParams)++] = p;
                    (yyval.instruction_params).mask = ((yyval.instruction_params).mask << 2) | p.type;
                    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 547 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_ATOM; (yyval.instruction_param).name = (yyvsp[(1) - (1)].str); (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL; ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 549 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_VAL; (yyval.instruction_param).name = (yyvsp[(1) - (2)].str); (yyval.instruction_param).extra = (yyvsp[(2) - (2)].int_t); (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL;;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 551 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_LABEL; (yyval.instruction_param).name = (yyvsp[(1) - (1)].str); (yyval.instruction_param).extra = 1; (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL;;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 553 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_LABEL; (yyval.instruction_param).name = (yyvsp[(1) - (2)].str); (yyval.instruction_param).extra = 0; (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL;;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 555 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_BLOCK; (yyval.instruction_param).name = (yyvsp[(2) - (3)].str); (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL;;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 557 "src/LanguageCompiler.y"
    {__TRACE; (yyval.instruction_param).type = TYPE_BLOCK; (yyval.instruction_param).name = (yyvsp[(1) - (1)].str); (yyval.instruction_param).modify.numMods = 0; (yyval.instruction_param).modify.mods = NULL;;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 559 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Block parameter unended.");;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 560 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Block parameter never started (stray \"}\").");;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 563 "src/LanguageCompiler.y"
    {__TRACE; (yyval.str) = (yyvsp[(1) - (1)].str);;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 564 "src/LanguageCompiler.y"
    {__TRACE;   (yyval.str) = (yyvsp[(1) - (1)].str);;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 568 "src/LanguageCompiler.y"
    {__TRACE; (yyval.str) = (yyvsp[(1) - (1)].str);;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 569 "src/LanguageCompiler.y"
    {__TRACE;   (yyval.str) = (yyvsp[(1) - (1)].str);;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 573 "src/LanguageCompiler.y"
    {__TRACE; (yyval.translation) = (yyvsp[(1) - (1)].translation);;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 574 "src/LanguageCompiler.y"
    {__TRACE; 
                    translation a = (yyvsp[(1) - (2)].translation); translation b = (yyvsp[(2) - (2)].translation);
                    int size = a.size + b.size, i;
                    if (size) {
                        (yyval.translation).items = (Translation_item*) malloc2(sizeof(Translation_item) * size);
                        (yyval.translation).size = size;
                        for(i=0; i<a.size; i++)
                            (yyval.translation).items[i] = a.items[i];
                        for(; i<size; i++)
                            (yyval.translation).items[i] = b.items[i-a.size];
                        free(a.items); free(b.items);
                        }
                    int numInnerLabels = a.numInnerLabels + b.numInnerLabels;
                    if (numInnerLabels) {
                        (yyval.translation).innerLabels = (char**) malloc2(sizeof(char*) * numInnerLabels);
                        (yyval.translation).numInnerLabels = numInnerLabels;
                        for(i=0; i<a.numInnerLabels; i++)
                            (yyval.translation).innerLabels[i] = a.innerLabels[i];
                        for(; i<numInnerLabels; i++)
                            (yyval.translation).innerLabels[i] = b.innerLabels[i-a.numInnerLabels];
                        free(a.innerLabels); free(b.innerLabels);
                        }
                    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 600 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation) = (yyvsp[(2) - (3)].translation);
                    //Add the command's code
                    command c; char* id = (yyvsp[(1) - (3)].str);
                    if ( !get_command_from_id(id, &c) )
                        err_sem("Command \"%s\" undefined.", id);
                    else {
                        (yyval.translation).items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                        (yyval.translation).items[0]->type = TRANS_COMMAND;
                        (yyval.translation).items[0]->value.command.name = id;
                        (yyval.translation).items[0]->value.command.code = c.code;
                        //If the code wasn't defined explicitly, we have to create a reference and write it later on
                        if (c.code == COMMAND_CODEUNKNOWN)
                            create_reference(id, &((yyval.translation).items[0]->value.command.code));
                        }
                    //Check number of parameters
                    if ((yyval.translation).size-1 != c.params.numParams)
                        err_sem("Command %s expects %d parameters, but %d provided.", id, c.params.numParams, (yyval.translation).size-1);
                    //Unify parameters and command (parameter width)
                    int i;
                    for (i=1; i < (yyval.translation).size; i++) {
                        command_param p = c.params.params[i-1];
                        if ((yyval.translation).items[i]->type == TRANS_LIT) {
                            //Check if the value verifies the width restriction
                            if (val_fits_in_bytes((unsigned int)(yyval.translation).items[i]->value.number.val, p.width))
                                (yyval.translation).items[i]->value.number.width = p.width;
                            else err_sem("Parameter %s.%s cant be %d : must fit in %d bytes.",
                                            id, p.name, (unsigned int)(yyval.translation).items[i]->value.number.val, p.width);
                            }
                        }
                    (yyval.translation).innerLabels = NULL; (yyval.translation).numInnerLabels = 0;
                    ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 632 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation).size = 1;
                    (yyval.translation).items = (Translation_item*) malloc2( sizeof(Translation_item) );
                    (yyval.translation).items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.translation).items[0]->type = TRANS_INNERLABEL;
                    (yyval.translation).items[0]->value.str = (yyvsp[(1) - (2)].str);
                    (yyval.translation).innerLabels = (char**) malloc2(sizeof(char*));
                    (yyval.translation).innerLabels[0] = (yyvsp[(1) - (2)].str); (yyval.translation).numInnerLabels = 1;
                    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 641 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation) = (yyvsp[(3) - (4)].translation);
                    //We need to unify: we know the width of the data, so it must be asigned to the values
                    //(which may fit in a smaller width, but must be adapted to the required width)
                    int i;
                    for (i=0; i<(yyval.translation).size; i++) {
                        int w = (yyvsp[(2) - (4)].int_t), v = (yyval.translation).items[i]->value.number.val;
                        //Check if the value verifies the width restriction
                        if (val_fits_in_bytes((unsigned int)v, w))
                            (yyval.translation).items[i]->value.number.width = w;
                        else err_sem("Width %d specified, but %d needs a larger width.", v, w);
                        }
                    (yyval.translation).innerLabels = NULL; (yyval.translation).numInnerLabels = 0;
                    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 655 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation).items = (Translation_item*) malloc2( sizeof(Translation_item) );
                    (yyval.translation).size = 1;
                    (yyval.translation).items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.translation).items[0]->type = TRANS_STRING;
                    (yyval.translation).items[0]->value.str = (yyvsp[(2) - (3)].str);
                    (yyval.translation).innerLabels = NULL; (yyval.translation).numInnerLabels = 0;
                    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 664 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Commands in the translation must end in \";\".");;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 665 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Labels in the translation must be followed by \":\".");;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 666 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Labels in the translation must be followed by \":\".");;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 667 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Raw data statements in the translation must end in \";\".");;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 668 "src/LanguageCompiler.y"
    {__TRACE; err_sin("Raw string statements in the translation must end in \";\".");;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 671 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation).items = (Translation_item*) malloc2(sizeof(Translation_item) );
                    (yyval.translation).items[0] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.translation).items[0]->type = TRANS_LIT;
                    (yyval.translation).items[0]->value.number.val = (yyvsp[(1) - (1)].int_t);
                    (yyval.translation).items[0]->value.number.width = 0; //Mark it 0. Unification will give the actual width.
                    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 678 "src/LanguageCompiler.y"
    {__TRACE;
                    translation a = (yyvsp[(1) - (2)].translation);
                    (yyval.translation).items = (Translation_item*) realloc2(a.items, sizeof(Translation_item) * (a.size + 1));
                    (yyval.translation).size = a.size + 1;
                    //Add new element
                    (yyval.translation).items[a.size] = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.translation).items[a.size]->type = TRANS_LIT;
                    (yyval.translation).items[a.size]->value.number.val = (yyvsp[(2) - (2)].int_t);
                    (yyval.translation).items[a.size]->value.number.width = 0; //Mark it 0. Unification will give the actual width.
                    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 690 "src/LanguageCompiler.y"
    {__TRACE; (yyval.translation) = (yyvsp[(1) - (1)].translation);;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 691 "src/LanguageCompiler.y"
    {//No parameters
                    (yyval.translation).items = (Translation_item*) malloc2(sizeof(Translation_item) * 1);
                    (yyval.translation).size = 1;
                  ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 696 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation).items = (Translation_item*) malloc2(sizeof(Translation_item) * 17);
                    (yyval.translation).size = 2;
                    (yyval.translation).items[1] = (yyvsp[(1) - (1)].Translation_item); // Position 0 is for command code
                    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 701 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.translation) = (yyvsp[(1) - (2)].translation);
                    if ((yyval.translation).size >= 17) err_sem("Instruction translation to commands is too big.");
                    else (yyval.translation).items[((yyval.translation).size)++] = (yyvsp[(2) - (2)].Translation_item);
                    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 709 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_LIT; (yyval.Translation_item)->value.number.val = (yyvsp[(1) - (1)].int_t);
                    (yyval.Translation_item)->value.number.width = 0; //Marked as 0 because it's unknown until unification.
                    ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 715 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_CURTARGET;
                    (yyval.Translation_item)->value.str = NULL;
                    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 721 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_LABEL;
                    (yyval.Translation_item)->value.str = (yyvsp[(1) - (1)].str);
                    ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 727 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_ID; //Unification will tell blocks from values
                    (yyval.Translation_item)->value.str = (yyvsp[(1) - (1)].str);
                    ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 733 "src/LanguageCompiler.y"
    {__TRACE;
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_BLOCK;
                    (yyval.Translation_item)->value.str = (yyvsp[(1) - (1)].str);
                    ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 739 "src/LanguageCompiler.y"
    {__TRACE; 
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_LOFFSET; (yyval.Translation_item)->value.str = (yyvsp[(1) - (2)].str);
                    ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 744 "src/LanguageCompiler.y"
    {__TRACE; 
                    (yyval.Translation_item) = (Translation_item) malloc2( sizeof(struct translation_item) );
                    (yyval.Translation_item)->type = TRANS_LTARGET; (yyval.Translation_item)->value.str = (yyvsp[(1) - (2)].str);
                    ;}
    break;



/* Line 1455 of yacc.c  */
#line 2605 "src/LanguageCompiler.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 749 "src/LanguageCompiler.y"


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

