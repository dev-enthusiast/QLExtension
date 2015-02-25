# Seasonal Claimed Capability report
#
# .change_ownership(month1, month2)
# .compare_scc_reports(month1, month2)
# .get_scc_filename(month)
# .download_scc_report()
# .read_scc_report(filename) 
#


##########################################################################
# check the changes in ownership between two months ... 
#
.change_ownership <- function(month1, month2)
{
  res1 <- .read_scc_report(month1, colIndex=1:30)
  res2 <- .read_scc_report(month2, colIndex=1:30)

  a1 <- res1[, c("Asset.ID", "Current.Lead.Participant")]
  names(a1)[2] <- "from.participant"
  a2 <- res2[, c("Asset.ID", "Current.Lead.Participant")]
  names(a2)[2] <- "to.participant"

  aa <- merge(a1, a2, all=TRUE)
  aa <- subset(aa, from.participant != to.participant)

  if (nrow(aa) > 0 ){
    rLog("Changes in ownership between", format(month1), "and", format(month2) )
    print(aa, row.names=FALSE)
  }
  
  aa
}


##########################################################################
# compare new/retired gen
#
.compare_scc_reports <- function(month1, month2)
{ 
  res1 <- .read_scc_report(month1, colIndex=1:30)
  res2 <- .read_scc_report(month2, colIndex=1:30)
  res <- rbind.fill(res1, res2)
  #res$Asset.ID <- as.numeric(res$Asset.ID)
  
  aux <- subset(res[,c("Asset.ID", "month", "WSCC")], Asset.ID != "")
  aux <- cast(aux, Asset.ID ~ month, I, fill=NA, value="WSCC")

  newGen <- rmGen <- NULL
  
  newGenID <- aux[which(is.na(aux[,2])),]$Asset.ID
  if (length(newGenID) > 0)
    newGen <- subset(res2, Asset.ID %in% newGenID)
  
  rmGenID <- aux[which(is.na(aux[,3])),]$Asset.ID
  if (length(rmGenID) > 0)
    rmGen <- subset(res1, Asset.ID %in% rmGenID)

  changeGen <- .change_ownership(month1, month2)
  
  list(newGen=newGen, rmGen=rmGen, changeGen=changeGen)
}


##########################################################################
# Get the filename from the 
#
.get_scc_filename <- function(months)
{
  DIR <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/",
    "NEPOOL/Units/SeasonalClaimedCapability/Raw/", sep="")

  files <- list.files(DIR, full.names=TRUE)

  isoMonths <- gsub("scc_(.*)_(.*)\\.xls", "\\1_\\2_", basename(files))
  isoMonths <- gsub("_", "", isoMonths)
  isoMonths <- as.Date(paste("1", isoMonths, sep=""), "%d%B%Y")

  names(files) <- format(isoMonths)
  files[format(months)]
}



##########################################################################
# Download this report from ISO
#
.download_scc_report <- function(year=NULL)
{
  # download the latest file
  if ( is.null(year) ) {
    url <- "http://www.iso-ne.com/genrtion_resrcs/snl_clmd_cap/index.html"
  } else {
    url <- paste("http://www.iso-ne.com/genrtion_resrcs/snl_clmd_cap/",
                 year, "/index.html", sep="")
  }
  links <- .getLinksInPage(url, pattern="scc.*\\.xls")

  DIR_OUT <- paste("S:/All/Structured Risk/NEPOOL/FTRs/ISODatabase/",
    "NEPOOL/Units/SeasonalClaimedCapability/Raw/", sep="")
  .download_and_archive(links, DIR_OUT=DIR_OUT)
}


##########################################################################
# read one xls file
#
.read_scc_report <- function(month, colIndex=c(1,2,3,5,6,10:14,16,22,28))
{
  filename <- .get_scc_filename(month)
  
  rLog("Reading file ", filename)
  aux <- read.xlsx2( filename, sheetIndex=2, startRow=2,
     colIndex=colIndex )
  colnames(aux)[grep("Intermittent", colnames(aux))] <- "Intermittent"
  aux <- cbind(month = month, aux)

  if ("Asset.ID" %in% colnames(aux))
    aux$Asset.ID <- as.integer(aux$Asset.ID)
  if ("Load.Zone" %in% colnames(aux))
    aux$Load.Zone <- as.integer(aux$Load.Zone)
  if ("WSCC" %in% colnames(aux))
    aux$WSCC <- as.numeric(aux$WSCC)
  if ("SSCC" %in% colnames(aux))
    aux$SSCC <- as.numeric(aux$SSCC)
  
  aux
}


##########################################################################
##########################################################################
#
.main <- function()
{
  require(CEGbase)
  require(reshape)
  require(xlsx)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.downloads.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/Units/lib.NEPOOL.scc_report.R")

  
  
  .download_scc_report()
  .download_scc_report(year=2012)

  
  month1 <- as.Date("2012-06-01")
  month2 <- as.Date("2012-07-01")
  
  res <- .compare_scc_reports(month1, month2)
  newGen <- res[["newGen"]]
  rmGen  <- res[["rmGen"]]

  aux <- newGen[,c("Asset.ID", "Generator.Name", "Fuel.Type", "WSCC",
            "Current.Lead.Participant", "State", "Load.Zone")]
  aux <- aux[order(-as.numeric(aux$WSCC)), ]
  print(aux)
  
}
