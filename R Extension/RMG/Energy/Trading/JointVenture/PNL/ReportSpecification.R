###############################################################################
# ReportSpecification.R
#
# Author: e14600
#
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")


# Specified as label = subset code
# Eg:
#  subset(x, eval( parse( text=COLUMN_SPEC[["Cash"]] ) ) )
#
COLUMN_SPEC = list(
        'Cash' 		= 	'contract.dt == as.Date( secdb.dateRuleApply( Sys.Date(), "J") )',
        'Prompt' 	= 	'contract.dt == as.Date( secdb.dateRuleApply( Sys.Date(), "J+1mJ") )',
        'Jun 08'	=	'contract.dt == as.Date( "2008-06-01" )',
        'Jul 08'	=	'contract.dt == as.Date( "2008-07-01" )',
        'MN 08'		=	'contract.dt == as.Date( "2008-06-01" ) | contract.dt == as.Date( "2008-07-01" )',
        'Aug 08'	=	'contract.dt == as.Date( "2008-08-01" )',
        'NQ 08'		=	'contract.dt == as.Date( "2008-07-01" ) | contract.dt == as.Date( "2008-08-01" )',
        'Sep 08'	=	'contract.dt == as.Date( "2008-09-01" )',
        'Oct 08'	=	'contract.dt == as.Date( "2008-10-01" )',
        'Nov 08'	=	'contract.dt == as.Date( "2008-11-01" )',
        'Dec 08'	=	'contract.dt == as.Date( "2008-12-01" )',
        'Q4 08'		=	'contract.dt >= as.Date( "2008-10-01" ) & contract.dt < as.Date( "2009-01-01" )',
        'XZ 08'		=	'contract.dt >= as.Date( "2008-11-01" ) & contract.dt < as.Date( "2009-01-01" )',
        '2008' 		= 	'format(contract.dt, "%Y") == "2008"',
        'FG 09'		=	'contract.dt == as.Date( "2009-01-01" ) | contract.dt == as.Date( "2009-02-01" )',
        'KM 09'		=	'contract.dt == as.Date( "2009-05-01" ) | contract.dt == as.Date( "2009-06-01" )',
        'NQ 09'		=	'contract.dt == as.Date( "2009-07-01" ) | contract.dt == as.Date( "2009-08-01" )',
        'Sep 09'	=	'contract.dt == as.Date( "2009-09-01" )',        
        '2009' 		= 	'format(contract.dt, "%Y") == "2009"',
        '2010' 		= 	'format(contract.dt, "%Y") == "2010"',
        '2011' 		= 	'format(contract.dt, "%Y") == "2011"',
        '2012' 		= 	'format(contract.dt, "%Y") == "2012"',
        'Total' 	= 	'TRUE'
        )


# Eg:
#  data[ eval( parse( text=ROW_SPEC[["PJM On-Peak"]] ) ), ]
#
ROW_SPEC = list( 
        'PJM Peak'			= 	"which( sapply( strsplit( data$curve.name, ' ' ), '[', 2 ) == 'PWJ' & sapply( strsplit( data$curve.name, ' ' ), '[', 3 ) == '5X16' )",
        'PJM Off-Peak'		= 	"which( sapply( strsplit( data$curve.name, ' ' ), '[', 2 ) == 'PWJ' & sapply( strsplit( data$curve.name, ' ' ), '[', 3 ) != '5X16' )",
        'NG Nymex'			= 	"which( data$curve.name == 'COMMOD NG EXCHANGE' )",
        'NEPOOL Peak'		= 	"which( sapply( strsplit( data$curve.name, ' ' ), '[', 2 ) == 'PWX' & sapply( strsplit( data$curve.name, ' ' ), '[', 3 ) == '5X16' )",
        'NEPOOL Off-Peak'	= 	"which( sapply( strsplit( data$curve.name, ' ' ), '[', 2 ) == 'PWX' & sapply( strsplit( data$curve.name, ' ' ), '[', 3 ) != '5X16' )",
        'Zone A Peak'		= 	"grep( 'PWY 5X16 WEST', data$curve.name )",
        'Zone A Off-Peak'	= 	"grep( 'PWY (2X16|7X8).* WEST', data$curve.name, perl=TRUE )",
        'Zone C Peak'		= 	"grep( 'PWY 5X16 CEN', data$curve.name )",
        'Zone C Off-Peak'	= 	"grep( 'PWY (2X16|7X8).* CEN', data$curve.name, perl=TRUE )",
        'Zone G Peak'		= 	"grep( 'PWY 5X16 HUDV', data$curve.name )",
        'Zone G Off-Peak'	= 	"grep( 'PWY (2X16|7X8).* HUDV', data$curve.name, perl=TRUE )",
        'Zone H Peak'		= 	"grep( 'PWY 5X16 MILL', data$curve.name )",
        'Zone H Off-Peak'	= 	"grep( 'PWY (2X16|7X8).* MILL', data$curve.name, perl=TRUE )",
        'Zone J Peak'		= 	"grep( 'PWY 5X16 NYC', data$curve.name )",
        'Zone J Off-Peak'	= 	"grep( 'PWY (2X16|7X8).* NYC', data$curve.name, perl=TRUE )"
        )


# Used to determine which columns to aggregate, and how to aggregate them
TYPE_SPEC = list( 
        pnl = 'sum',
        position = 'sum',
        price = 'mean',
        change= 'mean'
        )

