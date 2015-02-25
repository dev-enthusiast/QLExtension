######################################################################
#Functions to assists in development/testing/analsysis of weather VaR
#
#Author: Seam McWilliams
#
#
######################################################################
#
VARvsPNL = new.env(hash=TRUE)
#
######################################################################
library(reshape)

VARvsPNL$readTextFiles <- function()
{
	data <- list(DATE=c(), WNTRGD=c(), WNTRJP=c(), SUMRGD=c(), SUMRJP=c(), TOTAL=c())
	i <- 1
	while(i <= 93)
	{
			file <- paste("S:/All/Risk/Data/Weather VaR/",i,".txt",sep="")
			lines <- readLines(file)
			
			data[["DATE"]] <- append(data[["DATE"]], substr(lines[2], 7, nchar(lines[2]) - 3))
				
			if(length(grep("07WNTRGD",lines[7])) == 1)
			{
				data[["WNTRGD"]] <- append(data[["WNTRGD"]], as.integer(sub(",","",(substr(lines[7], 16, nchar(lines[7]) - 1)))))
				data[["WNTRJP"]] <- append(data[["WNTRJP"]], as.integer(gsub(",","",(substr(lines[8], 16, nchar(lines[8]) - 1)))))
				data[["SUMRGD"]] <- append(data[["SUMRGD"]], as.integer(gsub(",","",(substr(lines[9], 16, nchar(lines[9]) - 1)))))
				data[["SUMRJP"]] <- append(data[["SUMRJP"]], as.integer(gsub(",","",(substr(lines[10], 16, nchar(lines[10]) - 1)))))
				data[["TOTAL"]] <- append(data[["TOTAL"]], as.integer(gsub(",","",(substr(lines[11], 13, nchar(lines[10]) - 1)))))
			}else if(length(grep("08SUMRGD",lines[7])) == 1)
			{
				data[["SUMRGD"]] <- append(data[["SUMRGD"]], as.integer(gsub(",","",(substr(lines[7], 16, nchar(lines[7]) - 1)))))
				data[["SUMRJP"]] <- append(data[["SUMRJP"]], as.integer(gsub(",","",(substr(lines[8], 16, nchar(lines[8]) - 1)))))
				data[["TOTAL"]] <- append(data[["TOTAL"]], as.integer(gsub(",","",(substr(lines[9], 13, nchar(lines[8]) - 1)))))
				data[["WNTRGD"]] <- append(data[["WNTRGD"]], 0)
				data[["WNTRJP"]] <- append(data[["WNTRJP"]], 0)
			}
		
			i <- i + 1
	}
	
	return(data)
}

VARvsPNL$getBalPnL <- function(tFile)
{
	
	filePath <- "S:/All/Risk/Data/Weather VaR/"
	file <- paste(filePath, tFile, ".txt",sep="")

	lines <- readLines(file)
	
	pnl <- unlist(lapply(lines[6:98], .parseLine))
	
	
}

.parseLine <- function(line)
{
	l <- gsub("[[:alnum:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+","",line)
	l <- gsub("[ ]+[[:graph:]]+[ ]+[[:graph:]]+[ ]+[[:alnum:]]+","",l)
	l <- gsub(",","",l)
	return(as.integer(l))
}



data <- VARvsPNL$readTextFiles()
frameData <- as.data.frame(data)
frameData$DATE <- gsub('[[:upper:]]{1}[[:lower:]]+[,]{1}[ ]{1}', "", frameData$DATE)
frameData$DATE <- gsub('[ ]{1}[0-9]{1,2}[:]{1}[0-9]{2}', "", frameData$DATE)

len <- length(frameData$DATE)

frameData$DATE <- frameData$DATE[len:1]
frameData$WNTRGD <- frameData$WNTRGD[len:1]
frameData$WNTRJP <- frameData$WNTRJP[len:1]
frameData$SUMRGD <- frameData$SUMRGD[len:1]
frameData$SUMRJP <- frameData$SUMRJP[len:1]
frameData$TOTAL <- frameData$TOTAL[len:1]

GD08 <- VARvsPNL$getBalPnL("gd08")
GD07 <- VARvsPNL$getBalPnL("gd07")
JP08 <- VARvsPNL$getBalPnL("jp08")
JP07 <- VARvsPNL$getBalPnL("jp07")

frameData$GD07PNL <- GD07
frameData$JP07PNL <- JP07
frameData$GD08PNL <- GD08
frameData$JP08PNL <- JP08

frameData$GD07MADEMORE <- as.integer(frameData$GD07PNL > frameData$WNTRGD)
frameData$JP07MADEMORE <- as.integer(frameData$JP07PNL > frameData$WNTRJP)
frameData$GD08MADEMORE <- as.integer(frameData$GD08PNL > frameData$SUMRGD)
frameData$JP08MADEMORE <- as.integer(frameData$JP08PNL > frameData$SUMRJP)

frameData$GD07LOSTMORE <- as.integer((-1 * frameData$GD07PNL) > frameData$WNTRGD)
frameData$JP07LOSTMORE <- as.integer((-1 * frameData$JP07PNL) > frameData$WNTRJP)
frameData$GD08LOSTMORE <- as.integer((-1 * frameData$GD08PNL) > frameData$SUMRGD)
frameData$JP08LOSTMORE <- as.integer((-1 * frameData$JP08PNL) > frameData$SUMRJP)

write.csv(frameData, "S:/All/Risk/Data/Weather VaR/VARvsPNL.csv")