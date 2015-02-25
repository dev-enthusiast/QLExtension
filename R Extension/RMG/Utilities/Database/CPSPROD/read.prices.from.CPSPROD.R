# Read commod prices from CPSTEST database, where
# options=NULL
# options$pricing.dt.start  <- as.Date("2006-08-14")
# options$pricing.dt.end    <- as.Date("2006-08-21")   #optional
# options$contract.dt.start <- as.Date("2007-01-01")   #optional
# options$contract.dt.end   <- as.Date("2007-12-01")   #optional
# options$curve.names <- c("COMMOD NG Exchange",
#                          "COMMOD PWJ 5x16 WEST Physical")
# options$plot <- 1       # plot the results
# options$type <- "raw"   # or "corrected"
# options$file.dsn.path <-  "H:/user/R/RMG/Utilities/DSN/"   #MZ 03/06/2007
# options$file.dsn <- "CPSPROD.dsn"    #MZ 03/06/2007
#
# source("H:/user/R/RMG/Utilities/Database/CPSPROD/read.prices.from.CPSPROD.R")
# res <- read.prices.from.CPSPROD(options)
#
# Written by Adrian Dragulescu on 10-Jan-2006
#
# 03/06/2007  MZ
# Changed to use file dsn to connect CPSPROD instead of system or user dsn

read.prices.from.CPSPROD <- function(options){

  if (length(options$plot)==0){options$plot <- 0}
  if (length(options$type)==0){options$type <- "raw"}
  if (length(options$pricing.dt.start)==0){
    options$pricing.dt.start <- Sys.Date()-1}
  if (length(options$pricing.dt.end)==0){
    options$pricing.dt.end <- options$pricing.dt.start}
  if (length(options$contract.dt.start)==0){
    options$contract.dt.start <- as.Date(paste(format(options$pricing.dt.start,
        "%Y-%m"), "-01", sep=""))}
  if (length(options$contract.dt.end)==0){
    options$contract.dt.end <- as.Date("2029-12-01")}
  if (length(options$file.dsn)==0){options$file.dsn <- "CPSPROD.dsn"}
  if (length(options$file.dsn.path)==0){options$file.dsn.path <- "H:/user/R/RMG/Utilities/DSN/"}
  
  #---------------------------------------READ DATA IN---------------------
  require(RODBC); require(reshape)
  
  cat("Pulling CCG Forward prices ... ")
  query <- paste("SELECT PRICING_DATE, COMMOD_CURVE, START_DATE, PRICE, UPDATED_TIME ",
    "FROM FOITSOX.SDB_PRICES WHERE PRICING_DATE between TO_DATE('",
    format(options$pricing.dt.start, "%m/%d/%Y"), "','MM/DD/YYYY') and ",
    "TO_DATE('",format(options$pricing.dt.end, "%m/%d/%Y"), "','MM/DD/YYYY') ",
    "and START_DATE between TO_DATE('", format(options$contract.dt.start,
    "%m/%d/%Y"), "','MM/DD/YYYY') and TO_DATE('", format(options$contract.dt.end,
    "%m/%d/%Y"), "','MM/DD/YYYY')", sep="")
  if (length(options$curve.names)>0){
    options$curve.names <- toupper(options$curve.names)
    curves <- paste(options$curve.names, sep="", collapse="', '")
    query  <- paste(query," AND UPPER(COMMOD_CURVE) in ('", curves, "')", sep="")
  }
  
   # Use file dsn to connect to CPSPROD. MZ 03/06/2007
  connection.string <- paste("FileDSN=", options$file.dsn.path, options$file.dsn, 
     ";UID=stratdatread;PWD=stratdatread;", sep="")  
  con <- odbcDriverConnect(connection.string)  
  
  # con  <- odbcConnect("CPSPROD", uid="stratdatread", pwd="stratdatread")
  data <- sqlQuery(con, query)
  data[1:10,] 
       
  if ( options$type == "corrected" ) {
    queryCorrected <- paste("SELECT PRICING_DATE, COMMOD_CURVE, START_DATE, PRICE, UPDATED_TIME  ",
    "FROM FOITSOX.SDB_PRICE_UPDATES WHERE PRICING_DATE between TO_DATE('",
    format(options$pricing.dt.start, "%m/%d/%Y"), "','MM/DD/YYYY') and ",
    "TO_DATE('",format(options$pricing.dt.end, "%m/%d/%Y"), "','MM/DD/YYYY') ",
    "and START_DATE between TO_DATE('", format(options$contract.dt.start,
    "%m/%d/%Y"), "','MM/DD/YYYY') and TO_DATE('", format(options$contract.dt.end,
    "%m/%d/%Y"), "','MM/DD/YYYY')", sep="")
    if (length(options$curve.names)>0){
      queryCorrected  <- paste(queryCorrected," AND UPPER(COMMOD_CURVE) in ('", curves, "')", sep="")
    }                           
    dataCorrected <- sqlQuery(con, queryCorrected)
    dataCorrected[1:10,]
    if (dim(dataCorrected)[1] > 0){data <- rbind(data, dataCorrected)}
  }                                  
  odbcCloseAll()
  colnames(data) <- c("PRICING_DATE", "COMMOD_CURVE", "START_DATE", "PRICE",
                     "value")
  data$value <- as.numeric(data$value)

  MM <- cast(data, PRICING_DATE + COMMOD_CURVE + START_DATE ~ . , max)
  colnames(MM)[4] <- "value"
  res <- merge(data, MM, all.y=T)
  data <- res[,-which(colnames(res) == "value")]  # remove the update time

  data[, "PRICING_DATE"] <- as.Date(data[, "PRICING_DATE"])
  data[, "START_DATE"]   <- as.Date(data[, "START_DATE"])
  #data[, "COMMOD_CURVE"] <- as.character(data[, "COMMOD_CURVE"])
  #data[, "PRICE"] <- as.numeric(data[, "PRICE"])
  
  #--------------------------------------------------------------------------------
  if (options$plot==1){
    M3D <- tapply(data$PRICE, list(as.character(data$START_DATE), data$COMMOD_CURVE,
              as.character(data$PRICING_DATE)), sum, na.rm=T)
    M2D <- M3D[,,dim(M3D)[3]]      # get the most recent pricing date
    M2D <- tapply(data$PRICE, list(as.character(data$START_DATE), data$COMMOD_CURVE),
              sum, na.rm=T)
              
    windows(8,4)
    cols <- c("blue", "red", "green", "black")
    matplot(M2D, ylab="Price", xaxt="n", xlab="", type="l", lty="solid", col=cols)
    cdates <- as.Date(rownames(M2D))
    if (max(cdates)-min(cdates)>5*365){    # if more than 5 years, label Jan and Jul
      aux <- min(which(as.numeric(format(as.Date(rownames(M2D)), "%m"))%in% c(1,7)))
      ind.labx <- seq(aux,nrow(M2D), by=6)
    } else {
      ind.labx <- 1:nrow(M2D)
    }
    labx <- format(cdates, "%Y-%m")[ind.labx]
    axis(1, at=ind.labx, labels=labx, las=2, cex.axis=1.0)
    legend(x="topright", y=NULL, legend=colnames(M2D), bty="n", cex=0.7,
    text.col=cols)
  }
  return(data)
}
