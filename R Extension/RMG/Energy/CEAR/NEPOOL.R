# Customization for NEPOOL region
#
#
# Written by Adrian Dragulescu on 31-Aug-2005

get.hist.prices <- function(save, options){

  options$hdata.symbols   <- paste("NEPOOL_SMD_DA_",options$uNodes,"_CongComp", sep="")
  options$hdata.startDate <- as.Date("2003-04-01")
  options$hdata.endDate   <- Sys.Date()
  options$hdata.filename  <- "hdataSecDB.csv"
  hdata <- tsdbRead(options, save)
  return(list(hdata,options))
}


#=====================================================================
get.awarded.prices <- function(){
  
  con    <- odbcConnectExcel(options$xls.settlefile)
  Cleareddata   <- sqlFetch(con, "FTRRSLT")
  odbcCloseAll()
  Cleareddata <- Cleareddata[as.character(Cleareddata$F2) == "Constellation Energy Commoditi",]
  Cleareddata <- Cleareddata[-which(is.na(Cleareddata$F12)),]
  Cleareddata$F10 <- as.character(Cleareddata$F10)
  Cleareddata$F10[which(Cleareddata$F10=="ONPEAK")] <- "PEAK"

  for (index in 1:dim(Position)[1])
  {
       ind.Source <- which( (Cleareddata[,3]==as.numeric(as.character(Position[index,"Source"])))
                          & (as.character(Cleareddata[,10])==as.character(Position[index,"Class"])) )
       ind.Sink   <- which( (Cleareddata[,6]==as.numeric(as.character(Position[index,"Sink"])))
                          & (as.character(Cleareddata[,10])==as.character(Position[index,"Class"])) )
       ind <- intersect(ind.Source,ind.Sink)
       if (length(ind) == 1)
       {
            Position[index,"Cleared.Price"] <- Cleareddata[ind,12]
            Position[index,"Awarded"] <- "YES"
       }
   }
}

#=====================================================================
get.auction.prices <- function(){

HistoricalDA<- options$hdata.symbols
options$hdata.symbols <- paste("NEPL_FTRCP_PK_",uNodes,sep="")
options$hdata.filename <- "NEPL_FTRCP_PK.csv"
PeakFTRC <- tsdbRead.SecDB(options,save)

options$hdata.symbols <- paste("NEPL_FTRCP_OP",uNodes,sep="")
OffPeakFTRC <- tsdbRead.SecDB(options.save)
options$hdata.filename<-"NEPL_FTRCP_OP.csv"

options$hdata.symbols <- HistoricalDA

}
