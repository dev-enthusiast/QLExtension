###############################################################################
# VaRHistoryReport.R
#
# Author: e14600
library(zoo)
library(xlsReadWrite)
library(reshape)
library(ggplot2)
source("H:/user/R/RMG/Energy/Trading/Controls/Ticker/TickerUtils.R")
source("H:/user/R/RMG/Energy/Trading/Controls/RollingVaR.R")
source("H:/user/R/RMG/Energy/Trading/Controls/VaRHistoryReport/sparkline.R")
source("H:/user/R/RMG/Utilities/RLogger.R")

BASE_DIR = "H:/My Documents/Constellation Projects/VaRHistoryReport/"
REPORT_DIR = "C:/Documents and Settings/e14600/Desktop/"
REPORT_DIR = "S:/Risk/Trading Controls/Reports/VaR Limit Report/"
OUTPUT_PAGE = paste(BASE_DIR, "VaRHistoryReport.html", sep="")

readCurrentHistory <- function()
{
    asOfDate = Sys.Date()-1
    
    inputFile = paste( REPORT_DIR, "VaR Limit Report ", 
            format(asOfDate, "%m%d%y"), ".xls", sep="" )
    
    while( !file.exists( inputFile ) )
    {
        rLog( "Checking input file:", inputFile )
        asOfDate = asOfDate-1
        
        inputFile = paste( REPORT_DIR, "VaR Limit Report ", 
                format(asOfDate, "%m%d%y"), ".xls", sep="" )
    }
    
    rLog("Loading Limit File:", inputFile)
    data = read.xls( inputFile, sheet="Var History", dateTimeAs="isodatetime", 
            from=2 ) 
    
    data[,1] = NULL
    names(data)[1] = "Date"

    data$Date = as.Date(data$Date)
    
    badRows = which(is.na(data$Date))
    data = data[-badRows,]
    
    return( data )
}


printTableEntries <- function()
{
    limits = readCurrentHistory()
    
    for( colIndex in 2:ncol(limits)  )
    {
        portfolio = names(limits)[colIndex]
        #cat( "\n\n", portfolio, "\n", sep="" )
        
        #print(colIndex)
        colData = limits[ nrow(limits):1, c(1,colIndex) ]
        names(colData) = c("Date", "Historic")
        colData$Historic = as.numeric(colData$Historic)
        colData = na.omit(colData)
        
        fileName = paste( BASE_DIR, "sparklines/", portfolio, "_spark.png", sep="" )
        data = sparkline( colData$Historic, width=200, height=50, 
                plotType=png, filename=fileName )
        dev.off()
        
        
        meltedData = melt(colData, id="Date")
        historicMelted = meltedData$value[which(meltedData$variable=="Historic")]
        
        minValue = min(historicMelted)
        minRow = which(meltedData$value==minValue)[1]
        minForMelted = data.frame( Date=meltedData[minRow,"Date"], 
                variable="Min", value=minValue)
        
        maxValue = max(historicMelted)
        maxRow = which(meltedData$value==maxValue)[1]
        maxForMelted = data.frame( Date=meltedData[maxRow,"Date"], 
                variable="Max", value=maxValue)
        
        firstValue = historicMelted[1]
        firstRow = which(meltedData$value==firstValue)[1]
        firstForMelted = data.frame( Date=meltedData[firstRow,"Date"], 
                variable="First", value=firstValue)
        
        lastValue = historicMelted[length(historicMelted)]
        lastRow = which(meltedData$value==lastValue)[1]
        lastForMelted = data.frame( Date=meltedData[lastRow,"Date"], 
                variable="Last", value=lastValue)
        
        
        p = ggplot( data = meltedData, main="VaR History", 
                        mapping = aes(x = Date, y = value/1000000) ) +
                layer( geom = "line" ) +
                layer( data=minForMelted, geom="point", colour="blue" ) +
                layer( data=maxForMelted, geom="point", colour="blue" ) +
                layer( data=firstForMelted, geom="point", colour="red" ) +
                layer( data=lastForMelted, geom="point",  colour="red" ) +
                scale_y_continuous("VaR") + 
                scale_x_date(major = "months", format = "%b", name = NULL) 
        
        fullFileName = paste( BASE_DIR, "charts/", portfolio, ".png", sep="" )
        png( file=fullFileName, height=400, width=500)
        print(p)
        dev.off()
        
        niceName = gsub( ".", " ", portfolio, fixed=TRUE )
        
        portName = paste('<tr><td class="name">', niceName, "<br></td>", sep="")
        portStart = paste('<td class="min">', round(firstValue/1000000, digits=1), "<br></td>", sep="")
        portSpark = paste('<td class="chart"><a href="charts/', portfolio, '.png" title="', 
                niceName, '"><img class="chart" alt="', niceName, '" src="sparklines/',
                portfolio, '_spark.png"></a><br></td>', sep="")
        portEnd = paste('<td class="max">', round(lastValue/1000000, digits=1), "<br></td>", sep="")
        portMin = paste('<td class="first">', round(minValue/1000000, digits=1), "<br></td>", sep="")
        portMax = paste('<td class="last">', round(maxValue/1000000, digits=1), "<br></td></tr>", sep="")
        outputLine = paste( portName, portStart, portSpark, portEnd, 
                portMin, portMax, "\n", sep="\n")
        
        cat(outputLine)
    }
}

header = 
'
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<script src="js-global/FancyZoom.js" type="text/javascript"></script>
<script src="js-global/FancyZoomHTML.js" type="text/javascript"></script>
<meta content="text/html; charset=us-ascii" http-equiv="Content-Type" />
<LINK href="table.css" type="text/css" rel="stylesheet">
<title>VaR History Report</title>
</head>
<body onload="setupZoom()" >
<h2><img style="border: 0px solid ;" alt="banner" src="Banner.jpg" /><br /></h2>
<h2 style="text-align: left;">VaR History Report<br /></h2>
<table style="text-align: left; width: 650px;" border="0" cellpadding="2" cellspacing="2">
<tbody>
<tr style="text-align: center; font-family: Courier New,Courier,monospace;">
<td><br /></td>
<td>Start<br /></td>
<td>VaR History<br /></td>
<td>End<br /></td>
<td>Low<br /></td>
<td>High<br /></td>
</tr>

'

footer = 
'
</tbody>
</table>
<font size="-2"><span style="font-family: Courier New,Courier,monospace;">
<br />
<br />
<br />
<br />
<br />
CEG Risk Management Group<br />
Analytics, Trading Controls<br />
john.scillieri@constellation.com<br />
</span></font>
</body>
</html>
'

sink( file=OUTPUT_PAGE )
cat( header )
printTableEntries()
cat( footer )
sink()

