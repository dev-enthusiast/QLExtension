# Download the pjm DA eMkt results for DA market
# and email them
#
# .format_output
# .to_xlsx
#
# Written by Adrian Dragulescu on 2012-08-21


##################################################################
#
.format_output <- function(out)
{
  maxMW <- ddply(out, .(UnitName), function(x){ max(x$MW) } )
  names(maxMW)[2] <- "MW"
  maxMW <- maxMW[order(as.numeric(maxMW$MW)),]

  out$hour <- as.numeric(out$hour)
  txt0 <- dcast(out, UnitName ~ hour, function(x){round(sum(x))},
                fill=0, value.var="MW")

  aux <- subset(out, !is.na(UnitName))
  aux <- melt(aux, id=c("hour", "UnitName"))
  aux$value <- as.numeric(aux$value)
  aux$hour <- as.numeric(aux$hour)
  aux$variable <- as.character(aux$variable)

  require(reshape)
  aux <- cast(aux, hour ~ UnitName + variable, I, fill=NA)
  aux <- data.frame(aux)
  colnames(aux) <- gsub("\\.", " ", colnames(aux))
  colnames(aux) <- gsub("Harbor", "H", colnames(aux))
  
  TT <- NULL
  ind1 <- grepl("^(Con|Key)", names(aux))  
  TT[[1]] <- aux[, c(1, which(ind1))]

  ind2 <- grepl("^(BS|Delta|CPC2)", names(aux)) 
  TT[[2]] <- aux[, c(1, which(ind2))]
  
  ## ind3 <- grepl("^(Elwood)", names(aux))  
  ## TT[[3]] <- aux[, c(1, which(ind3))]

  ## ind4 <- grepl("^(Perry|HAW1|GSt|West)", names(aux))  
  ## TT[[4]] <- aux[, c(1, which(ind4))]

  ind3 <- grepl("^(HndLake)", names(aux))  
  TT[[3]] <- aux[, c(1, which(ind3))]

  ind4 <- grepl("^(SafeH)", names(aux))  
  TT[[4]] <- aux[, c(1, which(ind4))]

  ind5 <- grepl("^(R4|Crescent|Criterion|Pioneer)", names(aux))  
  TT[[5]] <- aux[, c(1, which(ind5))]

  indLeft <- which(!(ind1|ind2|ind3|ind4|ind5))[-1]
  indR <- length(indLeft) %/% 8  # remaining chunks
  if (length(indLeft) %% 8 != 0) {
    indR <- indR + 1
  }
  
  for (i in 1:indR) {
    r <- (8*(i-1)+1):min(8*i, length(indLeft))
    rLog("Range is:", r)
    TT[[5+i]] <- aux[, c(1, indLeft[r]) ]
  }
  
  # put it all toghether
  txt <- c(capture.output(print(txt0, row.names=FALSE)), "\n")
  for (g in 1:length(TT)) {
     txt <- c(txt,
       capture.output(print(TT[[g]], row.names=FALSE)),"", "")
   }

   txt <- c(txt, "", "", 
    "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/download_pjm_eMkt_DA", 
    "Contact: Adrian Dragulescu")

  
  txt
}


##################################################################
#
.to_xlsx <- function(asOfDate, out)
{
  require(xlsx)

  DIR <- "S:/Trading/Prop Trading/Gen Fleet Commitment/PJM/"
  fileout <- paste(DIR, "da_awards_pjm_", format(asOfDate), ".xlsx", sep="")

  out$hour <- as.numeric(out$hour)
  aux <- dcast(out, UnitName ~ hour, sum, fill=0, value.var="MW")

  write.xlsx2(aux, file=fileout, row.names=FALSE)

}

##################################################################
##################################################################

options(width=800)  # make some room for the output
options(stringsAsFactors=FALSE)  
require(CEGbase)
require(plyr)
require(reshape2)
require(XML)
source("H:/user/R/RMG/Energy/Trading/Congestion/PJM/lib.PJM.emarket.R")



returnCode <- 99   # 0 means success

rLog("Running: R/RMG/Energy/Trading/Congestion/Condor/download_pjm_eMkt_DA.R")
rLog("Start at ", as.character(Sys.time()))

asOfDate <- Sys.Date()+1         # it's DA market 
#asOfDate <- as.Date("2012-06-05")
DIR <- "S:/All/Structured Risk/NEPOOL/eMkt/XML/"


download_pjm_gen(logininfo="exgnptauto:Welcome_1", day=asOfDate, OUTDIR=DIR)
download_pjm_gen(logininfo="cpsiauto:Welcome_1", day=asOfDate, OUTDIR=DIR)


filein <- paste(DIR, format(asOfDate), "_GENBYPARTICIPANT_exgnptauto.xml", sep="")
out <- read_xml_genbyparticipant(filein)
#filein <- paste(DIR, format(asOfDate), "_GENBYPARTICIPANT_cpsiauto.xml", sep="")
#out2 <- read_xml_genbyparticipant(filein)


.to_xlsx(asOfDate, out)

if (nrow(out) >= 23) {
  returnCode <- 0                 # succeed only when you download it!
  rLog("Email results")
  to <- paste(c("midAtlCash@constellation.com",
    "matthew.marsh@constellation.com", "scott.dupcak@constellation.com",
    "EastDesk@constellation.com", "tyler.herrald@constellation.com",
    "Edward.Young@constellation.com", "patrick.smith@constellation.com",
    "paul.nelson@constellation.com", "robert.cirincione@constellation.com",
    "steve.taylor@constellation.com", "rahul.mehra@constellation.com",            
    "#WTMidAtlantic@constellation.com", "GenDispatchers@constellation.com",  
    "adrian.dragulescu@constellation.com"),
              sep="", collapse=",")
  #to <- "adrian.dragulescu@constellation.com"
  txt <- .format_output(out)
  
  sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
    to,
    paste("PJM Gen Awards for day", format(asOfDate)),
    c("File saved in S:/Trading/Prop Trading/Gen Fleet Commitment/PJM\n\n",
      txt))
  
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
