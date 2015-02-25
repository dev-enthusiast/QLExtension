# Calculate stress tests using linear approximation
#
#
# Written by Adrian Dragulescu on 14-Feb-2007

rm(list=ls())
require(RODBC); require(reshape)
options <- NULL
options$asOfDate <- as.Date("2007-01-31")

#-------------------- Load the positions ------------------------------------
file <- "S:/Risk/2007,Q1/Risk.Analytics/Stress.Tests/positions_31JAN2007.csv"
QQ <- read.csv(file)   # positions file
QQ$ContractMonth <- as.Date(paste(gsub("MONTH", "-", QQ$ContractMonth),
                                  "-01",sep=""))
names(QQ) <- c("counterparty", "contract.dt", "curve.name", "delta", "vega")
QQ$curve.name <- as.character(QQ$curve.name)

#-------------------- Load the prices ---------------------------------------
file <- paste("S:/All/Risk/Data/VaR/hPrices.",options$asOfDate,".RData",sep="")
load(file)
PP <- hP; hP <- NULL
names(PP)[3] <- "Price"
PP$Year <- as.numeric(format(PP$contract.dt, "%Y")) -
           as.numeric(format(options$asOfDate, "%Y"))
PP$Commod <- ""
ind <- grep(" PW", PP$curve.name); PP$Commod[ind] <- "Power"
ind <- grep(" NG", PP$curve.name); PP$Commod[ind] <- "Gas"

#--------------------- Load the specification file --------------------------
file <- "H:/user/R/RMG/Energy/Stress.Tests/specify.stress.tests.xls"
con  <- odbcConnectExcel(file) 
ST   <- sqlFetch(con, "Sheet1")
odbcCloseAll()
names(ST)[1] <- "Stress.Name"
ST$Stress.Name <- as.character(ST$Stress.Name)

#QQ <- subset(QQ, counterparty=="AFGASIBT")
source("H:/user/R/RMG/Energy/Stress.Tests/expand.stress.test.R")
res <- NULL
uScenarios <- unique(ST$Stress.Name)
for (s in 1:length(uScenarios)){
  scenario <- subset(ST, Stress.Name == uScenarios[s])
  sPP <- expand.stress.test(PP, scenario)
  res <- rbind(res, value.change(sPP, QQ, uScenarios[s]))
}

write.csv(res, file="S:/Risk/2007,Q1/Risk.Analytics/Stress.Tests/results.csv",
          row.names=F)




