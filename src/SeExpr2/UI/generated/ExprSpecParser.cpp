/* A Bison parser, made by GNU Bison 3.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* All symbols defined below should begin with ExprSpec or ExprSpecYY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with ExprSpecYY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define ExprSpecYYBISON 1

/* Bison version.  */
#define ExprSpecYYBISON_VERSION "3.5"

/* Skeleton name.  */
#define ExprSpecYYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define ExprSpecYYPURE 0

/* Push parsers.  */
#define ExprSpecYYPUSH 0

/* Pull parsers.  */
#define ExprSpecYYPULL 1


/* Substitute the variable and function names.  */
#define ExprSpecparse         ExprSpecparse
#define ExprSpeclex           ExprSpeclex
#define ExprSpecerror         ExprSpecerror
#define ExprSpecdebug         ExprSpecdebug
#define ExprSpecnerrs         ExprSpecnerrs
#define ExprSpeclval          ExprSpeclval
#define ExprSpecchar          ExprSpecchar
#define ExprSpeclloc          ExprSpeclloc

/* First part of user prologue.  */
#line 18 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"

#include <algorithm>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <typeinfo>
#ifdef SEEXPR_USE_ANIMLIB
#include <animlib/AnimCurve.h>
#include <animlib/AnimKeyframe.h>
#else
#define UNUSED(x) (void)(x)
#endif
#include <SeExpr2/Platform.h>
#include <SeExpr2/Mutex.h>
#include "ExprSpecType.h"
#include "Editable.h"
#include "ExprDeepWater.h"


/******************
 lexer declarations
 ******************/


#define SPEC_IS_NUMBER(x) \
    (dynamic_cast<ExprSpecScalarNode*>(x) != 0)
#define SPEC_IS_VECTOR(x) \
    (dynamic_cast<ExprSpecVectorNode*>(x) != 0)
#define SPEC_IS_STR(x) \
    (dynamic_cast<ExprSpecStringNode*>(x) != 0)

// declarations of functions and data in ExprParser.y
int ExprSpeclex();
int ExprSpecpos();
extern int ExprSpec_start;
extern char* ExprSpectext;
struct ExprSpec_buffer_state;
ExprSpec_buffer_state* ExprSpec_scan_string(const char *str);
void ExprSpec_delete_buffer(ExprSpec_buffer_state*);

//#####################################
// Keep track of mini parse tree nodes

// temporary to the parse... all pointers deleted at end of parse
static std::vector<ExprSpecNode*> specNodes;
/// Remember the spec node, so we can delete it later
static ExprSpecNode* remember(ExprSpecNode* node)
{specNodes.push_back(node);return node;}


/// list of strings duplicated by lexer to avoid error mem leak
static std::vector<char*> tokens;

char* specRegisterToken(char* rawString)
{
    char* tok=strdup(rawString);
    tokens.push_back(tok);
    return tok;
}

//######################################################################
// Expose parser API inputs/outputs to yacc as statics

// these are pointers to the arguments send into parse API
// made static here so the parser can see them in yacc actions
static std::vector<Editable*>* editables;
static std::vector<std::string>* variables;

static const char* ParseStr;    // string being parsed
static std::string ParseError;  // error (set from ExprSpecerror)
static ExprSpecNode* ParseResult; // must set result here since ExprSpecparse can't return it


//######################################################################
// Helpers used by actions to register data


/// Remember that there is an assignment to this variable (For autocomplete)
static void specRegisterVariable(const char* var)
{
    variables->push_back(var);
}

/// Variable Assignment/String literal should be turned into an editable
/// an editable is the data part of a control (it's model essentially)
static void specRegisterEditable(const char* var,ExprSpecNode* node)
{
    //std::cerr<<"we have editable var "<<var<<std::endl;
    if(!node){
        //std::cerr<<"   null ptr "<<var<<std::endl;
    }else if(ExprSpecScalarNode* n=dynamic_cast<ExprSpecScalarNode*>(node)){
        editables->push_back(new NumberEditable(var,node->startPos,node->endPos,n->v));
    }else if(ExprSpecVectorNode* n=dynamic_cast<ExprSpecVectorNode*>(node)){
        editables->push_back(new VectorEditable(var,node->startPos,node->endPos,n->v));
    }else if(ExprSpecStringNode* n=dynamic_cast<ExprSpecStringNode*>(node)){
        editables->push_back(new StringEditable(node->startPos,node->endPos,n->v));
    }else if(ExprSpecCCurveNode* n=dynamic_cast<ExprSpecCCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                ColorCurveEditable* ccurve=new ColorCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecVectorNode* ynode=dynamic_cast<ExprSpecVectorNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* interpnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
                    if(xnode && ynode && interpnode){
                        ccurve->add(xnode->v,ynode->v,interpnode->v);
                    }else{
                        valid=false;
                    }
                }
                if(valid) editables->push_back(ccurve);
                else delete ccurve;
            }else{
                //std::cerr<<"Curve has wrong # of args"<<args->nodes.size()<<std::endl;
            }
        }
    }else if(ExprSpecCurveNode* n=dynamic_cast<ExprSpecCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if((args->nodes.size())%3==0){
                CurveEditable* ccurve=new CurveEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i+=3){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecScalarNode* ynode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* interpnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
                    if(xnode && ynode && interpnode){
                        ccurve->add(xnode->v,ynode->v,interpnode->v);
                    }else{
                        valid=false;
                    }
                }
                if(valid) editables->push_back(ccurve);
                else{
                    delete ccurve;
                }
            }
        }
    }else if(ExprSpecColorSwatchNode* n=dynamic_cast<ExprSpecColorSwatchNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if(args->nodes.size()>0){
                ColorSwatchEditable* swatch=new ColorSwatchEditable(var,node->startPos,node->endPos);
                bool valid=true;
                for(size_t i=0;i<args->nodes.size();i++){
                    ExprSpecVectorNode* colornode=dynamic_cast<ExprSpecVectorNode*>(args->nodes[i]);
                    if(colornode){
                        swatch->add(colornode->v);
                    }else{
                        valid=false;
                    }
                }
                if(valid) editables->push_back(swatch);
                else delete swatch;
            }
        }
    }else if(ExprSpecAnimCurveNode* n=dynamic_cast<ExprSpecAnimCurveNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            // need 3 items for pre inf and post inf and weighting, plus 9 items per key
            if((args->nodes.size()-4)%9==0){
                AnimCurveEditable* animCurve=new AnimCurveEditable(var,node->startPos,node->endPos);
                bool valid=true;


#ifdef SEEXPR_USE_ANIMLIB
                if(ExprSpecStringNode* s=dynamic_cast<ExprSpecStringNode*>(args->nodes[0])){
                    animCurve->curve.setPreInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(ExprSpecStringNode* s=dynamic_cast<ExprSpecStringNode*>(args->nodes[1])){
                    animCurve->curve.setPostInfinity(animlib::AnimCurve::stringToInfinityType(s->v));
                }else valid=false;
                if(ExprSpecScalarNode* v=dynamic_cast<ExprSpecScalarNode*>(args->nodes[2])){
                    animCurve->curve.setWeighted(bool(v->v));
                }
                if(ExprSpecStringNode* v=dynamic_cast<ExprSpecStringNode*>(args->nodes[3])){
                    animCurve->link=v->v;
                }

                for(size_t i=4;i<args->nodes.size();i+=9){
                    ExprSpecScalarNode* xnode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i]);
                    ExprSpecScalarNode* ynode=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+1]);
                    ExprSpecScalarNode* inWeight=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+2]);
                    ExprSpecScalarNode* outWeight=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+3]);
                    ExprSpecScalarNode* inAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+4]);
                    ExprSpecScalarNode* outAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+5]);
                    ExprSpecStringNode* inTangType=dynamic_cast<ExprSpecStringNode*>(args->nodes[i+6]);
                    ExprSpecStringNode* outTangType=dynamic_cast<ExprSpecStringNode*>(args->nodes[i+7]);
                    ExprSpecScalarNode* weighted=dynamic_cast<ExprSpecScalarNode*>(args->nodes[i+8]);
                    if(xnode && ynode && inWeight && outWeight && inAngle && outAngle && inTangType && outTangType ){
                        animlib::AnimKeyframe key(xnode->v,ynode->v);
                        key.setInWeight(inWeight->v);
                        key.setOutWeight(outWeight->v);
                        key.setInAngle(inAngle->v);
                        key.setOutAngle(outAngle->v);
                        key.setInTangentType(animlib::AnimKeyframe::stringToTangentType(inTangType->v));
                        key.setOutTangentType(animlib::AnimKeyframe::stringToTangentType(outTangType->v));
                        key.setWeightsLocked(weighted->v);
                        animCurve->curve.addKey(key);
                    }else{
                        valid=false;
                    }
                }
                if(valid) editables->push_back(animCurve);
                else delete animCurve;
#else
                UNUSED(animCurve);
                UNUSED(valid);
#endif
            }
        }
    }else if(ExprSpecDeepWaterNode* n=dynamic_cast<ExprSpecDeepWaterNode*>(node)){
        if(ExprSpecListNode* args=dynamic_cast<ExprSpecListNode*>(n->args)){
            if(args->nodes.size()==12){
                DeepWaterEditable* deepWater=new DeepWaterEditable(var,node->startPos,node->endPos);
                bool valid=true;

                ExprSpecScalarNode* resolution=dynamic_cast<ExprSpecScalarNode*>(args->nodes[0]);
                ExprSpecScalarNode* tileSize=dynamic_cast<ExprSpecScalarNode*>(args->nodes[1]);
                ExprSpecScalarNode* lengthCutoff=dynamic_cast<ExprSpecScalarNode*>(args->nodes[2]);
                ExprSpecScalarNode* amplitude=dynamic_cast<ExprSpecScalarNode*>(args->nodes[3]);
                ExprSpecScalarNode* windAngle=dynamic_cast<ExprSpecScalarNode*>(args->nodes[4]);
                ExprSpecScalarNode* windSpeed=dynamic_cast<ExprSpecScalarNode*>(args->nodes[5]);
                ExprSpecScalarNode* directionalFactorExponent=dynamic_cast<ExprSpecScalarNode*>(args->nodes[6]);
                ExprSpecScalarNode* directionalReflectionDamping=dynamic_cast<ExprSpecScalarNode*>(args->nodes[7]);
                ExprSpecVectorNode* flowDirection=dynamic_cast<ExprSpecVectorNode*>(args->nodes[8]);
                ExprSpecScalarNode* sharpen=dynamic_cast<ExprSpecScalarNode*>(args->nodes[9]);
                ExprSpecScalarNode* time=dynamic_cast<ExprSpecScalarNode*>(args->nodes[10]);
                ExprSpecScalarNode* filterWidth=dynamic_cast<ExprSpecScalarNode*>(args->nodes[11]);
                if(resolution && tileSize && lengthCutoff && amplitude && windAngle && windSpeed && directionalFactorExponent && directionalReflectionDamping && flowDirection && sharpen && time && filterWidth){
                    deepWater->setParams(SeDeepWaterParams(resolution->v, tileSize->v, lengthCutoff->v, amplitude->v, windAngle->v, windSpeed->v, directionalFactorExponent->v, directionalReflectionDamping->v, flowDirection->v, sharpen->v, time->v, filterWidth->v));
                }else{
                    valid=false;
                }

                if(valid) editables->push_back(deepWater);
                else delete deepWater;
            }
        }
    }else{
        std::cerr<<"SEEXPREDITOR LOGIC ERROR: We didn't recognize the Spec"<<std::endl;
    }
}


/*******************
 parser declarations
 *******************/

// forward declaration
static void ExprSpecerror(const char* msg);


#line 331 "y.tab.c"

# ifndef ExprSpecYY_CAST
#  ifdef __cplusplus
#   define ExprSpecYY_CAST(Type, Val) static_cast<Type> (Val)
#   define ExprSpecYY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define ExprSpecYY_CAST(Type, Val) ((Type) (Val))
#   define ExprSpecYY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef ExprSpecYY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define ExprSpecYY_NULLPTR nullptr
#   else
#    define ExprSpecYY_NULLPTR 0
#   endif
#  else
#   define ExprSpecYY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef ExprSpecYYERROR_VERBOSE
# undef ExprSpecYYERROR_VERBOSE
# define ExprSpecYYERROR_VERBOSE 1
#else
# define ExprSpecYYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED
# define ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef ExprSpecYYDEBUG
# define ExprSpecYYDEBUG 0
#endif
#if ExprSpecYYDEBUG
extern int ExprSpecdebug;
#endif

/* Token type.  */
#ifndef ExprSpecYYTOKENTYPE
# define ExprSpecYYTOKENTYPE
  enum ExprSpectokentype
  {
    IF = 258,
    ELSE = 259,
    NAME = 260,
    VAR = 261,
    STR = 262,
    NUMBER = 263,
    AddEq = 264,
    SubEq = 265,
    MultEq = 266,
    DivEq = 267,
    ExpEq = 268,
    ModEq = 269,
    ARROW = 270,
    OR = 271,
    AND = 272,
    EQ = 273,
    NE = 274,
    LE = 275,
    GE = 276,
    UNARY = 277
  };
#endif

/* Value type.  */
#if ! defined ExprSpecYYSTYPE && ! defined ExprSpecYYSTYPE_IS_DECLARED
union ExprSpecYYSTYPE
{
#line 271 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"

    ExprSpecNode* n;
    double d;      // return value for number tokens
    char* s;       /* return value for name tokens.  Note: UNLIKE the regular parser, this is not strdup()'dthe string */

#line 412 "y.tab.c"

};
typedef union ExprSpecYYSTYPE ExprSpecYYSTYPE;
# define ExprSpecYYSTYPE_IS_TRIVIAL 1
# define ExprSpecYYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined ExprSpecYYLTYPE && ! defined ExprSpecYYLTYPE_IS_DECLARED
typedef struct ExprSpecYYLTYPE ExprSpecYYLTYPE;
struct ExprSpecYYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define ExprSpecYYLTYPE_IS_DECLARED 1
# define ExprSpecYYLTYPE_IS_TRIVIAL 1
#endif


extern ExprSpecYYSTYPE ExprSpeclval;
extern ExprSpecYYLTYPE ExprSpeclloc;
int ExprSpecparse (void);

#endif /* !ExprSpecYY_EXPRSPEC_Y_TAB_H_INCLUDED  */



#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define ExprSpecYY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ ExprSpectype_int8;
#elif defined ExprSpecYY_STDINT_H
typedef int_least8_t ExprSpectype_int8;
#else
typedef signed char ExprSpectype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ ExprSpectype_int16;
#elif defined ExprSpecYY_STDINT_H
typedef int_least16_t ExprSpectype_int16;
#else
typedef short ExprSpectype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ ExprSpectype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined ExprSpecYY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t ExprSpectype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char ExprSpectype_uint8;
#else
typedef short ExprSpectype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ ExprSpectype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined ExprSpecYY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t ExprSpectype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short ExprSpectype_uint16;
#else
typedef int ExprSpectype_uint16;
#endif

#ifndef ExprSpecYYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define ExprSpecYYPTRDIFF_T __PTRDIFF_TYPE__
#  define ExprSpecYYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define ExprSpecYYPTRDIFF_T ptrdiff_t
#  define ExprSpecYYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define ExprSpecYYPTRDIFF_T long
#  define ExprSpecYYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef ExprSpecYYSIZE_T
# ifdef __SIZE_TYPE__
#  define ExprSpecYYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define ExprSpecYYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define ExprSpecYYSIZE_T size_t
# else
#  define ExprSpecYYSIZE_T unsigned
# endif
#endif

#define ExprSpecYYSIZE_MAXIMUM                                  \
  ExprSpecYY_CAST (ExprSpecYYPTRDIFF_T,                                 \
           (ExprSpecYYPTRDIFF_MAXIMUM < ExprSpecYY_CAST (ExprSpecYYSIZE_T, -1)  \
            ? ExprSpecYYPTRDIFF_MAXIMUM                         \
            : ExprSpecYY_CAST (ExprSpecYYSIZE_T, -1)))

#define ExprSpecYYSIZEOF(X) ExprSpecYY_CAST (ExprSpecYYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef ExprSpectype_uint8 ExprSpec_state_t;

/* State numbers in computations.  */
typedef int ExprSpec_state_fast_t;

#ifndef ExprSpecYY_
# if defined ExprSpecYYENABLE_NLS && ExprSpecYYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define ExprSpecYY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef ExprSpecYY_
#  define ExprSpecYY_(Msgid) Msgid
# endif
#endif

#ifndef ExprSpecYY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define ExprSpecYY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define ExprSpecYY_ATTRIBUTE_PURE
# endif
#endif

#ifndef ExprSpecYY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define ExprSpecYY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define ExprSpecYY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define ExprSpecYYUSE(E) ((void) (E))
#else
# define ExprSpecYYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about ExprSpeclval being uninitialized.  */
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define ExprSpecYY_INITIAL_VALUE(Value) Value
#endif
#ifndef ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef ExprSpecYY_INITIAL_VALUE
# define ExprSpecYY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define ExprSpecYY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define ExprSpecYY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef ExprSpecYY_IGNORE_USELESS_CAST_BEGIN
# define ExprSpecYY_IGNORE_USELESS_CAST_BEGIN
# define ExprSpecYY_IGNORE_USELESS_CAST_END
#endif


#define ExprSpecYY_ASSERT(E) ((void) (0 && (E)))

#if ! defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef ExprSpecYYSTACK_USE_ALLOCA
#  if ExprSpecYYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define ExprSpecYYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define ExprSpecYYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define ExprSpecYYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef ExprSpecYYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define ExprSpecYYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef ExprSpecYYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define ExprSpecYYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define ExprSpecYYSTACK_ALLOC ExprSpecYYMALLOC
#  define ExprSpecYYSTACK_FREE ExprSpecYYFREE
#  ifndef ExprSpecYYSTACK_ALLOC_MAXIMUM
#   define ExprSpecYYSTACK_ALLOC_MAXIMUM ExprSpecYYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined ExprSpecYYMALLOC || defined malloc) \
             && (defined ExprSpecYYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef ExprSpecYYMALLOC
#   define ExprSpecYYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (ExprSpecYYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef ExprSpecYYFREE
#   define ExprSpecYYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE */


#if (! defined ExprSpecoverflow \
     && (! defined __cplusplus \
         || (defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL \
             && defined ExprSpecYYSTYPE_IS_TRIVIAL && ExprSpecYYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union ExprSpecalloc
{
  ExprSpec_state_t ExprSpecss_alloc;
  ExprSpecYYSTYPE ExprSpecvs_alloc;
  ExprSpecYYLTYPE ExprSpecls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define ExprSpecYYSTACK_GAP_MAXIMUM (ExprSpecYYSIZEOF (union ExprSpecalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define ExprSpecYYSTACK_BYTES(N) \
     ((N) * (ExprSpecYYSIZEOF (ExprSpec_state_t) + ExprSpecYYSIZEOF (ExprSpecYYSTYPE) \
             + ExprSpecYYSIZEOF (ExprSpecYYLTYPE)) \
      + 2 * ExprSpecYYSTACK_GAP_MAXIMUM)

# define ExprSpecYYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables ExprSpecYYSIZE and ExprSpecYYSTACKSIZE give the old and new number of
   elements in the stack, and ExprSpecYYPTR gives the new location of the
   stack.  Advance ExprSpecYYPTR to a properly aligned location for the next
   stack.  */
# define ExprSpecYYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        ExprSpecYYPTRDIFF_T ExprSpecnewbytes;                                         \
        ExprSpecYYCOPY (&ExprSpecptr->Stack_alloc, Stack, ExprSpecsize);                    \
        Stack = &ExprSpecptr->Stack_alloc;                                    \
        ExprSpecnewbytes = ExprSpecstacksize * ExprSpecYYSIZEOF (*Stack) + ExprSpecYYSTACK_GAP_MAXIMUM; \
        ExprSpecptr += ExprSpecnewbytes / ExprSpecYYSIZEOF (*ExprSpecptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined ExprSpecYYCOPY_NEEDED && ExprSpecYYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef ExprSpecYYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define ExprSpecYYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, ExprSpecYY_CAST (ExprSpecYYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define ExprSpecYYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          ExprSpecYYPTRDIFF_T ExprSpeci;                      \
          for (ExprSpeci = 0; ExprSpeci < (Count); ExprSpeci++)   \
            (Dst)[ExprSpeci] = (Src)[ExprSpeci];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !ExprSpecYYCOPY_NEEDED */

/* ExprSpecYYFINAL -- State number of the termination state.  */
#define ExprSpecYYFINAL  40
/* ExprSpecYYLAST -- Last index in ExprSpecYYTABLE.  */
#define ExprSpecYYLAST   692

/* ExprSpecYYNTOKENS -- Number of terminals.  */
#define ExprSpecYYNTOKENS  44
/* ExprSpecYYNNTS -- Number of nonterminals.  */
#define ExprSpecYYNNTS  11
/* ExprSpecYYNRULES -- Number of rules.  */
#define ExprSpecYYNRULES  59
/* ExprSpecYYNSTATES -- Number of states.  */
#define ExprSpecYYNSTATES  139

#define ExprSpecYYUNDEFTOK  2
#define ExprSpecYYMAXUTOK   277


/* ExprSpecYYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by ExprSpeclex, with out-of-bounds checking.  */
#define ExprSpecYYTRANSLATE(ExprSpecYYX)                                                \
  (0 <= (ExprSpecYYX) && (ExprSpecYYX) <= ExprSpecYYMAXUTOK ? ExprSpectranslate[ExprSpecYYX] : ExprSpecYYUNDEFTOK)

/* ExprSpecYYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by ExprSpeclex.  */
static const ExprSpectype_int8 ExprSpectranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    34,     2,     2,     2,    32,     2,     2,
      15,    16,    30,    28,    42,    29,     2,    31,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,    39,
      24,    38,    25,    19,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    37,     2,    43,    36,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,    41,    35,     2,     2,     2,
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
      17,    20,    21,    22,    23,    26,    27,    33
};

#if ExprSpecYYDEBUG
  /* ExprSpecYYRLINE[ExprSpecYYN] -- Source line where rule number ExprSpecYYN was defined.  */
static const ExprSpectype_int16 ExprSpecrline[] =
{
       0,   313,   313,   314,   319,   320,   324,   325,   330,   331,
     335,   336,   337,   338,   339,   340,   341,   345,   346,   347,
     348,   349,   350,   354,   359,   360,   361,   366,   367,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   392,   393,   394,   395,   396,   397,   398,   399,   400,
     425,   426,   427,   428,   433,   434,   439,   448,   460,   461
};
#endif

#if ExprSpecYYDEBUG || ExprSpecYYERROR_VERBOSE || 0
/* ExprSpecYYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at ExprSpecYYNTOKENS, nonterminals.  */
static const char *const ExprSpectname[] =
{
  "$end", "error", "$undefined", "IF", "ELSE", "NAME", "VAR", "STR",
  "NUMBER", "AddEq", "SubEq", "MultEq", "DivEq", "ExpEq", "ModEq", "'('",
  "')'", "ARROW", "':'", "'?'", "OR", "AND", "EQ", "NE", "'<'", "'>'",
  "LE", "GE", "'+'", "'-'", "'*'", "'/'", "'%'", "UNARY", "'!'", "'~'",
  "'^'", "'['", "'='", "';'", "'{'", "'}'", "','", "']'", "$accept",
  "expr", "optassigns", "assigns", "assign", "ifthenelse", "optelse", "e",
  "optargs", "args", "arg", ExprSpecYY_NULLPTR
};
#endif

# ifdef ExprSpecYYPRINT
/* ExprSpecYYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const ExprSpectype_int16 ExprSpectoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,    40,    41,   270,    58,    63,
     271,   272,   273,   274,    60,    62,   275,   276,    43,    45,
      42,    47,    37,   277,    33,   126,    94,    91,    61,    59,
     123,   125,    44,    93
};
# endif

#define ExprSpecYYPACT_NINF (-57)

#define ExprSpecpact_value_is_default(Yyn) \
  ((Yyn) == ExprSpecYYPACT_NINF)

#define ExprSpecYYTABLE_NINF (-1)

#define ExprSpectable_value_is_error(Yyn) \
  0

  /* ExprSpecYYPACT[STATE-NUM] -- Index in ExprSpecYYTABLE of the portion describing
     STATE-NUM.  */
static const ExprSpectype_int16 ExprSpecpact[] =
{
      57,    25,    23,   101,   -57,    72,    72,    72,    72,    72,
      72,    15,    57,   -57,   -57,   593,    72,    72,    72,    72,
      72,    72,    72,    61,    72,    72,    72,    72,    72,    72,
      72,    72,    26,   -57,   529,   -33,   -33,   -33,   -33,   184,
     -57,   -57,   593,    18,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
     551,   232,   253,   274,   295,   316,   337,   -57,   593,    27,
      22,   -57,   358,   379,   400,   421,   442,   463,   484,   505,
     -57,    72,    55,   572,   629,   645,   105,   105,   655,   655,
     655,   655,   113,   113,   -33,   -33,   -33,   -33,   134,     2,
     -57,   -57,   -57,   -57,   -57,   -57,   -57,    61,   -57,   -57,
     -57,   -57,   -57,   -57,   -57,   -57,   208,    61,    72,   -57,
       8,   -57,    72,    58,   612,   114,   101,    30,     8,   159,
     -57,    69,   -57,    -1,   -57,     8,   -57,    34,   -57
};

  /* ExprSpecYYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when ExprSpecYYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const ExprSpectype_int8 ExprSpecdefact[] =
{
       0,     0,    52,    51,    53,     0,     0,     0,     0,     0,
       0,     0,     0,     6,     8,     3,     0,     0,     0,     0,
       0,     0,     0,    54,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    51,     0,    39,    40,    41,    42,     0,
       1,     7,     2,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    59,    58,     0,
      55,    56,     0,     0,     0,     0,     0,     0,     0,     0,
      27,     0,     0,     0,    31,    32,    33,    34,    35,    36,
      37,    38,    43,    44,    45,    46,    47,    48,     0,     0,
      17,    18,    19,    20,    21,    22,    49,     0,    16,    10,
      11,    12,    13,    14,    15,     9,     0,    54,     0,    29,
       4,    57,     0,     0,    30,     0,     0,     0,     5,     0,
      50,    24,    28,     0,    23,     4,    26,     0,    25
};

  /* ExprSpecYYPGOTO[NTERM-NUM].  */
static const ExprSpectype_int8 ExprSpecpgoto[] =
{
     -57,   -57,   -56,    82,   -11,   -50,   -57,     0,   -29,   -57,
     -23
};

  /* ExprSpecYYDEFGOTO[NTERM-NUM].  */
static const ExprSpectype_int16 ExprSpecdefgoto[] =
{
      -1,    11,   127,   128,    13,    14,   134,    68,    69,    70,
      71
};

  /* ExprSpecYYTABLE[ExprSpecYYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If ExprSpecYYTABLE_NINF, syntax error.  */
static const ExprSpectype_uint8 ExprSpectable[] =
{
      15,    41,     1,    58,    59,    34,    35,    36,    37,    38,
      39,     1,    42,   125,   126,    40,    60,    61,    62,    63,
      64,    65,    66,    82,    72,    73,    74,    75,    76,    77,
      78,    79,    17,    18,    19,    20,    21,    22,    23,   135,
      16,    23,   120,   106,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
       1,    24,     2,     3,   107,     4,    32,    33,    67,     4,
     117,   131,     5,   133,   130,   138,     5,    32,    33,   137,
       4,   116,    12,   136,   121,     6,     7,     5,   123,     6,
       7,     8,     9,     0,    10,     8,     9,     0,    10,     0,
       6,     7,     0,     0,     0,     0,     8,     9,     0,    10,
      25,    26,    27,    28,    29,    30,     0,    41,   124,     0,
       0,     0,   129,    17,    18,    19,    20,    21,    22,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,    31,
       0,    58,    59,    55,    56,    57,     0,     0,     0,    58,
      59,    43,    24,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
      58,    59,     0,     0,     0,     0,    43,   119,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,     0,     0,     0,
       0,    43,   132,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
      58,    59,     0,     0,     0,    43,    81,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     0,     0,     0,    58,    59,     0,     0,     0,    43,
     122,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      43,   100,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,     0,     0,    58,
      59,    43,   101,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
      58,    59,    43,   102,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59,    43,   103,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
       0,     0,    58,    59,    43,   104,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,     0,     0,    58,    59,    43,   105,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     0,     0,     0,    58,    59,    43,   108,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    43,   109,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,     0,     0,    58,    59,    43,   110,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    58,    59,    43,
     111,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      43,   112,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,     0,     0,    58,
      59,    43,   113,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,     0,     0,     0,
      58,    59,    43,   114,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59,     0,   115,    80,    43,     0,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    99,    43,     0,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,     0,    58,    59,    43,
     118,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      43,     0,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,     0,     0,    58,
      59,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,     0,    58,    59,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,     0,     0,     0,    58,    59,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59,    53,    54,    55,    56,    57,     0,     0,
       0,    58,    59
};

static const ExprSpectype_int16 ExprSpeccheck[] =
{
       0,    12,     3,    36,    37,     5,     6,     7,     8,     9,
      10,     3,    12,     5,     6,     0,    16,    17,    18,    19,
      20,    21,    22,     5,    24,    25,    26,    27,    28,    29,
      30,    31,     9,    10,    11,    12,    13,    14,    15,    40,
      15,    15,    40,    16,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
       3,    38,     5,     6,    42,     8,     5,     6,     7,     8,
      15,    41,    15,     4,    16,    41,    15,     5,     6,   135,
       8,    81,     0,   133,   107,    28,    29,    15,   117,    28,
      29,    34,    35,    -1,    37,    34,    35,    -1,    37,    -1,
      28,    29,    -1,    -1,    -1,    -1,    34,    35,    -1,    37,
       9,    10,    11,    12,    13,    14,    -1,   128,   118,    -1,
      -1,    -1,   122,     9,    10,    11,    12,    13,    14,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    38,
      -1,    36,    37,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    17,    38,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    -1,    17,    43,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,
      -1,    17,    43,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    -1,    -1,    -1,    17,    42,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,    17,
      42,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      17,    39,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    17,    39,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    17,    39,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    17,    39,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    37,    17,    39,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    36,    37,    17,    39,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    -1,    -1,    -1,    36,    37,    17,    39,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    17,    39,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    37,    17,    39,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    17,
      39,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      17,    39,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    17,    39,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    -1,    -1,    -1,
      36,    37,    17,    39,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    -1,    39,    16,    17,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    16,    17,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    36,    37,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      17,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    -1,    -1,    -1,    36,
      37,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    -1,    -1,    -1,    36,    37,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    36,    37,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37,    28,    29,    30,    31,    32,    -1,    -1,
      -1,    36,    37
};

  /* ExprSpecYYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const ExprSpectype_int8 ExprSpecstos[] =
{
       0,     3,     5,     6,     8,    15,    28,    29,    34,    35,
      37,    45,    47,    48,    49,    51,    15,     9,    10,    11,
      12,    13,    14,    15,    38,     9,    10,    11,    12,    13,
      14,    38,     5,     6,    51,    51,    51,    51,    51,    51,
       0,    48,    51,    17,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    36,    37,
      51,    51,    51,    51,    51,    51,    51,     7,    51,    52,
      53,    54,    51,    51,    51,    51,    51,    51,    51,    51,
      16,    42,     5,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    16,
      39,    39,    39,    39,    39,    39,    16,    42,    39,    39,
      39,    39,    39,    39,    39,    39,    51,    15,    18,    43,
      40,    54,    42,    52,    51,     5,     6,    46,    47,    51,
      16,    41,    43,     4,    50,    40,    49,    46,    41
};

  /* ExprSpecYYR1[ExprSpecYYN] -- Symbol number of symbol that rule ExprSpecYYN derives.  */
static const ExprSpectype_int8 ExprSpecr1[] =
{
       0,    44,    45,    45,    46,    46,    47,    47,    48,    48,
      48,    48,    48,    48,    48,    48,    48,    48,    48,    48,
      48,    48,    48,    49,    50,    50,    50,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    51,
      51,    51,    51,    51,    52,    52,    53,    53,    54,    54
};

  /* ExprSpecYYR2[ExprSpecYYN] -- Number of symbols on the right hand side of rule ExprSpecYYN.  */
static const ExprSpectype_int8 ExprSpecr2[] =
{
       0,     2,     2,     1,     0,     1,     1,     2,     1,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     8,     0,     4,     2,     3,     7,     4,
       5,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     4,
       6,     1,     1,     1,     0,     1,     1,     3,     1,     1
};


#define ExprSpecerrok         (ExprSpecerrstatus = 0)
#define ExprSpecclearin       (ExprSpecchar = ExprSpecYYEMPTY)
#define ExprSpecYYEMPTY         (-2)
#define ExprSpecYYEOF           0

#define ExprSpecYYACCEPT        goto ExprSpecacceptlab
#define ExprSpecYYABORT         goto ExprSpecabortlab
#define ExprSpecYYERROR         goto ExprSpecerrorlab


#define ExprSpecYYRECOVERING()  (!!ExprSpecerrstatus)

#define ExprSpecYYBACKUP(Token, Value)                                    \
  do                                                              \
    if (ExprSpecchar == ExprSpecYYEMPTY)                                        \
      {                                                           \
        ExprSpecchar = (Token);                                         \
        ExprSpeclval = (Value);                                         \
        ExprSpecYYPOPSTACK (ExprSpeclen);                                       \
        ExprSpecstate = *ExprSpecssp;                                         \
        goto ExprSpecbackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        ExprSpecerror (ExprSpecYY_("syntax error: cannot back up")); \
        ExprSpecYYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define ExprSpecYYTERROR        1
#define ExprSpecYYERRCODE       256


/* ExprSpecYYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef ExprSpecYYLLOC_DEFAULT
# define ExprSpecYYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = ExprSpecYYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = ExprSpecYYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = ExprSpecYYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = ExprSpecYYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            ExprSpecYYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            ExprSpecYYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define ExprSpecYYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if ExprSpecYYDEBUG

# ifndef ExprSpecYYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define ExprSpecYYFPRINTF fprintf
# endif

# define ExprSpecYYDPRINTF(Args)                        \
do {                                            \
  if (ExprSpecdebug)                                  \
    ExprSpecYYFPRINTF Args;                             \
} while (0)


/* ExprSpecYY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef ExprSpecYY_LOCATION_PRINT
# if defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL

/* Print *ExprSpecYYLOCP on ExprSpecYYO.  Private, do not rely on its existence. */

ExprSpecYY_ATTRIBUTE_UNUSED
static int
ExprSpec_location_print_ (FILE *ExprSpeco, ExprSpecYYLTYPE const * const ExprSpeclocp)
{
  int res = 0;
  int end_col = 0 != ExprSpeclocp->last_column ? ExprSpeclocp->last_column - 1 : 0;
  if (0 <= ExprSpeclocp->first_line)
    {
      res += ExprSpecYYFPRINTF (ExprSpeco, "%d", ExprSpeclocp->first_line);
      if (0 <= ExprSpeclocp->first_column)
        res += ExprSpecYYFPRINTF (ExprSpeco, ".%d", ExprSpeclocp->first_column);
    }
  if (0 <= ExprSpeclocp->last_line)
    {
      if (ExprSpeclocp->first_line < ExprSpeclocp->last_line)
        {
          res += ExprSpecYYFPRINTF (ExprSpeco, "-%d", ExprSpeclocp->last_line);
          if (0 <= end_col)
            res += ExprSpecYYFPRINTF (ExprSpeco, ".%d", end_col);
        }
      else if (0 <= end_col && ExprSpeclocp->first_column < end_col)
        res += ExprSpecYYFPRINTF (ExprSpeco, "-%d", end_col);
    }
  return res;
 }

#  define ExprSpecYY_LOCATION_PRINT(File, Loc)          \
  ExprSpec_location_print_ (File, &(Loc))

# else
#  define ExprSpecYY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define ExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (ExprSpecdebug)                                                            \
    {                                                                     \
      ExprSpecYYFPRINTF (stderr, "%s ", Title);                                   \
      ExprSpec_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      ExprSpecYYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on ExprSpecYYO.  |
`-----------------------------------*/

static void
ExprSpec_symbol_value_print (FILE *ExprSpeco, int ExprSpectype, ExprSpecYYSTYPE const * const ExprSpecvaluep, ExprSpecYYLTYPE const * const ExprSpeclocationp)
{
  FILE *ExprSpecoutput = ExprSpeco;
  ExprSpecYYUSE (ExprSpecoutput);
  ExprSpecYYUSE (ExprSpeclocationp);
  if (!ExprSpecvaluep)
    return;
# ifdef ExprSpecYYPRINT
  if (ExprSpectype < ExprSpecYYNTOKENS)
    ExprSpecYYPRINT (ExprSpeco, ExprSpectoknum[ExprSpectype], *ExprSpecvaluep);
# endif
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  ExprSpecYYUSE (ExprSpectype);
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on ExprSpecYYO.  |
`---------------------------*/

static void
ExprSpec_symbol_print (FILE *ExprSpeco, int ExprSpectype, ExprSpecYYSTYPE const * const ExprSpecvaluep, ExprSpecYYLTYPE const * const ExprSpeclocationp)
{
  ExprSpecYYFPRINTF (ExprSpeco, "%s %s (",
             ExprSpectype < ExprSpecYYNTOKENS ? "token" : "nterm", ExprSpectname[ExprSpectype]);

  ExprSpecYY_LOCATION_PRINT (ExprSpeco, *ExprSpeclocationp);
  ExprSpecYYFPRINTF (ExprSpeco, ": ");
  ExprSpec_symbol_value_print (ExprSpeco, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp);
  ExprSpecYYFPRINTF (ExprSpeco, ")");
}

/*------------------------------------------------------------------.
| ExprSpec_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
ExprSpec_stack_print (ExprSpec_state_t *ExprSpecbottom, ExprSpec_state_t *ExprSpectop)
{
  ExprSpecYYFPRINTF (stderr, "Stack now");
  for (; ExprSpecbottom <= ExprSpectop; ExprSpecbottom++)
    {
      int ExprSpecbot = *ExprSpecbottom;
      ExprSpecYYFPRINTF (stderr, " %d", ExprSpecbot);
    }
  ExprSpecYYFPRINTF (stderr, "\n");
}

# define ExprSpecYY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (ExprSpecdebug)                                                  \
    ExprSpec_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the ExprSpecYYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
ExprSpec_reduce_print (ExprSpec_state_t *ExprSpecssp, ExprSpecYYSTYPE *ExprSpecvsp, ExprSpecYYLTYPE *ExprSpeclsp, int ExprSpecrule)
{
  int ExprSpeclno = ExprSpecrline[ExprSpecrule];
  int ExprSpecnrhs = ExprSpecr2[ExprSpecrule];
  int ExprSpeci;
  ExprSpecYYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             ExprSpecrule - 1, ExprSpeclno);
  /* The symbols being reduced.  */
  for (ExprSpeci = 0; ExprSpeci < ExprSpecnrhs; ExprSpeci++)
    {
      ExprSpecYYFPRINTF (stderr, "   $%d = ", ExprSpeci + 1);
      ExprSpec_symbol_print (stderr,
                       ExprSpecstos[ExprSpecssp[ExprSpeci + 1 - ExprSpecnrhs]],
                       &ExprSpecvsp[(ExprSpeci + 1) - (ExprSpecnrhs)]
                       , &(ExprSpeclsp[(ExprSpeci + 1) - (ExprSpecnrhs)])                       );
      ExprSpecYYFPRINTF (stderr, "\n");
    }
}

# define ExprSpecYY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (ExprSpecdebug)                          \
    ExprSpec_reduce_print (ExprSpecssp, ExprSpecvsp, ExprSpeclsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int ExprSpecdebug;
#else /* !ExprSpecYYDEBUG */
# define ExprSpecYYDPRINTF(Args)
# define ExprSpecYY_SYMBOL_PRINT(Title, Type, Value, Location)
# define ExprSpecYY_STACK_PRINT(Bottom, Top)
# define ExprSpecYY_REDUCE_PRINT(Rule)
#endif /* !ExprSpecYYDEBUG */


/* ExprSpecYYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef ExprSpecYYINITDEPTH
# define ExprSpecYYINITDEPTH 200
#endif

/* ExprSpecYYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   ExprSpecYYSTACK_ALLOC_MAXIMUM < ExprSpecYYSTACK_BYTES (ExprSpecYYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef ExprSpecYYMAXDEPTH
# define ExprSpecYYMAXDEPTH 10000
#endif


#if ExprSpecYYERROR_VERBOSE

# ifndef ExprSpecstrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define ExprSpecstrlen(S) (ExprSpecYY_CAST (ExprSpecYYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of ExprSpecYYSTR.  */
static ExprSpecYYPTRDIFF_T
ExprSpecstrlen (const char *ExprSpecstr)
{
  ExprSpecYYPTRDIFF_T ExprSpeclen;
  for (ExprSpeclen = 0; ExprSpecstr[ExprSpeclen]; ExprSpeclen++)
    continue;
  return ExprSpeclen;
}
#  endif
# endif

# ifndef ExprSpecstpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define ExprSpecstpcpy stpcpy
#  else
/* Copy ExprSpecYYSRC to ExprSpecYYDEST, returning the address of the terminating '\0' in
   ExprSpecYYDEST.  */
static char *
ExprSpecstpcpy (char *ExprSpecdest, const char *ExprSpecsrc)
{
  char *ExprSpecd = ExprSpecdest;
  const char *ExprSpecs = ExprSpecsrc;

  while ((*ExprSpecd++ = *ExprSpecs++) != '\0')
    continue;

  return ExprSpecd - 1;
}
#  endif
# endif

# ifndef ExprSpectnamerr
/* Copy to ExprSpecYYRES the contents of ExprSpecYYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for ExprSpecerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  ExprSpecYYSTR is taken from ExprSpectname.  If ExprSpecYYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static ExprSpecYYPTRDIFF_T
ExprSpectnamerr (char *ExprSpecres, const char *ExprSpecstr)
{
  if (*ExprSpecstr == '"')
    {
      ExprSpecYYPTRDIFF_T ExprSpecn = 0;
      char const *ExprSpecp = ExprSpecstr;

      for (;;)
        switch (*++ExprSpecp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++ExprSpecp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (ExprSpecres)
              ExprSpecres[ExprSpecn] = *ExprSpecp;
            ExprSpecn++;
            break;

          case '"':
            if (ExprSpecres)
              ExprSpecres[ExprSpecn] = '\0';
            return ExprSpecn;
          }
    do_not_strip_quotes: ;
    }

  if (ExprSpecres)
    return ExprSpecstpcpy (ExprSpecres, ExprSpecstr) - ExprSpecres;
  else
    return ExprSpecstrlen (ExprSpecstr);
}
# endif

/* Copy into *ExprSpecYYMSG, which is of size *ExprSpecYYMSG_ALLOC, an error message
   about the unexpected token ExprSpecYYTOKEN for the state stack whose top is
   ExprSpecYYSSP.

   Return 0 if *ExprSpecYYMSG was successfully written.  Return 1 if *ExprSpecYYMSG is
   not large enough to hold the message.  In that case, also set
   *ExprSpecYYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
ExprSpecsyntax_error (ExprSpecYYPTRDIFF_T *ExprSpecmsg_alloc, char **ExprSpecmsg,
                ExprSpec_state_t *ExprSpecssp, int ExprSpectoken)
{
  enum { ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *ExprSpecformat = ExprSpecYY_NULLPTR;
  /* Arguments of ExprSpecformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *ExprSpecarg[ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of ExprSpecYYARG. */
  int ExprSpeccount = 0;
  /* Cumulated lengths of ExprSpecYYARG.  */
  ExprSpecYYPTRDIFF_T ExprSpecsize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in ExprSpecchar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated ExprSpecchar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (ExprSpectoken != ExprSpecYYEMPTY)
    {
      int ExprSpecn = ExprSpecpact[*ExprSpecssp];
      ExprSpecYYPTRDIFF_T ExprSpecsize0 = ExprSpectnamerr (ExprSpecYY_NULLPTR, ExprSpectname[ExprSpectoken]);
      ExprSpecsize = ExprSpecsize0;
      ExprSpecarg[ExprSpeccount++] = ExprSpectname[ExprSpectoken];
      if (!ExprSpecpact_value_is_default (ExprSpecn))
        {
          /* Start ExprSpecYYX at -ExprSpecYYN if negative to avoid negative indexes in
             ExprSpecYYCHECK.  In other words, skip the first -ExprSpecYYN actions for
             this state because they are default actions.  */
          int ExprSpecxbegin = ExprSpecn < 0 ? -ExprSpecn : 0;
          /* Stay within bounds of both ExprSpeccheck and ExprSpectname.  */
          int ExprSpecchecklim = ExprSpecYYLAST - ExprSpecn + 1;
          int ExprSpecxend = ExprSpecchecklim < ExprSpecYYNTOKENS ? ExprSpecchecklim : ExprSpecYYNTOKENS;
          int ExprSpecx;

          for (ExprSpecx = ExprSpecxbegin; ExprSpecx < ExprSpecxend; ++ExprSpecx)
            if (ExprSpeccheck[ExprSpecx + ExprSpecn] == ExprSpecx && ExprSpecx != ExprSpecYYTERROR
                && !ExprSpectable_value_is_error (ExprSpectable[ExprSpecx + ExprSpecn]))
              {
                if (ExprSpeccount == ExprSpecYYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    ExprSpeccount = 1;
                    ExprSpecsize = ExprSpecsize0;
                    break;
                  }
                ExprSpecarg[ExprSpeccount++] = ExprSpectname[ExprSpecx];
                {
                  ExprSpecYYPTRDIFF_T ExprSpecsize1
                    = ExprSpecsize + ExprSpectnamerr (ExprSpecYY_NULLPTR, ExprSpectname[ExprSpecx]);
                  if (ExprSpecsize <= ExprSpecsize1 && ExprSpecsize1 <= ExprSpecYYSTACK_ALLOC_MAXIMUM)
                    ExprSpecsize = ExprSpecsize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (ExprSpeccount)
    {
# define ExprSpecYYCASE_(N, S)                      \
      case N:                               \
        ExprSpecformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      ExprSpecYYCASE_(0, ExprSpecYY_("syntax error"));
      ExprSpecYYCASE_(1, ExprSpecYY_("syntax error, unexpected %s"));
      ExprSpecYYCASE_(2, ExprSpecYY_("syntax error, unexpected %s, expecting %s"));
      ExprSpecYYCASE_(3, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s"));
      ExprSpecYYCASE_(4, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      ExprSpecYYCASE_(5, ExprSpecYY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef ExprSpecYYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    ExprSpecYYPTRDIFF_T ExprSpecsize1 = ExprSpecsize + (ExprSpecstrlen (ExprSpecformat) - 2 * ExprSpeccount) + 1;
    if (ExprSpecsize <= ExprSpecsize1 && ExprSpecsize1 <= ExprSpecYYSTACK_ALLOC_MAXIMUM)
      ExprSpecsize = ExprSpecsize1;
    else
      return 2;
  }

  if (*ExprSpecmsg_alloc < ExprSpecsize)
    {
      *ExprSpecmsg_alloc = 2 * ExprSpecsize;
      if (! (ExprSpecsize <= *ExprSpecmsg_alloc
             && *ExprSpecmsg_alloc <= ExprSpecYYSTACK_ALLOC_MAXIMUM))
        *ExprSpecmsg_alloc = ExprSpecYYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *ExprSpecp = *ExprSpecmsg;
    int ExprSpeci = 0;
    while ((*ExprSpecp = *ExprSpecformat) != '\0')
      if (*ExprSpecp == '%' && ExprSpecformat[1] == 's' && ExprSpeci < ExprSpeccount)
        {
          ExprSpecp += ExprSpectnamerr (ExprSpecp, ExprSpecarg[ExprSpeci++]);
          ExprSpecformat += 2;
        }
      else
        {
          ++ExprSpecp;
          ++ExprSpecformat;
        }
  }
  return 0;
}
#endif /* ExprSpecYYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
ExprSpecdestruct (const char *ExprSpecmsg, int ExprSpectype, ExprSpecYYSTYPE *ExprSpecvaluep, ExprSpecYYLTYPE *ExprSpeclocationp)
{
  ExprSpecYYUSE (ExprSpecvaluep);
  ExprSpecYYUSE (ExprSpeclocationp);
  if (!ExprSpecmsg)
    ExprSpecmsg = "Deleting";
  ExprSpecYY_SYMBOL_PRINT (ExprSpecmsg, ExprSpectype, ExprSpecvaluep, ExprSpeclocationp);

  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  ExprSpecYYUSE (ExprSpectype);
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int ExprSpecchar;

/* The semantic value of the lookahead symbol.  */
ExprSpecYYSTYPE ExprSpeclval;
/* Location data for the lookahead symbol.  */
ExprSpecYYLTYPE ExprSpeclloc
# if defined ExprSpecYYLTYPE_IS_TRIVIAL && ExprSpecYYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int ExprSpecnerrs;


/*----------.
| ExprSpecparse.  |
`----------*/

int
ExprSpecparse (void)
{
    ExprSpec_state_fast_t ExprSpecstate;
    /* Number of tokens to shift before error messages enabled.  */
    int ExprSpecerrstatus;

    /* The stacks and their tools:
       'ExprSpecss': related to states.
       'ExprSpecvs': related to semantic values.
       'ExprSpecls': related to locations.

       Refer to the stacks through separate pointers, to allow ExprSpecoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    ExprSpec_state_t ExprSpecssa[ExprSpecYYINITDEPTH];
    ExprSpec_state_t *ExprSpecss;
    ExprSpec_state_t *ExprSpecssp;

    /* The semantic value stack.  */
    ExprSpecYYSTYPE ExprSpecvsa[ExprSpecYYINITDEPTH];
    ExprSpecYYSTYPE *ExprSpecvs;
    ExprSpecYYSTYPE *ExprSpecvsp;

    /* The location stack.  */
    ExprSpecYYLTYPE ExprSpeclsa[ExprSpecYYINITDEPTH];
    ExprSpecYYLTYPE *ExprSpecls;
    ExprSpecYYLTYPE *ExprSpeclsp;

    /* The locations where the error started and ended.  */
    ExprSpecYYLTYPE ExprSpecerror_range[3];

    ExprSpecYYPTRDIFF_T ExprSpecstacksize;

  int ExprSpecn;
  int ExprSpecresult;
  /* Lookahead token as an internal (translated) token number.  */
  int ExprSpectoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  ExprSpecYYSTYPE ExprSpecval;
  ExprSpecYYLTYPE ExprSpecloc;

#if ExprSpecYYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char ExprSpecmsgbuf[128];
  char *ExprSpecmsg = ExprSpecmsgbuf;
  ExprSpecYYPTRDIFF_T ExprSpecmsg_alloc = sizeof ExprSpecmsgbuf;
#endif

#define ExprSpecYYPOPSTACK(N)   (ExprSpecvsp -= (N), ExprSpecssp -= (N), ExprSpeclsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int ExprSpeclen = 0;

  ExprSpecssp = ExprSpecss = ExprSpecssa;
  ExprSpecvsp = ExprSpecvs = ExprSpecvsa;
  ExprSpeclsp = ExprSpecls = ExprSpeclsa;
  ExprSpecstacksize = ExprSpecYYINITDEPTH;

  ExprSpecYYDPRINTF ((stderr, "Starting parse\n"));

  ExprSpecstate = 0;
  ExprSpecerrstatus = 0;
  ExprSpecnerrs = 0;
  ExprSpecchar = ExprSpecYYEMPTY; /* Cause a token to be read.  */
  ExprSpeclsp[0] = ExprSpeclloc;
  goto ExprSpecsetstate;


/*------------------------------------------------------------.
| ExprSpecnewstate -- push a new state, which is found in ExprSpecstate.  |
`------------------------------------------------------------*/
ExprSpecnewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  ExprSpecssp++;


/*--------------------------------------------------------------------.
| ExprSpecsetstate -- set current state (the top of the stack) to ExprSpecstate.  |
`--------------------------------------------------------------------*/
ExprSpecsetstate:
  ExprSpecYYDPRINTF ((stderr, "Entering state %d\n", ExprSpecstate));
  ExprSpecYY_ASSERT (0 <= ExprSpecstate && ExprSpecstate < ExprSpecYYNSTATES);
  ExprSpecYY_IGNORE_USELESS_CAST_BEGIN
  *ExprSpecssp = ExprSpecYY_CAST (ExprSpec_state_t, ExprSpecstate);
  ExprSpecYY_IGNORE_USELESS_CAST_END

  if (ExprSpecss + ExprSpecstacksize - 1 <= ExprSpecssp)
#if !defined ExprSpecoverflow && !defined ExprSpecYYSTACK_RELOCATE
    goto ExprSpecexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      ExprSpecYYPTRDIFF_T ExprSpecsize = ExprSpecssp - ExprSpecss + 1;

# if defined ExprSpecoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        ExprSpec_state_t *ExprSpecss1 = ExprSpecss;
        ExprSpecYYSTYPE *ExprSpecvs1 = ExprSpecvs;
        ExprSpecYYLTYPE *ExprSpecls1 = ExprSpecls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if ExprSpecoverflow is a macro.  */
        ExprSpecoverflow (ExprSpecYY_("memory exhausted"),
                    &ExprSpecss1, ExprSpecsize * ExprSpecYYSIZEOF (*ExprSpecssp),
                    &ExprSpecvs1, ExprSpecsize * ExprSpecYYSIZEOF (*ExprSpecvsp),
                    &ExprSpecls1, ExprSpecsize * ExprSpecYYSIZEOF (*ExprSpeclsp),
                    &ExprSpecstacksize);
        ExprSpecss = ExprSpecss1;
        ExprSpecvs = ExprSpecvs1;
        ExprSpecls = ExprSpecls1;
      }
# else /* defined ExprSpecYYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (ExprSpecYYMAXDEPTH <= ExprSpecstacksize)
        goto ExprSpecexhaustedlab;
      ExprSpecstacksize *= 2;
      if (ExprSpecYYMAXDEPTH < ExprSpecstacksize)
        ExprSpecstacksize = ExprSpecYYMAXDEPTH;

      {
        ExprSpec_state_t *ExprSpecss1 = ExprSpecss;
        union ExprSpecalloc *ExprSpecptr =
          ExprSpecYY_CAST (union ExprSpecalloc *,
                   ExprSpecYYSTACK_ALLOC (ExprSpecYY_CAST (ExprSpecYYSIZE_T, ExprSpecYYSTACK_BYTES (ExprSpecstacksize))));
        if (! ExprSpecptr)
          goto ExprSpecexhaustedlab;
        ExprSpecYYSTACK_RELOCATE (ExprSpecss_alloc, ExprSpecss);
        ExprSpecYYSTACK_RELOCATE (ExprSpecvs_alloc, ExprSpecvs);
        ExprSpecYYSTACK_RELOCATE (ExprSpecls_alloc, ExprSpecls);
# undef ExprSpecYYSTACK_RELOCATE
        if (ExprSpecss1 != ExprSpecssa)
          ExprSpecYYSTACK_FREE (ExprSpecss1);
      }
# endif

      ExprSpecssp = ExprSpecss + ExprSpecsize - 1;
      ExprSpecvsp = ExprSpecvs + ExprSpecsize - 1;
      ExprSpeclsp = ExprSpecls + ExprSpecsize - 1;

      ExprSpecYY_IGNORE_USELESS_CAST_BEGIN
      ExprSpecYYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  ExprSpecYY_CAST (long, ExprSpecstacksize)));
      ExprSpecYY_IGNORE_USELESS_CAST_END

      if (ExprSpecss + ExprSpecstacksize - 1 <= ExprSpecssp)
        ExprSpecYYABORT;
    }
#endif /* !defined ExprSpecoverflow && !defined ExprSpecYYSTACK_RELOCATE */

  if (ExprSpecstate == ExprSpecYYFINAL)
    ExprSpecYYACCEPT;

  goto ExprSpecbackup;


/*-----------.
| ExprSpecbackup.  |
`-----------*/
ExprSpecbackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  ExprSpecn = ExprSpecpact[ExprSpecstate];
  if (ExprSpecpact_value_is_default (ExprSpecn))
    goto ExprSpecdefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* ExprSpecYYCHAR is either ExprSpecYYEMPTY or ExprSpecYYEOF or a valid lookahead symbol.  */
  if (ExprSpecchar == ExprSpecYYEMPTY)
    {
      ExprSpecYYDPRINTF ((stderr, "Reading a token: "));
      ExprSpecchar = ExprSpeclex ();
    }

  if (ExprSpecchar <= ExprSpecYYEOF)
    {
      ExprSpecchar = ExprSpectoken = ExprSpecYYEOF;
      ExprSpecYYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      ExprSpectoken = ExprSpecYYTRANSLATE (ExprSpecchar);
      ExprSpecYY_SYMBOL_PRINT ("Next token is", ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
    }

  /* If the proper action on seeing token ExprSpecYYTOKEN is to reduce or to
     detect an error, take that action.  */
  ExprSpecn += ExprSpectoken;
  if (ExprSpecn < 0 || ExprSpecYYLAST < ExprSpecn || ExprSpeccheck[ExprSpecn] != ExprSpectoken)
    goto ExprSpecdefault;
  ExprSpecn = ExprSpectable[ExprSpecn];
  if (ExprSpecn <= 0)
    {
      if (ExprSpectable_value_is_error (ExprSpecn))
        goto ExprSpecerrlab;
      ExprSpecn = -ExprSpecn;
      goto ExprSpecreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (ExprSpecerrstatus)
    ExprSpecerrstatus--;

  /* Shift the lookahead token.  */
  ExprSpecYY_SYMBOL_PRINT ("Shifting", ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
  ExprSpecstate = ExprSpecn;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ExprSpecvsp = ExprSpeclval;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END
  *++ExprSpeclsp = ExprSpeclloc;

  /* Discard the shifted token.  */
  ExprSpecchar = ExprSpecYYEMPTY;
  goto ExprSpecnewstate;


/*-----------------------------------------------------------.
| ExprSpecdefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
ExprSpecdefault:
  ExprSpecn = ExprSpecdefact[ExprSpecstate];
  if (ExprSpecn == 0)
    goto ExprSpecerrlab;
  goto ExprSpecreduce;


/*-----------------------------.
| ExprSpecreduce -- do a reduction.  |
`-----------------------------*/
ExprSpecreduce:
  /* ExprSpecn is the number of a rule to reduce with.  */
  ExprSpeclen = ExprSpecr2[ExprSpecn];

  /* If ExprSpecYYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets ExprSpecYYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to ExprSpecYYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that ExprSpecYYVAL may be used uninitialized.  */
  ExprSpecval = ExprSpecvsp[1-ExprSpeclen];

  /* Default location. */
  ExprSpecYYLLOC_DEFAULT (ExprSpecloc, (ExprSpeclsp - ExprSpeclen), ExprSpeclen);
  ExprSpecerror_range[1] = ExprSpecloc;
  ExprSpecYY_REDUCE_PRINT (ExprSpecn);
  switch (ExprSpecn)
    {
  case 2:
#line 313 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { ParseResult = 0; }
#line 1891 "y.tab.c"
    break;

  case 3:
#line 314 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { ParseResult = 0; }
#line 1897 "y.tab.c"
    break;

  case 4:
#line 319 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1903 "y.tab.c"
    break;

  case 5:
#line 320 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1909 "y.tab.c"
    break;

  case 6:
#line 324 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1915 "y.tab.c"
    break;

  case 7:
#line 325 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1921 "y.tab.c"
    break;

  case 8:
#line 330 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1927 "y.tab.c"
    break;

  case 9:
#line 331 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        specRegisterVariable((ExprSpecvsp[-3].s));
        specRegisterEditable((ExprSpecvsp[-3].s),(ExprSpecvsp[-1].n));
      }
#line 1936 "y.tab.c"
    break;

  case 10:
#line 335 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1942 "y.tab.c"
    break;

  case 11:
#line 336 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1948 "y.tab.c"
    break;

  case 12:
#line 337 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1954 "y.tab.c"
    break;

  case 13:
#line 338 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1960 "y.tab.c"
    break;

  case 14:
#line 339 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1966 "y.tab.c"
    break;

  case 15:
#line 340 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 1972 "y.tab.c"
    break;

  case 16:
#line 341 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        specRegisterVariable((ExprSpecvsp[-3].s));
        specRegisterEditable((ExprSpecvsp[-3].s),(ExprSpecvsp[-1].n));
      }
#line 1981 "y.tab.c"
    break;

  case 17:
#line 345 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 1987 "y.tab.c"
    break;

  case 18:
#line 346 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 1993 "y.tab.c"
    break;

  case 19:
#line 347 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 1999 "y.tab.c"
    break;

  case 20:
#line 348 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 2005 "y.tab.c"
    break;

  case 21:
#line 349 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 2011 "y.tab.c"
    break;

  case 22:
#line 350 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 2017 "y.tab.c"
    break;

  case 23:
#line 355 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
    { (ExprSpecval.n) = 0; }
#line 2023 "y.tab.c"
    break;

  case 24:
#line 359 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                        { (ExprSpecval.n) = 0; }
#line 2029 "y.tab.c"
    break;

  case 25:
#line 360 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                            { (ExprSpecval.n) = 0;}
#line 2035 "y.tab.c"
    break;

  case 26:
#line 361 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0;}
#line 2041 "y.tab.c"
    break;

  case 27:
#line 366 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2047 "y.tab.c"
    break;

  case 28:
#line 367 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        if(SPEC_IS_NUMBER((ExprSpecvsp[-5].n)) && SPEC_IS_NUMBER((ExprSpecvsp[-3].n)) && SPEC_IS_NUMBER((ExprSpecvsp[-1].n))){
            (ExprSpecval.n)=remember(new ExprSpecVectorNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[-5].n),(ExprSpecvsp[-3].n),(ExprSpecvsp[-1].n)));
        }else (ExprSpecval.n)=0;
      }
#line 2057 "y.tab.c"
    break;

  case 29:
#line 372 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2063 "y.tab.c"
    break;

  case 30:
#line 373 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2069 "y.tab.c"
    break;

  case 31:
#line 374 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2075 "y.tab.c"
    break;

  case 32:
#line 375 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2081 "y.tab.c"
    break;

  case 33:
#line 376 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2087 "y.tab.c"
    break;

  case 34:
#line 377 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2093 "y.tab.c"
    break;

  case 35:
#line 378 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2099 "y.tab.c"
    break;

  case 36:
#line 379 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2105 "y.tab.c"
    break;

  case 37:
#line 380 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2111 "y.tab.c"
    break;

  case 38:
#line 381 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2117 "y.tab.c"
    break;

  case 39:
#line 382 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = (ExprSpecvsp[0].n); }
#line 2123 "y.tab.c"
    break;

  case 40:
#line 383 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        if(SPEC_IS_NUMBER((ExprSpecvsp[0].n))){
            ExprSpecScalarNode* node=(ExprSpecScalarNode*)(ExprSpecvsp[0].n);
            node->v*=-1;
            node->startPos=(ExprSpecloc).first_column;
            node->endPos=(ExprSpecloc).last_column;
            (ExprSpecval.n)=(ExprSpecvsp[0].n);
        }else (ExprSpecval.n)=0;
      }
#line 2137 "y.tab.c"
    break;

  case 41:
#line 392 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2143 "y.tab.c"
    break;

  case 42:
#line 393 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2149 "y.tab.c"
    break;

  case 43:
#line 394 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2155 "y.tab.c"
    break;

  case 44:
#line 395 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2161 "y.tab.c"
    break;

  case 45:
#line 396 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2167 "y.tab.c"
    break;

  case 46:
#line 397 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2173 "y.tab.c"
    break;

  case 47:
#line 398 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2179 "y.tab.c"
    break;

  case 48:
#line 399 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0; }
#line 2185 "y.tab.c"
    break;

  case 49:
#line 400 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        if((ExprSpecvsp[-1].n) && strcmp((ExprSpecvsp[-3].s),"curve")==0){
            (ExprSpecval.n)=remember(new ExprSpecCurveNode((ExprSpecvsp[-1].n)));
        }else if((ExprSpecvsp[-1].n) && strcmp((ExprSpecvsp[-3].s),"ccurve")==0){
            (ExprSpecval.n)=remember(new ExprSpecCCurveNode((ExprSpecvsp[-1].n)));
        }else if((ExprSpecvsp[-1].n) && strcmp((ExprSpecvsp[-3].s),"swatch")==0){
            (ExprSpecval.n)=remember(new ExprSpecColorSwatchNode((ExprSpecvsp[-1].n)));
        }else if((ExprSpecvsp[-1].n) && strcmp((ExprSpecvsp[-3].s),"animCurve")==0){
            (ExprSpecval.n)=remember(new ExprSpecAnimCurveNode((ExprSpecvsp[-1].n)));
        }else if((ExprSpecvsp[-1].n) && strcmp((ExprSpecvsp[-3].s),"deepWater")==0){
            (ExprSpecval.n)=remember(new ExprSpecDeepWaterNode((ExprSpecvsp[-1].n)));
        }else if((ExprSpecvsp[-1].n)){
            // function arguments not parse of curve, ccurve, or animCurve
            // check if there are any string args that need to be made into controls
            // but be sure to return 0 as this parseable
            if(ExprSpecListNode* list=dynamic_cast<ExprSpecListNode*>((ExprSpecvsp[-1].n))){
                for(size_t i=0;i<list->nodes.size();i++){
                    if(ExprSpecStringNode* str=dynamic_cast<ExprSpecStringNode*>(list->nodes[i])){
                        specRegisterEditable("<UNKNOWN>",str);
                    }
                }
            }
            (ExprSpecval.n)=0;
        }else (ExprSpecval.n)=0;
      }
#line 2215 "y.tab.c"
    break;

  case 50:
#line 425 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                  {(ExprSpecval.n) = 0; }
#line 2221 "y.tab.c"
    break;

  case 51:
#line 426 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 2227 "y.tab.c"
    break;

  case 52:
#line 427 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {  (ExprSpecval.n) = 0; }
#line 2233 "y.tab.c"
    break;

  case 53:
#line 428 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n)=remember(new ExprSpecScalarNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[0].d))); }
#line 2239 "y.tab.c"
    break;

  case 54:
#line 433 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = 0;}
#line 2245 "y.tab.c"
    break;

  case 55:
#line 434 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = (ExprSpecvsp[0].n);}
#line 2251 "y.tab.c"
    break;

  case 56:
#line 439 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
        {
       // ignore first argument unless it is a string (because we parse strings in weird ways)
       ExprSpecListNode* list=new ExprSpecListNode((ExprSpecloc).last_column,(ExprSpecloc).last_column);
       if((ExprSpecvsp[0].n) && SPEC_IS_STR((ExprSpecvsp[0].n))){
           list->add((ExprSpecvsp[0].n));
       }
       remember(list);
       (ExprSpecval.n)=list;
   }
#line 2265 "y.tab.c"
    break;

  case 57:
#line 448 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                 {

      if((ExprSpecvsp[-2].n) && (ExprSpecvsp[0].n) && ((SPEC_IS_NUMBER((ExprSpecvsp[0].n)) || SPEC_IS_VECTOR((ExprSpecvsp[0].n)) || SPEC_IS_STR((ExprSpecvsp[0].n))))){
          (ExprSpecval.n)=(ExprSpecvsp[-2].n);
          dynamic_cast<ExprSpecListNode*>((ExprSpecvsp[-2].n))->add((ExprSpecvsp[0].n));
      }else{
          (ExprSpecval.n)=0;
      }
    }
#line 2279 "y.tab.c"
    break;

  case 58:
#line 460 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                { (ExprSpecval.n) = (ExprSpecvsp[0].n);}
#line 2285 "y.tab.c"
    break;

  case 59:
#line 461 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"
                                {
        ExprSpecStringNode* str=new ExprSpecStringNode((ExprSpecloc).first_column,(ExprSpecloc).last_column,(ExprSpecvsp[0].s));
        //specRegisterEditable("<UNKNOWN>",str);
        // TODO: move string stuff out
        (ExprSpecval.n) = remember(str);
      }
#line 2296 "y.tab.c"
    break;


#line 2300 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter ExprSpecchar, and that requires
     that ExprSpectoken be updated with the new translation.  We take the
     approach of translating immediately before every use of ExprSpectoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     ExprSpecYYABORT, ExprSpecYYACCEPT, or ExprSpecYYERROR immediately after altering ExprSpecchar or
     if it invokes ExprSpecYYBACKUP.  In the case of ExprSpecYYABORT or ExprSpecYYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of ExprSpecYYERROR or ExprSpecYYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  ExprSpecYY_SYMBOL_PRINT ("-> $$ =", ExprSpecr1[ExprSpecn], &ExprSpecval, &ExprSpecloc);

  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpeclen = 0;
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);

  *++ExprSpecvsp = ExprSpecval;
  *++ExprSpeclsp = ExprSpecloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int ExprSpeclhs = ExprSpecr1[ExprSpecn] - ExprSpecYYNTOKENS;
    const int ExprSpeci = ExprSpecpgoto[ExprSpeclhs] + *ExprSpecssp;
    ExprSpecstate = (0 <= ExprSpeci && ExprSpeci <= ExprSpecYYLAST && ExprSpeccheck[ExprSpeci] == *ExprSpecssp
               ? ExprSpectable[ExprSpeci]
               : ExprSpecdefgoto[ExprSpeclhs]);
  }

  goto ExprSpecnewstate;


/*--------------------------------------.
| ExprSpecerrlab -- here on detecting error.  |
`--------------------------------------*/
ExprSpecerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  ExprSpectoken = ExprSpecchar == ExprSpecYYEMPTY ? ExprSpecYYEMPTY : ExprSpecYYTRANSLATE (ExprSpecchar);

  /* If not already recovering from an error, report this error.  */
  if (!ExprSpecerrstatus)
    {
      ++ExprSpecnerrs;
#if ! ExprSpecYYERROR_VERBOSE
      ExprSpecerror (ExprSpecYY_("syntax error"));
#else
# define ExprSpecYYSYNTAX_ERROR ExprSpecsyntax_error (&ExprSpecmsg_alloc, &ExprSpecmsg, \
                                        ExprSpecssp, ExprSpectoken)
      {
        char const *ExprSpecmsgp = ExprSpecYY_("syntax error");
        int ExprSpecsyntax_error_status;
        ExprSpecsyntax_error_status = ExprSpecYYSYNTAX_ERROR;
        if (ExprSpecsyntax_error_status == 0)
          ExprSpecmsgp = ExprSpecmsg;
        else if (ExprSpecsyntax_error_status == 1)
          {
            if (ExprSpecmsg != ExprSpecmsgbuf)
              ExprSpecYYSTACK_FREE (ExprSpecmsg);
            ExprSpecmsg = ExprSpecYY_CAST (char *, ExprSpecYYSTACK_ALLOC (ExprSpecYY_CAST (ExprSpecYYSIZE_T, ExprSpecmsg_alloc)));
            if (!ExprSpecmsg)
              {
                ExprSpecmsg = ExprSpecmsgbuf;
                ExprSpecmsg_alloc = sizeof ExprSpecmsgbuf;
                ExprSpecsyntax_error_status = 2;
              }
            else
              {
                ExprSpecsyntax_error_status = ExprSpecYYSYNTAX_ERROR;
                ExprSpecmsgp = ExprSpecmsg;
              }
          }
        ExprSpecerror (ExprSpecmsgp);
        if (ExprSpecsyntax_error_status == 2)
          goto ExprSpecexhaustedlab;
      }
# undef ExprSpecYYSYNTAX_ERROR
#endif
    }

  ExprSpecerror_range[1] = ExprSpeclloc;

  if (ExprSpecerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (ExprSpecchar <= ExprSpecYYEOF)
        {
          /* Return failure if at end of input.  */
          if (ExprSpecchar == ExprSpecYYEOF)
            ExprSpecYYABORT;
        }
      else
        {
          ExprSpecdestruct ("Error: discarding",
                      ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
          ExprSpecchar = ExprSpecYYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto ExprSpecerrlab1;


/*---------------------------------------------------.
| ExprSpecerrorlab -- error raised explicitly by ExprSpecYYERROR.  |
`---------------------------------------------------*/
ExprSpecerrorlab:
  /* Pacify compilers when the user code never invokes ExprSpecYYERROR and the
     label ExprSpecerrorlab therefore never appears in user code.  */
  if (0)
    ExprSpecYYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this ExprSpecYYERROR.  */
  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpeclen = 0;
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
  ExprSpecstate = *ExprSpecssp;
  goto ExprSpecerrlab1;


/*-------------------------------------------------------------.
| ExprSpecerrlab1 -- common code for both syntax error and ExprSpecYYERROR.  |
`-------------------------------------------------------------*/
ExprSpecerrlab1:
  ExprSpecerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      ExprSpecn = ExprSpecpact[ExprSpecstate];
      if (!ExprSpecpact_value_is_default (ExprSpecn))
        {
          ExprSpecn += ExprSpecYYTERROR;
          if (0 <= ExprSpecn && ExprSpecn <= ExprSpecYYLAST && ExprSpeccheck[ExprSpecn] == ExprSpecYYTERROR)
            {
              ExprSpecn = ExprSpectable[ExprSpecn];
              if (0 < ExprSpecn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (ExprSpecssp == ExprSpecss)
        ExprSpecYYABORT;

      ExprSpecerror_range[1] = *ExprSpeclsp;
      ExprSpecdestruct ("Error: popping",
                  ExprSpecstos[ExprSpecstate], ExprSpecvsp, ExprSpeclsp);
      ExprSpecYYPOPSTACK (1);
      ExprSpecstate = *ExprSpecssp;
      ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
    }

  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++ExprSpecvsp = ExprSpeclval;
  ExprSpecYY_IGNORE_MAYBE_UNINITIALIZED_END

  ExprSpecerror_range[2] = ExprSpeclloc;
  /* Using ExprSpecYYLLOC is tempting, but would change the location of
     the lookahead.  ExprSpecYYLOC is available though.  */
  ExprSpecYYLLOC_DEFAULT (ExprSpecloc, ExprSpecerror_range, 2);
  *++ExprSpeclsp = ExprSpecloc;

  /* Shift the error token.  */
  ExprSpecYY_SYMBOL_PRINT ("Shifting", ExprSpecstos[ExprSpecn], ExprSpecvsp, ExprSpeclsp);

  ExprSpecstate = ExprSpecn;
  goto ExprSpecnewstate;


/*-------------------------------------.
| ExprSpecacceptlab -- ExprSpecYYACCEPT comes here.  |
`-------------------------------------*/
ExprSpecacceptlab:
  ExprSpecresult = 0;
  goto ExprSpecreturn;


/*-----------------------------------.
| ExprSpecabortlab -- ExprSpecYYABORT comes here.  |
`-----------------------------------*/
ExprSpecabortlab:
  ExprSpecresult = 1;
  goto ExprSpecreturn;


#if !defined ExprSpecoverflow || ExprSpecYYERROR_VERBOSE
/*-------------------------------------------------.
| ExprSpecexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
ExprSpecexhaustedlab:
  ExprSpecerror (ExprSpecYY_("memory exhausted"));
  ExprSpecresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| ExprSpecreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
ExprSpecreturn:
  if (ExprSpecchar != ExprSpecYYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      ExprSpectoken = ExprSpecYYTRANSLATE (ExprSpecchar);
      ExprSpecdestruct ("Cleanup: discarding lookahead",
                  ExprSpectoken, &ExprSpeclval, &ExprSpeclloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this ExprSpecYYABORT or ExprSpecYYACCEPT.  */
  ExprSpecYYPOPSTACK (ExprSpeclen);
  ExprSpecYY_STACK_PRINT (ExprSpecss, ExprSpecssp);
  while (ExprSpecssp != ExprSpecss)
    {
      ExprSpecdestruct ("Cleanup: popping",
                  ExprSpecstos[*ExprSpecssp], ExprSpecvsp, ExprSpeclsp);
      ExprSpecYYPOPSTACK (1);
    }
#ifndef ExprSpecoverflow
  if (ExprSpecss != ExprSpecssa)
    ExprSpecYYSTACK_FREE (ExprSpecss);
#endif
#if ExprSpecYYERROR_VERBOSE
  if (ExprSpecmsg != ExprSpecmsgbuf)
    ExprSpecYYSTACK_FREE (ExprSpecmsg);
#endif
  return ExprSpecresult;
}
#line 469 "/home/alexandre/devel/SeExpr/src/SeExpr2/UI/ExprSpecParser.y"


/* ExprSpecerror - Report an error.  This is called by the parser.
(Note: the "msg" param is useless as it is usually just "sparse error".
so it's ignored.)
*/
static void ExprSpecerror(const char* /*msg*/)
{
    // find start of line containing error
    int pos = ExprSpecpos(), lineno = 1, start = 0, end = strlen(ParseStr);
    bool multiline = 0;
    for (int i = start; i < pos; i++)
	if (ParseStr[i] == '\n') { start = i + 1; lineno++; multiline=1; }

    // find end of line containing error
    for (int i = end; i > pos; i--)
	if (ParseStr[i] == '\n') { end = i - 1; multiline=1; }

    ParseError = ExprSpectext[0] ? "Syntax error" : "Unexpected end of expression";
    if (multiline) {
	char buff[30];
	snprintf(buff, 30, " at line %d", lineno);
	ParseError += buff;
    }
    if (ExprSpectext[0]) {
	ParseError += " near '";
	ParseError += ExprSpectext;
    }
    ParseError += "':\n    ";

    int s = std::max(start, pos-30);
    int e = std::min(end, pos+30);

    if (s != start) ParseError += "...";
    ParseError += std::string(ParseStr, s, e-s+1);
    if (e != end) ParseError += "...";
}

namespace SeExpr2 {
extern void specResetCounters(std::vector<std::pair<int,int> >& comments);
}


/* CallParser - This is our entrypoint from the rest of the expr library. 
   A string is passed in and a parse tree is returned.	If the tree is null,
   an error string is returned.  Any flags set during parsing are passed
   along.
 */

static SeExprInternal2::Mutex mutex;

/// Main entry point to parser
bool ExprSpecParse(std::vector<Editable*>& outputEditables,
    std::vector<std::string>& outputVariables,
    std::vector<std::pair<int,int> >& comments,
    const char* str)
{
    SeExprInternal2::AutoMutex locker(mutex);

    /// Make inputs/outputs accessible to parser actions
    editables=&outputEditables;
    variables=&outputVariables;
    ParseStr=str;

    // setup and startup parser
    SeExpr2::specResetCounters(comments); // reset lineNumber and columnNumber in scanner
    ExprSpec_buffer_state* buffer = ExprSpec_scan_string(str); // setup lexer
    ParseResult = 0;
    int resultCode = ExprSpecparse(); // parser (don't care if it is a parse error)
    UNUSED(resultCode);
    ExprSpec_delete_buffer(buffer);

    // delete temporary data -- specs(mini parse tree) and tokens(strings)!
    for(size_t i=0;i<specNodes.size();i++) delete specNodes[i];
    specNodes.clear();
    for(size_t i=0;i<tokens.size();i++) free(tokens[i]);
    tokens.clear();
    return true;
}

