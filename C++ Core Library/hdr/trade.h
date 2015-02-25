/****************************************************************
**
**	trade.h	- Trade class and APIs
**
**	Copyright 1993-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/trade/src/trade.h,v 1.79 2011/12/15 21:21:07 e19351 Exp $
**
****************************************************************/

#if !defined( IN_TRADE_H )
#define IN_TRADE_H

#if !defined( IN_SECOBJ_H )
#include <secobj.h>
#endif
#if !defined( IN_DATE_H )
#include <date.h>
#endif


/*
**	Net Effects data structures (see dtneteff.h for full definitions)
*/

typedef struct NetEffectsBookStructure		NET_EFFECTS_BOOK;
typedef struct NetEffectsNodeStructure		NET_EFFECTS_NODE;
typedef struct NetEffectsStructure			NET_EFFECTS;


/*
**	Constants for "Trade Control Vt" value type
*/

#define		TRADE_PASS_SET_VALUE			0x0001
#define		TRADE_PASS_DIDDLE				0x0002


/*
** Upload states
*/

typedef int TRADE_STATE;

#define		TRADE_STATE_NEW					0
#define		TRADE_STATE_INVALID				1
#define		TRADE_STATE_ERROR				2
#define		TRADE_STATE_SENT				3

#define		TRADE_STATE_AMEND				4
#define		TRADE_STATE_AMEND_INVALID		5
#define		TRADE_STATE_AMEND_SENT			6
#define		TRADE_STATE_AMEND_ERROR			7

#define		TRADE_STATE_QUEUED				8
#define		TRADE_STATE_QUEUED_INVALID		9
#define		TRADE_STATE_QUEUED_ERROR		10
#define		TRADE_STATE_QUEUED_CANCEL		11

#define		TRADE_STATE_CANCEL				12
#define		TRADE_STATE_CANCEL_SENT			13
#define		TRADE_STATE_CANCEL_ERROR		14
#define		TRADE_STATE_DONE				15
#define		TRADE_STATE_NOT_APPLICABLE		16
#define		TRADE_STATE_DELETABLE			16	// Synonym

#define		TRADE_STATE_IGNORE				17

#define		TRADE_STATE_UNMATCHED 			18
#define		TRADE_STATE_NOT_ALLOCATED 		19
#define		TRADE_STATE_ALLOCATED_HELD 		20
#define		TRADE_STATE_COUNT				21


//	define values for the ConfirmFlag VT
#define		TRADE_CONFIRM			0
#define		TRADE_BYPASS_CONFIRM	1

/*
**	define Hold Allocations Flags
*/

#define		TRADE_HAF_SEND 			0x001
#define		TRADE_HAF_COMPLETE 		0x002


/*
**	Trade, TofR, Structured Trade name format strings
*/

#define		TRADE_FORMAT_NAME			"Trade %10u"

#define		MATCHED_STATE_NAME			"Matched"
#define		UNMATCHED_STATE_NAME		"UnMatched"


#define 	TOFR_FORMAT_NAME_FIRST		"TofR %ld 0"
#define		TOFR_FORMAT_NAME_LAST		"TofR %ld ~"
#define		TOFR_FORMAT_NAME			"TofR %ld %ld"

#define 	PET_FORMAT_NAME_FIRST		"PET %lu 0"
#define		PET_FORMAT_NAME_LAST		"PET %lu ~"
#define		PET_FORMAT_NAME				"PET %lu %d"
#define		PET_FORMAT_NAME_TRANS		"PET %lu %d %d"

#define		STRUCTURE_FORMAT_NAME		"Structured Trade %10lu"


/*
**	define Match types
*/

#define		MATCH_TYPE_TRADER_SALES		"Trader-Sales"
#define		MATCH_TYPE_SALES_CUST		"Sales-Customer"
#define		MATCH_TYPE_TRADER_TRADER	"Trader-Trader"
#define		MATCH_TYPE_TRADER_CUST		"Trader-Customer"


/*
**	NEW					- New trade
**	INVALID				- Trade has validation errors
**	SENT				- Trade has been sent to the uploader
**	ERROR				- Upload returned an error
**
**	AMEND				- Trade has been sent and acknowledged, but
**						  subsequently modified
**	AMEND_INVALID		- Same as AMEND, but validation errors were detected
**						  when the modifications were made
**	AMEND_SENT			- Amendment has been sent to the uploader
**	AMEND_ERROR			- Amendment upload returned an error
**
**	QUEUED				- Trade or amendment has been sent but not yet
**						  acknowledged, subsequent to sending, the trade was
**						  modified
**	QUEUED_INVALID		- Same as QUEUED, but validation errors were detected
**						  for the most recent modifications
**	QUEUED_ERROR		- The acknowledgement for the prior upload message
**						  returned an error
**	QUEUED_CANCEL		- Deleted but still waiting for ack from previous send
**
**	CANCEL				- Deleted and ready to send cancel
**	CANCEL_SENT			- Cancel sent, waiting for ack
**	CANCEL_ERROR		- Cancel upload returned an error
**
**	DONE				- Trade has been uploaded and acknowledged without
**						  errors
**
**	NOT_APPLICABLE		- Mainframe feed not applicable (e.g. Deletable)
**
**	IGNORE				- Explicit indication not to feed to mainframe
**						  for any reason
**
**
**	Transition Diagram
**	------------------
**	From				To			Process
**	----				---------	--------------------------------------
**	NEW					INVALID		SecDbValidate with SDB_VALIDATE_ERRORs
**						SENT		SecDbAdd or SecDbUpdate if Feed Mainframe( trade )
**						NOT_APPL.	SecDbAdd or SecDbUpdate if !Feed Mainframe( trade )
**						NOT_APPL.	TRADE_MSG_CANCEL
**
**	INVALID				NEW			SecDbValidate without SDB_VALIDATE_ERRORs
**						NOT_APPL.	TRADE_MSG_CANCEL
**
**	SENT				DONE		TRADE_MSG_ACK_GOOD
**						ERROR		TRADE_MSG_ACK_FAIL
**						QUEUED		SetValue
**						CANCEL		TRADE_MSG_CANCEL
**
**	ERROR				AMEND		SetValue
**						NOT_APPL.	TRADE_MSG_CANCEL
**
**	AMEND				AMEND_INV.	SecDbValidate with SDB_VALIDATE_ERRORs
**						AMEND_SENT	SecDbAdd or SecDbUpdate
**						CANCEL		TRADE_MSG_CANCEL
**
**	AMEND_INVALID		AMEND		SecDbValidate without SDB_VALIDATE_ERRORs
**						CANCEL		TRADE_MSG_CANCEL
**
**	AMEND_SENT			DONE		TRADE_MSG_ACK_GOOD
**						AMEND_ERROR	TRADE_MSG_ACK_FAIL
**						QUEUED		SetValue
**						QUEUED_CAN.	TRADE_MSG_CANCEL
**
**	AMEND_ERROR			AMEND		SetValue
**						CANCEL		TRADE_MSG_CANCEL
**
**	QUEUED				QUEUED_INV.	SecDbValidate with SDB_VALIDATE_ERRORs
**						QUEUED_ERR.	TRADE_MSG_ACK_FAIL
**						AMEND		TRADE_MSG_ACK_GOOD
**						QUEUED_CAN.	TRADE_MSG_CANCEL
**
**	QUEUED_INVALID		QUEUED		SecDbValidate without SDB_VALIDATE_ERRORs
**						QUEUED_ERR.	TRADE_MSG_ACK_FAIL
**						AMEND_INV.	TRADE_MSG_ACK_GOOD
**						QUEUED_CAN.	TRADE_MSG_CANCEL
**
**	QUEUED_ERROR		AMEND		SetValue
**						CANCEL		TRADE_MSG_CANCEL
**
**	QUEUED_CANCEL		CANCEL		TRADE_MSG_ACK_GOOD or TRADE_MSG_FAIL
**
**	CANCEL				CANCEL_SENT	SecDbAdd or SecDbUpdate
**
**	CANCEL_SENT			CANCEL_ERR.	TRADE_MSG_ACK_FAIL
**						NOT_APPL.	TRADE_MSG_ACK_GOOD
**
**	CANCEL_ERROR					Error sink
**
**	NOT_APPLICABLE		NEW			SetValue if Feed Mainframe( trade )
**
**	DONE				AMEND		SetValue if Feed Mainframe( trade )
**						CANCEL		TRADE_MSG_CANCEL if Feed Mainframe( trade )
**						NOT_APPL.	TRADE_MSG_CANCEL if !Feed Mainframe( trade )
*/


/*
**	Special messages for trade objects to support upload states
*/

#define		TRADE_MSG_CANCEL				( SDB_MSG_USER + 1 )	// In lieu of SecDbDelete
#define		TRADE_MSG_ACK_GOOD				( SDB_MSG_USER + 2 )	// Mainframe acknowledgement - no errors
#define		TRADE_MSG_ACK_FAIL				( SDB_MSG_USER + 3 )	// Mainframe acknowledgement - errors
#define		TRADE_MSG_POS_EFFECTS			( SDB_MSG_USER + 4 )	// Modify Portfolios
#define		TRADE_MSG_ACK_NOT_APPLICABLE	( SDB_MSG_USER + 5 )	// Mainframe acknowledgement - not applicable/not supported

#define		TOFR_MSG_ACK_GOOD				( SDB_MSG_USER + 6 )	// Mainframe acknowledgement - no errors
#define		TOFR_MSG_ACK_FAIL				( SDB_MSG_USER + 7 )	// Mainframe acknowledgement - errors
#define		TOFR_MSG_ACK_NOT_APPLICABLE		( SDB_MSG_USER + 8 )	// Mainframe acknowledgement - not applicable/not supported


#define		TRADE_MAX_DEALS			3						// Maximum number of mainframe deals per trade


/*
**	Error constants
*/

#define		ERR_TRADE_CANCEL_PENDING		8290
#define		ERR_TRADE_CANCEL_PRIOR_DAY		8291
#define		ERR_TRADE_VALIDATION			8292


/*
**	Flags for TradeFromStructure
*/

#define		TRADE_FLAG_IGNORE_TRADE_DEF		0x0001	// If set, ignore validation due to Trade Defaults object
#define		TRADE_FLAG_IGNORE_WARNINGS		0x0002	// If not set, fail if there are any validation warnings
#define		TRADE_FLAG_IGNORE_ERRORS		0x0004	// If not set, fail if there are any validation errors


/*
** Return codes for Exercise & Expire
*/

#define		TRADE_TRANSACTIONS				1
#define		TRADE_NO_TRANSACTIONS			2

/*
**	Standard prefix for SecDb-generated External Trade ID
*/

#define		TRADE_EXTERNAL_ID_PREFIX		"SDB"

// Reference Type constants

#define		ALREADY_MODIFIED_TEXT	"Modified by prior day amend"
#define		MODIFIED_BY_TBO_TEXT	"Modified by Trade Backout"
#define		REBOOK_TEXT				"Prior day re-book"
#define		BACKOUT_TEXT			"Prior day backout"
#define		SWAPOP_TEXT             "Swap Operation"
#define		MODIFIED_BY_SWAPOP_TEXT "Modified by Swap Operator"


// TradeAmendBackoutBehaviour Constants

const int TRADE_AMEND_USE_DEFAULT        = 0; // Get default behaviour which is BACKOUT_SAME for now
const int TRADE_AMEND_BACKOUT_SAME       = 1; // Backout only the trade which is being amended
const int TRADE_AMEND_BACKOUT_SUBSEQUENT = 2; // Backout the trade being amended and all subsequent position effects

/*
**	Define a structure for the MsgData for TRADE_MSG_ACK_GOOD,
**	TRADE_MSG_ACK_FAIL, and TRADE_MSG_ACK_NOT_APPLICABLE
*/

typedef struct TradeAckData
{
	int		WhichDeal;

	double	MfDealNumber;

	char	*ErrorText;

} TRADE_ACK_DATA;


/*
**	Define a structure for the MsgData for TOFR_MSG_ACK_GOOD,
**	TOFR_MSG_ACK_FAIL, and TOFR_MSG_ACK_NOT_APPLICABLE
*/

typedef struct TofRAckData
{
	double	MfDealNumber;

	char	*ErrorText;

} TOFR_ACK_DATA;


/*
**	Define a structure for the MsgData for TRADE_MSG_POS_EFFECTS
*/

typedef struct TradePosEffectsData
{
	int		IncludePaymentEffect;

} TRADE_POS_EFFECTS_DATA;


/*
**	Define a structure for registering trade types (and its associated
**	price validation callback).
*/

#define		TRADE_TYPE_SIZE		32

typedef int (* TRADE_VALIDATE_PRICE_FUNC )(
	SDB_OBJECT 			*SecPtr,	// Trade whose Unit Price is being validated
	SDB_M_DATA			*MsgData,	// MsgData that was passed to TradeValueFuncUnitPrice
	SDB_VALUE_TYPE		ValueType,	// Value type of Unit Price
	double				Price 		// Spot price being validated, i.e., Current Unit Price( SecPtr )
);									// Returns TRUE if validation message added, FALSE if price valid

typedef struct TradeTypeInfoStructure
{
	char	*TradeType;			// Trade type, e.g. "Exercise", "Assign", etc.

	double	EffQuantFactor;		// Quantity of object, i.e. position effect on one security traded;
								// e.g. "Buy" is 1.0, "Sell" is -1.0

	TRADE_VALIDATE_PRICE_FUNC
			ValidatePrice;		// Optional price validation callback

	char	*Securities;		// Name of value type containing position effects
								// (other than the security traded itself and the
								// payment forward); "" means no other effects

	SDB_VALUE_FUNC
			USDECrossValueFunc;	// Value function to use for trade's USDE Cross;
					   			// NULL means USDE Cross is "USD/" + Payment Unit( Trade )

	int		PreserveEffQuant;	// If TRUE, whenever changing trade type to or from
								// this trade type, preserve the old effective quantity,
								// i.e., Quantity *= new EffQuantFactor / old EffQuantFactor

	int		ZeroUnitPrice;		// All trades of this type have Unit Price == 0

} TRADE_TYPE_INFO;



/*
**	Prototypes
*/

DLLEXPORT SDB_OBJECT
		*TradeDuplicate(			SDB_OBJECT *SecPtr, int CreateSecurity ),
		*TradeFromStructure(		SDB_DB *Db, char *OldTradeNameForUpdate, DT_VALUE *ValuesStruct, int Flags ),
		*TradeRelated(				char *SecurityTraded, SDB_OBJECT *RefTrade, double SellQuantity, char *TradeType, DATE TradeDate, int GroupNumber, char *Category );

DLLEXPORT int
		TradeIsPriorDay(			SDB_OBJECT *SecPtr ),
		TradeModifyPriorDay(		SDB_OBJECT *SecPtr, char *BackoutNameBuf, char *RebookNameBuf ),
		TradeDelete(				SDB_DB *DbID, char *TradeName ),
		TradeValidatePrice(			SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price ),
		TradeValidatePositivePrice(	SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price ),
		TradeValidateZeroPrice(		SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price ),
		TradeSetMethod( 			SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType ),
		TradeTypeIsOpeningType(		const char *TradeType ),
		TradeValidateUpdate(		SDB_OBJECT *NewTrade, SDB_OBJECT *OldTrade ),
		MatchIDGetTrades(           SDB_UNIQUE_ID MatchID, DT_VALUE *Trades, SDB_DB *Db ),
		MatchIDGetTradesOfRecord(   SDB_UNIQUE_ID MatchID, DT_VALUE *TradesOfRecord, SDB_DB *Db ),
		TradeIDGetPETs(             SDB_UNIQUE_ID TradeID, DT_VALUE *PETs, SDB_DB *Db ),
		ExternalTradeIDGetTrades(   const char *ExternalTradeID, DT_VALUE *Trades, SDB_DB *Db ),
		TradeStateToString(			double TradeState, char *Buffer, int MaxLen ),
		TradeGetEntityGroup( 		SDB_OBJECT *SecPtr, SDB_OBJECT *CustSecPtr, SDB_OBJECT **pEGSecPtr ),
		GetSecurityNamesInRange(	char *SecDbClassName, char *Start, char *End, DT_VALUE *ResultArray, int *NumFound, SDB_DB *Db ),
		TradeValidateGroupTraderLocation( SDB_OBJECT *Trade );
		
DLLEXPORT const char
		*TradeGetMethod(			void *MethodsStructPtr, const char *MethodName ),
		*TradeTypeGetOppositeType(	const char *TradeType );

DLLEXPORT int
		TradeAmendBackoutBehaviour( int NewBehaviour = TRADE_AMEND_USE_DEFAULT );

		
#endif
