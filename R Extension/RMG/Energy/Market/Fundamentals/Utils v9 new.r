
#Products.NYMEX.curve.market(numberOfYears = 5, commodity1= "ImpVol of NG", commodity2 = NA, commodity3=NA,
#start.mo=8, nickname="NG Implied Volatility", num.months=12,	as.of=Sys.Date()-3,
#the.title = "Historical Imp Vol Term Structure as of week ending")
#
#

#Products.NYMEX.curve.market(numberOfYears = 5, commodity1= "Close of NG", commodity2 = NA, commodity3=NA,
#start.mo=8, nickname="NG", num.months=12,	as.of=Sys.Date()-3,
#the.title = "Historical Fwd Strip Term Structure as of week ending")


#  Products.NYMEX.curve.market(numberOfYears = 5, 
#  							commodity1= "0.42*Close of HO", commodity2 = "2*0.42*Close of HU", commodity3="-3*Close of CL",
#   								start.mo=8, nickname="3-2-1 Crack",
#   				             num.months=12,	as.of=Sys.Date()-1,
#   				             Verbose=F)


# Standing on a given day of the year (e.g. today or a week ago)
# pull out all the forward months (.e.g 24 of them) for a given commodity
# and plot it year over year in a seasonality-like plot.
# This shows the Historical Forward Strip Term Structure as of a given date.

yearly.curve.retriever3 <- function(commodity1 = NA, commodity2 = NA, commodity3 = NA, 
nickname= paste(Uts.String.right(commodity1,2), "-", Uts.String.right(commodity2,2)),
start.mo=strsplit(as.character(Sys.Date()),"-")[[1]][2],
start.year=strsplit(as.character(Sys.Date()),"-")[[1]][1], num.months=36,	
as.of = format(Sys.Date()-1, "%m/%d/%Y"), Plot=F)

{
  Verbose=F
	# Get the signs out of each commodity; leave the commodity without a sign
	if(!is.na(commodity1)) {
			if(Uts.String.left(commodity1,1)%in% c("+","-")) {
			sign1 <- Uts.String.left(commodity1,1) 
			commodity1 <- Uts.String.right(commodity1,length(Uts.String.str.to.vec(commodity1))-1)
		} else {
			sign1 <-"+"; commodity1 <- Uts.String.right(commodity1,length(Uts.String.str.to.vec(commodity1)))
		}
	}
	
	if(!is.na(commodity2)) {
		if(Uts.String.left(commodity2,1)%in% c("+","-")) {
			sign2 <- Uts.String.left(commodity2,1) 
			commodity2 <- Uts.String.right(commodity2,length(Uts.String.str.to.vec(commodity2))-1)
		} else {
			sign2 <-"+"; 
			commodity2 <- Uts.String.right(commodity2,length(Uts.String.str.to.vec(commodity2)))
		}
	}
	
	if(!is.na(commodity3)) {
		if(Uts.String.left(commodity3,1)%in% c("+","-")) {
			sign3 <- Uts.String.left(commodity3,1) 
			commodity3 <- Uts.String.right(commodity3,length(Uts.String.str.to.vec(commodity3))-1)
		} else {
			sign3 <-"+"; 
			commodity3 <- Uts.String.right(commodity3,length(Uts.String.str.to.vec(commodity3)))
		}
	}
	
	# Make a sequence of dates from the first day of the start month (and year) to the date corresponding
	# to num.months later. month.seq and year.seq are the months and years corresponding to these dates.
	
	date.seq <- seq(as.Date(paste(start.year,"-",start.mo,"-1", sep="")), length=num.months, by="months")
	
  month.seq <- format(date.seq, "%m")
	year.seq <- format(date.seq, "%Y")
	
	# month.seq gives a vector of complete names
  # month.seq <- months(date.seq)
	
  # Get the contract month letters from "month number"
	month.letters <- as.vector(apply(data.frame(month.seq), 1, Uts.Other.contract.letters))

	# Make the query
	if(!is.na(commodity3)) {		# cracks of 3 commodities
		ATTR <- paste(seq(1, length(month.letters), by=1),
    ": IF ", commodity1, "_", year.seq, month.letters, " is defined AND ", commodity2," is defined AND ", commodity3
    , " is defined THEN ",  "1 week average of ", commodity1, "_",  year.seq, month.letters, 
    sign2, "1 week average of ", commodity2, "_",  year.seq, month.letters, 
    sign3, "1 week average of ", commodity3, "_",  year.seq, month.letters," ENDIF", "\n",  sep="")
  }	else if(!is.na(commodity2)) {		# spread of two commodities
		ATTR <- paste(seq(1, length(month.letters), by=1),
   ": IF ", commodity1, "_", year.seq, month.letters, " is defined AND ", commodity2," is defined THEN ",
	 "1 week average of ", commodity1, "_",  year.seq, month.letters, " - 1 week average of ", commodity2, "_",  year.seq, month.letters, " ENDIF", "\n",  sep="")
	 }else if(!is.na(commodity1)) {				# Simple futures
		ATTR <- paste(seq(1, length(month.letters), by=1),
  ": IF ", commodity1, "_", year.seq, month.letters, " is defined THEN ",
  "1 week average of ", commodity1, "_",  year.seq, month.letters, " ENDIF", "\n",  sep="")
	 } 

	query <- paste("%exec.missingdatananfill: Fill_Forward \n", "%exec.holidaynanfill: Fill_Forward \n",
	"SHOW", "\n", paste(eval(as.name(paste("ATTR", sep=""))), collapse="\n"), "\n", "WHEN Date is ", 
  as.of, sep="")
	if (Verbose) cat(query)
	
	data.lim <- read.LIM(query)
	# data <- import.data.mim(query, ColumnSpec=list(1), Server=LIMsettings[[1]], Port=LIMsettings[[2]])

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

	FC <- as.vector(t(data.lim[,2:length(data.lim[1,])]))
	contract <- paste(year.seq, month.letters, sep="")

	if(Plot) {
		plot(x=1:length(FC), y=FC,
			main=paste("NYMEX ", nickname, " Forward Curve",sep=""), sub="",
			xlab="Contract", ylab=paste("Value as of",as.of),
			col=1, xaxt="n", ylim=range(FC,0,na.rm=T))
		lines(x=1:length(FC), y=FC, lwd=3, col=5)
		axis(side=1, at=1:length(FC), labels=contract)
	}
    eval(parse(text=paste("ret.frame <- data.frame(contract, ", paste("FC",start.year, sep=""),"=FC)")))
	return(ret.frame)
}



Products.NYMEX.curve.market <- function(numberOfYears=6, num.months=36, start.mo= as.character(as.integer(strsplit(as.character(Sys.Date()),"-")[[1]][2])+1), 
as.of = Sys.Date()-1, commodity1 = NA, commodity2=NA, commodity3=NA, the.title = "Historical Fwd Strip Term Structure as of week ending",  nickname= "Commodity") {

# The following cmd forces as.of to be numerical in case user enters it as a character Date
as.of = as.Date(as.of, "%m/%d/%Y")

#the.title = paste(the.title,as.of)
Verbose=F
	
  # Check nickname.  Can only have dots and letters since it will become the name of columns
	nickname <- nickname.check(nickname)

	current.year <- as.integer(strsplit(as.character(Sys.Date()),"-")[[1]][1])
	start.year <-   as.integer(strsplit(as.character(Sys.Date()),"-")[[1]][1]) - numberOfYears
	year.span <- start.year : current.year
	
	the.data.frame <- (Month.out = 1:num.months)
	colNames <-c("Month.out")
	for (i in year.span) {
		eff.date <- format(as.of - 365*(current.year-i), "%m/%d/%Y")
		                                           	
    eval(parse(text=paste("NYMEX.Curve <- yearly.curve.retriever3(commodity1=commodity1, commodity2=commodity2, commodity3=commodity3, start.mo=start.mo, start.year=",i,", num.months=num.months,	as.of=eff.date)",sep="")))
		eval(parse(text=paste("the.data.frame <- (data.frame(the.data.frame, NYMEX.Curve))",sep="")))
		eval(parse(text=paste("colNames <- append(colNames,c(\"contract",i,"\",\"FC",i,"\"))",sep="")))
	}
	dimnames(the.data.frame)[[2]] <- colNames
	# In R
  eval(parse(text=paste("assign(\"NYMEX.",nickname,".Curve\", the.data.frame, env = .GlobalEnv, immediate=T)",sep="")))

	
	# Create/print a message that informs user what the data is stored
  
  cat("\n", paste("PLEASE NOTE: The data for ", nickname, " is a data frame called NYMEX.", nickname,".Curve", sep=""), "\n")
  
 
	######### CREATE a message box to display above message ##############
	
  #	create the x vector for the plot of all the forwards together
	the.x <- vector(mode= "character",0)
	
  for (i in 1: nrow(the.data.frame)) {
		the.x[i] <- Uts.String.right(as.character(the.data.frame[i,2]),1)
	}

	# Find range for plots.  First put only the y data together into a table
	the.colors = c( "yellow", "turquoise","green" , "blue" ,"orange","pink", "coral", "red", "black", 
  "mediumturquoise", "orchid", "hotpink",  "maroon","grey","darkgrey", "darkblue", "darkgreen", 
  "brown","lavender", "darkgreen", "firebrick", "olivedrab")            
  

	lineType = 1:30   
	yrange = range(the.data.frame[,seq(3, length(the.data.frame), 2) ],na.rm=T)  #find.the.range(the.data.frame); #
	# the.title <- paste("Historical Fwd Strip Term Structure as of week ending",as.of)
	
  plot(x=seq(1, num.months,1), y=the.data.frame[,3],  
  ylim = yrange, type="l", sub=nickname, col=the.colors[3], lty=3, xlab="Months Out",
  ylab="Value")
  
  mtext(the.title, side=3, line=2.5, font=2 )  

	whichCol <- (numberOfYears + 1)*2
	
  axis(side=3, at=1:num.months, as.character(the.data.frame[,whichCol]), cex.axis=0.8)
	
	#### TO FINISH:
  #### CHANGE THICKNESS AND DARKER COLOR AS TIME GETS CLOSER TO CURRENT DATE!!! 
  
  for (i in seq(5, length(the.data.frame),2) ) {
		if(i==length(the.data.frame)) {
			lineWidth <- 4
		} else {
			lineWidth <- 3
		}
		lines(x=seq(1, num.months,1), y=the.data.frame[,i], col =the.colors[i], lty=lineType[i],lwd=lineWidth)
	}
	
		
	keystrings <- dimnames(the.data.frame)[[2]][seq(3, length(the.data.frame), 2)]
	      
  legend(x="topleft", inset=0,legend=keystrings, fill=the.colors[seq(3,length(the.data.frame),2)], bty="n", 
  col=the.colors[seq(3,length(the.data.frame),2)], text.col=the.colors[seq(3,length(the.data.frame),2)], 
  lty=seq(3,length(the.data.frame),2), cex=0.65)
  
	return(the.data.frame)
}

#  Testing yearly.curve.retriever3 
# default of as.of is yesterday date (mm/dd/yyyy)

# yearly.curve.retriever3(commodity1 = "NG", commodity2 = NA, commodity3 = NA, start.mo = 7, start.year = 2006, num.months=6, as.of = "06/05/2006")
#yearly.curve.retriever3(commodity1 = "NG", commodity2 = NA, commodity3 = NA, start.mo = 7, start.year = 2006, num.months=6, Verbose = F, Plot=T)
#




     
FC.TermStruc.Initialize <- function(NumYrs, NumMonthsTS, StartMoTS,
AsOfTS, comdt, the.title.TS, nick, directory="C:/BalanceSheets/UpdatedFiles/"){

  # Create NYMEX NG forward curve
  
  jpeg(paste(directory,"TermStruc.NG.Fwrd.jpg",sep=""), width = 420, height = 350)
  TS.FC.NG <- Products.NYMEX.curve.market(NumYrs, NumMonthsTS, StartMoTS,
  AsOfTS, comdt, NA, NA, the.title.TS, nick)
  dev.off()

  return(TS.FC.NG)
}


ImpVol.TermStruc.Initialize <- function(NumYrs, NumMonthsTS, StartMoTS, 
AsOfTS, comdtVol, the.title.Vol, nick, directory="C:/BalanceSheets/UpdatedFiles/"){

  # Create NYMEX NG forward curve
  
  jpeg(paste(directory,"TermStruc.ImpVol.NG.jpg",sep=""), width = 420, height = 350)
  TS.ImpVol.NG <- Products.NYMEX.curve.market(NumYrs, NumMonthsTS, StartMoTS, AsOfTS, comdtVol ,NA, NA, the.title.Vol, nick)
  dev.off()

  return(TS.ImpVol.NG)
}








# Testing Gas.NYMEX.curve.market

#Gas.NYMEX.curve.market(start.mo=strsplit(as.character(Sys.Date()),"-")[[1]][2],
#start.year=strsplit(as.character(Sys.Date()),"-")[[1]][1], num.months=6, 
#as.of=format(Sys.Date()-1, "%m/%d/%Y"))


# FUNCTION Gas.NYMEX.curve.market
# Returns a constructed NYMEX forward curve based on market data from LIM.
# (6/2/2003)

#Gas.NYMEX.curve.market(start.mo=7,start.year=strsplit(as.character(Sys.Date()),"-")[[1]][1],num.months=12, 
#as.of=format(Sys.Date()-5, "%m/%d/%Y"))


Gas.NYMEX.curve.market <- function(start.mo=strsplit(as.character(Sys.Date()),"-")[[1]][2],
start.year=strsplit(as.character(Sys.Date()),"-")[[1]][1],num.months=36, 
as.of=format(Sys.Date()-1, "%m/%d/%Y"))
{
	# Make a sequence of dates from the first day of the start month (and year) to the date corresponding
	# to num.months later. month.seq and year.seq are the months and years corresponding to these dates.
	
	
	# Notes about function's arguments
	# start.mo is a string
	# start.year is a string
	# num.months is an interger
	# as.of is a "mm/dd/yyyy" string
  Verbose=FALSE
  start.mo=as.character(start.mo)
  start.year = as.character(start.year)
  date.seq <- seq(as.Date(paste(start.year,"-",start.mo,"-1", sep="")), length=num.months, by="months")
	
  month.seq <- format(date.seq, "%m")
	year.seq <- format(date.seq, "%Y")
	
	# month.seq gives a vector of complete names
  # month.seq <- months(date.seq)
	
  # Get the contract month letters from "month number"
	month.letters <- as.vector(apply(data.frame(month.seq), 1, Uts.Other.contract.letters))
	
	# Make the query
	ATTR <- paste(seq(1, length(month.letters), by=1),
  ": IF NG_", year.seq, month.letters, " IS defined THEN ",
  "Close of NG_",  year.seq, month.letters, " ENDIF", "\n",  sep="")

	query <- paste("SHOW", "\n", paste(eval(as.name(paste("ATTR", sep=""))), collapse="\n"), "\n",  "WHEN Date is ", as.of, sep="")
	if(Verbose==TRUE){cat(query)}
	data.lim <- read.LIM(query)
  
	if(nrow(data.lim)==0)
	{
		message(paste("There is no NYMEX gas data for the specified period as of ", as.of, ". Please select another day.", sep=""))
		return()
	}

	if(length(which(!is.na(unlist(data.lim[1,2:length(data.lim[1,])]))))==0 & length(data.lim[,1])==1)
	{
		message(paste("There is no NYMEX gas data for the specified period as of ", as.of, ". Please select another day.", sep=""))
		return()
	}

	FC <- as.vector(t(data.lim[,2:length(data.lim[1,])]))
	contract <- paste(year.seq, month.letters, sep="")
	
  #graphsheet(Name="NYMEX Curve")
	plot(x=1:length(FC), y=FC,
		main="NYMEX Gas Forward Curve", sub="",
		xlab="Contract", ylab=paste("Value as of",as.of),
		col=1, xaxt="n", ylim=range(FC,na.rm=T))   
    
  lines(x=1:length(FC), y=FC, lwd=3, col=5)
	axis(side=1, at=1:length(FC), labels=contract)
	ret.frame <- data.frame(contract, FC)
	assign("NYMEX.Gas.Curve", ret.frame, env =.GlobalEnv, immediate=T)
	
	
	# make sure this function does what it is supposed to do (i.e. like quiOpenView)
  # message("To DO")
  
  # S-Plus Command
  # guiOpenView("data.frame", "NYMEX.Gas.Curve")

  #print(head(NYMEX.Gas.Curve,2))
  
  return(ret.frame)
}

# No Longer used
#"Uts.String.3way.split" = function(string, sep = ".")
#{
#		 ## split string into two pieces
#		 ##   according to the separator character sep
#		 library(Hmisc)
#		 x <- unPaste(string, sep)
#		 return(list(first = x[[1]], second = x[[2]], third = x[[3]]))
#}

# No Longer Used
#"Uts.String.split" = function(string, sep = ".")
#{
#		 ## split string into two pieces
#		 ##   according to the separator character sep
#		 x <- unpaste(string, sep)
#		 return(list(first = x[[1]], second = x[[2]]))
#}


FC.NG.Initialize <- function(StartMo, StartYr, NumMonths, AsOf, directory="C:/BalanceSheets/UpdatedFiles/"){
  
  # Create NYMEX NG forward curve
  jpeg(paste(directory,"NG.forward.curve.jpg",sep=""),width = 420, height = 350)
  NG.FC <<- Gas.NYMEX.curve.market(StartMo, StartYr, NumMonths, AsOf)
  dev.off()
  
  return(NG.FC)
}



"Uts.Other.contract.letters" = function(Month)
{
		 months.names <- as.vector(months(as.Date(paste(Month, "/1/2000", sep = ""),format="%m/%d/%Y")))
		 letter <- switch(months.names,
		 		 January = "F",
		 		 February = "G",
		 		 March = "H",
		 		 April = "J",
		 		 May = "K",
		 		 June = "M",
		 		 July = "N",
		 		 August = "Q",
		 		 September = "U",
		 		 October = "V",
		 		 November = "X",
		 		 December = "Z")
		 return(letter)
}


"Uts.String.right" = function(str, num)
{
		 if(nchar(str) < num)
		 		 return("")
		 vec <- Uts.String.str.to.vec(str)
		 ret <- paste(vec[(nchar(str) - (num - 1)):nchar(str)], collapse = "")
		 return(ret)
}

"Uts.String.left" = function(str, num)
{
		 if(nchar(str) < num)
		 		 return("")
		 vec <- Uts.String.str.to.vec(str)
		 ret <- paste(vec[1:num], collapse = "")
		 return(ret)
}

"Uts.String.str.to.vec" = function(string)
{
		 n <- nchar(string)
		 substring(string, 1.:n, 1.:n)
}


nickname.check <- function (nickname) {
	temp <- Uts.String.str.to.vec(nickname)
	
	for (i in 1:length(temp)) {
		if(temp[i] !="." &  ( !temp[i]%in%c(letters,1,2,3,4,5,6,7,8,9,0,".") ) &  ( !temp[i]%in%c(LETTERS,1,2,3,4,5,6,7,8,9,0,".") ) ) temp[i] <- "."
	}
	return(Uts.String.vec.to.str(temp) )
}

"Uts.String.vec.to.str" = function(vec)
{
		 paste(vec, collapse = "")
}


# Was never used. Uses older ts class in SPLUS
#ts.format <- function(ts.symbol, start.date=) {
#	LIMquery <- "SHOW COSXPUS: DOE.COSXPUS WHEN date is last day of the month"
#	col.tags <- c(1)
#	the.data <<- na.omit(import.data.mim(LIMquery,col.tags, LIMsettings[[1]], 0))
#	the.month <- as.integer(months(the.data[1,1]))
#	the.year <- as.integer((Uts.String.right(as.character(the.data[1,1]),4)))
#	the.start <- the.year + (the.month-1)/12
#	the.data.ts <- ts(data=the.data[,2:length(the.data)],start=the.start,deltat=1/12)

# 	uuu <- ts(rnorm(36),start=1989,freq = 12)

#	return(the.data.ts)
#}


"Uts.String.zap.spaces" = function(string)
{
		 n <- nchar(string)
		 x <- substring(string, 1:n, 1:n)
		 paste(x[x != " "], collapse = "")
}

                               
"Uts.String.sub" = function(pattern, replacement, x)
{
		 # (pat) in pattern and matching \1 in replacement are not implemented.
		 # E.g., gsub(pat="([ab][ab]*)", rep="<\\1>", "abcd") will not return "<ab>cd".
		 gsub1 <- function(pattern, replacement, x)
		 {
		 		 val <- character()
		 		 while(nchar(x) > 0 && (re <- regexpr(pattern, x)) > 0) {
		 		 		 if((ml <- attr(re, "match.length")) > 0) {
		 		 		 		 val <- paste(val, substring(x, 1, 1 + re -
		 		 		 		 		 2), replacement, sep = "")
		 		 		 		 x <- substring(x, re + ml)
		 		 		 }
		 		 		 else {
		 		 		 		 val <- paste(sep = "", val, substring(x, 1,
		 		 		 		 		 1 + re - 1), replacement)
		 		 		 		 x <- substring(x, re + ml + 1)
		 		 		 }
		 		 }
		 		 if(nchar(x) > 0)
		 		 		 val <- paste(sep = "", val, x)
		 		 val
		 }
		 unlist(lapply(x, gsub1, pattern = pattern[1], replacement = 
		 		 replacement[1]))
}



####  LIM.Gas.storage.weekly is not USED  #####

#LIM.Gas.storage.weekly("Consuming East", 6)
"LIM.Gas.storage.weekly" = function(survey, num.years)
{
	if(survey == "Consuming East") {
		attr <- "Weekly GasStorage of front EIA.CONSUMING.E"
		attrD <- "Weekly GasStorageChange of front EIA.CONSUMING.E"
	}
	else if(survey == "Consuming West") {
		attr <- "Weekly GasStorage of front EIA.CONSUMING.W"
		attrD <- "Weekly GasStorageChange of front EIA.CONSUMING.W"
	}
	else if(survey == "Producing") {
		attr <- "Weekly GasStorage of front EIA.PRODUCING"
		attrD <- "Weekly GasStorageChange of front EIA.PRODUCING"
	}
	else if(survey == "Total") {
		attr <- "Weekly GasStorage of front EIA.TOTAL.US"
		attrD <- "Weekly GasStorageChange of front EIA.TOTAL.US"
	}
	EXEC.statement <- ""
	label.1 <- Uts.String.zap.spaces(survey)
	label.2 <- paste("Delta", Uts.String.zap.spaces(survey), sep = "")
	LET.statement <- paste("LET ATTR ", label.1, " = ", attr, "\n", "LET ATTR ", label.2, " = ", attrD, sep = "")
	SHOW.statement <- paste("SHOW ", "\n", label.1, "0", ": ", label.1, sep = "")
	col.tags <- c(as.character(paste(label.1, "0", sep = "")), rep(" ", num.years * 2 + 1))
	for(i in 1:num.years) {
		temp.label <- paste(label.1, i, ":", sep = "")
		SHOW.statement <- paste(SHOW.statement, temp.label, label.1, i, "years ago", sep = " ")
		col.tags[i + 1] <- paste(label.1, i, sep = "")
	}
	SHOW.statement <- paste(SHOW.statement, "\n", label.2, "0", ": ", label.2, sep = "")
	col.tags[num.years + 2] <- paste(label.2, "0", sep = "")
	for(i in 1:num.years) {
		temp.label <- paste(label.2, i, ":", sep = "")
		SHOW.statement <- paste(SHOW.statement, temp.label, label.2, i, "years ago", sep = " ")
		col.tags[num.years + i + 2] <- paste(label.2, i, sep = "")
	}
	WHEN.statement <- "WHEN Date is within 1 year and date is Thursday"
	LIM.query <- paste(EXEC.statement, LET.statement, SHOW.statement, WHEN.statement)
	return(list(LIM.query = LIM.query, col.tags = col.tags))
}

#  Changed Function
add.NAs.toYearsEnd <- function (x.df) {
	dimNames <- dimnames(x.df)[[2]]
 	   
	numRows <- nrow(x.df)
	# Find the last month of data in the data frame
	date.components = strsplit(as.character(x.df$Date[nrow(x.df)]), "-")
  # e.g. for date.components 
  # "2007" "12"   "01"  
  last.month <- as.integer(date.components[[1]][2])
	if (last.month == 12) return(x.df)
	
  if (last.month != 12){
  last.year <- as.integer(date.components[[1]][1])
	
	# Fill in by as many months as it takes to the end of the year
	the.dates <- as.character(x.df[,1])
	the.values <- x.df[,2]
	
    for (i in 1:(12-last.month)) {
  		the.dates <- append(the.dates, paste(last.year,"-",last.month+i,"-","1",sep=""))
  		the.values <- append(the.values,NA)        
  	}
  
	x.df <- data.frame(strptime(the.dates, format="%Y-%m-%d"),the.values)
	dimnames(x.df)[[2]] <- dimNames
	
	return(x.df)
  }
}


"Uts.String.replace.char" = function(target, oldchar, newchar)
{
		 ## Replace all occurrences of oldchar with newchar in target string
		 vec <- Uts.String.str.to.vec(target)
		 vec[vec == oldchar] <- newchar
		 string <- Uts.String.vec.to.str(vec)
		 string
}







