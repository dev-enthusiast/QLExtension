# Check the spread regression in Slang.
#
# Written by Wang Yu on Dec-2005

#rm(list=ls())

library(xtable)
library(SecDb)
source("H:/user/R/RMG/Models/SecDB_NY_NEPOOL_Spreads/Load.Tsdb.DAM.CommodCurves.R")
source("H:/user/R/RMG/Models/SecDB_NY_NEPOOL_Spreads/check.Regression.R")
source("H:/user/R/RMG/Models/SecDB_NY_NEPOOL_Spreads/make.pdf.R")


getRegression <- function( market, location )
{
    Regression = new.env(hash=TRUE)
    
    SecDBMonth = c('F','g','H','j','k','m','n','Q','U','v','X','z')
    
    if( market %in% c('NEPOOL','PWX') )
    {
        Container = secdb.getValueType('BAL Close PWX Spreads','Contents')
        Regression$Bases = Container$Bases
        Regression$StartDate = secdb.convertTime(Container$Config$'Start Date')
        Regression$EndDate =  secdb.convertTime(Container$Config$'End Date')
        
        RegressionInfo = Container$Regressions[[location]]
        
    } else if( market %in% c('NYPP','PWY') )
    {        
        Container = secdb.getValueType('BAL Close PWY Spreads','Contents')
        Regression$Bases = Container$Bases;
        
        Regression$StartDate = secdb.convertTime(Container$RegParms$'Start Date')
        Regression$EndDate =  secdb.convertTime(Container$RegParms$'End Date');
        Regression$Adjust = data.frame(
                unlist(Container$'Spread Adjustments'$Contracts)
                )
        
        for( x in Container$'Spread Adjustments' )
        { 
            Regression$Adjust = cbind(Regression$Adjust, data.frame(unlist(x)))
        }
        
        Regression$Adjust[,1] = NULL
        colnames(Regression$Adjust) = names(Container$'Spread Adjustments')
        
        RegressionInfo = Container$Regressions[[location]]

        Regression$Loc$StartDate = secdb.convertTime(RegressionInfo$START);
        Regression$Loc$EndDate = secdb.convertTime(RegressionInfo$END);
        Regression$Loc$Base = RegressionInfo$Base;
        Regression$Loc$PTID = RegressionInfo$PTID;
        Regression$Loc$Zone = RegressionInfo$Zone;
        
        RegressionInfo = RegressionInfo$regs;
    } 
    else
    {  
        stop('Unsupported region', market)
    }    
    
    
    Buckets = names(RegressionInfo);
    Regression$Buckets = Buckets;
    for( counter in 1:length(Buckets) )
    {
        Bucket = Buckets[counter];
        
        RegressionMatrix=NULL
        RegressionMatrix$Stats = matrix(nrow=length(SecDBMonth),ncol=5)
        for( Month_Idx in 1:length(SecDBMonth) )
        {    
            Sec_Month = SecDBMonth[Month_Idx];
            statFields = RegressionInfo[[Bucket]][[Sec_Month]]
            RegressionMatrix$Stats[Month_Idx,1] = statFields$intercept$Value;
            RegressionMatrix$Stats[Month_Idx,2] = statFields$intercept$StdDev;
            RegressionMatrix$Stats[Month_Idx,3] = statFields$slope$Value;
            RegressionMatrix$Stats[Month_Idx,4] = statFields$slope$Value;
            RegressionMatrix$Stats[Month_Idx,5] = statFields$stats$Rsq;
            
        }
        
        varName = paste("Bucket_", Bucket, sep="")
        Regression[[varName]] = RegressionMatrix
        
    }
    
    return( Regression );
}

futcurve <- function( curve, StartCon, EndCon )
{
    tsdbCurve = commodToTSDB(curve)
    
    slang = paste('TsdbFunc("futcurve",Date("', StartCon, '"),Date("', EndCon,  
            '"),"', tsdbCurve, '")', sep='' )
    curveData = secdb.evaluateSlang(slang)
    return(data.frame(curveData))
}

commodToTSDB <- function(commod)
{
    
    letters = strsplit(commod,"")[[1]]
    ind=which(letters==" ");
    
    market=substr(commod, ind[1]+1, ind[2]-1)
    
    tsdbSymbol=secdb.getValueType(commod,'Tsdb Symbol Rule');
    tsdbSymbol=sub('!', market, tsdbSymbol);
    
    ind=gregexpr('^', tsdbSymbol, fixed=TRUE)[[1]]
    tsdbSymbol=substr(tsdbSymbol, 5, ind[1]-2)
    tsdbSymbol = paste("PRC_", tsdbSymbol, sep="")
    
    return(tsdbSymbol)
}


save=NULL
save$Analysis$all <- 1
save$dir$plots = "H:/Regression/plots/"
save$dir$report = "H:/Regression/reports/"
##save$dir$plots    <-  "S:/Risk/2007,Q3/Risk.Analytics/Model.Validation/SecDB.Spreads/Plots_Tables/"
##save$dir$report   <-  "S:/Risk/2007,Q3/Risk.Analytics/Model.Validation/SecDB.Spreads/Final_Reports/"

options=NULL
options$plot <- 1
options$email<-0

options$DerivedCurves <- c("COMMOD PWX 5x16 CT PHYSICAL")

options$BaseCurves  <- c ("COMMOD PWX BUCKET PHYSICAL",
        "COMMOD PWY BUCKET HUDV PHYSICAL",
        "COMMOD PWY BUCKET NYC PHYSICAL",
        "COMMOD PWY BUCKET WEST PHYSICAL")

names(options$BaseCurves)<-c("Hub","HUDV","NYC","WEST")

# SecDB Future/Fwd curves to be compared at
options$PWY$StartCon <- "01Apr2008"
options$PWY$EndCon <- "31Dec2012"
# SecDB Future/Fwd curves to be compared at
options$PWX$StartCon <- "01Apr2008"
options$PWX$EndCon <- "31Dec2012"

# options$AsOfDate <- format(Sys.time(),"%d%b%Y")
options$AsOfDate <- "31Mar2008"

# StartDT and EndDT are used to Fetch FutCurve inside Matlab
options$PWY$StartDT <- options$AsOfDate
options$PWY$EndDT <- options$AsOfDate
options$PWX$StartDT <- options$AsOfDate
options$PWX$EndDT <- options$AsOfDate

# DAM hisotry used for Regression will be fetched from SecDB below

# DAM history to be used for regression
# options$PWX$Start <- "15-Mar-2003 01:00:00"
# options$PWX$End <- "30-Dec-2005 00:00:00"

# DAM history to be used for regression
options$PWY$Start<- "01-Nov-2000 01:00:00"
options$PWY$End <- "31-Mar-2008 00:00:00"




#for (counter in seq_along(options$DerivedCurves))
#{  

counter = 1
# Prepare for Fetch SecDB regression using Matlab Server
CommodStr <- strsplit(options$DerivedCurves[counter]," ")
CommodStr <- CommodStr[[1]]
names(CommodStr) <- c("Commod","Market","Bucket","DerivedLoc","Physical")

Market <- CommodStr["Market"]
DerivedLoc <- CommodStr["DerivedLoc"]
Bucket <- CommodStr["Bucket"]

# Get SecDB Regression
SysRegMM = getRegression(Market,DerivedLoc)  


# Extract History time window used for last regression in SecDB
if (Market == "PWX")
{
    # ISO-NE 
    options$PWX$Start <- SysRegMM$StartDate
    options$PWX$End <- SysRegMM$EndDate 
    options$BaseCurve <- options$BaseCurves[ SysRegMM$Bases[[1]] ]
    options$BaseCurve <- gsub("BUCKET",Bucket,options$BaseCurve) 
    if (options$BaseCurve == "COMMOD PWX 2x16H PHYSICAL")
    {
        options$BaseCurve = "COMMOD PWX 2x16 PHYSICAL"
    }
    options$BaseLoc <- SysRegMM$Bases[[1]]
}

if (Market == "PWY")
{
    # NYPP
    options$PWY$Start<- SysRegMM$Loc$StartDate
    options$PWY$End <-  SysRegMM$Loc$EndDate 
    options$BaseCurve <- options$BaseCurves[SysRegMM$Loc$Base]
    options$BaseCurve <- gsub("BUCKET",Bucket,options$BaseCurve)
    options$BaseLoc <- SysRegMM$Loc$Base
}

BaseLoc <- options$BaseLoc
DateParms <- options[[Market]]
CommodCurves <-  c( options$BaseCurve, 
        options$DerivedCurves[counter])

# Get SecDB Future Curves
FutDerivedCurves <-  futcurve(options$DerivedCurves[counter],DateParms$StartCon, DateParms$EndCon)
FutBaseCurves <-  futcurve(options$BaseCurve, DateParms$StartCon, DateParms$EndCon)

CurveMonths <-  length(FutBaseCurves$date)
FutPrice <- data.frame(Bucket = as.character(rep(Bucket,CurveMonths)),
        Month = as.numeric(substr(FutDerivedCurves$date,6,7)),
        FutExp = as.Date(FutDerivedCurves$date),
        BasePrice = FutBaseCurves$value,
        DerivedPrice = FutDerivedCurves$value,
        SdbFit = rep(NA,CurveMonths),
        RMGFit = rep(NA,CurveMonths),
        CILower = rep(NA,CurveMonths),
        CIUpper = rep(NA,CurveMonths),
        PILower = rep(NA,CurveMonths),
        PIUpper = rep(NA,CurveMonths)
        )


# Get Historical DAM prices used for Regression                   

 TsdbInfo <- Load.Tsdb.DAM.CommodCurves(CommodCurves,DateParms)
 BaseIdx <- 1
 DerivedIdx <- 2

 InputMM <- data.frame(
         month = format(TsdbInfo[[1]]$time, "%m"),
         Bucket = I(TsdbInfo[[BaseIdx]]$bucket),
         Base = TsdbInfo[[BaseIdx]]$value,
         Derived = TsdbInfo[[DerivedIdx]]$value )
 
 InputMM$month = as.integer(InputMM$month)
names(InputMM)[3:4] = CommodCurves


# RMG Regression 
Res = check.Regression(InputMM, FutPrice, 
        paste(Market,DerivedLoc,sep=""), options, save)
RegressionMM <- Res$Parms
FutPrice <- Res$Results

# Check SecDB Regression and RMG Regression
# Possible reasons for Difference include
# Tsdb DAM prices cleaning between Strst's regression and RMG regression
# Generate xtable files in tex format
for( bucket in names(RegressionMM) )
{
    aux <- paste(DerivedLoc, " Regressed Against ", BaseLoc, " for bucket ", bucket, sep="")
    
    fileName <- paste(save$dir$plots, "RMG_Regression_",Market,DerivedLoc,gsub(" ","",bucket),".tex", sep="")
    RMG_OutM <- data.frame(Month=month.abb, RegressionMM[[bucket]])
    print(xtable(as.data.frame(RMG_OutM),
                    caption=paste("RMG Regression in Market",Market,"of", aux,sep=" "),
                    display=c("s","s","f","f","f","f","f"),
                    digits=c(0,0,4,4,4,4,4)), file=fileName)
    
    fileName <- paste(save$dir$plots, "SecDB_Regression_",Market,DerivedLoc,gsub(" ","",bucket),".tex", sep="")
    SysBucket <- paste("Bucket_",gsub(" ","",bucket),sep="")
    SecDB_OutM <- data.frame(Month=month.abb, SysRegMM[[SysBucket]][[1]])
    names(SecDB_OutM) <- c("Month","Intercept","StdDev.Intercept","Slope","StdDev.Slope","R.Sqared")
    print(xtable(as.data.frame(SecDB_OutM),
                    caption=paste("SecDB Regression in Market",Market,"of", aux,sep=" "),
                    display=c("s","s","f","f","f","f","f"),
                    digits=c(0,0,4,4,4,4,4)), file=fileName)
    
    fileName <- paste(save$dir$plots,"Diff_RMG_SecDB_Regression_",Market,DerivedLoc,gsub(" ","",bucket),".tex", sep="") 
    Diff_OutM <- data.frame(Month=month.abb,
            Diff.Intercept = RMG_OutM$Intercept - SecDB_OutM$Intercept,
            Diff.Slope = RMG_OutM$Slope - SecDB_OutM$Slope,
            Diff.Intercept.Over.StdDev = (RMG_OutM$Intercept - SecDB_OutM$Intercept)/SecDB_OutM$StdDev.Intercept,
            Diff.Slope.Over.StdDev = (RMG_OutM$Slope - SecDB_OutM$Slope)/SecDB_OutM$StdDev.Slope)
    print(xtable(as.data.frame(Diff_OutM),
                    caption=paste("Difference between RMG and SecDB Regression in Market",Market,"of", aux,sep=" "),
                    display=c("s","s","f","f","f","f"),
                    digits=c(0,0,4,4,4,4)), file=fileName)                                              
}

# Compare Marks with SecDB Regression Computed Prices for Manual Adjustments

# underscore _ is replaced by dot . when transferred from Matlab back to R

RMG_Regression <- RegressionMM[[Bucket]]
SysBucket <- paste("Bucket_",gsub(" ","",Bucket),sep="")
SecDb_Regression <- SysRegMM[[SysBucket]][[1]]
colnames(SecDb_Regression) <- c("Intercept","StdDev.Intercept","Slope","StdDev.Slope","R.Sqared")

for (CurveMonth in 1:length(FutPrice$FutExp)) 
{
    
    Month <- FutPrice$Month[CurveMonth]
    SecDB_Slope <- SecDb_Regression[Month,"Slope"]
    SecDB_Intercept <- SecDb_Regression[Month,"Intercept"]
    FutPrice$SdbFit[CurveMonth] <- FutPrice$BasePrice[CurveMonth]*SecDB_Slope+SecDB_Intercept
    
}

fileName <- paste(save$dir$plots,"FutPrices_",Market,DerivedLoc,Bucket,".tex", sep="")
FutPrice$FutExp = as.character(FutPrice$FutExp)
FutPrice_LongTable <- xtable(FutPrice[,-c(1,2)],
        caption=paste("Future Prices from Marks and Regression"),
        display=c("s","s","f","f","f","f","f","f","f","f"),
        digits=c(0,0,2,2,2,2,2,2,2,2)) 
print(FutPrice_LongTable,
        tabular.environment = "longtable",
        floating = FALSE,
        file=fileName)    

fileName <- paste(save$dir$plots,"FutPrices_",Market,DerivedLoc,Bucket,".csv", sep="")
write.csv(FutPrice,file=fileName) 

fileName <- paste(save$dir$plots,"FutPrices_",Market,DerivedLoc,Bucket,".pdf", sep="")
if (save$Analysis$all){pdf(fileName, width=8.0, height=6.0)}
matplot(FutPrice[,c("DerivedPrice","SdbFit","RMGFit","PILower","PIUpper")],
        col = c("blue","black","blue","red","red"),
        type = c("p","b","b","l","l"),
        pch = c("P","S","R"),
        main="Future Prices from Regression and System Marks",
        cex.main=1, ylab="Prices",xaxt="n")
Ticks <-  seq(1,length(FutPrice$FutExp),by=3)
axis(side=1,at = Ticks ,labels=FutPrice$FutExp[Ticks],las=2,cex.axis=0.7)
mtext("Lines mark Prediction Intervals",line = 0,cex = 0.7)
mtext("P marks SecDB prices",line = -1,cex = 0.7)
mtext("S marks SecDB Fits",line = -2,cex = 0.7)
mtext("R marks RMG Fits",line = -3,cex = 0.7)

if (save$Analysis$all){dev.off()}

make.pdf(save,options,paste(Market,DerivedLoc,sep=""),Bucket)

#}
