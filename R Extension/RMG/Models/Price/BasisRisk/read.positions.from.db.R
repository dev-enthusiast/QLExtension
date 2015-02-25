# Read the house positions from CPSPROD database 
#
#

read.positions.from.db <- function(save, options){

  #---------------------------------------READ DATA IN---------------------
  con <- odbcConnectAccess("S:/Risk/Projects/Database/CPSTest.mdb")  
  curves <- sqlFetch(con, "qselCurveNameToLocation")  # Frank's curve to region mapping
  curves[1:10,]
  odbcCloseAll()

  loptions=NULL; loptions$asOfDate <- options$asOfDate
  if (length(options$mapped.region.name)>0){
    loptions$regions <- options$mapped.region.name}
  data <- read.deltas.from.CPSPROD(loptions)  
  data[1:10,]
  #-------------------------------------------------------------------------
  data[, "CONTRACT_DATE"] <- as.character(as.Date(data[, "CONTRACT_DATE"]))
  ind <- which(data[, "CONTRACT_DATE"] %in% paste(options$fdata$months,"-01", sep=""))
  data <- data[ind,]    # select only the months we are interested

  #------------ select the subregion ID's that were specified 
  curves <- subset(curves, srID %in% options$quantity.table$values)

  ind    <- which(tolower(as.character(data[,"COMMOD_CURVE"])) %in%
            tolower(as.character(curves[,"Curve Name"])))
  data <- data[ind, ]   # select only the curves that we are interested
  
  ind <- pmatch(tolower(as.character(data[,"COMMOD_CURVE"])),
         tolower(as.character(curves[,"Curve Name"])), duplicates.ok=T)
  ind.NA <- which(is.na(ind))
  if (length(ind.NA)>0){
    source("H:/user/R/RMG/Utilities/sendemail.R")
    from    <- "adrian.dragulescu@constellation.com"
    to      <- "frank.hubbard@constellation.com, wang.yu@constellation.com"
    subject <- "Curves that are missing from Frank's list"
    body    <- paste(unique(data[ind.NA, "COMMOD_CURVE"]), collapse=", ")
    sendemail(from, to, "", subject, body, "")
  }
  data$srID <- curves$srID[ind]
  #------------ create the short.names column in data
  ind <- match(data$srID, options$quantity.table$values)
  data$short.names <- ordered(options$quantity.table$ind[ind], 
                              levels=options$short.names)
  data[,"BOOK_ACCT_TREATMENT"] <- as.character(data[,"BOOK_ACCT_TREATMENT"])  
  Q.tot <- aggregate(data$DELTA_VALUE, list(data$CONTRACT_DATE, data$short.names,
                          data[,"BOOK_ACCT_TREATMENT"]), sum)
  colnames(Q.tot) <- c("month", "zone", "accounting", "delta")
  
##   Q.tot <- tapply(data$DELTA_VALUE, list(data$CONTRACT_DATE, data$short.names,
##                           data[,"BOOK_ACCT_TREATMENT"]), sum)
##   rownames(Q.tot) <- format(as.Date(rownames(Q.tot)), "%Y-%m")
  
  return(Q.tot)
}











#   Q <- tapply(data$position, list(data$deliveryDate, data$Subregion,
#                                   data$peak), sum)
#   signif(Q, digits=2)

#   if (length(ind.NA)>0){
#     source("H:/user/R/RMG/Utilities/sendemail.R")
#     from    <- "adrian.dragulescu@constellation.com"
#     to      <- "frank.hubbard@constellation.com, wang.yu@constellation.com"
#     subject <- "Curves that are missing from Frank's list"
#     body    <- paste(unique(data[ind.NA, "COMMOD_CURVE"]), collapse=", ")
#     sendemail(from, to, "", subject, body, "")
#   }

#   aux <- as.Date(unique(data$forDate))
#   if ((length(aux)!=1)& (aux != options$asOfDate)){
#     aux <- paste("Position data is from", aux)
#     cat(aux, "\n"); flush.console()
#   }

#   all.zones <-  c("Hub",options$zone.names)
#   data <- data[which(data$Region==options$region),]  # select only this region
#   data$Subregion <- as.character(data$Subregion)
#   if (options$region=="NEPOOL"){
#     data$Subregion <- gsub("Boston", "NEMA",  data$Subregion)
#     data$Subregion <- gsub("Mass Hub", "Hub", data$Subregion)
#     data$Subregion <- gsub("W.Mass", "WMASS", data$Subregion)
#   } 
#   ind  <- which(data$Subregion %in% all.zones)
#   data <- data[ind,]    # leave out the Ancillaries, VLR, SOX, etc. 

  
#   data$Subregion <- factor(data$Subregion, levels=all.zones)
#   data$deliveryDate <- format(as.Date(data$deliveryDate), "%Y-%m")
#   data$peak <- "Offpeak"
#   data$peak[grep("5x16", as.character(data$curveName))] <- "Peak"

#   ind <- pmatch(tolower(as.character(data[,"COMMOD_CURVE"])),
#          tolower(as.character(curves[,"Curve Name"])), duplicates.ok=T)
#   ind.NA <- which(is.na(ind)); (unique(data[ind.NA, "COMMOD_CURVE"]))
#   data$Subregion <- curves[ind,"Subregion"]
  

#   aux <- subset(curves, Region %in% uRegions)

#   pmatch(as.character(data[,"COMMOD_CURVE"]), as.character(curves[,"Curve Name"]))
#   pmatch(tolower(as.character(data[1:5,"COMMOD_CURVE"])),
#          tolower(as.character(curves[,"Curve Name"])))
  
#   ind <- pmatch(tolower(as.character(data[,"COMMOD_CURVE"])),
#          tolower(as.character(curves[,"Curve Name"])), duplicates.ok=T)
#   ind.NA <- which(is.na(ind)); (unique(data[ind.NA, "COMMOD_CURVE"]))
  
#   unique(curves$Subregion)
#   unique(data[,"MARKET_FAMILY"])
#   unique(data[,"MAPPED_REGION_NAME"])
