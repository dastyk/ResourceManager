// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2013 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file parser.tab.h
 ** Define the  ObjParser ::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
// //                    "%code requires" blocks.
#line 12 "parser.y" // lalr1.cc:371

    #include <iostream>
    #include <string>
    #include <vector>
    #include <stdint.h>

    using namespace std;

    namespace ObjParser {
        class Scanner;
        class Interpreter;
    }

#line 58 "parser.tab.h" // lalr1.cc:371

# include <cassert>
# include <vector>
# include <iostream>
# include <stdexcept>
# include <string>
# include "stack.hh"
# include "location.hh"
#include <typeinfo>
#ifndef YYASSERT
# include <cassert>
# define YYASSERT assert
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 9 "parser.y" // lalr1.cc:371
namespace  ObjParser  {
#line 81 "parser.tab.h" // lalr1.cc:371



  /// A char[S] buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current state.
  template <size_t S>
  struct variant
  {
    /// Type of *this.
    typedef variant<S> self_type;

    /// Empty construction.
    variant ()
      : yytname_ (YY_NULL)
    {}

    /// Construct and fill.
    template <typename T>
    variant (const T& t)
      : yytname_ (typeid (T).name ())
    {
      YYASSERT (sizeof (T) <= S);
      new (yyas_<T> ()) T (t);
    }

    /// Destruction, allowed only if empty.
    ~variant ()
    {
      YYASSERT (!yytname_);
    }

    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    build ()
    {
      YYASSERT (!yytname_);
      YYASSERT (sizeof (T) <= S);
      yytname_ = typeid (T).name ();
      return *new (yyas_<T> ()) T;
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    build (const T& t)
    {
      YYASSERT (!yytname_);
      YYASSERT (sizeof (T) <= S);
      yytname_ = typeid (T).name ();
      return *new (yyas_<T> ()) T (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as ()
    {
      YYASSERT (yytname_ == typeid (T).name ());
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const
    {
      YYASSERT (yytname_ == typeid (T).name ());
      YYASSERT (sizeof (T) <= S);
      return *yyas_<T> ();
    }

    /// Swap the content with \a other, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsability.
    /// Swapping between built and (possibly) non-built is done with
    /// variant::move ().
    template <typename T>
    void
    swap (self_type& other)
    {
      YYASSERT (yytname_);
      YYASSERT (yytname_ == other.yytname_);
      std::swap (as<T> (), other.as<T> ());
    }

    /// Move the content of \a other to this.
    ///
    /// Destroys \a other.
    template <typename T>
    void
    move (self_type& other)
    {
      YYASSERT (!yytname_);
      build<T> ();
      swap<T> (other);
      other.destroy<T> ();
    }

    /// Copy the content of \a other to this.
    template <typename T>
    void
    copy (const self_type& other)
    {
      build<T> (other.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytname_ = YY_NULL;
    }

  private:
    /// Prohibit blind copies.
    self_type& operator=(const self_type&);
    variant (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ ()
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[S];
    } yybuffer_;

    /// Whether the content is built: if defined, the name of the stored type.
    const char *yytname_;
  };


  /// A Bison parser.
  class  Parser 
  {
  public:
#ifndef YYSTYPE
    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // BOOLEAN
      char dummy1[sizeof(bool)];

      // REAL
      // real
      // optreal
      // optreal2
      char dummy2[sizeof(float)];

      // POSITION
      // TEXCOORD
      // NORMAL
      // POINT
      // FACE
      // OBJECT
      // S
      // MTLLIB
      // USEMTL
      // NAME
      // SEP
      // optname
      char dummy3[sizeof(std::string)];

      // createface
      char dummy4[sizeof(std::vector<std::vector<uint32_t>>)];

      // indices
      char dummy5[sizeof(std::vector<uint32_t>)];

      // INTEGER
      char dummy6[sizeof(uint32_t)];

      // line
      // statement
      char dummy7[sizeof(void*)];
};

    /// Symbol semantic values.
    typedef variant<sizeof(union_type)> semantic_type;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        TOKEN_END = 0,
        TOKEN_REAL = 258,
        TOKEN_INTEGER = 259,
        TOKEN_POSITION = 260,
        TOKEN_TEXCOORD = 261,
        TOKEN_NORMAL = 262,
        TOKEN_POINT = 263,
        TOKEN_FACE = 264,
        TOKEN_OBJECT = 265,
        TOKEN_S = 266,
        TOKEN_MTLLIB = 267,
        TOKEN_USEMTL = 268,
        TOKEN_NAME = 269,
        TOKEN_BOOLEAN = 270,
        TOKEN_SEP = 271
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Internal symbol number.
    typedef int symbol_number_type;

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols, and symbols from each type.

  basic_symbol (typename Base::kind_type t, const location_type& l);

  basic_symbol (typename Base::kind_type t, const bool v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const float v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<std::vector<uint32_t>> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const std::vector<uint32_t> v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const uint32_t v, const location_type& l);

  basic_symbol (typename Base::kind_type t, const void* v, const location_type& l);


      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      ~basic_symbol ();

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// -1 when this symbol is empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      enum { empty = 0 };

      /// The symbol type.
      /// -1 when this symbol is empty.
      token_number_type type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;

    // Symbol constructors declarations.
    static inline
    symbol_type
    make_END (const location_type& l);

    static inline
    symbol_type
    make_REAL (const float& v, const location_type& l);

    static inline
    symbol_type
    make_INTEGER (const uint32_t& v, const location_type& l);

    static inline
    symbol_type
    make_POSITION (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_TEXCOORD (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_NORMAL (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_POINT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_FACE (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_OBJECT (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_S (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_MTLLIB (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_USEMTL (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_NAME (const std::string& v, const location_type& l);

    static inline
    symbol_type
    make_BOOLEAN (const bool& v, const location_type& l);

    static inline
    symbol_type
    make_SEP (const std::string& v, const location_type& l);


    /// Build a parser object.
     Parser  (ObjParser::Scanner &scanner_yyarg, ObjParser::Interpreter &driver_yyarg);
    virtual ~ Parser  ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
     Parser  (const  Parser &);
     Parser & operator= (const  Parser &);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yytoken   the lookahead token type, or yyempty_.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         symbol_number_type yytoken) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yylhs     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yylhs);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (token_type t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const signed char yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned char yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const signed char yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const signed char yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned char yytable_[];

  static const signed char yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned char yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned char yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned char yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param s         The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// "empty" when empty.
      symbol_number_type type_get () const;

      enum { empty = 0 };

      /// The state.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    // Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 51,           //< Last index in yytable_.
      yynnts_ = 9,  //< Number of nonterminal symbols.
      yyempty_ = -2,
      yyfinal_ = 27, //< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 17    //< Number of tokens.
    };


    // User arguments.
    ObjParser::Scanner &scanner;
    ObjParser::Interpreter &driver;
  };

  // Symbol number corresponding to token number t.
  inline
   Parser ::token_number_type
   Parser ::yytranslate_ (token_type t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      15,    16
    };
    const unsigned int user_token_number_max_ = 271;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

  inline
   Parser ::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
      switch (other.type_get ())
    {
      case 15: // BOOLEAN
        value.copy< bool > (other.value);
        break;

      case 3: // REAL
      case 20: // real
      case 22: // optreal
      case 23: // optreal2
        value.copy< float > (other.value);
        break;

      case 5: // POSITION
      case 6: // TEXCOORD
      case 7: // NORMAL
      case 8: // POINT
      case 9: // FACE
      case 10: // OBJECT
      case 11: // S
      case 12: // MTLLIB
      case 13: // USEMTL
      case 14: // NAME
      case 16: // SEP
      case 21: // optname
        value.copy< std::string > (other.value);
        break;

      case 24: // createface
        value.copy< std::vector<std::vector<uint32_t>> > (other.value);
        break;

      case 25: // indices
        value.copy< std::vector<uint32_t> > (other.value);
        break;

      case 4: // INTEGER
        value.copy< uint32_t > (other.value);
        break;

      case 18: // line
      case 19: // statement
        value.copy< void* > (other.value);
        break;

      default:
        break;
    }

  }


  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {
    (void) v;
      switch (this->type_get ())
    {
      case 15: // BOOLEAN
        value.copy< bool > (v);
        break;

      case 3: // REAL
      case 20: // real
      case 22: // optreal
      case 23: // optreal2
        value.copy< float > (v);
        break;

      case 5: // POSITION
      case 6: // TEXCOORD
      case 7: // NORMAL
      case 8: // POINT
      case 9: // FACE
      case 10: // OBJECT
      case 11: // S
      case 12: // MTLLIB
      case 13: // USEMTL
      case 14: // NAME
      case 16: // SEP
      case 21: // optname
        value.copy< std::string > (v);
        break;

      case 24: // createface
        value.copy< std::vector<std::vector<uint32_t>> > (v);
        break;

      case 25: // indices
        value.copy< std::vector<uint32_t> > (v);
        break;

      case 4: // INTEGER
        value.copy< uint32_t > (v);
        break;

      case 18: // line
      case 19: // statement
        value.copy< void* > (v);
        break;

      default:
        break;
    }
}


  // Implementation of basic_symbol constructor for each type.

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const bool v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const float v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<std::vector<uint32_t>> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::vector<uint32_t> v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const uint32_t v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}

  template <typename Base>
   Parser ::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const void* v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  template <typename Base>
  inline
   Parser ::basic_symbol<Base>::~basic_symbol ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
    switch (yytype)
    {
      case 15: // BOOLEAN
        value.template destroy< bool > ();
        break;

      case 3: // REAL
      case 20: // real
      case 22: // optreal
      case 23: // optreal2
        value.template destroy< float > ();
        break;

      case 5: // POSITION
      case 6: // TEXCOORD
      case 7: // NORMAL
      case 8: // POINT
      case 9: // FACE
      case 10: // OBJECT
      case 11: // S
      case 12: // MTLLIB
      case 13: // USEMTL
      case 14: // NAME
      case 16: // SEP
      case 21: // optname
        value.template destroy< std::string > ();
        break;

      case 24: // createface
        value.template destroy< std::vector<std::vector<uint32_t>> > ();
        break;

      case 25: // indices
        value.template destroy< std::vector<uint32_t> > ();
        break;

      case 4: // INTEGER
        value.template destroy< uint32_t > ();
        break;

      case 18: // line
      case 19: // statement
        value.template destroy< void* > ();
        break;

      default:
        break;
    }

  }

  template <typename Base>
  inline
  void
   Parser ::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
      switch (this->type_get ())
    {
      case 15: // BOOLEAN
        value.move< bool > (s.value);
        break;

      case 3: // REAL
      case 20: // real
      case 22: // optreal
      case 23: // optreal2
        value.move< float > (s.value);
        break;

      case 5: // POSITION
      case 6: // TEXCOORD
      case 7: // NORMAL
      case 8: // POINT
      case 9: // FACE
      case 10: // OBJECT
      case 11: // S
      case 12: // MTLLIB
      case 13: // USEMTL
      case 14: // NAME
      case 16: // SEP
      case 21: // optname
        value.move< std::string > (s.value);
        break;

      case 24: // createface
        value.move< std::vector<std::vector<uint32_t>> > (s.value);
        break;

      case 25: // indices
        value.move< std::vector<uint32_t> > (s.value);
        break;

      case 4: // INTEGER
        value.move< uint32_t > (s.value);
        break;

      case 18: // line
      case 19: // statement
        value.move< void* > (s.value);
        break;

      default:
        break;
    }

    location = s.location;
  }

  // by_type.
  inline
   Parser ::by_type::by_type ()
     : type (empty)
  {}

  inline
   Parser ::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
   Parser ::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
   Parser ::by_type::move (by_type& that)
  {
    type = that.type;
    that.type = empty;
  }

  inline
  int
   Parser ::by_type::type_get () const
  {
    return type;
  }

  inline
   Parser ::token_type
   Parser ::by_type::token () const
  {
    // YYTOKNUM[NUM] -- (External) token number corresponding to the
    // (internal) symbol number NUM (which must be that of a token).  */
    static
    const unsigned short int
    yytoken_number_[] =
    {
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271
    };
    return static_cast<token_type> (yytoken_number_[type]);
  }
  // Implementation of make_symbol for each symbol type.
   Parser ::symbol_type
   Parser ::make_END (const location_type& l)
  {
    return symbol_type (token::TOKEN_END, l);

  }

   Parser ::symbol_type
   Parser ::make_REAL (const float& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_REAL, v, l);

  }

   Parser ::symbol_type
   Parser ::make_INTEGER (const uint32_t& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_INTEGER, v, l);

  }

   Parser ::symbol_type
   Parser ::make_POSITION (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_POSITION, v, l);

  }

   Parser ::symbol_type
   Parser ::make_TEXCOORD (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_TEXCOORD, v, l);

  }

   Parser ::symbol_type
   Parser ::make_NORMAL (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_NORMAL, v, l);

  }

   Parser ::symbol_type
   Parser ::make_POINT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_POINT, v, l);

  }

   Parser ::symbol_type
   Parser ::make_FACE (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_FACE, v, l);

  }

   Parser ::symbol_type
   Parser ::make_OBJECT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_OBJECT, v, l);

  }

   Parser ::symbol_type
   Parser ::make_S (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_S, v, l);

  }

   Parser ::symbol_type
   Parser ::make_MTLLIB (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_MTLLIB, v, l);

  }

   Parser ::symbol_type
   Parser ::make_USEMTL (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_USEMTL, v, l);

  }

   Parser ::symbol_type
   Parser ::make_NAME (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_NAME, v, l);

  }

   Parser ::symbol_type
   Parser ::make_BOOLEAN (const bool& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_BOOLEAN, v, l);

  }

   Parser ::symbol_type
   Parser ::make_SEP (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TOKEN_SEP, v, l);

  }


#line 9 "parser.y" // lalr1.cc:371
} //  ObjParser 
#line 1218 "parser.tab.h" // lalr1.cc:371




#endif // !YY_YY_PARSER_TAB_H_INCLUDED
