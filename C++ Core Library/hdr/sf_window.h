/****************************************************************
**
**  SF_WINDOW.H    - Generic windowing symbol driver  
**
**  Copyright (c) 2014 - Exelon Corporation, Chicago, IL
**
**	TSDB has traditionally been used with data sources having good
**  random access capability, such as files, where the overhead of
**  fetching small chunks of data is relatively small. 
**
**  Data sources which have substantial overhead locating and/or
**  building datasets (e.g. relational databases) are better suited
**  to requesting the entire range up-front, or at least in
**  progressively larger chunks.
**
**  This file attempts to provide a basic windowing model, built on
**  top of TSDB's SQL driver support, which simplifies implementation
**  of new symbol function drivers to a subclass of WindowSymbol
**  implementing:
**		1) the window proposal function `ProposeBufferWindow`
**		2) the window loading function `LoadData`
**
**  To use, symbol table drivers should set SYMBOL::SymData to an
**  appropriate subclass of WindowSymbol, which should be valid
**  for the life of the TSDB subsystem, and provide a symbol function
**  SymFuncSqlBinary and SymFuncSqlGmt which call SymFuncWindow.
**
**  This symbol function supports boundary hinting on most symbol
**  operations.
**
**  $Log: sf_window.h,v $
**  Revision 1.2  2014/09/29 12:06:03  e19351
**  Clean up WindowSymbol a bit. Correct interpretation of SYM_POS_BEFORE.
**  AWR: #347855
**
**  Revision 1.1  2014/09/17 18:35:38  e19351
**  Add a generic hinted paging symbol function, to simplify TSDB symbol
**  source development, and a unified SQL data source on top of it.
**  AWR: #347855
**
**
****************************************************************/

#include <portable.h>
#include <kool_ade.h>
#include <turbo.h>
#include <newsym.h>
#include <datatype.h>
#include <tsdbst.h>
#include <datatype.h>
#include <map>

#ifdef BUILDING_TSDB_WINDOW_DRIVER
#    define TSDB_WINDOW_DRIVER_API SECDB_DLL_EXPORT
#else
#    define TSDB_WINDOW_DRIVER_API SECDB_DLL_IMPORT
#endif

// TSDB symbol function for window-based symbols
extern "C" TSDB_WINDOW_DRIVER_API int SymFuncWindow( int OpType, SYMBOL *Sym, DATE Date, int Flags, void *Ptr );

namespace Tsdb
{

// Default window buffer starting size
const size_t DEFAULT_WINDOW_BUFFER_SIZE = 12500;
// Default limit on point count to pass to stored procedures
const size_t DEFAULT_POINT_LIMIT = TIME_MAX;

// Buffer window storage class
class SymbolWindowBuffer;
// Map of buffers to start date
typedef std::map< DATE, SymbolWindowBuffer * > BufferMap;

// Symbol class, used by symbol function for all symbol operations.
// Drivers implementing windowing symbols should subclass WindowSymbol
// and provide an implementation for `LoadData`.
class TSDB_WINDOW_DRIVER_API WindowSymbol
{
public:
	// Constructor
	// Parameters:
	//   name - symbol name
	//   modifier - symbol modifier, if any
	//   isRealTime - TRUE if TimeStamp symbol, FALSE if daily
	WindowSymbol( std::string name, std::string modifier, bool isRealTime );
	
	// Clean up
	virtual ~WindowSymbol();
	
	// Release all loaded data buffers, resetting symbol to unloaded state.
	void ResetBuffer();
	
	// Retrieve the next buffer window, advancing internal iterator.
	// If upperBound is non-zero and the current buffer's range extends
	// beyond upperBound, this function may return false instead of loading
	// the next buffer.
	// Returns whether successful.
	bool Read( DATE upperBound = 0 );
	
	// Seek to a given date or the first date after it, if missing.
	// Parameters:
	//   date - date to seek
	//   upperBound - optional upper bound hint for search
	// 
	// If upperBound is non-zero and < target (first point >= date),
	// the symbol will seek to an arbitrary point in the range
	// [upperBound, target], including the implied end marker between
	// buffers.
	//
	// Accordingly, if upperBound < date, it's possible the final position
	// will be < date.
	//
	// Returns whether successful.
	bool Seek( DATE date, DATE upperBound = 0 );
	
	// Seek to a given date or the first date preceding it, if missing.
	// Parameters:
	//   date - date to seek
	//   lowerBound - optional lower bound hint for search
	// 
	// If lowerBound is non-zero and > target (last point <= date),
	// the symbol will seek to an arbitrary point in the range
	// [target, first point >= lowerBound], which will be > date 
	// (and therefore not prior).
	//
	// Accordingly, if lowerBound >= date, it's possible the final position
	// will be >= date.
	//
	// Returns whether successful.
	bool SeekPrior( DATE date, DATE lowerBound = 0 );
	
	// Update a SYMBOL structure with the current buffer.
	// This will set the PointBuffer and iterators (CurPoint, EndPoint) on the
	// given SYMBOL.
	void AssignBuffer( SYMBOL *symbol );
	
	// Get name of symbol.
	std::string GetName() { return name; }
	// Get modifier of symbol, if any.
	std::string GetModifier() { return modifier; }
	
	// Check whether symbol is stored at a second granularity (versus daily).
	bool IsRealTime() { return isRealTime; }
	
protected:
	// Data fetch for symbol drivers
	// Parameters:
	//   buffer - curve buffer to fill
	//   start - inclusive lower bound of range to fetch
	//   end - inclusive upper bound of range to fetch
	//   limit - maximum number of points to fetch
	// Returns: whether successful
	virtual bool LoadData( DT_CURVE *Buffer, DATE Start, DATE End, int Limit = DEFAULT_POINT_LIMIT ) = 0;
	
	// Get proposed buffer window which contains the specified date.
	// Parameters:
	//   date - date searched for (buffer window must contain this)
	//   lowerBound - start of data range surrounding date which has yet to be loaded
	//   upperBound - end of data range surrounding date which has yet to be loaded
	virtual std::pair< DATE, DATE > ProposeBufferWindow( DATE date, DATE lowerBound, DATE upperBound );
	
	// Get the initial seek range, used as a reference by the windowing algorithm.
	// Returns 0 if no seek has been performed since the last call to ResetBuffer.
	std::pair< DATE, DATE > GetInitialBufferRange();

private:
	// Disallow copy and assignment; won't work.
	WindowSymbol( const WindowSymbol &other );
	void operator=( const WindowSymbol &other );

	// Get the window range surrounding date. 
	//
	// Parameters:
	//   date - date to find window range for
	//   lowerHint - lower bound hint to cap unloaded range surrounding 'date'
	//   upperHint - upper bound hint to cap unloaded range surrounding 'date'
	//
	// In the event that no currently loaded buffer contains 'date', the entire
	// unloaded range surrounding 'date' will be capped by the hints (so we
	// don't ask for more data than the user needs), and sent to
	// ProposeBufferWindow for a buffer window proposal.
	//
	// Returns range representing window (or window proposal) containing 'date'.
	std::pair< DATE, DATE > calcWindowContaining( DATE date, DATE lowerHint, DATE upperHint );

	// Iterate through loaded buffers from the left, stopping at the latest
	// with lowerBound <= 'date'. If not found, returns `buffers.end()`.
	// In math jargon, find the limit of the time series approaching 'date'
	// from negative infinity, ignoring any points not yet loaded.
	// Doesn't load data.
	BufferMap::iterator findWindowLimitLeft( DATE date );

	// Iterate through loaded buffers from the right, stopping at the earliest
	// with upperBound >= 'date'. If not found, returns `buffers.end()`.
	// In math jargon, find the limit of the time series approaching 'date'
	// from positive infinity, ignoring any points not yet loaded.
	// Doesn't load data.
	BufferMap::iterator findWindowLimitRight( DATE date );

	// Fetch buffer window containing given date, from cache if available.
	//
	// Parameters:
	//   date - date to locate window for
	//   lowerBound - passed to calcWindowContaining (used if window unloaded)
	//   upperBound - passed to calcWindowContaining (used if window unloaded)
	//
	// Returns buffer containing 'date'.
	BufferMap::iterator fetchWindowContaining( DATE date, DATE lowerBound, DATE upperBound );
	
	// Fetch buffer window before given buffer, from cache if available.
	//
	// Parameters:
	//   iter - buffer to search relative to
	//   lowerBound - passed to calcWindowContaining (used if window unloaded)
	//
	// Returns buffer prior to 'iter'.
	BufferMap::iterator fetchPrevWindow( BufferMap::iterator iter, DATE lowerBound );
	
	// Fetch buffer window after given buffer, from cache if available.
	//
	// Parameters:
	//   iter - buffer to search relative to
	//   upperBound - passed to calcWindowContaining (used if window unloaded)
	//
	// Returns buffer following 'iter'.
	BufferMap::iterator fetchNextWindow( BufferMap::iterator iter, DATE upperBound );

	// Get offset of date (or the next date, if not found) in given buffer.
	// Use by searching buffers in order, starting with the latest buffer with
	// lower bound <= date.
	//
	// Parameters:
	//   buffer - buffer to search
	//   date - target date
	//
	// Returns offset of first point with date >= 'date', or -1 if all points
	// in the buffer are < 'date'.
	long searchBuffer( SymbolWindowBuffer *buffer, DATE date );

	// Get offset of prior date in given buffer.
	// Use by searching buffers in reverse order, starting with
	// the first buffer with upper bound >= date.
	//
	// Parameters:
	//   buffer - buffer to search
	//   date - target date
	//
	// Returns offset of latest point <= 'date', or -1 if all points in the
	// buffer are > 'date'.
	long searchBufferPrior( SymbolWindowBuffer *buffer, DATE date );
	
	// Load given date range from database. setting ret to new buffer.
	//
	// Parameters:
	//   startDate - inclusive lower bound of range to load
	//   endDate - inclusive upper bound of range to load
	//   hint - Iterator directly after the new buffer, or buffers.end().
	//          See the hinted versions of stl::map::insert.
	//
	// Returns whether successful.
	BufferMap::iterator load( DATE startDate, DATE endDate, BufferMap::iterator hint );
	
	// Symbol name
	std::string name;
	// Symbol modifier (if any)
	std::string modifier;
	// Whether symbol is "RealTime" (second granularity, versus daily granularity)
	bool isRealTime;
	
	// Currently selected buffer
	BufferMap::iterator currentBuffer;
	// Initial buffer, used as reference by windowing algorithm
	BufferMap::iterator initialBuffer;
	// Current index within buffer
	size_t currentOffset;

	// Map of buffers containing data queried since open,
	// by the lower bound of that buffer.
	BufferMap buffers;
};

}