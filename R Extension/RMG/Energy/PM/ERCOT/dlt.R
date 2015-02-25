# Say what this beautiful piece of software does ... 
#
#
# Written by Lori Simpson, 4-Nov-2008

############ Load all necessary packages
rm(list=ls())
setwd("S:/All/Portfolio Management/Simpson/transmission_planning/")
require("SecDb"); require(reshape); require(xlsReadWrite)
## source("S:/all/Risk/Software/R/Prod/Utilities/write.xls.R")
## args(write.xls)
## args(write.csv)


############## To connect to database
require(RODBC)
DSN_PATH = paste("FileDSN=",
  "S:/All/Risk/Software/R/prod/Utilities/DSN/CPSERCOT.dsn;",
  "UID=cps_ops;", "PWD=cps_ops;", sep="")
channel = odbcDriverConnect(DSN_PATH)
oom <- sqlQuery(channel,
"SELECT * FROM E_CPS.ERCOT_CONGESTION_DISCLOSURE
WHERE REPORT_MONTH>'01JAN08'")
#test <- sqlQuery(channel,"SELECT MAX(DAY) FROM E_CPS.ERCOT_CONGESTION_DISCLOSURE
# WHERE REPORT_MONTH>'01janT08'")

names(oom) <- gsub("_", ".", tolower(names(oom)))
cat("Finished data pull from CPSERCOT.\n")

############## Break contigency into plants, zone, type, etc
oom$contigency=as.character(oom$contigency)
hello=strsplit(oom$contigency,"_")
uName = sapply(hello,function(x){
#  browser() --use this to debug, hit Q to quit
  paste(x[4:length(x)], collapse="_")})
uName=data.frame(uName)
hello2=strsplit(uName$uName,"\t")
hello2=matrix(hello2)
hello2=data.frame(hello2)
take3 <- function(x){x[1:3]}
f3 <- t(sapply(hello, take3))
f3=data.frame(f3)
seperated=cbind(hello2,f3)
names(seperated)=c("unit","type","zone","id")
#  aux=do.call(rbind, hello[1:5])  #this didn't work to transfer from list to matrix

############ To get mapping--don't need this anymore
#setwd("S:/All/Portfolio Management/Simpson/transmission_planning/")
#oom.map = read.csv("200810oomcontingencyinput.csv")
######## Merge oom and oom.map--don't need this anymore
#brauning2 = merge(brauning, resource.map, by.x = "resource.unit", by.y = "gencode")
#oomm=merge(oom,oom.map,by.x='CONTIGENCY',by.y='CONTIGENCY',all.x=TRUE)
#om=subset(oomm,Wind=="wind")

################# Recombine tables
om = cbind(oom, seperated)
om$contigency=NULL
names(om)[4]= "value"
om$unit=as.character(om$unit)
# get rid of data to save memory
oom=NULL
uName=NULL
hello2=NULL
take3=NULL
f3=NULL

############ Subset of data that is in the west, do we want to also get OOM up?
unique(om$type)
#should subset into data for oomup and oomdown
omwest=om[grep("^W", om$zone),]  
omnorth=om[grep("^N", om$zone),]
omsouth=om[grep("^S", om$zone),]
omhou=om[grep("^H", om$zone),]
#grep lists place numbers in om and then omwest takes those rows

############ Set NA's to zero and then remove those values
omwest$value[is.na(omwest$value)]=0
omnorth$value[is.na(omnorth$value)]=0
omsouth$value[is.na(omsouth$value)]=0
omhou$value[is.na(omhou$value)]=0

omwest=subset(omwest, value>0)
omnorth=subset(omnorth, value>0)
omsouth=subset(omsouth, value>0)
omhou=subset(omhou, value>0)


############ Save data.frame OM
# save(omwest2,file="c:/temp/westoom.RData")
# omwest2=load(file="c:/temp/westoom.RData")

################# Cast types--can't do right now bc too much data
#omm = subset(om, select = c (report.month, day, interval, value, type, 
                            #zone, id, name, wind))
omw <- cast(omwest, ... ~ type, I)
omn <- cast(omnorth, ... ~ type, I)
oms <- cast(omsouth, ... ~ type, I)
omh <- cast(omhou, ... ~ type, I)

names(omw) <- gsub("_", ".", tolower(names(omw)))
names(omn) <- gsub("_", ".", tolower(names(omn)))
names(oms) <- gsub("_", ".", tolower(names(oms)))
names(omh) <- gsub("_", ".", tolower(names(omh)))



############ Subset of omo that have 1 in oomevdi or oomiol column
omw$oomtest=1
omn$oomtest=1
oms$oomtest=1
omh$oomtest=1

# First replace all NA in columns oomevdi and oomiol with 0
#west
omw$oomevdi[is.na(omw$oomevdi)]=0
omw$oomiol[is.na(omw$oomiol)]=0
omw [["oomtest"]] = with(omw,
                      ifelse (oomevdi==1, 1,
                          ifelse (oomiol==1, 1, 
                              0)))
unique(omw[,c("oomevdi", "oomiol", "oomtest")]) #Checks correctness
omw$report.month2=as.character(omw$report.month)

#north
omn$oomevdi[is.na(omn$oomevdi)]=0
omn$oomiol[is.na(omn$oomiol)]=0
omn [["oomtest"]] = with(omn,
                      ifelse (oomevdi==1, 1,
                          ifelse (oomiol==1, 1, 
                              0)))
#unique(omn[,c("oomevdi", "oomiol", "oomtest")]) #Checks correctness
omn$report.month2=as.character(omn$report.month)

#south
oms$oomevdi[is.na(oms$oomevdi)]=0
oms$oomiol[is.na(oms$oomiol)]=0
oms [["oomtest"]] = sith(oms,
                      ifelse (oomevdi==1, 1,
                          ifelse (oomiol==1, 1, 
                              0)))
#unique(oms[,c("oomevdi", "oomiol", "oomtest")]) #Checks correctness
oms$report.month2=as.character(oms$report.month)

#houston
omh$oomevdi[is.na(omh$oomevdi)]=0
omh$oomiol[is.na(omh$oomiol)]=0
omh [["oomtest"]] = hith(omh,
                      ifelse (oomevdi==1, 1,
                          ifelse (oomiol==1, 1, 
                              0)))
#unique(omh[,c("oomevdi", "oomiol", "oomtest")]) #Checks correctness
omh$report.month2=as.character(omh$report.month)

############################################################
############  Subset data that has an actual oom quantity
cat("Subset data that has actual oom quantity ...\n")
oomdispatchw=subset(omw,oomtest==1)
oomdispatchn=subset(omn,oomtest==1)
oomdispatchs=subset(oms,oomtest==1)
oomdispatchh=subset(omh,oomtest==1)

#west
  oomdispatchw$eoomdnqty[is.na(oomdispatchw$eoomdnqty)]=0
  oomdispatchw$eoomqty[is.na(oomdispatchw$eoomqty)]=0
  oomdispatchwd=subset(oomdispatchw,eoomdnqty>0)
  oomdispatchwu=subset(oomdispatchw,eoomqty>0)
#north
  oomdispatchn$eoomdnqty[is.na(oomdispatchn$eoomdnqty)]=0
  oomdispatchn$eoomqty[is.na(oomdispatchn$eoomqty)]=0
  oomdispatchnd=subset(oomdispatchn,eoomdnqty>0)
  oomdispatchnu=subset(oomdispatchn,eoomqty>0)
#south
  oomdispatchs$eoomdnqty[is.na(oomdispatchs$eoomdnqty)]=0
  oomdispatchs$eoomqty[is.na(oomdispatchs$eoomqty)]=0
  oomdispatchsd=subset(oomdispatchs,eoomdnqty>0)
  oomdispatchsu=subset(oomdispatchs,eoomqty>0)
#Houston
  oomdispatchh$eoomdnqty[is.na(oomdispatchh$eoomdnqty)]=0
  oomdispatchh$eoomqty[is.na(oomdispatchh$eoomqty)]=0
  oomdispatchhd=subset(oomdispatchh,eoomdnqty>0)
  oomdispatchhu=subset(oomdispatchh,eoomqty>0)

############################################################
############  Sum total oom downs by interval by day
names(oomdispatchwd)=gsub("eoomdnqty","value",tolower(names(oomdispatchwd)))
names(oomdispatchwu)=gsub("eoomqty","value",tolower(names(oomdispatchwu)))
names(oomdispatchnd)=gsub("eoomdnqty","value",tolower(names(oomdispatchnd)))
names(oomdispatchnu)=gsub("eoomqty","value",tolower(names(oomdispatchnu)))
names(oomdispatchsd)=gsub("eoomdnqty","value",tolower(names(oomdispatchsd)))
names(oomdispatchsu)=gsub("eoomqty","value",tolower(names(oomdispatchsu)))
names(oomdispatchhd)=gsub("eoomdnqty","value",tolower(names(oomdispatchhd)))
names(oomdispatchhu)=gsub("eoomqty","value",tolower(names(oomdispatchhu)))

## Deletes unnecessary columns
#Down
numberListd = c(5,5,5,5,6,6,6,6,6,6,6)
  for(n in numberListd) {oomdispatchwd[,n]=NULL  }
  for(n in numberListd) {oomdispatchnd[,n]=NULL  }
  for(n in numberListd) {oomdispatchsd[,n]=NULL  }
numberListdh = c(5,5,5,6,6,6,6,6,6,6)
  for(n in numberListdh) {oomdispatchhd[,n]=NULL  }
#Up
numberListu = c(5,5,5,5,5,6,6,6,6,6,6)
  for(n in numberListu) {oomdispatchwu[,n]=NULL  }
  for(n in numberListu) {oomdispatchnu[,n]=NULL  }
  for(n in numberListu) {oomdispatchsu[,n]=NULL  }
numberListuh = c(5,5,5,5,6,6,6,6,6,6)
  for(n in numberListuh) {oomdispatchhu[,n]=NULL  }

################ Add full date
#West
  oomdispatchwd$date=oomdispatchwd$report.month+(oomdispatchwd$day-1)*60*60*24+
                  oomdispatchwd$interval*15*60
  oomdispatchwu$date=oomdispatchwu$report.month+(oomdispatchwu$day-1)*60*60*24+
                  oomdispatchwu$interval*15*60
#North
  oomdispatchnd$date=oomdispatchnd$report.month+(oomdispatchnd$day-1)*60*60*24+
                  oomdispatchnd$interval*15*60
  oomdispatchnu$date=oomdispatchnu$report.month+(oomdispatchnu$day-1)*60*60*24+
                  oomdispatchnu$interval*15*60
#South
  oomdispatchsd$date=oomdispatchsd$report.month+(oomdispatchsd$day-1)*60*60*24+
                  oomdispatchsd$interval*15*60
  oomdispatchsu$date=oomdispatchsu$report.month+(oomdispatchsu$day-1)*60*60*24+
                  oomdispatchsu$interval*15*60
#Houston
  oomdispatchhd$date=oomdispatchhd$report.month+(oomdispatchhd$day-1)*60*60*24+
                  oomdispatchhd$interval*15*60
  oomdispatchhu$date=oomdispatchhu$report.month+(oomdispatchhu$day-1)*60*60*24+
                  oomdispatchhu$interval*15*60


################ Sum intervals
cat("Sum intervals ... \n")
# West
  wd <- aggregate(oomdispatchwd$value, by=list(oomdispatchwd$date), sum) 
  wu <- aggregate(oomdispatchwu$value, by=list(oomdispatchwu$date), sum) 
  colnames(wd)=c("date","wdoomqty")
  colnames(wu)=c("date","wuoomqty")
  wd$date2=as.character(wd$date)
  wu$date2=as.character(wu$date)

# North
  nd <- aggregate(oomdispatchnd$value, by=list(oomdispatchnd$date), sum) 
  nu <- aggregate(oomdispatchnu$value, by=list(oomdispatchnu$date), sum) 
  colnames(nd)=c("date","ndoomqty")
  colnames(nu)=c("date","nuoomqty")
  nd$date2=as.character(nd$date)
  nu$date2=as.character(nu$date)

# South
  sd <- aggregate(oomdispatchsd$value, by=list(oomdispatchsd$date), sum) 
  su <- aggregate(oomdispatchsu$value, by=list(oomdispatchsu$date), sum) 
  colnames(sd)=c("date","sdoomqty")
  colnames(su)=c("date","suoomqty")
  sd$date2=as.character(sd$date)
  su$date2=as.character(su$date)

# Hou
  hd <- aggregate(oomdispatchhd$value, by=list(oomdispatchhd$date), sum) 
  hu <- aggregate(oomdispatchhu$value, by=list(oomdispatchhu$date), sum) 
  colnames(hd)=c("date","hdoomqty")
  colnames(hu)=c("date","huoomqty")
  hd$date2=as.character(hd$date)
  hu$date2=as.character(hu$date)

######################################################################
# Merge 8 sets of data together
cat("Merging the 8 data sets together ...\n")
combine=merge(wd,wu,all=T)
combine=merge(combine,nd,all=T)
combine=merge(combine,nu,all=T)
combine=merge(combine,sd,all=T)
combine=merge(combine,su,all=T)
combine=merge(combine,hd,all=T)
combine=merge(combine,hu,all=T)
head(combine)

# Set all N/A to zero
combine$wdoomqty[is.na(combine$wdoomqty)]=0
combine$wuoomqty[is.na(combine$wuoomqty)]=0
combine$ndoomqty[is.na(combine$ndoomqty)]=0
combine$nuoomqty[is.na(combine$nuoomqty)]=0
combine$sdoomqty[is.na(combine$sdoomqty)]=0
combine$suoomqty[is.na(combine$suoomqty)]=0
combine$hdoomqty[is.na(combine$hdoomqty)]=0
combine$huoomqty[is.na(combine$huoomqty)]=0



# res2 for hourly sums NOT USED
#fulldates=as.POSIXlt(oomdispatch2$date)
#consoldate=(fulldates$year+1900)
#
#oomdispatch2$hour=oomdispatch2$report.month+(oomdispatch2$day-1)*60*60*24+                  
#                  (floor((oomdispatch2$interval)/4))*60*60
#res2 <- aggregate(oomdispatch2$value, 
#        by=list(oomdispatch2$hour), mean) 
#colnames(res2)=c("date","oomqty")
#
#res2$hour2=as.character(res2$date)
#
########SUBSET with wind-only QSE
#units=unique(omwst$unit)
#units=as.data.frame(units)
#names(units)[1]="unit"




#####To check any data by writing to excel
cat("Saving to xls ...\n")
outputFile = paste("testwd.", Sys.Date(), ".xls", sep="")
file.remove(outputFile)
write.xls(wd, outputFile)
outputFile2 = paste("testwu.", Sys.Date(), ".xls", sep="")
file.remove(outputFile2)
write.xls(wu, outputFile2)
outputFile3 = paste("testcbn.", Sys.Date(), ".xls", sep="")
file.remove(outputFile3)
write.xls(combine, outputFile3)

cat("Done.\n")
