# Retrieve Positions from Traders' spreadsheet
# Retrieve historical FTR auction clearing prices from tsdb
# Retrieve historical DA LMP prices from tsdb
#
# Return a data.frame for position with weighted adjusted quantity
# Return a data.frame for historical FTR auction clearing prices
# Return a data.frame for histrocial DA historal prices

position.history<- function(options,save)

{

   require(RODBC)
#-----------------------------------Read spreadsheet for positions------------
  con    <- odbcConnectExcel(options$xls.inputfile)
  tables <- sqlTables(con)
  data   <- sqlFetch(con, "RMG")
  odbcCloseAll()

  if (length(options$Region) == 0 ) { options$Region <- "NEPOOL" }
  if (options$Region == "NEPOOL")
  {
     data$SourceID <- data$SourceNodeNum
     data$SinkID <-   data$SinkNodeNum
  }

#-----------------------------------Prepare Position Data---------------------

# Retrieve Annual data


  paths <- paste(data$Class,data$Type,data$SourceID,"to",data$SinkID,sep=" ")

  MW    <- tapply(data$MW, paths, sum)
  aux   <- unlist(strsplit(names(MW)," "))

  Position <- data.frame(MW = MW, Class = aux[seq(1,length(aux), 5)],
       Type   = aux[seq(2,length(aux), 5)],
       Source = aux[seq(3,length(aux), 5)],
       Sink   = aux[seq(5,length(aux), 5)])

}
