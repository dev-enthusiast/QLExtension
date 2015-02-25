# CTR units are lots
# 
#


source("H:/user/R/RMG/Finance/BlackScholes/blackscholes.R")
require(RODBC)
options <- NULL
options$asOfDate <- as.Date("2006-05-19")
options$metacalc.id <- 456   # rmsys valuation as of 19-May-2006
#options$deal.id <- c(395423, 414186, 438781)    # rmsys dealid
options$con <- odbcConnect("RMSYSU", uid="RMSYS", pwd="RMSYS") 

deal.ids <- paste("'", options$deal.id, "'", sep="", collapse=", ")
query <- paste("select deal_id, leg_id, version, instrument_type_id,",
  "buy_sell_flag, call_put_flag, start_date, end_date, freq ",
  "from deal_details where instrument_type_id=4")
ddetails  <- sqlQuery(options$con, query)

query <- paste("select vrp.cob, vrp.val_id, vrp.deal_id, vrp.deal_version,",
  "vrp.leg_id, vrp.vrp.side, vrp.contract_date, vrp.price, vrp.strike, ",
  "vrp.fxrate, vrp.vol, vrp. ir, vrp.corr, vrp.notional, vrp.mtm, vrp.exp_date, ",
  "vrp.delta, vrp.gamma, vrp.theta, vrp.vega from val_results_position vrp, ",
  "calc_kickoff ck, deals d where ck.METACALC_ID = ", options$metacalc.id,
  " and vrp.VAL_ID = ck.CALC_ID and vrp.DEAL_ID = d.DEAL_ID and ",
  "vrp.DEAL_ID in (select deal_id from deal_details where ",
  "INSTRUMENT_TYPE_ID = 4)", sep="")
vrp<- sqlQuery(options$con, query)
vrp[1:3,]   # you include the option premiums too. 

open.deals <- unique(vrp$DEAL_ID)  
ddetails   <- ddetails[which(ddetails$DEAL_ID %in% open.deals),]

MM <- merge(vrp, ddetails, all=T)
MM <- subset(MM, INSTRUMENT_TYPE_ID==4) # final list of all exchange options!

# select only the deal.ids that you requested
if (length(options$deal.id)>0){
  MM <- subset(MM, DEAL_ID %in% options$deal.id)
}

levels(MM$CALL_PUT_FLAG) <- c("CALL", "PUT")
MM$DF <- exp(-MM$IR*as.numeric(MM$EXP_DATE-MM$COB)/365)

data <- data.frame(S = MM$PRICE, sigma = MM$VOL,
   Tex = as.numeric(MM$EXP_DATE-MM$COB)/365, K = MM$STRIKE, r = 0,
   type = MM$CALL_PUT_FLAG)
val <- blackscholes(data)$value  
MM$RMG.MTM <- val*MM$DF*MM$NOTIONAL

res <- MM[,c("DEAL_ID", "LEG_ID", "CONTRACT_DATE", "MTM", "RMG.MTM")]
res$rel.diff <- (res$RMG.MTM-res$MTM)/res$MTM
res$abs.diff <- (res$RMG.MTM-res$MTM)

plot(sort(res$rel.diff))

plot(sort(res$abs.diff))

sum(res$abs.diff)/sum(res$MTM)



#query <- "select * from val_results_position vrp, calc_kickoff ck where vrp.val_id = ck.calc_id and vrp.deal_id  = 395423 and ck.METACALC_ID = 220 order by vrp.LEG_ID"

require(RQuantLib)
Tex <- as.numeric(vrp$EXP_DATE-vrp$COB)/365
AmericanOption("put", vrp$PRICE, vrp$STRIKE, 0, 0.0000001, Tex, vrp$VOL)

require(fOptions)
res <- BAWAmericanApproxOption("p", S=vrp$PRICE, X=vrp$STRIKE, Tex,
                        r=vrp$IR, b=0, vrp$VOL)
attributes(res)$price

res <- BSAmericanApproxOption("p", S=vrp$PRICE, X=vrp$STRIKE, Tex,
                        r=0, b=0, vrp$VOL)
attributes(res)$price*exp(-vrp$IR*Tex)


