#-------------------------------------------------------------------------------
# Get MatlabChanges and put into a formatted weekly reports
# Haibin.Sun@constellation.com
# Change inputs DateBeg & DateEnd and run

rm(list=ls())

RetrieveMatlabProductionChanges <- function(DateBeg,DateEnd,DateAsOf,MaxTry=10)
{
    library(RODBC)
    DSN  <- "FileDSN=//ceg/cershare/All/Risk/Software/R/prod/Utilities/DSN/CPSPROD.dsn"
    SQL  <- paste(DSN, ';UID=APPCM;PWD=APPCM;', sep='')
    ITry <- 1
    Conn <- NULL
    while( ITry<MaxTry & !('RODBC' %in% class(Conn)) )
    {
        Conn <- odbcDriverConnect(SQL)
        ITry <- ITry+1
    }
    if( ITry>MaxTry )
        cat('Connection to CPSProd failed after',MaxTry,'connection attemps')

    SQL <- paste('SELECT NVL(ITEM_ID,-1)                              AS ITEM_ID,',
                        'NVL(PRODUCT_NAME,\'NA\')                     AS PROJECT_NAME,',
                        'NVL(ABSTRACT,\'NA\')                         AS ABSTRACT,',
                        'DECODE(DEVELOPER,\' \',USER_NAME,DEVELOPER)  AS DEVELOPER,',
                        'NVL(TAG_DATE,TO_DATE(\'2000\',\'YYYY\'))     AS TAG_DATE,',
                        'NVL(STATUS_NAME,\'NA\')                      AS STATUS,',
                        'NVL(COMMIT_COMMENT,\'NA\')                   AS COMMIT_COMMENT,',
                        'NVL(ASSIGNEE,\'NA\')                         AS ASSIGNEE,',
                        'NVL(FILENAME,\'NA\')                         AS FILENAME,',
                        'NVL(REVISION,\'NA\')                         AS REVISION',
                 'FROM   APPCM.SOXCPSAPP_MATLAB',
                 'WHERE  TAG_NAME = \'INSTALL\'',
                        'AND TAG_DATE BETWEEN TO_DATE(\'',format(DateBeg,'%m%d%y %H:%M'),'\',\'mm/dd/yy HH24:mi\')',
                        'AND TO_DATE(\'',format(as.POSIXct(as.POSIXlt(DateEnd+1))-1,'%m%d%y %H:%M'),'\',\'mm/dd/yy HH24:mi\')',
                  sep = ' ')
    MatlabChanges = sqlQuery(Conn, SQL);
    odbcClose(Conn);

    Idx                    <- which( as.character(MatlabChanges$PROJECT_NAME) != 'NA' )
    Projects               <- as.data.frame(unique(paste(MatlabChanges$PROJECT_NAME[Idx], ' - ', MatlabChanges$ABSTRACT[Idx])))
    names(Projects)        <- 'Projects & Abstracts'
    Idx                    <- sort(as.character(MatlabChanges$DEVELOPER),index.return=TRUE)$ix
    MatlabChanges          <- MatlabChanges[Idx,]
    MatlabChanges          <- MatlabChanges[c('DEVELOPER','TAG_DATE','FILENAME','REVISION','COMMIT_COMMENT')]
    MatlabChanges$TAG_DATE <- as.Date(as.POSIXlt(MatlabChanges$TAG_DATE))
    DatePeriod             <- as.data.frame(paste(format(DateBeg,'%Y%m%d'),' - ',format(DateEnd,'%Y%m%d')))
    names(DatePeriod)      <- 'Date Period'
  
    source('H:/_Work/Programming. R/Functions/lib.excel.functions.R')
    FileName  <- paste('H:/_Work/_BCC/',format(DateAsOf,'%Y%m%d'),'.xls',sep='')
    SheetName <- 'Matlab.Changes'

    CP                <- NULL
    CP$Font           <- "Bold"
    CP$FontSize       <- 10
    CP$FontColorIndex <- 9
    CP$range          <- c('A1','A1')
    XLS$write.xls(DatePeriod, FileName, SheetName, top.cell='A1',propertyList=CP)
    CP$range          <- c('A4','A4')
    XLS$write.xls(Projects, FileName, SheetName, top.cell='A4',propertyList=CP)
    ProjectCellID     <- paste('A',length(Projects[[1]])+6,sep='')
    CP$range          <- c(ProjectCellID, XLS$advance.cell.coordinates(ProjectCellID,0,length(MatlabChanges)))
    XLS$write.xls(MatlabChanges,FileName,SheetName, top.cell=ProjectCellID,propertyList=CP)
    XLS$delete.sheet(FileName, 'Sheet1')

    return(MatlabChanges)
}

#-------------------------------------------------------------------------------
DateBeg       <- as.Date('2008-05-02')
DateEnd       <- as.Date('2008-05-08')
DateAsOf      <- Sys.Date()
MatlabChanges <- RetrieveMatlabProductionChanges(DateBeg,DateEnd,DateAsOf,MaxTry=10)