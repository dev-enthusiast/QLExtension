# Linear Prodiction Function
projectedVal <- function(x,y,PredictYr) {
     new.x <- data.frame(x = as.integer(PredictYr))
     predict(lm(y ~ x), new.x)
}


# Funtion to Plot inner, outer and projected values

"LD.Plot.bandplot" = function(lowOuter, highOuter, lowInner, highInner, Normal.outer, Projected, Modeled,past.12.val, Normal.Inner, xlab="Months Out", ylab="Value", keyStrings, the.title, the.subtitle, ...)
{
		# Projected values are temporary removed
    # lowOuter and highOuter are lists containing
		# the x (as their first component) and the y (as their second component) vectors

    oldpar <- par(no.readonly=TRUE)
    
		# Plot the Outer Band
		x.range <- range(lowOuter[[1]],highOuter[[1]])
		y.range <- range(unlist(lowOuter[[2]]), unlist(highOuter[[2]]),Projected, Modeled, past.12.val)
		plot(type="n",1:4, xlim=x.range, ylim=y.range, axes=T, ylab="value", xlab="Months Out")


	  if(class(lowOuter[[1]]) == "POSIXt" | class(lowOuter[[1]]) == "POSIXlt" ) {
				 x <- strptime(c(as.character(lowOuter[[1]]), rev(as.character(lowOuter[[1]]))), format = "%Y-%m-%d")
		}  else { x <- c(lowOuter[[1]], rev(lowOuter[[1]]))}


		y <- c(unlist(highOuter[[2]]), unlist(rev(lowOuter[[2]])) )
		polygon(x, y, density=-1, border="paleturquoise", col="paleturquoise")
		#polygon(x, y, density=-1, border="tan", col="tan")

		# Plot the Inner Band

    par(new=T, xaxs="d")

    if(class(lowInner[[1]]) == "POSIXt" | class(lowInner[[1]]) == "POSIXlt" ) {
		 		 x <- strptime(c(as.character(lowInner[[1]]), rev(as.character(lowInner[[1]]))), format = "%Y-%m-%d")
		}  else { x <- c(lowInner[[1]], rev(lowInner[[1]])) }

    y <- c(unlist(highInner[[2]]), unlist(rev(lowInner[[2]])) )
    polygon(x, y, density=-1, border="lightgrey", col="lightgrey")
    # col=par("col"))
    
	 	lines(x=1:12, y=Normal.outer, col="mediumturquoise", lwd=3, lty=3)   # Many year norm (as many as requested in parent routine)
	 	lines(x=1:12, y=Projected, col="darkorange", lwd=3, lty=2)   # Projected futures values by Regression
	 	lines(x=1:12, y=Modeled, col="red", lwd=3, lty=2)   # Modeled Values, currently we are using EIA values
    lines(x=1:12, y=past.12.val, col="darkgreen", lwd=3, lty=1) # Recent past 12 months
    lines(x=1:12, y=Normal.Inner, col="darkgrey", lwd=3, lty=3)   # 3-year norm (as many as requested in parent routine)
		title(main=the.title, sub=the.subtitle)

    
    legend(x="topleft", inset=0, legend=keyStrings, cex=.8,fill=c("mediumturquoise","darkorange","red","darkgreen","darkgrey"), bty="n",
    col=c("mediumturquoise","darkorange","red","darkgreen","darkgrey"), text.col=c("mediumturquoise","darkorange","red","darkgreen","darkgrey"), 
    lty=c(3,2,2,1,3), lwd=rep(3,5))

    par(oldpar)

}



#temp.data=round(timeSeries.format(ts.symbol="DOE.NGMPPUS", the.title="NG", the.subtitle="Marketed Wet Production [Bcf/d]",
#start.date=1997, statsOverYears =6, FileSave="C:/BalanceSheets/UpdatedFiles/temp.plot.jpg"),3)


timeSeries.format <- function(ts.symbol="", the.title="", the.subtitle="", start.date, statsOverYears =6, FileSave="", Verbose=F) 
{
  #Verbose= T
  start.date=format(as.Date(paste(as.numeric(start.date)-1,"-12-31", sep="")), "%m/%d/%Y") 
	
  LIMquery <- paste("SHOW \n ","1: ",ts.symbol, "  \nWHEN  \ndate is first day of the month  \nand date is after ", start.date, "\n", sep="")
	if(Verbose) cat(LIMquery)
	# col.tags <- c(1)

  the.data <<- na.omit(read.LIM(LIMquery))
	the.data <- add.NAs.toYearsEnd(the.data) ; 	# if we do not have 12 months for the last year, it will fill in the earliest values that it finds in the data returned from LIM.

  # The following command is temporarily disregarded
	# assign("the.data", the.data, where=1, immediate=T)

	# Note that the.data[1,1] is numeric of the following form "1985-01-01 est"

	the.start <- the.data[1,1]
  mo.start = as.integer(strsplit(as.character(the.start), "-")[[1]][2])
	yr.start = as.integer(strsplit(as.character(the.start), "-")[[1]][1])

	the.data.timeSeries <- ts(data=the.data[,2:length(the.data)], start = c(yr.start, mo.start), frequency = 12)

  firstYear <- start(the.data.timeSeries)[1]
	lastYear <- end(the.data.timeSeries)[1]

  # Convert the.data.timeSeries to a matrix, whose number of cols is 12
	tsmat <- t(matrix(the.data.timeSeries,nrow=12));

	# Create the names columns and rows of the new matrix

  colnames(tsmat) = unique(months(the.data[,1], abbreviate = TRUE))
  rownames(tsmat) = firstYear:lastYear


  #	Do the yearly stats (i.e. within a row)
 	the.frame <- as.data.frame(tsmat)
	the.frame$Q1 <- as.double(rowMeans(the.frame[, 1:3]))
	the.frame$Q2 <- as.double(rowMeans(the.frame[, 4:6]))
	the.frame$Q3 <- as.double(rowMeans(the.frame[, 7:9]))
	the.frame$Q4 <- as.double(rowMeans(the.frame[, 10:12]))
	the.frame$FullYr <- as.double(rowMeans(the.frame[,1:12]))

  #	Now do the Monthly stats (i.e. within a column)
	numRows <- nrow(the.frame)
	current.yr = format(Sys.Date(), "%Y")
  current.mo = format(Sys.Date(), "%b")

  current.row =  (1:nrow(the.frame))[rownames(the.frame)==current.yr]
  current.col =  (1:ncol(the.frame))[colnames(the.frame)==current.mo]

  
  
  # futures.months is a dataframe whose dim is statsOverYears by (current.mo to Dec)
  future.months <- the.frame[(current.row-statsOverYears):(current.row-1) , current.col:ncol(the.frame)]
  
  # past.data is a dataframe whose dim is statsOverYears by (Jan to (current.mo -1))
  past.months <- the.frame[(current.row-statsOverYears+1):current.row, 1:(current.col-1)]

  # Note that the comb.data is a data frame that contains the right names of months but not years due to the shift!!
  comb.data = cbind(past.months, future.months)
	
	
	if(numRows-statsOverYears<=0) {
		statsOverYears <- numRows-1
		warning(paste("Requested more years of statistics than is available; reduced to maximum allowed  (",statsOverYears, ")", sep=""))
	}

	# Calculate the parameters of the Outer and Inner Band
	
   	for (i in 1:ncol(comb.data)){

   		the.frame[numRows+1,i] <- min(comb.data[,i])             
   		the.frame[numRows+2,i] <- mean(comb.data[,i])
   		the.frame[numRows+3,i] <- max(comb.data[,i])
      the.frame[numRows+5,i] <- as.double(min(comb.data[(nrow(comb.data)-2):nrow(comb.data),i]))
   		the.frame[numRows+6,i] <- as.double(mean(comb.data[(nrow(comb.data)-2):nrow(comb.data),i]))
   		the.frame[numRows+7,i] <- as.double(max(comb.data[(nrow(comb.data)-2):nrow(comb.data),i]))
    }
    
    
    # Projection from Jan to last month.
    if(class(past.months)=="data.frame") {    # this happens when you are NOT in Feb and past.months becomes a vector instead of d.f.
      for (i in 1:ncol(past.months)) {
        the.frame[numRows+4,i] <- projectedVal(x=as.integer(dimnames(comb.data)[[1]]),y=past.months[,i], PredictYr=(as.integer(current.yr)+1) )
      }
    } else {                                  #this happens when you are in Feb and the table has degenerated into a vector (single col for Jan)
#      for (i in 1:ncol(past.months)) {
        the.frame[numRows+4,1] <- projectedVal(x=as.integer(dimnames(comb.data)[[1]]),y=past.months, PredictYr=(as.integer(current.yr)+1) )
#      }    
    }
    
    # Projection from Current month to Dec (Actually Full Yr Column)
    if(class(past.months)=="data.frame") {
      offset <- ncol(past.months)
    } else {
      offset <- 1
    }
    for (i in 1:ncol(future.months)) {
    the.frame[numRows+4,i+offset] <- projectedVal(x=as.integer(dimnames(future.months)[[1]]), future.months[,i],PredictYr=as.integer(current.yr))
    }
     
  
  # dimnames(the.frame)[[1]] <- c(firstYear:lastYear, "Min.Outer", "Mean.Outer", "Max.Outer",
  #  paste("Proj.",dimnames(the.frame)[[1]][lastRow],sep=""), "Min.3yr","Mean.3yr","Max.3yr")

  dimnames(the.frame)[[1]] <- c(firstYear:lastYear, "Min.Outer", "Mean.Outer", "Max.Outer", "Seas. Proj.", "Min.3yr","Mean.3yr","Max.3yr")


  the.frame2 = cbind(the.frame[,current.col:12], the.frame[,1:(current.col-1)])
	Modeled.val =  as.vector(unlist(cbind(the.frame[current.row, (current.col:12)], the.frame[current.row+1, 1:(current.col-1)])))
  # Past twelve values
  past.12.mo <- c(unlist(the.frame[current.row-1 , current.col:12]),unlist(the.frame[current.row , 1:(current.col-1)]))
  
  # Let's do some plotting
  jpeg(filename = FileSave, width = 560, height = 440)
  
	LD.Plot.bandplot(lowOuter = list(Month=1:12, Value=as.vector(the.frame2[numRows+1,1:12])), # Low Series, Outer Band
								highOuter = list(Month=1:12, Value=as.vector(the.frame2[numRows+3,1:12])),# High Series, Outer Band
								lowInner = list(Month=1:12, Value=as.vector(the.frame2[numRows+5,1:12])),  # Low Series, Inner Band
								highInner = list(Month=1:12, Value=as.vector(the.frame2[numRows+7,1:12])) ,# High Series, Inner Band
								Normal.outer = unlist(the.frame2[numRows+2,1:12]),  									# normal line of Outer band (normal based on more years)
								Projected = unlist(the.frame2[numRows+4,1:12]),
                Modeled = Modeled.val, past.12.val= past.12.mo,
								Normal.Inner = unlist(the.frame2[numRows+6,1:12]),  									# normal line of Inner band (normal based on less years)
								keyStrings =  c(paste(statsOverYears,"-Yr Normal",sep=""),"Seas. Proj.","Modeled","Past 12 months", "3-yr Normal"),
								the.title = paste(statsOverYears, "- & 3-Yr Behavior of ", the.title, sep=""), the.subtitle=the.subtitle)
    dev.off()

	return(the.frame)
}


BalSheet.Initialize <- function(tsSymbol, theTitle , theSubtitle, StartDate, StatsOverYears, directory){
  # Remove blank string "" from tsSymbol and theSubtitle
  
  if (length((1:length(tsSymbol))[tsSymbol==""]) != 0){
    tsSymbol <- tsSymbol[-(1:length(tsSymbol))[tsSymbol==""]]
    theSubtitle <- theSubtitle[-(1:length(tsSymbol))[tsSymbol==""]]
  }
  
  # Change StartDate from number of days (automatic change when you change from Excel to R) to a character date 
  #StartDate <- format(as.Date('2006-01-01')+as.numeric(StartDate)-38718, '%m/%d/%Y')

  ts.results <- NULL
    
    for (j in 1:length(tsSymbol)){
  
    ts.results[[j]] <- round(timeSeries.format(tsSymbol[j], theTitle, theSubtitle[j], StartDate, StatsOverYears, FileSave=paste(directory, "ts.res.plot",j,".jpg", sep="")),3)
    }
  ts.results<<-ts.results
  return(ts.results)
}