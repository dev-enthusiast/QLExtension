# Get all Bellama's spreadsheets and construct one big file for CEaR 
#
#
# Written by Adrian Dragulescu on 18-Mar-2005

rm(list=ls())
all.NA <- function(x){all(is.na(x))}
any.NA <- function(x){any(is.na(x))}

get.data <- function(options){

  con    <- odbcConnectExcel(paste(options$dir, options$filename, sep=""))
  tables <- sqlTables(con)
  data   <- sqlFetch(con, "FTR")
  odbcCloseAll()

  ind  <- which(apply(data, 2, all.NA))
  if (length(ind)!=0){data <- data[,-ind]} # remove empty columns
  ind  <- which(apply(data, 1, all.NA))
  if (length(ind)!=0){data <- data[-ind,]} # remove empty rows
  data <- data[,1:8]  # the first 9 columns are the right ones
  ind  <- which(apply(data, 1, any.NA))
  if (length(ind)!=0){data <- data[-ind,]} # remove rows with NA cells
  data <- cbind(data, options$file)
  rownames(data) <- 1:nrow(data)

  write.table(data, file=options$outfile, append=T, sep=",", row.names=F,
              col.names=F)
}

#=======================================================================
#-------------------------MAIN PROGRAM----------------------------------
require(RODBC)
options=NULL
# <<<<<<< consolidate.PJM.bids.R
# options$month <- "200603"
options$dir <- "S:/All/Risk/Data/FTRs/PJM/Bellama, Brian/2007-12/"
options$outfile <- "S:/All/Risk/Data/FTRs/PJM/Bellama, Brian/2007-12/ConsolidatedBids.csv"
options$nodemap <- "S:/All/Risk/Data/FTRs/PJM/node.map.200707.csv"

all.files <- list.files(options$dir, pattern=".xls")
file.remove(options$outfile)

#------------------------------Combine in one big file
for (file in all.files)
{
  file
  options$filename <- file;
  options$filename
  get.data(options)
}


#------------------------------Do some more formatting-------------
MM <- read.csv(options$outfile, header=F, as.is=c(1:5,7:8))
colnames(MM) <- c("Buy.Sell", "SourceNodeName", "SinkNodeName", "Bucket",
                  "Type",  "MW", "BidPrice","Period", "Strategy")
MM[,"Bucket"] <- toupper(MM[,"Bucket"])
MM[,"Bucket"] <- ifelse(MM[,"Bucket"]=="ONPEAK", "PEAK", MM[,"Bucket"])
MM[,"Bucket"] <- ifelse(MM[,"Bucket"]=="24H", "FLAT", MM[,"Bucket"])

ind <- which(MM[,"Buy.Sell"]=="Sell")
if (length(ind)!=0){   # for SELLS flip the Sink and Source
  aux <- MM[ind,"SourceNodeName"]
  MM[ind,"SourceNodeName"] <- MM[ind,"SinkNodeName"]
  MM[ind,"SinkNodeName"]   <- aux
  MM[ind,"BidPrice"] <- -MM[ind,"BidPrice"]
}
MM <- MM[,-1]

#write.table(MM, file=options$outfile, sep=",", row.names=F)

#-----------------------------Do the node mapping------------------
nodemap <- read.csv(options$nodemap, header=F, skip=2, comment.char="")
nodemap <- nodemap[,1:2]
colnames(nodemap) <- c("name","ptid")
nodemap[1:10,]

N <- dim(MM)[1]
ind <- match(as.character(MM$SourceNodeName), nodemap$name, nomatch=NA)
if (N==length(ind)){MM$SourceNodeNumber <- nodemap$ptid[ind]}
ind <- match(as.character(MM$SinkNodeName), nodemap$name, nomatch=NA)
if (N==length(ind)){MM$SinkNodeNumber <- nodemap$ptid[ind]}
(ind.source <- which(is.na(MM$SourceNodeNumber)))
(ind.sink   <- which(is.na(MM$SinkNodeNumber)))
if (length(c(ind.source, ind.sink))!= 0){
  paste("Not all nodes have been translated properly.")
}

#------------------------------Report on positions-----------------
#MM <- cbind(MM, Month=options$month)
MM <- cbind(MM, Matlab.Bucket=as.numeric(factor(MM[,"Bucket"],
           levels=c("PEAK","OFFPEAK","FLAT"))))
MM <- cbind(MM, Matlab.Type=as.numeric(factor(MM[,"Type"],
           levels=c("Obligation","Option"))))
           
MM <- cbind(MM, Matlab.StartingDate=I(MM[,"Period"]))
MM[,"Matlab.StartingDate"] <- sub("Q1","01Jun2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("Q2","01Sep2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("Q3","01Dec2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("Q4","01Mar2008",MM[,"Matlab.StartingDate"])

MM[,"Matlab.StartingDate"] <- sub("JAN","01Jan2008",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("FEB","01Feb2008",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("MAR","01Mar2008",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("APR","01Apr2008",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("MAY","01May2008",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("JUN","01Jun2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("JUL","01Jul2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("AUG","01Aug2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("SEP","01Sep2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("OCT","01Oct2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("NOV","01Nov2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.StartingDate"] <- sub("DEC","01Dec2007",MM[,"Matlab.StartingDate"])

MM <- cbind(MM, Matlab.EndingDate=I(MM[,"Period"]))
MM[,"Matlab.EndingDate"]<- sub("Q1","31Aug2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"]<- sub("Q2","30Nov2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"]<- sub("Q3","28Feb2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"]<- sub("Q4","31May2008",MM[,"Matlab.EndingDate"])

MM[,"Matlab.EndingDate"] <- sub("JAN","31Jan2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("FEB","28Feb2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("MAR","31Mar2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("APR","30Apr2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("MAY","31May2008",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("JUN","30Jun2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("JUL","31Jul2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("AUG","31Aug2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("SEP","30Sep2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("OCT","31Oct2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("NOV","30Nov2007",MM[,"Matlab.EndingDate"])
MM[,"Matlab.EndingDate"] <- sub("DEC","31Dec2007",MM[,"Matlab.EndingDate"])

# Annual2007/2008
MM[,"Matlab.StartingDate"] <- sub("All","01Jun2007",MM[,"Matlab.StartingDate"])
MM[,"Matlab.EndingDate"] <- sub("All","31May2008",MM[,"Matlab.EndingDate"])


MM[1:10,]

write.csv(MM, file=options$outfile)

