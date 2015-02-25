# Retrieve and Price Data
# data is a data.frame for Positions, read from an excel file submitted by trader
# hdata is historical price data from SecDB, (tsdb in particular)
#
# Written by Adrian Dragulescu on 23-Mar-2005

main.analysis.CEAR <- function(save, options){
  


#-----------------------------------Read spreadsheet for positions------------
con    <- odbcConnectExcel(options$xls.inputfile)
tables <- sqlTables(con) 
data   <- sqlFetch(con, "RMG")
odbcCloseAll()

#-----------------------------------Prepare Position Data---------------------
options$paths <- paste(data$Bucket, data$Type, data$SourceNodeNum,
                       "to", data$SinkNodeNum, sep=" ")
MW    <- tapply(data$MW, options$paths, sum)
aux   <- unlist(strsplit(names(MW)," "))

Position <- data.frame(MW = MW, Bucket = toupper(aux[seq(1,length(aux), 5)]),
     Type   = aux[seq(2,length(aux), 5)],
     Source = aux[seq(3,length(aux), 5)],
     Sink   = aux[seq(5,length(aux), 5)])

Position$SourceName <- as.character(data$SourceNodeName[
     pmatch(as.character(Position$Source),as.character(data$SourceNodeNumber))])
Position$SinkName   <- as.character(data$SinkNodeName[
     pmatch(as.character(Position$Sink),as.character(data$SinkNodeNumber))])
Position$Cleared.Price  <- 0
Position$Realized.Price <- 0
Position$Awarded <- "NO"

options$uPaths <- rownames(Position)
rownames(Position) <- 1:nrow(Position)

#----------------------------------------------Read historical price data
options$uNodes <- unique(c(data$SourceNodeNum, data$SinkNodeNum))
aux   <- get.hist.prices(save, options)
hdata <- aux[[1]];  options <- aux[[2]]

#----------------------------------------------Calculate historical payoff
source(paste(options$source.dir, "historical.path.Payoff.R", sep=""))
hist.prices <- historical.path.Payoff(hdata, data, Position, options)


sim.prices  <- list()


# #---------------------------------------Simulate prices--------------------
# fhours <- seq(options$fdata$startDate, options$fdata$endDate, by="hour")
# fdata  <- cbind(year  = as.numeric(format(fhours, "%Y")),
#                 month = as.numeric(format(fhours, "%m")),
#                 day   = as.numeric(format(fhours, "%d")),
#                 hour  = as.numeric(format(fhours, "%H")))
# fdata <- as.data.frame(fdata)
# fdata$date <- as.Date(paste(fdata$year, fdata$month, fdata$day, sep="-"))
# fdata$season <- calculate.season(fdata[,c("year", "month")], options)$season

# for (c in buckets){
#   ind <- grep(c, uPaths)
#   if (length(ind)==0){next}
#   ind.Nodes <- unique(as.character(c(Position[ind,"Source"], Position[ind, "Sink"])))
#   ind <- which(uNodes %in% as.numeric(ind.Nodes))

#   MM  <- hdata[,c(1:4,4+ind)]
#   fdata.class <- fdata
#   if (c=="OFFPEAK"){
#     MM  <- select.offpeak(MM); fdata.class <- select.offpeak(fdata)}
#   if (c=="PEAK"){
#     MM  <- select.onpeak(MM); fdata.class <- select.onpeak(fdata)}
#   MM  <- cbind(MM, date=as.Date(paste(MM$year, MM$month, MM$day, sep="-")),
#        season=calculate.season(MM[,c("year", "month")], options)$season)

#   options$noZones <- length(ind)
#   options$fdata$noDays    <- nrow(fdata.class)
#   options$zone.names      <- colnames(hdata)[4+ind]
#   sim.prices[[c]] <- simulate.spreads(MM, fdata.class, save, options)
# }

return(list(Position, sim.prices, hdata, save, options))
}










