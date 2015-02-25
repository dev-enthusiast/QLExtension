# Utility to move the retail spreads from PM East Retail Portfolio to the
# congestion portfolio.
#
# 
# S:\All\Structured Risk\CNE\PMEast_Retail_Template.xls
#
#
# Written by AAD on 17-Jul-2009


require(CEGbase); require(SecDb); require(PM); require(reshape)

source("H:/user/R/RMG/Energy/Trading/Congestion/lib.book.trade.R")

asOfDate <- as.Date(secdb.dateRuleApply(Sys.Date(), "-1b"))
dirIn <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/_Positions/"
fileName <- paste(dirIn, "positionsAAD_", as.character(asOfDate), ".RData", sep="")
load(fileName)
rLog("Loaded", fileName)  # loads variable QQ


QQ <- subset(QQ, portfolio=="NEPOOL FP PORTFOLIO")
QQ <- subset(QQ, mkt == "PWX")  # only NEPOOL 
## QQQ <- QQ[grepl("CT ", QQ$delivery.point),]
## QQQ <- subset(QQQ, delta>0)
## QQQ <- subset(QQ, eti=="PZSDEUH")

#QQ <- subset(QQ, trade.date == asOfDate)    # <--- trades from yesterday!!!
QQ <- QQ[,c("eti", "curve", "contract.dt", "delivery.point",
  "delta", "bucket", "quantity.uom")]
QQ$year <- format(QQ$contract.dt, "%Y")


sort(unique(QQ$delivery.point))

#aggQQ <- .find_nodes_with_positions(QQ)

#deliveryPt <- "BUCKSPRT.4140 DA"   # Z11
#deliveryPt <- "FLAGG_PD.PINE DA"   #
#deliveryPt <- "GRAHAM.MIS DA"      #
#deliveryPt <- "SEMA DA"
# MAINE, MAINE-SC, MAINE DA
deliveryPt <- NULL

etis <- unique(subset(QQ, delivery.point=="CT DA")$eti)

cast(QQ,  year ~ ., sum, fill=NA, value="delta",
  subset=delivery.point %in% c("CT DA", "CT"))

cast(QQ,  contract.dt ~ bucket, sum, fill=NA, value="delta",
  subset=delivery.point %in% c("SEMA DA", "SEMA"))

cast(QQ,  eti ~ year, sum, fill=NA, value="delta",
  subset=delivery.point=="CT DA")

res <- cast(QQ, contract.dt ~ eti, sum, fill=NA, value="delta",
  subset=delivery.point=="CT DA")

#etis <- c("PZSDEV4", "PZSDEUH")  # CL&P Cal11 slice + Kleen
#etis <- c("PZSDNCO", "PZSDNDC")  # RI slice
#etis <- c("PZSDVBW")             # on 2010-03-10 we won MECO WMA
#etis <- c("PZSDVBT", "PZSDVBU")  # on 2010-03-10 we won MECO NH
#etis <- c("PZSDVBX", "PZSDVBY",
#          "PZSDVBZ", "PZSDVC0")    # on 2010-03-10 we won MECO SEMA
#etis <- c("PZSEEA2", "PZSEE9W")  # on 2010-05-04 we won CL&P standard 11-13,
#                                 # -0.25 hedge costs peak on one piece, 12, 13
#etis <- c("PZSEBM3")  # gave -0.40 hedge costs onpeak only, 12 and 13
#etis <- c("PZSEBM3", "PZSEEA2", "PZSEE9W")
#etis <- c("PZSEDPH")
#etis <- "PZSEMTR"
#etis <- "PZSF0GX"        # about 125 MW CT load
#etis <- "PZSFE31"
#etis <- "PZSFEMV"  #  20% CMP load Sep10-Feb11  7 MW Maine
#etis <- "PZSFENK"   # 100% UI SOLR load Oct10-Dec10, 
#etis <- "PZSFIGM"    #  25% NSTAR NEMA Ind Load for Oct10-Dec10.
#etis <- "PZSFOGI"    #  50% RI Large Load for Oct10-Dec10, about 110 MW peak!
#etis <- c("PZSFYIN", "PZSFYIO")   # MECO Sema residential K11-V11, about 200 MW
#etis <- "PZSFYIM"    # MECO Wma residential K11-V11, about 240 MW
# etis <- "PZSFYIJ"     # MECO NEMA residential K11-V11, about 176 MW
etis <- "PAS1RSM"     # CL&P load 12,13 block, about 100 MW

#etis <- NULL
deliveryPt <- c("CT DA", "CT")
qq <- .filter_positions_move(QQ, asOfDate, deliveryPt=deliveryPt, etis=etis)
qq <- subset(qq, month >= as.Date("2011-04-01") & month <= as.Date("2013-12-01"))


startDate <- qq$month[1] 
endDate   <- qq$month[nrow(qq)]
  
deliveryPt <- "CT DA"
PP <- .get_prices(asOfDate, qq)

# adjust prices for hedge costs... 
adjPP <- PP
#adjPP[, 2:4] <- adjPP[, 2:4] + 0.25         # positive hedge costs
ind <- which(format(adjPP$month, "%Y")=="2012")
adjPP[ind, 2:4] <- adjPP[ind, 2:4] + 0.10 
## ind <- which(format(adjPP$month, "%Y")=="2013")
## adjPP[ind, 2] <- adjPP[ind, 2] - 0.25

# customize the calc -- YOU NEED TO EDIT/SAVE the delivery point in the calc
.customize_calc_2x3(qq, adjPP)   # 2 commods, 3 buckets

print(qq, row.names=FALSE)   # paste this in the xlsx for reference


## REMEMBER!  CONGIBT BUYS NECIBT SELLS!






## aux1 <- subset(QQ, delivery.point=="MAINE" & contract.dt==as.Date("2010-08-01"))
## aux2 <- subset(QQ, delivery.point=="MAINE DA" & contract.dt==as.Date("2010-08-01"))
## aux3 <- subset(QQ, delivery.point=="MAINE-SC" & contract.dt==as.Date("2010-08-01"))

## # just to CT to start
## ind <- grep("^CT DA", colnames(QQQ))
## qq  <- QQQ[, c(1,ind)]
## pp  <- PP[, c(1,ind)]

## source("H:/user/R/RMG/Utilities/Interfaces/PM/R/secdb.customizeCalculator.R")


## print(qq, row.names=FALSE)

## # customize the hub calc now ... 
## qqHub <- QQQ[, 1:4]; qqHub[,2:4] <- -qq[,2:4]
## ppHub <- PP[, 1:4]
## secdb.customizeCalculator(fullCalcName, quantity=qqHub, fixPrice=ppHub)
















