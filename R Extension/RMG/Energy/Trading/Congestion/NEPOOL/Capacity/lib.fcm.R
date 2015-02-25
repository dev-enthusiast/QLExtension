# Functions to deal with FCM 
#
# download_FCM_results - by hand now as they are not standardized
#  I had to adjust the 2010/11 format, as it was not standard.
#
# composite_offer_partners
# make_fcm_awards_archive
# read_nonpriceretirements

########################################################################
# find who could be our composite offer partners
# We're looking for assets with higher summer qualified capacity then
#   winter
#
# X is the contents of a FCM awards spreadsheet as returned by ...
#
composite_offer_partners <- function(X)
{
  uX <- unique(X[,c("ptid", "Name", "Intermittent", "Generating.Fuel.Type",
    "Modeled.Capacity.Zone.", "Load.Zone.Name", "Summer.Qualified.Capacity",
    "Winter.Qualified.Capacity")])

  uX$diff <- as.numeric(uX$Summer.Qualified.Capacity) - as.numeric(uX$Winter.Qualified.Capacity)
  uX <- uX[order(uX$diff), ]

  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Units/lib.NEPOOL.scc_report.R")
  scc <- .read_scc_report(prevMonth(), colIndex=NULL)
  scc <- scc[,c("Asset.ID", "Current.Lead.Participant", "WSCC", "SSCC")]
  scc$diff <- as.numeric(scc$SSCC) - as.numeric(scc$WSCC)

  aScc <- cast(scc, Current.Lead.Participant ~ ., sum, value="diff")
  colnames(aScc)[2] <- "diff"
  aScc <- aScc[order(aScc$diff), ]
  print(aScc, row.names=FALSE)

  # maybe brookfield....
  # nothing 

  
  
}






########################################################################
# Aggregate the results from the FCM auctions into a master file
#
#
make_fca_awards_archive <- function(do.save=FALSE)
{
  DIR <- "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/Auctions"
  files <- list.files(paste(DIR, "/Raw", sep=""), full.names=TRUE,
    pattern="^fca.*")

  res <- NULL
  for (fname in files) {                       # column names are different!
    rLog("Reading file", basename(fname))
    aux <- read.xlsx2(fname, sheetIndex=1)
    ind <- grep("c\\.+", colnames(aux))       # you may get a spurious column
    if (length(ind) > 0)
      aux <- aux[,-ind]
    
    ind.M <- grep("^X[[:digit:]]{5}", colnames(aux))
    if (length(ind.M) != 12)
      stop("Problem reading the months")
    colnames(aux)[ind.M] <- as.character(as.Date(as.numeric(gsub("X", "",
      colnames(aux)[ind.M]))-25569, origin="1970-01-01"))
    
    ind <- apply(aux, 1, function(x){!all(is.na(x))}) 
    aux <- aux[ind,]
    ind <- apply(aux, 2, function(x){!all(is.na(x))}) 
    aux <- aux[,ind]
    
    bux <- melt(aux, id=1:(ind.M[1]-1))
    colnames(bux)[(ncol(bux)-1):ncol(bux)] <- c("month", "cleared.mw")
    colnames(bux)[1] <- "ptid"
    bux[,1] <- as.integer(bux[,1])
    ## colnames(bux) <- c("ID", "Name", "Resource.Type", "Fuel.Type", "DR.Type",
    ##   "Modeled.Capacity.Zone.ID", "Modeled.Capacity.Zone",
    ##   "Final.Capacity.Zone.ID", "Final.Capacity.Zone", "State", "Load.Zone",
    ##   "Status", "period", "month")

    ind <- which(is.na(bux$ptid))
    if (length(ind)>0) {
      bux <- bux[-ind,]
    }
    suppressWarnings(bux$cleared.mw <- as.numeric(bux$cleared.mw))
    ind <- which(is.na(bux$cleared.mw))
    if (length(ind)>0)
      bux <- bux[-ind,]
    
    bux$month <- as.Date(as.character(bux$month))
     
    
    res[[fname]] <- bux
  }

  aux <- ldply(res, function(x){
    y <- x[,c("ptid", "month", "cleared.mw")]
    y
  })
  
  #lapply(res, head)
  #res.all <- do.call(rbind, res)

  if (do.save) {
    write.csv(aux, file=paste(DIR, "fca_results.csv", sep=""),
      row.names=FALSE)
    save(aux, file=paste(DIR, "fca_results.RData", sep=""))
  }

  res
}

########################################################################
#
read_nonpriceretirements <- function()
{
  "S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/NEPOOL/FCM/NonPriceRetirementRequests"
}



########################################################################
########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/Nepool/Capacity/lib.fcm.R")
  
  make_fca_awards_archive()


  
}
