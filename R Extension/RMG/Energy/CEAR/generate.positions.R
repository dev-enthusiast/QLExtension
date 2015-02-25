# Given a set of high level paths, generate the QQ file
#
#
#

months <- seq(as.Date("2008-05-01"), by="month", length.out=6) 
paths  <- matrix(NA, nrow=1, ncol=3)
colnames(paths) <- c("source", "sink", "quantity")

#paths[1,] <- c("West", "Cen",  "500")
#paths[1,] <- c("DunW", "NYC", "200")
paths[1,] <- c("HudV", "NYC",  "150")
#paths[1,] <- c("HudV", "DunW", "200")

root    <- "COMMOD PWY"
buckets <- c("5x16", "7x8", "2x16H")
hours   <- c(320, 240, 144)  # per month

data <- NULL
for (ind.b in 1:length(buckets)){
  for (ind.p in 1:nrow(paths)){
    pSource  <- paste(root, buckets[ind.b], paths[ind.p, "source"], "physical")
    qqSource <- data.frame(curve.name=toupper(pSource), contract.dt=months,
      vol.type="NA", position=-as.numeric(paths[ind.p, "quantity"])*hours[ind.b])
    pSink    <- paste(root, buckets[ind.b], paths[ind.p, "sink"],   "physical")
    qqSink   <- data.frame(curve.name=toupper(pSink), contract.dt=months,
      vol.type="NA", position=as.numeric(paths[ind.p, "quantity"])*hours[ind.b])
   
    data <- rbind(data, rbind(qqSource, qqSink))
  }
}



#---------------------------------------------------------------------------
months <- seq(as.Date("2008-04-01"), by="month", length.out=21) 
paths  <- matrix(NA, nrow=1, ncol=3)
colnames(paths) <- c("source", "sink", "quantity")

paths[1,] <- c("", "Maine ",  "300")

root    <- "COMMOD PWX "
buckets <- c("5x16 ", "7x8 ", "2x16 ")
hours   <- c(320, 240, 144)  # per month

data2 <- NULL
for (ind.b in 1:length(buckets)){
  for (ind.p in 1:nrow(paths)){
    pSource  <- paste(root, buckets[ind.b], paths[ind.p, "source"], "physical", sep="")
    qqSource <- data.frame(curve.name=toupper(pSource), contract.dt=months,
      vol.type="NA", position=-as.numeric(paths[ind.p, "quantity"])*hours[ind.b])
    pSink    <- paste(root, buckets[ind.b], paths[ind.p, "sink"],   "physical", sep="")
    qqSink   <- data.frame(curve.name=toupper(pSink), contract.dt=months,
      vol.type="NA", position=as.numeric(paths[ind.p, "quantity"])*hours[ind.b])
   
    data2 <- rbind(data2, rbind(qqSource, qqSink))
  }
}


data <- rbind(data, data2)


#=============================================================================
#     READ DATA FROM A SPREADSHEET
#
require(RODBC)
filename <- "H:/JBMay08.xls"
con    <- odbcConnectExcel(filename)
tables <- sqlTables(con) 
data   <- sqlFetch(con, "VAR Calc")
odbcCloseAll()

#ind <- which(is.na(data[,"Source Commod"]))
data <- data[!is.na(data[,"Source Commod"]),]
data <- data[, c("Source Commod", "Sink Comod", "Max MWs")]
data = data[-12,]
names(data) = c("Source Commod", "Sink Comod", "MW")
DD <- rbind(cbind(as.character(data[,"Source Commod"]), -data[, "MW"]),
            cbind(as.character(data[,"Sink Comod"]),     data[, "MW"]))
DD <- data.frame(DD)
names(DD) <- c("curve.name", "position")
DD$curve.name <- toupper(as.character(DD$curve.name))
DD$position   <- as.numeric(as.character(DD$position) )
DD <- aggregate(DD$position, list(DD$curve.name), sum)
names(DD) <- c("curve.name", "position")

DD$mult  <- NA
ind.peak <- grep("5X16", DD$curve.name)
DD$mult[ind.peak]  <- 336
ind.2x16 <- grep("2X16", DD$curve.name)
DD$mult[ind.2x16]  <- 144
ind.7x8 <- grep("7X8", DD$curve.name)
DD$mult[ind.7x8]  <- 248
ind.off <- grep("OFFPEAK", DD$curve.name)
DD$mult[ind.off]  <- 248 + 144

DD <- subset(DD, position != 0)
DD$position <- DD$position * DD$mult

#DD$curve.name <- gsub("PWX 2X16 INDEX", "PWX 2X16H INDEX", DD$curve.name)
DD$mult <- NULL
DDD <- expand.grid(curve.name = DD$curve.name,
  contract.dt=seq(as.Date("2008-05-01"), length.out=1, by="month"))
DDD$curve.name <- toupper(as.character(DDD$curve.name))
DDD <- merge(DDD, DD, all=T)
DDD$vol.type <- "NA"
data <- DDD


#######################################################################3
require(RODBC)
filename <- "H:/user/R/RMG/Energy/CEAR/jaya.nypp.cal08.xls"
con    <- odbcConnectExcel(filename)
tables <- sqlTables(con) 
data   <- sqlFetch(con, "Sheet1")
odbcCloseAll()
names(data) <- gsub("#", ".", names(data))
data$curve.name <- as.character(toupper(data$curve.name))
data$vol.type <- "NA"
data$contract.dt <- as.Date(data$contract.dt)


