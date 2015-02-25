#
#
#


FTR.options <- NULL
FTR.options$region <- "NEPOOL"
FTR.options$books <- c("JBACCFTR", "JBACCTCC", "JBFTR",
                       "JBMTMFTR", "JBMTMTCC", "NYTCCTMP")
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.congestion.R")



asOfDate  <- as.Date("2008-03-14")
portfolio <- "EAST PM CONGESTION TRD PORT"


filepath <- "S:/All/Structured Risk/NEPOOL/FTRs/Analysis/2008-05/CCG Stat Bids May08.csv"
QQ <- FTR.makePositionsForVaR(filepath, NEPOOL)

nodeNames <- c("UN.HARRIS  115 HAR4", "UN.HARRIS  115 HAR2")
#nodeNames <- c(".Z.MAINE")
commods   <- FTR.nodeToCommod(nodeNames, NEPOOL$MAP, NEPOOL$region)






source("H:/user/R/RMG/Energy/Trading/Congestion/lib.congestion.R")







