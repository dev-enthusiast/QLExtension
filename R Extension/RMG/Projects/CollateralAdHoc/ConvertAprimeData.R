memory.limit( 4095 )
require( reshape )
require( RODBC )

source("H:/user/R/RMG/Projects/CollateralAdHoc/StressFramework.R")

asOfDate = .getAsOfDate()
asOfDate = as.Date("2009-01-02")


aprimeFile =  paste( "S:/Risk/Temporary/CollateralAllocation/",
        format( asOfDate, "%Y%m%d"), "/SourceData/AllPos_AGMTHScale_preibt_",
        format( asOfDate, "%d%b%y"), ".csv", sep="" )
aprimeData = read.csv( aprimeFile )


megaData = .getMegaData( asOfDate )
marginableAgreements = sort( megaData$"Netting Agreement"[which(megaData$CSA != "")] )
nymexAgreements = megaData$"Netting Agreement"[which(megaData$ShortName=="NYMEX")]
marginableAgreements = sort( c( as.character(marginableAgreements), 
        as.character( nymexAgreements ) ) )


aprime09 = subset( aprimeData, ContractYear=="2009" )
aprime09 = aprime09[,sort(names(aprime09))]


test = subset( aprime09, credit_nettingagreement %in% 
                marginableAgreements )
aggregate(test$pos_adj, list(test$commodity_grp), 
        function(x){round( sum(x, na.rm=TRUE ) ) } )


test3 = aprime09[which(aprime09$Marginal != "UnMargin"),]
aggregate(test3$pos_adj, list(test3$commodity_grp), 
        function(x){round( sum(x, na.rm=TRUE ) ) } )

aprimesMarginableAgreements = sort(unique(subset( aprimeData, Marginal == "Marginal")$credit_nettingagreement))
aprimeMarginableDifferences = setdiff( aprimesMarginableAgreements, marginableAgreements )
print( aprimeMarginableDifferences )

raftMarginableDifferences = setdiff( marginableAgreements, aprimesMarginableAgreements )
print( raftMarginableDifferences )

unique(aprimeData$counterparty[which(aprimeData$credit_nettingagreement %in% aprimeMarginableDifferences[-1])])

aprimesLCHeldAgreements = sort(unique(subset( aprimeData, Marginal == "LCHeld")$credit_nettingagreement))
setdiff( aprimesLCHeldAgreements, marginableAgreements )


aprimeNonMargined = c(aprimesLCHeldAgreements, aprimesMarginableAgreements)
setdiff( aprimeNonMargined, marginableAgreements)

setdiff( marginableAgreements, aprimeNonMargined)


aprimeMap = unique(aprimeData[,c("credit_nettingagreement", "Marginal")])
aprimeMap = aprimeMap[do.call(order, aprimeMap),]

raftMap = unique( megaData[,c("Netting Agreement", "CSA") ])
raftMap$Marginal = ifelse( raftMap$CSA=="", "UnMargin", "Marginal" )
raftMap$CSA = NULL
names( aprimeMap ) = names( raftMap ) = c("netting_agreement", "marginal")

finalMap = merge( raftMap, aprimeMap, by="netting_agreement", suffix=c(".raft", ".aprime"), all=TRUE )