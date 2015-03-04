#define GSCVSID "$Header: /home/cvs/src/secdb/src/slang_util.cpp,v 1.6 2001/11/27 23:23:47 procmon Exp $"
#define BUILDING_SECDB
#include <portable.h>

#include <dtformat.h>
#include <clex.h>

#include <secdb/slang_util.h>
#include <secexpr.h>

#include <ios> // should be <ios>
CC_USING( std::ios );

#include <map>
#include <ccstl.h>

#include <ostream>
CC_USING( std::ostream );

#include <sstream>
CC_USING( std::ostringstream );

#include <string>
CC_USING( std::string );

#include <stdio.h>

extern HASH* SlangFunctionHash; // defined in sdb_eval.c


/****************************************************************
**	Routine: TokenToOperator
**	Returns: NodeType or -1 if not an operator
**	Action :
****************************************************************/

struct Operator_Info
{
    Operator_Info() : type(-1), precedence(0), left_assoc(true) {}
    Operator_Info( SLANG_NODE_TYPE t, int p, bool l ) : type(t), precedence(p), left_assoc(l) {}

    SLANG_NODE_TYPE type;
    int             precedence;
    bool            left_assoc;
};

class Unary_Lookup
{
public:
    Unary_Lookup() : arity(), unary() {}
    Unary_Lookup( SLANG_NODE_TYPE t1, int p1, bool l1, SLANG_NODE_TYPE t2, int p2, bool l2 ) : arity( t1, p1, l1 ), unary( t2, p2, l2 ) {}
    Unary_Lookup( SLANG_NODE_TYPE t1, int p1, bool l1 )                                      : arity( t1, p1, l1 ), unary( t1, p1, l1 ) {}
    Unary_Lookup( SLANG_NODE_TYPE t1, int p1, bool l1, Operator_Info const& un )             : arity( t1, p1, l1 ), unary( un )         {}
    Unary_Lookup( Operator_Info const& ar,             SLANG_NODE_TYPE t2, int p2, bool l2 ) : arity( ar ),         unary( t2, p2, l2 ) {}

    Operator_Info&       operator[]( bool want_unary )       { return want_unary ? unary : arity; }
    Operator_Info const& operator[]( bool want_unary ) const { return want_unary ? unary : arity; }

private:
    Operator_Info arity; // non-unary
    Operator_Info unary;
};


Operator_Info const& find_operator_info( TOKEN token, bool unary )
{
  
    typedef CC_STL_MAP( TOKEN, Unary_Lookup ) Operator_Table;

    static Operator_Table my_table;
	if( my_table.empty())
	{                                        //  !unary                                | unary
	                                         //  function               precedence     | function                 precedence
	                                         //                             left assoc |                             left assoc
	    my_table[ POUND        ] = Unary_Lookup( SLANG_POUND,           11, false );
		my_table[ PLUS_PLUS    ] = Unary_Lookup( SLANG_POST_INCREMENT,  10, true,    /*|*/  SLANG_PRE_INCREMENT,  8, false );
	    my_table[ MINUS_MINUS  ] = Unary_Lookup( SLANG_POST_DECREMENT,  10, true,    /*|*/  SLANG_PRE_DECREMENT,  8, false );
		my_table[ DOUBLE_COLON ] = Unary_Lookup( SLANG_SCOPE_OPERATOR,  10, true  );
	    my_table[ LBRACKET     ] = Unary_Lookup( SLANG_SUBSCRIPT,       10, true  );
		my_table[ DOT          ] = Unary_Lookup( SLANG_COMPONENT,       10, true  );
	    my_table[ LPAREN       ] = Unary_Lookup( SLANG_VALUE_TYPE,       9, true  );
		my_table[ AT_SIGN      ] = Unary_Lookup( SLANG_AT_FUNC,          9, true  );
	    my_table[ DOUBLE_AT    ] = Unary_Lookup( Operator_Info(),                    /*|*/  SLANG_DOUBLE_AT_FUNC, 9, true  );
		my_table[ DOLLAR_SIGN  ] = Unary_Lookup( Operator_Info(),                    /*|*/  SLANG_TYPE_DOLLAR,    8, false );
	    my_table[ AMPERSAND    ] = Unary_Lookup( Operator_Info(),                    /*|*/  SLANG_ADDRESS_OF,     8, false );
		my_table[ EXCLAMATION  ] = Unary_Lookup( Operator_Info(),                    /*|*/  SLANG_UNARY_NOT,      8, false );
	    my_table[ MINUS        ] = Unary_Lookup( SLANG_SUBTRACT,         6, true,    /*|*/  SLANG_UNARY_MINUS,    8, false );
		my_table[ STAR         ] = Unary_Lookup( SLANG_MULTIPLY,         7, true,    /*|*/  SLANG_STAR,           8, false );
	    my_table[ SLASH        ] = Unary_Lookup( SLANG_DIVIDE,           7, true  );
		my_table[ PLUS         ] = Unary_Lookup( SLANG_ADD,              6, true  );
	    my_table[ LT           ] = Unary_Lookup( SLANG_LESS_THAN,        5, true  );
		my_table[ GT           ] = Unary_Lookup( SLANG_GREATER_THAN,     5, true  );
	    my_table[ LT_EQ        ] = Unary_Lookup( SLANG_LESS_OR_EQUAL,    5, true  );
		my_table[ GT_EQ        ] = Unary_Lookup( SLANG_GREATER_OR_EQUAL, 5, true  );
	    my_table[ EQUALITY     ] = Unary_Lookup( SLANG_EQUAL,            4, true  );
		my_table[ NOT_EQ       ] = Unary_Lookup( SLANG_NOT_EQUAL,        4, true  );
	    my_table[ COMPARE      ] = Unary_Lookup( SLANG_COMPARE,          4, true  );
		my_table[ LOGICAL_AND  ] = Unary_Lookup( SLANG_LOGICAL_AND,      3, true  );
	    my_table[ LOGICAL_OR   ] = Unary_Lookup( SLANG_LOGICAL_OR,       2, true  );
		my_table[ EQUAL        ] = Unary_Lookup( SLANG_ASSIGN,           1, false );
	    my_table[ PLUS_EQ      ] = Unary_Lookup( SLANG_ADD_ASSIGN,       1, false );
		my_table[ MINUS_EQ     ] = Unary_Lookup( SLANG_SUBTRACT_ASSIGN,  1, false );
	    my_table[ TIMES_EQ     ] = Unary_Lookup( SLANG_MULTIPLY_ASSIGN,  1, false );
		my_table[ DIVIDE_EQ    ] = Unary_Lookup( SLANG_DIVIDE_ASSIGN,    1, false );
	    my_table[ AND_EQ       ] = Unary_Lookup( SLANG_AND_ASSIGN,       1, false );
		my_table[ COLON_EQUAL  ] = Unary_Lookup( SLANG_COLON_ASSIGN,     0, false );
	}
	return my_table[token][unary];
}

SLANG_NODE_TYPE TokenToOperator(
	bool	Unary,
	int		PrevToken,
	TOKEN	Token,
	bool&   LeftAssociative,
	int&    Precedence
)
{
    Operator_Info const& info = find_operator_info( Token, Unary );
	LeftAssociative = info.left_assoc;
	Precedence      = info.precedence;
	return info.type;
}

/*
**	Used to pass parameters to ExprToString
*/

static long& indent_level(ios& stream)
{
    static int indent_level_flag = ios::xalloc(); // we are guaranteed this to be 0 when we alloc it.
	return stream.iword( indent_level_flag );
}

ostream& do_indent(ostream& stream)
{
    stream << string( 4*indent_level(stream), ' ' );
    return stream;
}

class Set_Indent
{
public:
    Set_Indent( ostream& stream, int value = 1, bool absolute = false )
	  : m_stream(stream),
		m_old_val(indent_level(stream))
    {
	    if( !absolute ) value += m_old_val;
		indent_level(stream) = value;
		if( !absolute && value != m_old_val ) stream << "\n" << do_indent;
	}

    ~Set_Indent()
    {
	    indent_level(m_stream) = m_old_val;
	}

private:
    ostream& m_stream;
    long const m_old_val;
};

class Paren_Adder
{
public:
    Paren_Adder( ostream& stream, bool flag = true )
	  : m_stream(stream),
		m_flag(flag),
		m_close( ")" )
    {
	    if( m_flag )
		    m_stream << "(";
	}
    Paren_Adder( ostream& stream, char const* open, char const* close, bool flag = true )
	  : m_stream(stream),
		m_flag(flag),
		m_close(close)
    {
	    if( m_flag )
		    m_stream << open;
	}
    ~Paren_Adder()
    {
	    if( m_flag )
		    m_stream << m_close;
	}
private:
    ostream& m_stream;
    bool const m_flag;
    char const* m_close;
};

class Slang_Child_Printer
{
public:
    Slang_Child_Printer( SLANG_NODE const* root, int prec, bool is_left, bool eval_once )
	  : m_root(root), m_prec(prec), m_is_left(is_left), m_eval_once(eval_once) {}
    void print( ostream& out ) const;

private:
    SLANG_NODE const* const m_root;
    int const m_prec;
    bool const m_is_left;
    bool const m_eval_once;
};

class Slang_Printer;

class Slang_Children_Printer
{
public:
    Slang_Children_Printer( Slang_Printer const& printer, int start, int last, char const* delim, bool newline )
	  : m_printer(printer), m_start(start), m_last(last), m_delim(delim), m_newline(newline) {}
    void print( ostream& out ) const;

private:
    Slang_Printer const& m_printer;
    int const m_start;
    int const m_last;
    char const* const m_delim;
    bool const m_newline;
};

ostream& operator<<( ostream& out, Slang_Child_Printer const& child )
{
    child.print( out );
	return out;
}

ostream& operator<<( ostream& out, Slang_Children_Printer const& child )
{
    child.print( out );
	return out;
}

class Slang_Printer
{
public:
    Slang_Printer( ostream& out,
				   SLANG_NODE const* root,
				   int prec,
				   bool is_left,
				   bool eval_once )
	  : m_out(out),
		m_root(root),
		m_prec(prec),
		m_is_left(is_left),
		m_eval_once(eval_once)
    {}

    void print() const;

private:
    friend class Slang_Children_Printer;

    Slang_Child_Printer stream_child( int child, int prec, bool is_left ) const
    {
	    return Slang_Child_Printer( m_root->Argv[child], prec, is_left, m_eval_once );
	}
    Slang_Child_Printer stream_other( SLANG_NODE const* node ) const
    {
	    return Slang_Child_Printer( node, m_prec, m_is_left, m_eval_once );
	}
    void print_binary_op( char const* op, TOKEN tok ) const
    {
	    int curr_prec;
		Paren_Adder padder( m_out, need_prec_parens( tok, curr_prec ));
		m_out << stream_child( 0, curr_prec, true ) << op << stream_child( 1, curr_prec, false );
	}
    bool need_prec_parens( TOKEN tok, int& curr_prec ) const
    {
	    bool curr_left;
		TokenToOperator( false, -1, tok, curr_left, curr_prec );
		return (( curr_prec < m_prec ) ||
				( curr_prec == m_prec && curr_left != m_is_left ));
	}
    string print_name( char const* str ) const
    {
	    string res = str;
	    CLEXCTX* const ctx = SlangCLexNew( 0, str );
		if( ctx && clex_gettok( ctx ) && clex_gettok( ctx ) && ctx->token != EOF_TOKEN )
		    res = "\""  + res + "\"";

		if( ctx )
		    clex_stop( ctx );

		return res;
	}
    string print_root_name() const
    {
	    return this->print_name( SLANG_NODE_STRING_VALUE( m_root ));
	}
    void indent() const
    {
	    m_out << "\n" << do_indent;
	}

    Slang_Children_Printer stream_expr_list( int start, int last, char const* delim, bool newline = false ) const
    {
	    return Slang_Children_Printer( *this, start, last, delim, newline );
	}
    Slang_Children_Printer stream_expr_list( int start, char const* delim, bool newline = false ) const
    {
	    return Slang_Children_Printer( *this, start, m_root->Argc, delim, newline );
	}  
    void print_expr_list( int start, int last, char const* delim, bool newline = false ) const
    {
	    if( m_root->Argc > start )
		{
		    m_out << stream_child( start, -1, false );
			for( int num = start + 1; num < last; ++num )
			{
				m_out << delim;
				if( newline ) this->indent();
				m_out << stream_child( num, -1, false );
			}
		}
	}
    void print_expr_list( int start, char const* delim, bool newline = false ) const
    {
	    this->print_expr_list( start, m_root->Argc, delim, newline );
	}

    ostream& m_out;
    SLANG_NODE const* const m_root;
    int const m_prec;
    bool const m_is_left;
    bool const m_eval_once;
};

void Slang_Child_Printer::print( ostream& out ) const
{
    Slang_Printer( out, m_root, m_prec, m_is_left, m_eval_once ).print();
}

void Slang_Children_Printer::print( ostream& out ) const
{
    m_printer.print_expr_list( m_start, m_last, m_delim, m_newline );
}

void Slang_Printer::print() const
{
// FIX - hardcoded precedences -- should call TokenToOperator once for each

	switch( m_root->Type )
	{
		case SLANG_ASSIGN:				print_binary_op( " = ",   EQUAL       ); return;
		case SLANG_ADD_ASSIGN:			print_binary_op( " += ",  PLUS_EQ     ); return;
		case SLANG_SUBTRACT_ASSIGN:		print_binary_op( " -= ",  MINUS_EQ    ); return;
		case SLANG_MULTIPLY_ASSIGN:		print_binary_op( " *= ",  TIMES_EQ    ); return;
		case SLANG_DIVIDE_ASSIGN:		print_binary_op( " /= ",  DIVIDE_EQ   ); return;
		case SLANG_COLON_ASSIGN:		print_binary_op( " := ",  COLON_EQUAL ); return;
		case SLANG_AND_ASSIGN:			print_binary_op( " &= ",  AND_EQ      ); return;
		case SLANG_BINARY_AT_OP:		print_binary_op( " @ ",   AT_SIGN     ); return;
		case SLANG_ADD:					print_binary_op( " + ",   PLUS        ); return;
		case SLANG_SUBTRACT:			print_binary_op( " - ",   MINUS       ); return;
		case SLANG_MULTIPLY:			print_binary_op( " * ",   STAR        ); return;
		case SLANG_DIVIDE:				print_binary_op( " / ",   SLASH       ); return;
		case SLANG_EQUAL:				print_binary_op( " == ",  EQUALITY    ); return;
		case SLANG_NOT_EQUAL:			print_binary_op( " != ",  NOT_EQ      ); return;
		case SLANG_GREATER_THAN:		print_binary_op( " > ",   GT          ); return;
		case SLANG_LESS_THAN:			print_binary_op( " < ",   LT          ); return;
		case SLANG_GREATER_OR_EQUAL:	print_binary_op( " >= ",  GT_EQ       ); return;
		case SLANG_LESS_OR_EQUAL:		print_binary_op( " <= ",  LT_EQ       ); return;
		case SLANG_LOGICAL_AND:			print_binary_op( " && ",  LOGICAL_AND ); return;
		case SLANG_LOGICAL_OR:			print_binary_op( " || ",  LOGICAL_OR  ); return;
		case SLANG_COMPARE:				print_binary_op( " <=> ", COMPARE     ); return;
	    case SLANG_TYPE_DOLLAR:			m_out << "$"  << stream_child( 0,  8, false ); return;
	    case SLANG_UNARY_MINUS:			m_out << "-"  << stream_child( 0,  8, false ); return;
	    case SLANG_UNARY_NOT:			m_out << "!"  << stream_child( 0,  8, false ); return;
	    case SLANG_ADDRESS_OF:			m_out << "&"  << stream_child( 0,  8, false ); return;
	    case SLANG_POUND:				m_out << "#"  << stream_child( 0, 11, false ); return;
	    case SLANG_STAR:				m_out << "*"  << stream_child( 0,  8, false ); return;
	    case SLANG_PRE_INCREMENT:		m_out << "++" << stream_child( 0,  8, true  ); return;
	    case SLANG_PRE_DECREMENT:		m_out << "--" << stream_child( 0,  8, true  ); return;
	    case SLANG_POST_INCREMENT:		m_out << stream_child( 0, 8, false ) <<  "++"; return;
	    case SLANG_POST_DECREMENT:		m_out << stream_child( 0, 8, false ) <<  "--"; return;
		case SLANG_TYPE_SCOPED_VARIABLE:
	    case SLANG_SCOPE_OPERATOR:		m_out << m_root->Argv[ 0 ]->StringValue <<  "::" << print_name( m_root->Argv[ 1 ]->StringValue ); return;
	    case SLANG_COMPONENT:			m_out << stream_child( 0, 10, true  )        <<  "." << print_root_name(); return;
	    case SLANG_SUBSCRIPT:			m_out << stream_child( 0, 10, true  )        << "[ " << stream_expr_list( 1, ", " ) << " ]"; return;
	    case SLANG_VALUE_TYPE_FUNC:		m_out << stream_child( 0,  9, false )        << "( " << stream_expr_list( 1, ", " ) << " )"; return;
	    case SLANG_VALUE_TYPE:			m_out << print_root_name()                   << "( " << stream_expr_list( 0, ", " ) << " )"; return;
	    case SLANG_AT_FUNC:				m_out <<  "@" << stream_child( 0, 9, false ) << "( " << stream_expr_list( 1, ", " ) << " )"; return;
	    case SLANG_DOUBLE_AT_FUNC:		m_out << "@@" << stream_child( 0, 9, false ) << "( " << stream_expr_list( 1, ", " ) << " )"; return;
	    case SLANG_MEMBER_FUNC:			m_out << stream_child( 0, 10, true ) << "." << print_root_name() << "( " << stream_expr_list( 1, ", " ) << " )"; return;
		case SLANG_TYPE_LOCAL_VARIABLE:
	    case SLANG_TYPE_VARIABLE:		m_out << print_root_name(); return;
		case SLANG_TYPE_NULL:			m_out << "NULL"; return;
	    default:						m_out << "<unknown node: " << SlangNodeNameFromType( m_root->Type ) << ">"; return;

		case SLANG_ARRAY_INIT:
		{
			Paren_Adder pa(m_out, "[", "]");

			// For nested array inits, e.g., TableInit, do one elem per line
			if( m_root->Argc > 0 && m_root->Argv[ 0 ]->Type == SLANG_ARRAY_INIT )
			{
			    {
					Set_Indent id1(m_out);
					m_out << stream_expr_list( 0, ",", true );
				}
				indent();
			}
			else
			{
			    m_out << stream_expr_list( 0, ", " );
			}
			return;
		}

		case SLANG_TYPE_EVAL_ONCE:
		{
			if( m_root->ValueDataType == DtSlang )
			{
			    m_out << stream_other( ((DT_SLANG *) SLANG_NODE_GET_POINTER( m_root, DtSlang, NULL ))->Expression );
				return;
			}

			Paren_Adder ev1( m_out, "EvalOnce( ", " )", m_eval_once );

			if( m_root->Argc )
			{
			    m_out << stream_child( 0, -1, false );
				return;
			}

			if( m_root->ValueDataType == NULL )
			{
			    m_out << "Internal Secdb Error";
				return;
			}

			DT_VALUE
			        FormatSpecValue,
			        NodeValue,
			        StringValue;

			DT_MSG_FORMAT_INFO
			        FormatSpec;


			FormatSpecValue.Data.Pointer = &FormatSpec;

			FormatSpec.Buffer 			= NULL;
			FormatSpec.BufferSize 		= 0;
			FormatSpec.OutForm.Width 	= -1;
			FormatSpec.OutForm.Decimal 	= -1;
			FormatSpec.OutForm.Flags	= OUTFLAG_CONCISE | OUTFLAG_TRIM_LEADING | OUTFLAG_LEFT;
			FormatSpec.OutForm.ScaleAmt = 1;
			FormatSpec.Code				= 'v';

			StringValue.DataType		= DtString;

			SLANG_NODE_GET_VALUE( m_root, &NodeValue );

			if( !DT_OP( DT_MSG_FORMAT, &FormatSpecValue, &NodeValue, NULL ))
			{
			    if( !DTM_TO( &StringValue, &NodeValue ))
				{
				    m_out << ErrGetString();
				}
				else
				{
				    m_out <<(char const*) StringValue.Data.Pointer;
					DTM_FREE( &StringValue );
				}
			}
			else
			{
			    m_out <<  FormatSpec.Buffer;
				free( FormatSpec.Buffer );
			}
		}
		return;

		case SLANG_TYPE_CONSTANT:
		{
			if( m_root->ValueDataType == DtString && !( m_root->Flags & SLANG_NODE_NAMED_CONST ))
			{
				Paren_Adder quoter(m_out,"\"","\"");

				char const* s = (char const*) m_root->ValueData.Pointer;
				char const* t = s;

				for(; *s; ++s )
				    if( *s < ' ' || *s > '~' ) // if we have an unprintable character, replace \n's with "\n" and \t's with "\t" and all others with \d
					{
					    m_out << string( t, s ) << "\\";
						if( *s == '\n' || *s == '\t' )
						    m_out << ( *s == '\n' ? 'n' : 't' );
						else
						    m_out << (int)*s;
						t = s + 1;
					}

				m_out << t;
				return;
			}

			if( SLANG_NODE_STRING_VALUE( m_root ) )
			{
			    m_out << SLANG_NODE_STRING_VALUE( m_root );
				return;
			}

			if( m_root->ValueDataType == DtNull )
			{
			    m_out << "Null";
				return;
			}

			DT_VALUE	StrValue,
			            NodeValue;


			StrValue.DataType = DtString;
			SLANG_NODE_GET_VALUE( m_root, &NodeValue );
			DTM_TO( &StrValue, &NodeValue );
			m_out <<  (char *) StrValue.Data.Pointer;
			DTM_FREE( &StrValue );

			return;
		}

		case SLANG_TYPE_VARIABLE_INDEX:
		{
		    m_out << print_name((char const *) m_root->ValueData.Pointer )
				  << "[ " << SLANG_NODE_INDEX( m_root ) << " ]";
			return;
		}

		case SLANG_BLOCK:
		{
			indent();
			m_out << "{";
			{
				Set_Indent id1(m_out);
				m_out << stream_expr_list( 0, ";", true );
			}
			indent();
			m_out << "}";
			return;
		}

		case SLANG_DATA_TYPE_CREATOR:
		case SLANG_RESERVED_FUNC:
		{
			SLANG_FUNCTION_INFO const* const FunctionInfo = (SLANG_FUNCTION_INFO *) HashLookupByBucket( SlangFunctionHash, SLANG_NODE_STRING_VALUE( m_root ), SLANG_NODE_BUCKET( m_root ));
			bool const NoParens   = FunctionInfo && ( FunctionInfo->ParseFlags & SLANG_NO_PARENS );
			bool const BoundBlock = FunctionInfo && ( FunctionInfo->ParseFlags & SLANG_BIND_BLOCK );

			// COLON is a hack to get a precedence level of 0
			int dummy_prec;
			Paren_Adder padder( m_out, BoundBlock && this->need_prec_parens( COLON, dummy_prec ));

			char const* const root_name = SLANG_NODE_STRING_VALUE( m_root );

			if( 0 == stricmp( "if", root_name ))
			{
			    m_out << "if( " << stream_child( 0, -1, false ) << " )";
				if( m_root->Argc > 1 )
				{
					if( m_root->Argv[ 1 ]->Type != SLANG_BLOCK )
					{
					    Set_Indent id1( m_out );
					}

					m_out << stream_child( 1, -1, false );
					if( m_root->Argc > 2 )
					{
						indent();
						m_out <<  ":";
						if( m_root->Argv[ 2 ]->Type != SLANG_BLOCK )
						{
							if( m_root->Argv[ 2 ]->Type == SLANG_RESERVED_FUNC && 0 == stricmp( "if", m_root->Argv[ 2 ]->StringValue ))
								m_out << " ";
							else
							{
							    Set_Indent id1( m_out );
							}
						}
						m_out << stream_child( 2, -1, false );
					}
				}
				return;
			}

			if( 0 == stricmp( "Link", root_name ) || 0 == stricmp( "LinkFile", root_name ))
			{
				// Skip last arg which is filled in at parse time
			    m_out << root_name;
				m_out << "( " << stream_expr_list( 0, m_root->Argc - 1, ", " ) << " )";
				return;
			}

			if( m_root->Type == SLANG_DATA_TYPE_CREATOR && m_root->ValueDataType )
			    m_out << m_root->ValueDataType->Name;
			else
			    m_out << root_name;

			{
			    Paren_Adder pa1( m_out, !NoParens );

				if( m_root->Argc > (int) BoundBlock )
				{
				    m_out << " " << stream_expr_list( 0, m_root->Argc - BoundBlock, ", " ) << " ";
				}
			}

			if( BoundBlock )
			{
			    bool const up_block = (m_root->Argv[ m_root->Argc - 1 ]->Type != SLANG_BLOCK);
				Set_Indent id1(m_out, up_block);
				m_out << stream_child( m_root->Argc - 1, -1, false );
			}
		}
	}
}


/****************************************************************
**	Routine: SlangParseTreeToString
**	Returns: Number of bytes written to string (before \0)
**	Action : Write string representation of parse tree to given
**			 buffer.
****************************************************************/

void SlangParseTreeToString(
	SLANG_NODE const*   Root,
	int			        IndentLevel,
	ostream& out
)
{
    SLANG_ERROR_INFO const& info = Root->ErrorInfo;

	if( info.ModuleType == SLANG_MODULE_OBJECT || info.ModuleType == SLANG_MODULE_FILE )
	{
	    char buff[128];
		snprintf( buff, sizeof(buff), "\n/* [%.32s:%d-%d] */\n\n", info.ModuleName, info.BeginningLine, info.EndingLine );
		buff[127] = '\0';
		out << buff;
	}

	Set_Indent in1( out, IndentLevel, true );
	out << Slang_Child_Printer( Root, -1, false, true );
}


/****************************************************************
**	Routine: SlangParseTreeToBriefString
**	Returns: Number of bytes written to string (before \0)
**	Action : Write string representation of parse tree to given
**			 buffer. Don't put in module comment, hide eval
**			 onces
****************************************************************/

void SlangParseTreeToBriefString(
	SLANG_NODE const*   Root,
	int			        IndentLevel,
	ostream& out
)
{
	Set_Indent in1( out, IndentLevel, true );
	out << Slang_Child_Printer( Root, -1, false, false );
}

