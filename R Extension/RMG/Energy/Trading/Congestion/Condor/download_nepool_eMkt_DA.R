# Download the nepool DA eMkt results for DA market
# and archive them
#
# Written by Adrian Dragulescu on 2011-03-18
#
# You can check on ntplot: 
#   showload("Mystic 9", "4Sep12", "Self Schedule") 
#



##################################################################
##################################################################

options(width=400)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(reshape)
require(XML)
source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.nts.R")

returnCode <- 99   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_nepool_eMkt_DA.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate <- Sys.Date()+1         # it's DA market 


if (asOfDate <= as.Date("2012-04-17")) {
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.emarket_pre2012-02-17.R")
} else {
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.emarket.R")
}

DIR <- "S:/All/Structured Risk/NEPOOL/eMkt/XML/"

company <- if (asOfDate >= as.Date("2013-02-01")) "EXGN" else "CCG"


##################################################################
# get the GENBYPARTICIPANT
download_emkt_file(day=asOfDate, token="genbyparticipant", OUTDIR=DIR,
  company=company)


filein <- paste(DIR, "genbyparticipant_", format(asOfDate), ".xml", sep="")
out <- read_xml_genbyparticipant(filein, make.xlsx=TRUE)
if (asOfDate < as.Date("2014-12-01"))
  out <- out[,!grepl("^ED", colnames(out))]

ind <- grepl(c("^(ED|KLEN|MYS|GRAN)"), names(out))  # the big ones
out1 <- out[, c(1, which(ind)), drop=FALSE]
out2 <- out[, which(!ind), drop=FALSE] 


if (nrow(out) >= 23) {
  returnCode <- 0                 # succeed only when you download it!
  rLog("Email results")
  to <- paste(c("NECash@constellation.com", "EastDesk@constellation.com", 
    "patrick.smith@constellation.com",
    "steve.taylor@constellation.com", "rahul.mehra@constellation.com"),
    sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"

  fileout <- paste("S:/All/Structured Risk/NEPOOL/Generation Related/",
      "Boston Gen/Plant Dispatch/DA_Awards/DA Awards ", format(asOfDate),
      ".xlsx", sep="")
  
  sendEmail("adrian.dragulescu@constellation.com",
    to,
    paste("NEPOOL Gen Awards file processed"),
    c("File saved in S:/All/Structured Risk/NEPOOL/Generation Related/Boston Gen/Plant Dispatch/DA_Awards folder.\n", 
      capture.output(print(out1, row.names=FALSE)), "", "", "and the smaller ones ...", 
      if (ncol(out2) > 1) capture.output(print(out2, row.names=FALSE)) else "",
      "\n\nProcmon job: RMG/R/Reports/Energy/Trading/Congestion/download_nepool_eMkt_DA",
      "Contact: Adrian Dragulescu\n"),
    attachments=fileout)

  .submit_nts_schedule(out, asOfDate, do.email=TRUE)
  
}


rLog("Done at ", as.character(Sys.time()))


if( interactive() ){
  print( returnCode )
} else {
  q( status = returnCode )
}




##########################################################################
##########################################################################
## res <- download_emkt_file(day=asOfDate, token="scheduledetail")
## filein <- paste(DIR, "scheduledetail_", format(asOfDate), ".xml", sep="")
