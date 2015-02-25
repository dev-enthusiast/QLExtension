/****************************************************************
**
**  ExcelValue.h    - Excel value (XLOPER) wrapper.
**
**  Copyright (c) 2011 - Constellation Energy Group, Baltimore, MD
**  Author:         Jason O'Brien (e19351)
**  Date:           2012-02-13
**
****************************************************************/

#ifndef IN_FCADLIBX_EXCELVALUE_H
#define IN_FCADLIBX_EXCELVALUE_H

#include <fcadlib/base.h>
#include <fcadlib/xlcall.h>
#include <string>
#include <cstdarg>

namespace Gs {

/// An internal (single) reference (xltypeSRef).
/// Binary compatible with xlref12.
struct ExcelRef
{
	/// Default constructor.
	ExcelRef() {}

	/// Constructor for a single cell block.
	ExcelRef( RW rwFirst, RW rwLast, COL colFirst, COL colLast ) {
		this->rwFirst = rwFirst;
		this->rwLast = rwLast;
		this->colFirst = colFirst;
		this->colLast = colLast;
	}

	/// Retrieve compatible xlref12 representation.
	operator const xlref12&() const {
		return *reinterpret_cast< const xlref12 * >( this );
	};

	RW rwFirst;
	RW rwLast;
	COL colFirst;
	COL colLast;
};

/// A variable-sized structure holding an external reference ( xltypeRef ).
/// Binary compatible with xlmref12.
struct ExcelExternalRef
{
	/// Default constructor, single block unless specialized new was used.
	ExcelExternalRef( WORD count = 1 ) {
		this->count = count;
	}

	/// Constructor for a single cell block.
	ExcelExternalRef( const ExcelRef &ref ) {
		this->count = 1;
		this->reftbl[ 0 ] = ref;
	}

	/// Retrieve compatible xlmref12 representation.
	operator const xlmref12&() const {
		return *reinterpret_cast< const xlmref12 * >( this );
	};

	WORD count;
	ExcelRef reftbl[ 1 ];

	/// Special new, gets around the dynamically sized struct nonsense.
	/// Use for multiple external references.
	static void *operator new ( size_t size, size_t count ) {
		if( count < 2 ) return new char[ size ];
		return new char[ size + sizeof( ExcelRef ) * ( count-1 ) ];
	}

private:
	/// Prevent copying, as we don't know how large the other guy is.
	ExcelExternalRef( const ExcelExternalRef &other ) {}
	/// Prevent assignment, as we don't know how large the other guy is.
	void operator=( const ExcelExternalRef &other ) {}
};

/// A static-sized structure holding an external reference ( xltypeRef ).
/// Binary compatible with xlmref12.
template< int N > struct ExcelStaticRef
{
	/// Default constructor, single block unless specialized new was used.
	ExcelStaticRef() {
		this->count = N;
	}

	/// Constructor for a single cell block.
	ExcelStaticRef( const ExcelRef &ref ) {
		this->count = 1;
		this->reftbl[ 0 ] = ref;
	}	

	/// Retrieve compatible xlmref12 representation.
	operator const ExcelExternalRef&() const {
		return *reinterpret_cast< const ExcelExternalRef * >( this );
	};

	/// Retrieve compatible xlmref12 representation.
	operator const xlmref12&() const {
		return *reinterpret_cast< const xlmref12 * >( this );
	};

	WORD count;
	ExcelRef reftbl[ N ];
};


/// Wrap excel values for interop with XLL API. A little wasteful, as it favors
/// duplication over tracking memory management, but none of this stuff is 
/// a critical path and the alternative is messy.
class EXPORT_CLASS_FCADLIBX ExcelValue
{
public:
	/// Construct a "missing" Excel value.
	ExcelValue();
	~ExcelValue();

	/// Copy constructor.
	ExcelValue( const ExcelValue &other );

	/// Construct ExcelValue from Excel return value.
	ExcelValue( const xloper12 &oper );

	/// Construct ExcelValue from Excel4 return value.
	ExcelValue( const xloper &oper );
	
	/// Convert a null-terminated string into Excel length-counted format.
	/// Without this, MSVC will send empty strings to the boolean constructor..
	explicit ExcelValue( const char *str );

	/// Convert a string into Excel length-counted format.
	ExcelValue( const std::string &str );

	/// Convert a wide string into Excel length-counted format.
	ExcelValue( const std::wstring &str );

	/// Store an integer in an Excel value.
	explicit ExcelValue( int value );

	/// Store a double in an Excel value.
	explicit ExcelValue( double value );

	/// Store a boolean in an Excel value.
	explicit ExcelValue( bool value );

	/// Store a simple reference in an Excel value.
	ExcelValue( const ExcelRef &ref );

	/// Store an external reference in an Excel value.
	ExcelValue( IDSHEET sheet, const ExcelRef &mref );

	/// Store an external reference in an Excel value.
	/// This allows us to use the ExcelStaticRef helper more easily.
	ExcelValue( IDSHEET sheet, const ExcelExternalRef &mref );

	/// Store an external reference in an Excel value ( may just be sheet ID ).
	/// Some excel functions ( ex. xlSheetId ) return the sheet ID with a null ref.
	ExcelValue( IDSHEET sheet, const ExcelExternalRef *mref );

	/// Assignment operator, cleans up previous value.
	const ExcelValue &operator=( const ExcelValue &other );

	/// Assignment operator, cleans up previous value.
	const ExcelValue &operator=( const xloper12 &other );

	/// Assignment operator, cleans up previous value.
	const ExcelValue &operator=( const xloper &other );
	
	/// Get a string value ( Warning: Value must be ASCII ).
	std::string getString() const;

	/// Get a wide string value.
	std::wstring getWideString() const;

	/// Get the worksheet ID of an external reference.
	IDSHEET getSheetId() const;

	/// Get pointer to external reference, valid while value remains unchanged.
	const ExcelExternalRef *getExternalRef() const;

	/// Get stored integer value.
	int getInteger() const;

	/// Get stored double value.
	double getNumber() const;

	/// Get pointer to external reference, valid while value remains unchanged.
	const ExcelRef &getRef() const;

	/// Get an error code value.
	int getError() const;

	/// Get a boolean value.
	bool getBoolean() const;

	/// Whether value holds a string.
	bool isString() const;

	/// Whether value holds an external reference.
	bool isExternalRef() const;

	/// Whether value holds an integer.
	bool isInteger() const;
	
	/// Whether value holds a number ( double ).
	bool isNumber() const;

	/// Whether value holds a reference.
	bool isRef() const;

	/// Whether value represents an error.
	bool isError() const;

	/// Whether value holds a boolean.
	bool isBoolean() const;
	
	/// Execute an arbitrary excel function, storing result into this value.
	int execute( int function );
	/// Execute an arbitrary excel function, storing result into this value.
	int execute( int function, const ExcelValue &arg1 );
	/// Execute an arbitrary excel function, storing result into this value.
	int execute( int function, const ExcelValue &arg1, const ExcelValue &arg2 );
	/// Execute an arbitrary excel function, storing result into this value.
	int execute( int function, const ExcelValue &arg1, const ExcelValue &arg2, const ExcelValue &arg3 );
	/// Execute an arbitrary excel function, storing result into this value.
	int execute( int function, const ExcelValue &arg1, const ExcelValue &arg2, const ExcelValue &arg3, const ExcelValue &arg4 );

	/// Helper method, retrieve active sheet and store in current value.
	int executeCurrentSheetId();
	
private:
	/// Cleanup value (releasing memory, etc).
	void cleanup();
	
	/// Handle actual function execution.
	int executeInternal( int function, int count, const ExcelValue *args[] );

	/// Duplicate an external reference into this value.
	void fromExternalRef( IDSHEET sheet, const ExcelExternalRef *ref );

	// Inner value being wrapped.
	xloper12 val;
};

}

#endif