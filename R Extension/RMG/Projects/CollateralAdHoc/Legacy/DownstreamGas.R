###############################################################################
# DownstreamGas.R
#
# Author: e14600
#
memory.limit( 4095 )
library(reshape)
source("H:/user/R/RMG/Utilities/RLogger.R")
source("H:/user/R/RMG/Utilities/load.R")
source("H:/user/R/RMG/Utilities/dollar.R")
source("H:/user/R/RMG/Utilities/Database/SAS/VPort.R")


################################################################################
# File Namespace
#
DownstreamGas = new.env(hash=TRUE)


################################################################################
DownstreamGas$getCNEBooks <- function()
{
    
cneBooks = 
"CEIDEMHG
CEISPMHG
CEISPOHG
CEIVENHG
CEIBSMTM
CEIOTMTM
OGDENDEM
OGDENPHYS
CESAED
CESMED
CETMED
OGDENMOD
OGDENHGP
OGDENHGS
CCGSUPACC
FINLEGACC
SUPLEGACC
SUPLEGNPNS
SUPNONFRMACC
FINAECOHDG
FINANRHDG
FINCHIHDG
FINDAWNHDG
FINDEMHDG
FINLEGCG
FINMICHHDG
FINNORHDG
FINOGTHDG
FINPGEHDG
FINSOCHDG
FINTCOHDG
CCGFINMTM
FINLEGMTM
EOGDEM1
NFG1DEM1
NFG2DEM1
NFG3DEM1
NFGESSDEM
TCOISSDEM1
TGTFSSDEM1
TGTFSSDEM2
TGTISSDEM1
TGTNNSDEM1
EOGPHYS
NFG1PHYS
NFG2PHYS
NFG3PHYS
NFGESSPHY
TCOISSPHY
TGTFSSPHYS
TGTFSSPHYS2
TGTNNSPHYS
CNESAED
CNESMED
EOGMOD
NFG1MOD
NFG2MOD
NFG3MOD
NFGESSMOD
TGTFSSMOD
TGTFSSMOD2
TGTISSMOD
TGTNNSMOD
EOGHGP1
EOGHGS1
NFG1HGP1
NFG1HGS1
NFG2HGP1
NFG2HGS1
NFG3HGP1
NFG3HGS1
NFGESSHGP
NFGESSHGS
TCOISSHGP
TCOISSHGS
TGTFSSHGP1
TGTFSSHGP2
TGTFSSHGS1
TGTFSSHGS2
TGTISSHGP1
TGTISSHGS1
TGTNNSHGP1
TGTNNSHGS1
CNESUPED
CETAED
CNETAED
CNETRNED
CORNERTPHDG
NOVOL
SUPTPHDG
CORNERPRICAP
CORNERRECCAP
CORNERSECCAP
SUPTPCAP
SUPTPMOD
"
    cneBooks = strsplit( cneBooks, "\n" )[[1]]
    cneBooks = sort( unique( cneBooks ) )
    
    return( cneBooks )
}


################################################################################
DownstreamGas$getBaltimoreBooks <- function()
{
    asOfDate = "2008-10-10"
    
    accrualBooks = VPort$booksForPortfolio( "CPS Accrual Portfolio", asOfDate )
    mtmBooks = VPort$booksForPortfolio( "CPS Mark to Market Portfolio", asOfDate )
    
    books = sort( unique( c( accrualBooks, mtmBooks ) ) )
    
    return( books )
}


################################################################################
DownstreamGas$getStorageBooks <- function()
{
    
storageBooks = "CEHUSTED
ENCAIBT
NATLIBT
RMROMACC
ANRDEM
BAYSTDEM
CENHDDEM
ENBDEM
ENCADEM
GOOSDEM
LIBSTDEM
LODI2DEM
LODIDEM
MICHBDEM
MICHDEM
NATLDEMB
NATLDEMC
NATLDEMD
NATSTDEM
STGBDEM
STGCHDEM
UNICDEM
UNIDDEM
UNIDEM
UNIDEMB
UNOFDEM
MTMSTED
MTMTPED
ANRPHYS
BAYSTPHY
CENHDPHY
ENBPHY
ENCAPHY
GOOSPHYS
LIBSTPHY
LODI2PHYS
LODIPHYS
MICHBPHY
MICHPHYS
NATLPHYB
NATLPHYS
NIMOPHYS
STGBPHY
STGCHPHY
STORPHYS
UNICPHY
UNIDPHY
UNIPHY
UNIPHYB
UNOFPHY
STORAED
STORAED2
STORMED
STORMED2
ACCNGED
ANRMOD
BAYSTMOD
CANSTOR
CENHDMOD
CENHDPH2
CENHDPH3
DVACCRUS
ECANSTOR
ENBMOD
ENBRDACC
ENCAMOD
ENCMOD2
ENCPHY2
GOOSMOD
LIBSTMOD
LODI2MOD
LODIMOD
MICHBMOD
MICHED
MICHMOD
NATLMODB
NATLMODC
NATLMODD
NATLSTOR
NATSTRED
NIMOMOD
NIMOPYMT
RNSPTGAS
STCSTRED
STGBMOD
STGCHMOD
STOR1ED
STORFIN
SWSTRGAS
UNICMOD
UNIDMOD
UNIMOD
UNIMODB
UNISTED
UNOFMOD
VIRSTRED
WESTSTOR
ANRHDG
BAYSTHGP
BAYSTHGS
CENHDHDG
ECANSTHG
ENBHDG
ENCAFX
ENCAHDG
ENCAHDGP
GOOSHGP
GOOSHGS
LIBSTHGP
LIBSTHGS
LODI2HGP
LODI2HGS
LODIHGP
LODIHGS
MCPHYSNG
MICHBHGP
MICHBHGS
MICHHDG
MICHHDGS
MTMHGSTR
MTMHGTPT
MTMSTVOL
NATLHDGB
NATLHDGC
NATLHGCP
NATSTRHG
RMROMMTM
STGBHGP
STGBHGS
STGCHHDG
STORFNHG
SWSTRHDG
UNIBHGS
UNICHDGP
UNICHDGS
UNIDHDGP
UNIDHDGS
UNIHDG
UNIHDGB
UNOFHGP
UNOFHGS
WESSTRHG
"
 
    
    storageBooks = strsplit( storageBooks, "\n" )[[1]]
    storageBooks = sort( unique( storageBooks ) )

    return( storageBooks )
}


################################################################################
DownstreamGas$getTransportBooks <- function()
{

transportBooks = "ALLTPHGA
ALLTPIBT
CALACCED
FPACHDG
GASFXPED
NASTR5ED
NEACCED
NETPIBT
NGPLHDGA
NGPLIBT
NGPSTRED
NSTARNPS
RMBAACC
RMDMACC
TRPNPSED
UNIBIBT
WIRACCED
FORMERED
TRANAED2
TRNAED
TRNAED2
TRNMED
TRNMED2
ALGTHDG2
ALGTPHDG
ALLTPHDG
ANRTHDG2
ANRTPHDG
BAYSTHDG
BROTRHED
CANTRHDG
CGTTPHDG
COLTPHDG
DKTPED
DKTPHDG
DOMTPHDG
DUKTPHDG
DVACCRUT
EMPTPHDG
ENOITHDG
GLGTHDG
IRQTHDG2
IRQTPHDG
MELTRAED
MGTTPHDG
MID2HGED
NBTPHDG
NE2TPED
NECAPHDG
NGPTHDG2
NGPTPHDG
NNGTPHDG
NOVTPHDG
NRGTRHDG
NWPPHDG
ONEITHDG
OZKTPHDG
PGETPHDG
PNGTPHDG
PPLTPHDG
RMBAMTM
RMDMMTM
SNTTPHDG
SWGASHDG
SWTPHGED
TCPLHDG
TCPLHED
TENCTHDG
TENTHDG2
TENTPHDG
TETTPHDG
TRNKPHDG
TRZTHDG2
TRZTPHDG
VIRTRAED
WESTRNHG
ALGTPCAP
ALGTPDEM
ALLTPAM2
ALLTPAMT
ALLTPCAP
ANRTPCAP
BAYSTCAP
BORTRAED
CALTRAED
CANTRANS
CEGTTMOD
CGTTPCAP
CGTTPDEM
COLTPCAP
DKTPCAP
DOMTPCAP
DUKTPCAP
DVACCRUB
ECANCAPC
EMPTPCAP
ENBSTRED
ENOGEMOD
ENOITCAP
GLGTCAP
IRQTPCAP
IRQTPDEM
KAPTRAED
NBTPCAP
NECAPGAS
NGPTPCAP
NGPTPDEM
NNGTPCAP
NNGTPDEM
NOVTPCAP
NRGTRCAP
NWPTPCAP
ONEITCAP
OZKTPCAP
PGETPCAP
PMCAPGAS
PMCAPHDG
PNGTPCAP
PPLTPCAP
PPLTPDEM
RKCAPGAS
SNTTPCAP
SWCAPGAS
SWTPCPED
TCPLCAP
TCPLCED
TENCTCAP
TENTPCAP
TENTPDEM
TETTPCAP
TETTPDEM
TRNKPCAP
TRZTPCAP
TRZTPDEM
WESTTRAN
WIRTRAED
ALGTCAP2
ANRTCAP2
GASACHDG
IRQTCAP2
MGTTPCAP
MID2TPED
NE2HGED
NGPTCAP2
TENTCAP2
TRZTCAP2
"

    transportBooks = strsplit( transportBooks, "\n" )[[1]]
    transportBooks = sort( unique( transportBooks ) )
    
    return( transportBooks )
}


################################################################################
DownstreamGas$getDownstreamBooks <- function(cneBooks, baltimoreBooks )
{
    
    BOOK_FILE = "H:/user/R/RMG/Projects/CollateralAdHoc/AllDownstreamBooksPlusCNEAndBaltimore.csv"
    allBooks = read.csv(BOOK_FILE)
    
    dsBooks = sort( unique( allBooks$SUB_BOOK ) )
#    dsBooks = setdiff( dsBooks, cneBooks )
#    dsBooks = setdiff( dsBooks, baltimoreBooks )

    return( dsBooks )
}


################################################################################
data = read.csv( "S:/Risk/Temporary/CollateralAllocation/20081010/SourceData/AllPos_AGMTH_preibt_10OCT08.csv" )


cneBooks = DownstreamGas$getCNEBooks()

baltimoreBooks = DownstreamGas$getBaltimoreBooks()

storageBooks = DownstreamGas$getStorageBooks()

transportBooks = DownstreamGas$getTransportBooks()

dsBooks = DownstreamGas$getDownstreamBooks( cneBooks, baltimoreBooks )

downstreamData = subset( data, book_name %in% dsBooks )
#downstreamData = subset( downstreamData, Tenor != "PRE" )
dim( downstreamData )
dollar( sum( downstreamData$Exposure, na.rm = TRUE ) )

downstreamData$EXPOSURE_FLAG = "External"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$counterparty %in% dsBooks )] = "Houston - Downstream"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$counterparty %in% baltimoreBooks )] = "Baltimore"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$counterparty %in% cneBooks )] = "CNE"
downstreamData$EXPOSURE_FLAG[ grep("^XG", downstreamData$counterparty ) ] = "Houston - Storage"
downstreamData$EXPOSURE_FLAG[ grep("^XM", downstreamData$counterparty ) ] = "Houston - Transport"


names(downstreamData) = toupper( names( downstreamData ) )
downstreamData = downstreamData[, sort(names(downstreamData)) ]
names(downstreamData)[which(names(downstreamData) == "EXPOSURE")] = "value"

finalData = cast( downstreamData, 
        COUNTERPARTY + CREDIT_NETTINGAGREEMENT ~ EXPOSURE_FLAG, 
        sum, na.rm = TRUE, fill = 0, margins = c("grand_col")  )

write.csv( downstreamData, row.names=FALSE, file="C:/Documents and Settings/e14600/Desktop/downstreamData.csv" )

write.csv( finalData, row.names=FALSE, file="C:/Documents and Settings/e14600/Desktop/exposureByFlagData.csv" )




ibts = subset(downstreamData, downstreamData$BOOK_NAME %in% dsBooks & downstreamData$COUNTERPARTY %in% dsBooks)

ibtBooks = sort( unique( ibts$BOOK_NAME ) )

for( book in ibtBooks )
{
    cpList = sort( unique( subset( ibts, BOOK_NAME == book )$COUNTERPARTY ) )
    for( cp in cpList )
    {
        val1 = sum( ibts$value[ which(ibts$COUNTERPARTY==cp & ibts$BOOK_NAME==book)], na.rm=TRUE )
        val2 = sum( ibts$value[ which(ibts$COUNTERPARTY==book & ibts$BOOK_NAME==cp)], na.rm=TRUE )
        if( val1 != (-1*val2) )
        {
            rLog("cp1:", book, "cp2:", cp, "val1=", val1, "val2=", val2)
        }
    }
}


################################################################################
# Lets try getting numbers directly from rmsys
# delete the expired positions per the Raft query
library(RODBC)
dsnPath = "FileDSN=//NAS-OMF-01/cpsshare/All/Risk/Software/R/prod/Utilities/DSN/RMSYSP.dsn"
conString = paste( dsnPath, ";UID=rmsys_read;PWD=rmsys_read;", sep="")
chan = odbcDriverConnect(conString)


asOfDate = as.Date( "2008-10-10" )

rmsysQuery = paste( "SELECT
                deal_number, 
                contract_number, 
                deal_type, 
                source_system, 
                dealer, 
                counterparty, 
                trading_entity, 
                dealt_date, 
                value_date, 
                maturity_date, 
                settlement_date, 
                buy_sell_flag, 
                commodity, 
                volume, 
                price, 
                notional_amount, 
                notional_amount_currency, 
                market_price, 
                mtm, 
                mtm_currency, 
                undelivered_amount, 
                book,
                call_put, 
                strike_price, 
                premium, 
                delta, 
                unit, 
                hub,
                region, 
                broker, 
                accounting,
                instrument,
                exp_date
                FROM rmsys.raft_deal
                WHERE cob_date = '", format( asOfDate, "%d%b%y" ), 
        "' and trading_entity = 'CPS'", sep="" )

rmsysData = sqlQuery(chan, rmsysQuery)

rmsysFilterData = sqlQuery(chan, "SELECT MAX(exp_date), contract_number, value_date, maturity_date
                FROM rmsys.raft_deal
                WHERE cob_date = '10Oct08'
                AND trading_entity = 'CPS'
                GROUP BY contract_number, value_date, maturity_date")
odbcClose(chan)
names(rmsysFilterData)[1] = "EXP_DATE"
rmsysFilterData = subset(rmsysFilterData, EXP_DATE <= as.POSIXct('2008-10-10') )
rmsysFilterData$DELETE = TRUE
rmsysClean = merge( rmsysData, rmsysFilterData, all.x=TRUE )

rowsToRemove = which( 
        rmsysClean$DELETE == TRUE & 
                rmsysClean$INSTRUMENT == 'FINSWAP' & 
                rmsysClean$DEAL_TYPE %in% c("FFSW", "BSW", "SWINGSWAP") )
if( length(rowsToRemove) != 0 ) rmsysClean = rmsysClean[ -rowsToRemove,]


# put the rmsys data in the same order as the secdb data
rmsysClean = rmsysClean[ ,c("DEAL_NUMBER", "DEAL_TYPE", "SOURCE_SYSTEM", 
                "DEALER", "COUNTERPARTY", "TRADING_ENTITY", "DEALT_DATE", "VALUE_DATE", 
                "MATURITY_DATE", "SETTLEMENT_DATE", "BUY_SELL_FLAG", "COMMODITY", "VOLUME", 
                "PRICE", "NOTIONAL_AMOUNT", "NOTIONAL_AMOUNT_CURRENCY", "MARKET_PRICE",
                "MTM", "MTM_CURRENCY", "BOOK", "CONTRACT_NUMBER")] 
classes = sapply( rmsysClean, data.class )
rmsysClean[ , which( classes == "POSIXt" ) ] = 
        lapply( rmsysClean[ , which( classes == "POSIXt" ) ], 
                function(x) { as.Date(x) } )
rmsysClean[ , which( classes == "factor" ) ] = 
        lapply( rmsysClean[ , which( classes == "factor" ) ], 
                function(x) { as.character(x) } )


downstreamData = subset( rmsysClean, BOOK %in% dsBooks )
#downstreamData = subset( downstreamData, VALUE_DATE >= as.Date("2008-10-10") )
dim( downstreamData )
sum( downstreamData$MTM, na.rm = TRUE )

downstreamData$EXPOSURE_FLAG = "External"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$COUNTERPARTY %in% dsBooks )] = "Houston - Downstream"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$COUNTERPARTY %in% baltimoreBooks )] = "Baltimore"
downstreamData$EXPOSURE_FLAG[ which( downstreamData$COUNTERPARTY %in% cneBooks )] = "CNE"
downstreamData$EXPOSURE_FLAG[ grep("^XG", downstreamData$COUNTERPARTY ) ] = "Houston - Storage"
downstreamData$EXPOSURE_FLAG[ grep("^XM", downstreamData$COUNTERPARTY ) ] = "Houston - Transport"


downstreamData = downstreamData[, sort(names(downstreamData)) ]
write.csv( downstreamData, row.names=FALSE, file="C:/Documents and Settings/e14600/Desktop/downstreamData.csv" )


names(downstreamData)[which(names(downstreamData) == "MTM")] = "value"

finalData = cast( downstreamData, 
        COUNTERPARTY ~ EXPOSURE_FLAG, 
        sum, na.rm = TRUE, fill = 0, margins = c("grand_col")  )

write.csv( finalData, row.names=FALSE, file="C:/Documents and Settings/e14600/Desktop/exposureByFlagData.csv" )

