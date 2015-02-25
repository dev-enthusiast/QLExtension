CheckLIMData.r <- function(deal.Index = "IGBBL00", directory, Verbose=F)
{
  source(paste(directory, "read.LIM.r", sep=""))

  if(!is.na(deal.Index)) {
    query <- paste("SHOW", "\n", "Index: ", "Index of ",deal.Index, "\n",
    "WHEN","\n",
    "Date is ", deal.Index, " First data day", "\n",
    "or date is ", deal.Index, " last data day" ,sep=" ")
  }

    if (Verbose) cat(query)

    data.lim <- read.LIM(query)
    data.lim=na.omit(data.lim)

	if(nrow(data.lim)==0)
	{
		print(data.lim)
		message("No data found for the specified index")
		return()
	}

    # Now throw out a picture
    if(!is.na(deal.Index)) {
        queryAll <- paste("SHOW", "\n", "Index: ", "Index of ",deal.Index, "\n",sep=" ")
        dataAll.lim <- read.LIM(queryAll)
        dataAll.lim=na.omit(dataAll.lim)
    }
    if(!is.na(deal.Index)) {
        query1yr <- paste("SHOW", "\n", "Index: ", "Index of ",deal.Index, "\n", "WHEN date is within 1 year",sep=" ")
        data1yr.lim <- read.LIM(query1yr)
        data1yr.lim=na.omit(data1yr.lim)
    }

#  jpeg(paste(directory, "HistData.jpg", sep=""), width = 640, height = 520)
  windows(width = 12, height = 7)
  par(mfrow=c(2,1))
  temp <- dataAll.lim
  temp$Date <- as.Date(temp$Date)
  plot(temp, main="Historical Data", sub="All data", type="p", col="light blue", lwd=1, ylab=paste(deal.Index,"   [$/mmbtu]",sep="") )
  points(dataAll.lim, cex = .25, col = "dark red")
  plot(data1yr.lim, sub="Last 12 months", type="b", col="light blue", lwd=1, ylab=paste(deal.Index,"   [$/mmbtu]",sep="") )
  points(data1yr.lim, cex = .5, col = "dark red")
  lines(data1yr.lim, col = "light blue")
  par(mfrow=c(2,1))
#  dev.off()
  
  dataAll.lim<<-dataAll.lim
  return(as.character(data.lim[,1]))

}