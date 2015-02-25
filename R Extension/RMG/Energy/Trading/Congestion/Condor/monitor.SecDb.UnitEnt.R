## Report to monitor the value of the M89 unit entitlements in SecDb

## Returns a data frame containing flat report for a given list of unit entitlements
fetch.flatReport <- function( unitEnts =c("Mystic 8", "Mystic 9"), asOfDate=Sys.Date(), startDate=nextMonth(Sys.Date()), endDate=as.Date("2018-12-31"),outFile="M89_eflt.csv"){
    outFile <- paste( getwd(), outFile, sep="\\" )
    outFile <- gsub("\\\\", "\\", outFile)
    outFile <- gsub("////", "\\", outFile)
    UEList <- paste("[",paste( '"', unitEnts, collapse='",', sep=""), '"]', sep="" )

    str <- paste( 'Link("AAS: Unit Ent Dump Report"); UEs = ', UEList, '; outFile = $"', outFile,'";@AAS::UnitEntDumpReport(UEs,Date("',format(asOfDate, "%d%b%y"),'"),Date("', format(startDate, "%d%b%y"),'"),Date("',format(endDate, "%d%b%y"), '"),outFile, "ELEC FLAT REPORT");', sep="" )

    res <- secdb.evaluateSlang(str)

    if( file.exists(outFile)){
        ret <- read.csv(outFile)
        cnames <- names(ret)
        names(ret) <- gsub("[.]", "", cnames)
        ret$month <- format.dateMYY( ret$ActiveContracts )
        ret <- ret[order(ret$month), ]
    }else{
        ret <- NULL
    }
    ret
}

fetch.lrr <- function( unitEnts =c("Mystic 8", "Mystic 9"), asOfDate=Sys.Date(), startDate=nextMonth(Sys.Date()), endDate=as.Date("2018-12-31"),outFile="M89_lrr.csv"){
    outFile <- paste( getwd(), outFile, sep="\\" )
    outFile <- gsub("\\\\", "\\", outFile)
    outFile <- gsub("////", "\\", outFile)
    UEList <- paste("[",paste( '"', unitEnts, collapse='",', sep=""), '"]', sep="" )

    str <- paste( 'Link("AAS: Unit Ent Dump Report"); UEs = ', UEList, '; outFile = $"', outFile,'";@AAS::UnitEntDumpReport(UEs,Date("',format(asOfDate, "%d%b%y"),'"),Date("', format(startDate, "%d%b%y"),'"),Date("',format(endDate, "%d%b%y"), '"),outFile, "Location Risk Report (CSV)");', sep="" )

    res <- secdb.evaluateSlang(str)

    if( file.exists(outFile)){
        ret <- read.csv(outFile)
        cnames <- names(ret)
        names(ret) <- gsub("[.]", "", cnames)
    }else{
        ret <- NULL
    }
    ret
}
DB = "!Bal ProdSource;!BAL Home"
Sys.setenv(SECDB_SOURCE_DATABASE=DB)
Sys.setenv(SECDB_DATABASE="Prod")
require(SecDb); require(CEGbase);
require(PM);require(ggplot2);
require(plyr);require(reshape2);require(scales);require(xtable)

setwd("S:/All/Structured Risk/NEPOOL/Temporary/Mystic89_old/")

asOfDate <- secdb.dateRuleApply( Sys.Date(), "-1b", calendar = "CPS-BAL")
startDate <- max(as.Date("2015-04-01"), nextMonth(asOfDate))

endDate <- as.Date("2018-09-30")
unitEnts <- c("Mystic 8", "Mystic 9")
outFile <- paste("M89_SecDb_", format(asOfDate, "%Y-%m-%d"), ".csv", sep="")
figFile <- paste("M89_SecDb_", format(asOfDate, "%Y-%m-%d"), ".png", sep="")

if (file.exists(outFile)){
    file.remove(outFile)
    file.remove(figFile)
}

fltRep <- fetch.flatReport(unitEnts=unitEnts, asOfDate=asOfDate, startDate=startDate, endDate=endDate, outFile=outFile)

if( is.null(fltRep) ){
    stop("Unable to run flat report in SecDb")
}

## get Energy data and filter out emmissions
fltRep <- subset(fltRep, tolower(ServiceType)=="energy" )
fltrSec <-  c(" SO2 ", " NOX ", " CO2 " )

#idx <- do.call( cbind, Map(f=function(x){ret <- which(grepl(x,fltRep$SecDbSecurityName,ignore.case=TRUE)); ret}, fltrSec), )
#idx <- sort(unique(as.vector(idx)))
#fltRep <- fltRep[-idx, ]
mthPnl <- ddply( fltRep, .(month), summarise, pnl=sum(TodayValue, na.rm=TRUE))

##Get Secdb Discount Factor to calculate Nominal
df <- secdb.getDiscountFactor(as.Date(asOfDate), unique(mthPnl$month))
mthPnl <- merge( mthPnl, df, by.x="month", by.y="dates")
mthPnl$Nom <- mthPnl$pnl/mthPnl$DF
yrPnl <- ddply( mthPnl, .(Year=format(month, "%Y")), summarise, pnl=sum(Nom, na.rm=TRUE))
yrPnl$pnl <- prettyNum(yrPnl$pnl, big.mark=",")

plotTitle <- paste("Energy CashFlow M8&9 (Old SecDB) as of ", format(asOfDate, "%d%b%y"), "\nNom. PnL($ MM) = ", prettyNum(round(sum(mthPnl$Nom)/1e6, digits = 2), big.mark = ",", nsmall = 2 ), sep="")


fig <- ggplot( data =mthPnl, aes(x=month, y=Nom/1e6))
fig <- fig+geom_bar( stat="identity", fill="skyblue2")
fig <- fig+xlab("Month")+ylab("PnL $MM")+ggtitle(plotTitle)+scale_x_date(labels = date_format("%Y-%m"), breaks='3 months')+scale_y_continuous(breaks=seq(-2.5, ((max(mthPnl$pnl)/1e6)%/%5+1)*5, 2.5))
fig
ggsave(filename=figFile, plot=fig, width=11, height=9)

contact <- paste("\n\nContact: Ankur Salunkhe<br>",
         "Procmon job: RMG/R/Reports/Energy/Trading/Congestion/monitor.SecDb.UnitEnt")
stDt <- format.dateMYY(startDate, -1)
tableHTML <- print(xtable(yrPnl), type = "html")
msg <- paste("
<html>
    <body>
      <h4> Mystic 8&9 Energy value from old secdb Trade (ALGCG-18.5c) ", stDt, "-U18</h4>
      <img src=\"file:", paste(getwd(), figFile, sep="/"), "\" height=\"655\" width=\"800\" ><br>
      <p><font size=\"2\">Flat report details <a href=\"", paste( getwd(), outFile, sep="/"), "\">here</a></font><br><br></p>\n",tableHTML,"
      <p><font size=\"1\">", contact, "</font></p>
    </body>
</html>", sep="")

emailTitle <- paste("M89 ALGCG HeatRate Swap : ", format(asOfDate, "%Y-%m-%d"))
sendEmail("OVERNIGHT_PM_REPORTS@constellation.com",
#          paste(c("ankur.salunkhe@constellation.com", "#WTNEPOOLNE@constellation.com"), sep="", collapse=","),
        paste(c("ankur.salunkhe@constellation.com"), sep="", collapse=","),
          emailTitle, msg, html = TRUE, links=TRUE, attachment=figFile)














