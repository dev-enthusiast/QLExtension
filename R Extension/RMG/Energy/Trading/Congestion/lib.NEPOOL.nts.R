# deal with submitting to NTS (Near Term Schedule)
#
#
#



##################################################################
# Send an email to ExternalData with a spreadsheet for NTS
# @param out is the output of the xml file, from read_xml_genbyparticipant
#
.submit_nts_schedule <- function(out, asOfDate, do.email=FALSE)
{
  rLog("Making the NTS schedules ...")
  OUTDIR <- paste("S:/All/Structured Risk/NEPOOL/Generation Related/",
    "Boston Gen/Plant Dispatch/DA_Awards/nts_schedule/", sep="")
  fileout <- paste(OUTDIR, "nts_schedule_", format(asOfDate),
                   ".xlsx", sep="")

  row1 <- c("NTS Submission", rep("ForeRiver", 2), rep("Kleen", 2),
    rep("Mystic 7", 2), rep("Mystic 8 LNG", 2), rep("Mystic 9 LNG", 2),
    rep("Granite Ridge", 2))
  row2 <- c("NEPOOL", rep(c("Self Schedule", "Fuel Schedule"), 6))

  
  # convert the string to a POSIXct! Rrr
  # "EDT" goes to "-0400" and "EST" goes to "-0500"
  dt <- out$hourBeginning
  dt <- gsub(" EDT", "-0400", dt)
  dt <- gsub(" EST", "-0500", dt)
  dt <- as.POSIXct(dt, format="%Y-%m-%d %H:%M:%S%z") + 3600  # make them hour ending
  # excel only understands GMT.  There will be issues on fall DST ...
  dt <- as.POSIXct(substring(format(dt), 1, 19), tz="GMT", origin="1970-01-01")


  
  out <- out[,-1]
  out <- as.matrix(out)
  out[which(is.na(out))] <- 0  # replace NA's with 0
  zz <- rep(0.0, nrow(out))
  ZZ <- data.frame(qty = zz, fuel = zz)
  
  FR <- KL <- M7 <- M8 <- M9 <- GR <- ZZ
  
  if ("ED1A_MW" %in% colnames(out)) 
    FR <- data.frame(qty  = FR$qty + out[,"ED1A_MW"],
                     fuel = FR$fuel + out[,"ED1A_MW"]*7.3)
  if ("ED1B_MW" %in% colnames(out))
    FR <- data.frame(qty  = FR$qty + out[,"ED1B_MW"],
                     fuel = FR$fuel + out[,"ED1B_MW"]*7.3)
  
  if ("KLEN_MW" %in% colnames(out)) 
    KL <- data.frame(qty  = out[,"KLEN_MW"],
                     fuel = out[,"KLEN_MW"]*7 )

  if ("MYS7_MW" %in% colnames(out)) 
    M7 <- data.frame(qty  = out[,"MYS7_MW"],
                     fuel = out[,"MYS7_MW"]*17)

  if ("MYS8_MW" %in% colnames(out)) 
    M8 <- data.frame(qty  = out[,"MYS8_MW"],
                     fuel = out[,"MYS8_MW"]*7.3)

  if ("MYS9_MW" %in% colnames(out)) 
    M9 <- data.frame(qty  = out[,"MYS9_MW"],
                     fuel = out[,"MYS9_MW"]*7.3)

  if ("GRAN_MW" %in% colnames(out)) 
    GR <- data.frame(qty  = out[,"GRAN_MW"],
                     fuel = out[,"GRAN_MW"]*7.3)

  TT <- cbind(dt, FR, KL, M7, M8, M9, GR)

  require(xlsx)
  wb <- createWorkbook()
  sheet <- createSheet(wb)
  addDataFrame(TT, sheet, startRow=3, startColumn=1,
    col.names=FALSE, row.names=FALSE)
  cb <- CellBlock(sheet, 1, 1, 2, ncol(TT))
  CB.setRowData(cb, row1, 1)
  CB.setRowData(cb, row2, 2)
  autoSizeColumn(sheet, 1)
  
  saveWorkbook(wb, fileout)
  rLog("Wrote file", fileout)

  if (do.email) {
    rLog("Send file to ExternalData")
    to <- paste("External.Data2@constellation.com", "NEcash@constellation.com",
      "abizar.shahpurwala@constellation.com", "raymond.wetzel@constellation.com", 
      "norman.kate@constellation.com", "ccggasschedulingall@constellation.com", sep=",")
    subject <- "NTS Submission"
    out <- c("Procmon job: RMG/R/Reports/Energy/Trading/Congestion/download_nepool_eMkt_DA",
             "Contact: Adrian Dragulescu\n")
    sendEmail("OVERNIGHT_PM_REPORTS@constellation.com", to, 
      subject, out,
      attachments=fileout)
  }
}


##################################################################
##################################################################
#
.test <- function()
{
  require(CEGbase)
  require(reshape)
  require(XML)
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.emarket.R")
  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.NEPOOL.nts.R")

  DIR <- "S:/All/Structured Risk/NEPOOL/eMkt/XML/"

  asOfDate <- as.Date("2013-12-29")
  filein <- paste(DIR, "genbyparticipant_", format(asOfDate), ".xml", sep="")
  out <- read_xml_genbyparticipant(filein, make.xlsx=TRUE)

  # redo a bunch of days ... 
  days <- seq(as.Date("2014-01-01"), as.Date("2014-01-16"), by="1 day")
  for (day in days) {
    asOfDate <- as.Date(day, origin="1970-01-01")
    filein <- paste(DIR, "genbyparticipant_", format(asOfDate), ".xml", sep="")
    out <- read_xml_genbyparticipant(filein, make.xlsx=TRUE)
    
    .submit_nts_schedule(out, asOfDate, do.email=FALSE)
  }

}



  #dt <- as.POSIXct(gsub(":00$", "00", out$hourBeginning),
  #  format="%Y-%m-%dT%H:%M:%S", tz="GMT") + 3600  # make them hour ending
