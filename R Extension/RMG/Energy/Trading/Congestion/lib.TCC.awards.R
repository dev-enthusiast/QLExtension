# look at the awards 
#
# .download_awards           from NYISO web site
# .load_awards_csv           if downloaded by hand
# .payoff_by_participant
# .read_exgen_awards_xlsx    our database
#
# Trade Control pulls the data here:
#   S:\Wholesale_Ops\Trade Control\Trade Capture\Congestion Booking\NY TCCs\2014\Monthly\0814
# The file downloaded from NYISO is in
#   S:\Trading\NYISO\NY Congestion\Monthly Auctions\2014\0814
#
#

#################################################################
# Query the NYISO website for the awards by participant
# 
# @param auction a CRR auction
# @param company a vector of company names, if NULL return all
# @return a data.frame with 
#
#                       
.download_awards <- function(auction=NULL, startDt=NULL, endDt=NULL,
   company=c("Exelon Generation Company LLC",
     "Constellation Energy Commodities Group, Inc."))
{
  if (!is.null(auction)) {
    startDt <- auction$startDt
    endDt   <- auction$endDt
  }
  if (is.null(startDt))
    startDt <- Sys.Date() - 50*365  # !
  if (is.null(endDt))
    endDt <- Sys.Date() + 50*365    # !
  
  
  library(rJava)
  URL <- "http://tcc.nyiso.com/tcc/public/view_summary_of_transmission_contracts.do"

  DIR <- "S:/All/Structured Risk/NEPOOL/Software/Utilities/htmlunit/htmlunit-2.15/lib/"
  .jinit()
  # load the htmlunit jars 
  lapply(list.files(
    "S:/All/Structured Risk/NEPOOL/Software/Utilities/htmlunit/htmlunit-2.15/lib/",
      full.names=TRUE), .jaddClassPath)
 
  
  webClient <- new(J("com.gargoylesoftware.htmlunit.WebClient"))
  page <- webClient$getPage(URL)
  field.startDt <- page$getElementsByName("arStartDateTransmissionContracts")$get(0L)
  field.startDt$setText( format(startDt, "%m/%d/%Y") )
  field.endDt <- page$getElementsByName("arEndDateTransmissionContracts")$get(0L)
  field.endDt$setText( format(endDt, "%m/%d/%Y") )
  button <- page$getElementsByTagName("button")$get(0L)

  # click the button and get the results
  awds <- button$click()$getWebResponse()$getContentAsString() 
  webClient$closeAllWindows()   # close the shop 
  
  x <- read.csv( textConnection(awds) )
  x <- x[1:(nrow(x)-3),]  # remove the last 3 rows
  x$Start.Date <- as.Date(x$Start.Date, "%d-%b-%Y")
  x$End.Date   <- as.Date(x$End.Date, "%d-%b-%Y")
  
  if (!is.null(startDt))
    x <- subset(x, Start.Date >= startDt)
  
  if (!is.null(endDt))
    x <- subset(x, End.Date <= endDt)
  
  if (!is.null(company))
    x <- subset(x, Primary.Holder %in% company)
  
  x
}


#################################################################
# Load the awards you download from
# http://tcc.nyiso.com/tcc/public/view_summary_of_transmission_contracts.do
# @param filename
# @param auction a CRR auction
#
.load_awards_csv <- function(filename, auction=NULL)
{
  AWD <- read.csv(filename)
  suppressWarnings(AWD[,1] <- as.numeric(AWD[,1]))
  AWD <- AWD[!is.na(AWD[,1]),]
  
  AWD$Start.Date <- as.Date(AWD$Start.Date, "%d-%b-%Y")
  AWD$End.Date <- as.Date(AWD$End.Date, "%d-%b-%Y")

  if (!is.null(auction))
    AWD <- subset(AWD, Start.Date == auction$startDt & End.Date == auction$endDt)

  AWD <- AWD[order(AWD$Primary.Holder, -AWD$MW.Summer, AWD$POI.ID), ]
  
  # unique(AWD[,-1])
  # need to aggregate them.  some bids are the same.
  AWD
}


#################################################################
# Calculate the payoff by participant
# @param awd the data.frame with awards
# @param auction a CRR_Auction 
#
.payoff_by_participant <- function( participant, AWD, auction )
{
  awd <- subset(AWD, Primary.Holder == participant)

  # make the paths
  paths <- vector("list", length=nrow(awd))
  for (r in seq_len(nrow(awd))) {
    paths[[r]] <- Path(source.ptid = awd[r,"POI.ID"],
                       sink.ptid   = awd[r,"POW.ID"],
                       mw = max(awd[r,"MW.Summer"], awd[r,"MW.Winter"]),
                       auction = auction)
  }

  pp <- get_payoff(paths)

  pp
}



#################################################################
# Read the awards from the archive file
# @param filename location of the awards (look at the format)
# @param sheetIndex an integer
# @return a data.frame with all the info you need
#
.read_exgen_awards_xlsx <- function(
   filename = "S:/Trading/Nodal FTRs/NYISO/Auctions/awards.xlsx",
   sheetIndex = 1, sheetName=NULL)
{
  library(xlsx)

  if (is.null(sheetName)) {  
    aux <- read.xlsx2(file=filename, sheetIndex=sheetIndex,
      colClasses=c(rep("character", 4), rep("Date", 2), rep("character", 2),
        rep("numeric", 3), "character"))
  } else {
    aux <- read.xlsx2(file=filename, sheetName=sheetName,
      colClasses=c(rep("character", 4), rep("Date", 2), rep("character", 2),
        rep("numeric", 3), "character"))
  }
  
  aux <- aux[aux$auction != "",]
  aux$auctionName <- aux$auction
  
  uAuctions <- parse_auctionName( unique(aux$auctionName) , region="NYPP")
  if (nrow(aux) == 1)
    uAuctions <- list(uAuctions)
  uAuctions <-  do.call(rbind, lapply(uAuctions, as.data.frame))

  aux <- merge(aux[,c("auctionName", "buy.sell", "bucket", "mw",
                "source.ptid", "sink.ptid", "strategy", "book")], uAuctions, by="auctionName")

  return (aux)
}
 

#################################################################
# Investigate one participant
#
..study_participant <- function()
{
  participant <- "DC Energy New York, LLC"
  
  awd  <- .download_awards( startDt=as.Date("2014-05-01"), company=NULL )
  X <- subset(awd, Primary.Holder==participant)
  

}


#################################################################
#
TCC.loadPositions <- function(from="XLSX",
                              asOfDate=Sys.Date())
{
  load_ISO_environment(region="NYPP")
 
  if (from == "XLSX") {
    x <- .read_exgen_awards_xlsx()
  }

  # get only the active deals
  y <- subset(x, startDt <= asOfDate & endDt >= asOfDate)
  y <- cbind(path.no=1:nrow(y), y)

  y
}




#################################################################
#################################################################
# 
.test <- function()
{
  require(CEGbase)
  require(CRR)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.awards.R")

  
  ##########################################################################
  # TODO: After awards
  #
  auction <- parse_auctionName("Q14", region="NYPP")
  awd <- .download_awards( auction )

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.TCC.auction.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NYPP.TCC.R")
  update_clearingprice_archive()



  
  ##########################################################################
  # Investigate payoff by participant
  #
  filename <- "//CEG/CERShare/Trading/Nodal FTRs/NYISO/Auctions/Q14/TransmissionContracts.csv"
  auction <- parse_auctionName("N14", region="NYPP")
  AWD <- .load_awards_csv(filename, auction=auction)
  
  participant <- "Castleton Commodities Merchant Trading L.P."
  participant <- "DC Energy New York, LLC"

  awd  <- .download_awards( startDt=as.Date("2014-05-01"), company=NULL )
  X <- subset(awd, Primary.Holder==participant)
  X <- subset(X, grepl("^PORT_JEFF", POW.Name) )

  
  X <- subset(AWD, !(POI.ID %in% c(23786, 23512)))
  pp <- .payoff_by_participant(participant, X, auction)

  summary(pp)


  
  split(AWD[,-c(1:4, 13:15)], AWD$Primary.Holder)

  sort(unique(AWD$Primary.Holder))
  awd <- subset(AWD, Primary.Holder == "DC Energy New York, LLC")
  awd <- subset(AWD, Primary.Holder == "Castleton Commodities Merchant Trading L.P.")
  


  
  

}
