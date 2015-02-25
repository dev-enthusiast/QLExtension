
# Example
#get.TS.Fcurve(commodity1="Close of NG", start.mo=9, start.year=2006, num.months=6, as.of = "6/1/2006", Verbose=F, 
#the.title="NG Term Structure", ToSave=F, directory = "C:/BalanceSheets/UpdatedFiles/", savefile="NG_TS_FC_3D")
##
# get.TS.Furve generates the term structure of Fwrd curves for a given number of contracts for all dates after "as.of" 
# and either plot or save a 3D plot of hist term structure of Fwd curves.

# 'commodity1' is a string. the symbol of the commodity e.g. "Close of NG"
# 'start.mo' is the start month of the fwd curve
# 'start.year' is the start year of the fwd curve
# 'num.months' is the number of futures contract you want to display in your fwd curve
# 'as.of' is the historical date - initial pricing date- of format "m/d/yyyy"
# 'the.title' is the title of the 3D plot
# 'ToSave' is a logical input; if true the 3D plot is saved in 'savefile'
# 'savefile' provides the name of the 3D plot e.g. "NG_TS_FC_3D". This will be save as a jpeg in the same directory as the code "directory input".
# 'directory' (required) is the path where function 'Utils.r' is saved and where the 3D plot will be stored e.g. directory = "C:/BalanceSheets/UpdatedFiles/"

get.TS.Fcurve <- function(commodity1, start.mo, start.year, num.months, as.of, Verbose=F, the.title, ToSave=T, directory = "C:/BalanceSheets/UpdatedFiles/", savefile="")
{
  source(paste(directory,"Utils.r",sep="")) # Not needed if called from 'Fundamentals.xls'
  start.mo=as.character(start.mo)
  start.year = as.character(start.year)
  as.of = format(as.Date(as.of, format="%m/%d/%Y")-1, "%m/%d/%Y")
	
  # Get the signs out of each commodity; leave the commodity without a sign
	if(!is.na(commodity1)) {
			if(Uts.String.left(commodity1,1)%in% c("+","-")) {
			sign1 <- Uts.String.left(commodity1,1)
			commodity1 <- Uts.String.right(commodity1,length(Uts.String.str.to.vec(commodity1))-1)
		} else {
			sign1 <-"+"; commodity1 <- Uts.String.right(commodity1,length(Uts.String.str.to.vec(commodity1)))
		}
	}

	# Make a sequence of dates from the first day of the start month (and year) to the date corresponding
	# to num.months later. month.seq and year.seq are the months and years corresponding to these dates.

	date.seq <- seq(as.Date(paste(start.year,"-",start.mo,"-1", sep="")), length=num.months, by="months")

  month.seq <- format(date.seq, "%m")
	year.seq <- format(date.seq, "%Y")

  year.temp = apply(matrix(year.seq,nc=1),1,Uts.String.right,1)
	# month.seq gives a vector of complete names
  # month.seq <- months(date.seq)

  # Get the contract month letters from "month number"
	month.letters <- as.vector(apply(data.frame(month.seq), 1, Uts.Other.contract.letters))

	# Make the query for simple futures
  if(!is.na(commodity1)) {
    contract <- paste(month.letters, year.temp, sep="")


  ATTR <- paste(contract,": IF ", commodity1, "_", year.seq, month.letters, " is defined THEN ",
  "1 week average of ", commodity1, "_",  year.seq, month.letters, " ENDIF", "\n",  sep="")

	 }

	query <- paste("%exec.missingdatananfill: Fill_Forward \n", "%exec.holidaynanfill: Fill_Forward \n",
	"SHOW", "\n", paste(eval(as.name(paste("ATTR", sep=""))), collapse="\n"), "\n", "WHEN Date is after ",
  as.of, "\n", "and date is Wednesday", sep="")
	if (Verbose) cat(query)

	data.lim <- read.LIM(query)

  if(nrow(data.lim)==0)
	{
		print(data.lim)
		#display.messagebox(paste("There is no NYMEX gas data for the specified period as of ", as.of, ". Please select another day.", sep=""))
		message(paste("There is no NYMEX gas data for the specified period as of ", as.of, ". Please select another day.", sep=""))
		return()
	}

  if(length(which(!is.na(unlist(data.lim[1,2:length(data.lim[1,])]))))==0 & length(data.lim[,1])==1)
 	{
 		junk <<- data.lim
 		print(paste("There is no NYMEX gas data for the specified period as of ", as.of, ". Please select another day.", sep=""))
 		stop("Error")
 	}

  # Now create a 3D Plot
  require(TeachingDemos)
  require(tcltk)
  require(lattice)
  require(tkrplot)
  TS_FC = t(as.matrix(data.lim[,-1]))
  colnames(TS_FC) <- format(as.Date(data.lim[,1],"%m/%d/%Y"), "%d%b")

  TS_FC_rot =  TS_FC[,sort(1:ncol(TS_FC), decreasing = TRUE)]
  if(ncol(TS_FC_rot)<=6){y.denom <- ncol(TS_FC_rot)}else{y.denom<-6}
  if(nrow(TS_FC_rot)<=6){x.denom <- nrow(TS_FC_rot)}else{x.denom <-6}
  
  if(ToSave == TRUE){
  jpeg(paste(directory, savefile, ".jpg", sep=""), width = 640, height = 520)

  # Ugly code, one could use the function "pretty"
  
  print(wireframe(TS_FC_rot, scales =list(cex=0.8,distance = c(2,10,2),x=list(arrows=FALSE, at=seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom)),
  labels=c(rownames(TS_FC_rot)[seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom))]),rot=0)
  ,y=list(arrows=FALSE,cex=0.8, tck=.5, at= seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom)), labels=c(colnames(TS_FC_rot)[seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom))]), rot=45)
  ,z=list(arrows=FALSE)),ylab="Contracts",xlab="", zlab="", drape=TRUE, main=the.title))
  dev.off()
  }else{
  wireframe(TS_FC_rot, scales =list(cex=0.8,distance = c(2,10,2),x=list(arrows=FALSE, at=seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom)),
  labels=c(rownames(TS_FC_rot)[seq(1,nrow(TS_FC_rot),by=floor(nrow(TS_FC_rot)/x.denom))]),rot=0)
  ,y=list(arrows=FALSE,cex=0.8, tck=.5, at= seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom)), labels=c(colnames(TS_FC_rot)[seq(1,ncol(TS_FC_rot),floor(ncol(TS_FC_rot)/y.denom))]), rot=45)
  ,z=list(arrows=FALSE)),ylab="Contracts",xlab="", zlab="", drape=TRUE, main=the.title)
  }
}




