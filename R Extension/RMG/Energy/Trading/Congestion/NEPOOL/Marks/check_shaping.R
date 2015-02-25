# Check forward shaping vs. history
#
# Erik spreadsheet location:
# S:\Trading\Bucht\Nepool Marks Inputs.xls, modify the Roll sheet!
#
#

require(CEGbase); require(FTR); require(PM)
require(lattice)


FTR.load.ISO.env("NEPOOL")
NEPOOL$hP <- NULL

ptid <- 4004; deliveryPoint <- "CT DA"

spreadLMP <- PM:::.get_hist_fwd_stacked(ptid=ptid,
  deliveryPoint=deliveryPoint, buckets=c("Peak", "Offpeak", "Flat",
  "7X8", "2X16H"), endDate=as.Date("2015-12-01"))

## spreadLMP$mon  <- as.numeric(format(spreadLMP$month, "%m"))
## spreadLMP$year <- as.numeric(format(spreadLMP$month, "%Y"))

## xyplot(spread ~ mon | bucket, data=spreadLMP, groups=year,
##   type="o", scales="free", subset=source=="Historical")


months <- seq(as.Date("2003-01-01"),as.Date("2009-12-01"),by="1 month")
shape0309 <- .calc_shapefactors(spreadLMP, months)

correctedShape0309 <- .constrain_shapefactors(shape0309)
print(correctedShape0309, row.names=FALSE)

fwdmonths <- subset(spreadLMP, source=="Fwd")$month
shape  <- correctedShape0309
thisbucket <- "OFFPEAK"  #"PEAK"
fwdSpread <- .apply_shape_fwd(spreadLMP, fwdmonths, shape, bucket=thisbucket)


# print the results ... 
res <- cast(fwdSpread, year ~ mon, I, fill=NA, value="newspread",
  subset=bucket==thisbucket)
res[,2:ncol(res)] <- round(res[,2:ncol(res)],2) 
print(res, row.names=FALSE)







######################################################################


marksForDate <- Sys.Date()
asOfDate  <- as.Date(secdb.dateRuleApply(marksForDate, "-0b")) 
startDate <- as.Date(secdb.dateRuleApply(marksForDate, "+1e+1d"))
endDate   <- as.Date("2020-12-01")   # mark up to here   
toMonths  <- seq(startDate, endDate, by="1 month")


ptid <- 4001; deliv.pt <- "MAINE DA"

histNodeMark <- PM:::.getHistMark(ptid)
histNodeMark$source <- "Historical"

secDbNodeMark <- PM:::.getSecDbMark(deliv.pt, asOfDate, startDate, endDate)

#skipDates <- seq(as.Date("2003-01-01"), as.Date("2003-12-31"), by="1 day")
skipDates <- NULL
pmView <- fwd.PMview.default(list(ptid=ptid, nodeType="LOAD"), toMonths,
  skipDates=skipDates)


#subset(pmView, component=="congestion" & bucket=="5X16")
## GG <- cast(pmView, month ~ bucket, I, fill=NA,
##   subset=component=="congestion")

pmMark <- constructNodeMark(pmView, asOfDate, startDate, endDate)
pmMark$source <- "pmView"


# rbind for plotting
allMarks <- rbind(histNodeMark, secDbNodeMark, pmMark)  
allMarks <- subset(allMarks, month >= as.Date("2004-01-01"))
spread <- plotMarks(allMarks, deliv.pt, main=paste("Hub to", deliv.pt),
                    hist=72, fwd=48, save=FALSE)
