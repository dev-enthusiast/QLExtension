/* $Header: /home/cvs/src/book/src/of_book.h,v 1.16 2004/11/29 23:48:02 khodod Exp $ */
/****************************************************************
**
**	OF_BOOK.H	- Header information for Book and Position
**				  related functions
**	
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_OF_BOOK_H )
#define IN_OF_BOOK_H

#include <secdb.h>

/*
**	Define constants
*/

#define	BOOK_HASH_VAL_SIZE				10		// i.e. (int) log10( MAX_ULONG )
#define	BOOK_NAME_SIZE					(SDB_SEC_NAME_SIZE - ( BOOK_HASH_VAL_SIZE + 1 + 5 ))
#define BOOK_PROFCTR_NAME_SIZE			8		// externally imposed
#define	BOOK_PROFCTR_TYPE				"Profit Center"
#define BOOK_PROFCTR_BADCHARS			" \t\b\n/~`@#$%^&*()_-=+[]|\\:'\"?<>,."

#define 	BOOK_CUSTOMER_BOOK_PREFIX		"Acct"
#define 	CUSTOMER_ACCOUNT_PREFIX			"CustAcct"

typedef uint32_t BOOK_HASH;

/*
**	Prototype functions
*/

DLLEXPORT int
		BookPositionDecrement(		char const* BookName, char const* SecurityName, 
									double QuantityOffset, 
                                    SDB_UNIQUE_ID TradeID, SDB_DB *Db ),
		BookPositionIncrement(		char const* BookName, char const* SecurityName, 
                                    double QuantityOffset, 
                                    SDB_UNIQUE_ID TradeID, SDB_DB *Db ),
		BookPositionSet(			char const* BookName, char const* SecurityName, 
                                    double QuantityOffset, 
                                    SDB_UNIQUE_ID TradeID, SDB_DB *Db );

DLLEXPORT SDB_OBJECT
		*BookPositionLookup(		char const* BookName, char const* SecurityName, SDB_DB *Db );

DLLEXPORT int
		BookPositionLookupName(		char const* BookName, char const* SecurityName, char *ComputedName, SDB_DB *Db );

DLLEXPORT BOOK_HASH
		BookHashFunc(				char const* String );

DLLEXPORT int
		BookValidateTradeParties(		SDB_DB *Db, char const* Book1, char const* Book2, char const* Group, SDB_OBJECT *OptionalTrade ),
		BookValidateGroup(				SDB_OBJECT *Portfolio, char const* Group ),
		AccountIdGetCustomerAccount( 		char const* BookId, char	CustomerAccount[ SDB_SEC_NAME_SIZE ]),
	 	BookIdCompanyGetCustomerBook( 	char const* BookId, char const* Company , char *Customer );

#endif
