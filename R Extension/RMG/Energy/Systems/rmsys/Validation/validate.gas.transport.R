# Look at one transportation deal.id from rmsys 
#
# Written by Adrian Dragulescu on 4-Dec-2006


require(RODBC)
options <- NULL
options$asOfDate <- as.Date("2006-03-16")
options$metacalc.id <- 1114   # rmsys valuation
options$deal.id <- 662376    # rmsys dealid
options$con <- odbcConnect("RMSYSP", uid="rmsys_ops", pwd="rmsys_ops") 

query <- paste("select * from deal_details where deal_id =", options$deal.id)
deal  <- sqlQuery(options$con, query)
demand <- deal[which(deal$INSTRUMENT_TYPE_ID==7), ] # demand payments
deal   <- deal[which(deal$INSTRUMENT_TYPE_ID==8), ] # transport spread option

#--------------------------------------------- Get valuation info ----
query <- paste("select * from val_results_position vrp, calc_kickoff ck",
  " where vrp.val_id = ck.calc_id and vrp.deal_id=", options$deal.id,
  " and ck.METACALC_ID=", options$metacalc.id, " order by vrp.LEG_ID", sep="")
vrp <- sqlQuery(options$con, query)

vrp <- vrp[order(vrp[,"CONTRACT_DATE"]), ]
uContracts  <- sort(unique(vrp[,"CONTRACT_DATE"]))
MM    <- data.frame(contract.dt=uContracts, F1=NA, F2=NA, V1=NA, V2=NA,
                    Rho=NA, K=NA, Tx=NA, DF=NA)
aux    <- subset(vrp, PRICE_INDEX!=1)
MM$F1  <- aux[which(aux$SIDE==1),"PRICE"]   # long side price
MM$F2  <- aux[which(aux$SIDE==2),"PRICE"]   # short side price
MM$V1  <- aux[which(aux$SIDE==1),"VOL"]     # long side volatility
MM$V2  <- aux[which(aux$SIDE==2),"VOL"]     # short side volatility
MM$Rho <- aux[which(aux$SIDE==1),"CORR"]    # correlation
MM$K   <- aux[which(aux$SIDE==1),"STRIKE"]  # strike
MM$Tx  <- as.numeric((as.Date(MM$contract.dt)-options$asOfDate)/365.25)
MM$DF  <- exp(-MM$Tx*aux[which(aux$SIDE==1),"IR"])    # discount factor
MM$Q   <- aux[which(aux$SIDE==1),"NOTIONAL"]   # notional
fuel.charge <- merge(MM, deal, by.x="contract.dt", by.y="START_DATE",
   all=F, all.x=T)$FUEL2   # get the fuel charge for the relevant months
MM$HR  <- 1/(1-fuel.charge)   # correct for fuel charge 

options$method <- "kirk"
source("H:/user/R/RMG/Finance/BlackScholes/spread.option.R")
unit.val <- spread.option(MM, options)$value

source("H:/user/R/RMG/Finance/BlackScholes/xls.spread.option.R")
unit.val <- xls.spread.option(MM$F1, MM$F2, MM$V1, MM$V2, MM$Rho, MM$K,
                          MM$Tx, MM$DF, MM$HR, "value")

val <- data.frame(rmg.val=array(NA, dim=nrow(MM)), rmsys.val=NA)
val$rmsys.val <- 2*aux[which(aux$SIDE==1),"MTM"]  # there are two sides...
val$rmg.val   <- MM$Q*unit.val$value
val$abs.diff.val  <- val$rmg.val - val$rmsys.val
val$rel.diff.val  <- (val$rmg.val - val$rmsys.val)/val$rmsys.val

plot(range(MM$contract.dt), range(c(val$rmsys.val,val$rmg.val)), type="n",
     ylab="Value, $")
points(MM$contract.dt, val$rmsys.val, col="red", pch=1)
points(MM$contract.dt, val$rmg.val, col="blue", pch=2)
legend(x="topright", legend=c("RMSYS", "RMG"), pch=c(1,2),
       col=c("red", "blue"), bty="n")

odbcClose(options$con)   





##############################################################################
##############################################################################






#---------------------------------------------- Get the prices ----
uPriceIndex <- unique(deal[,"PRICE_INDEX"])
uContracts  <- sort(as.Date(unique(deal[,"CONTRACT_DATE"])))
aux <- paste(uPriceIndex, sep="", collapse="', '")
query <- paste("select index_num, from_date, to_date, price from price_points where EFFECTIVE_DATE=TO_DATE('", options$asOfDate, "', 'YYYY-MM-DD') and INDEX_NUM in ('", aux, "')", sep="")
fwds  <- sqlQuery(options$con, query)
