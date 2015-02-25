## This will check the position files in S:/All/Risk/Data/VaR/prod/Positions on the last system date
#  New curves that don't exist in "allcurves.xls" will be recorded in "positions.new.curves.asofdate.xls" 
#  Then these new curves will be added to the "allcurve.xls" file, and re-sorted by name
#  asofdate can be entered by user when calling this function
#
#  This code will use "excel.operations.R" on H:/user/R/RMG/Models/Price/ForwardCurve/Network/
#  written on 2008-01-21, by JB
# To use, type: 
# source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/positions.new.curves.R")
# positions.new.curves() 

positions.new.curves <- function(asOfDate = NULL){
  
  require(RODBC)
  require(SecDb)         
  
  if(is.null(asOfDate)) asOfDate = as.Date(secdb.dateRuleApply(Sys.Date(),"-1b"))
  load(paste("S:/All/Risk/Data/VaR/prod/Positions/vcv.positions.",asOfDate,".RData", sep =""))
  idx =which(QQ.all$DELTA != 0)
  pos.curves = unique(toupper(QQ.all$CURVE_NAME[idx])) #curvenames from the position file
  allcurve.filepath  <- "H:/user/R/RMG/Models/Price/ForwardCurve/Network/allcurves.xls"
  allcurve.con   <- odbcConnectExcel(allcurve.filepath)
  allcurve.table <- sqlTables(allcurve.con)
  allcurve.data  <- sqlFetch(allcurve.con, "allcurves")  #"allcurves" is table name in the workbook
  odbcCloseAll()
  allcurve.curves <- unique(toupper(allcurve.data$SecDbCurve))
  new.curves <- pos.curves[-which(pos.curves %in% allcurve.curves)] #find unregistered curves
  if (length(new.curves) >= 1) { #if there are new curves, then merge the new curves to the old ones, save and set properties
    #write new curve names to a file
    source("S:/all/Risk/Software/R/Prod/Utilities/write.xls.R")
    newcurves.filepath <- paste("H:/user/R/RMG/Models/Price/ForwardCurve/Network/position.newcurves.", asOfDate, ".xls", sep ="")
    newcurves <-data.frame(new.curves)
    colnames(newcurves) = paste("SecDbCurve")
    write.xls(newcurves, newcurves.filepath, "new curves", visible = "FALSE")
   
   
    #combine tables and save to a new table
    source("H:/user/R/RMG/Models/Price/ForwardCurve/Network/excel.operations.R")
    comments = paste("positions on ",asOfDate, sep= "")
    newTable <- excel.combine.tables(newcurves.filepath, allcurve.filepath, "new curves", "allcurves", add = "rows", 
                          filetype = "xls", tag = FALSE, "TAG", comments) 
    
  
    #save to a new sheet
    #newsheet.name = paste("allcurves_new_", asOfDate, sep ="")
    write.xls(newTable$table, allcurve.filepath, "allcurves", visible = "FALSE")
    
    #delete old allcurves sheet
    #excel.delete.sheet(allcurve.filepath, "allcurves")
    
    #browser()
    propertyList = list(Font = TRUE, Font.colorIndex = 1, Font.size = 10, Comment = FALSE)
    excel.set.column.property(allcurve.filepath,"allcurves", "A", propertyList)

    #set properties of the added rows of new allcurve sheet(set first column only)
    propertyList = list(Font = TRUE, Font.colorIndex = 3, Font.size = 10, Comment = FALSE)
    times = 1
    if(length(newTable$idx)> 20) times = ceiling(length(newTable$idx)/20)
    #select up to 20 cells each time. Cannot select too many cells at once in Excel.
          for( j in c( 0:(times-1))){ 
          sub.idx <- newTable$idx[j*20+1:(j*20+20)]+1
          sub.idx<-sub.idx[!is.na(sub.idx)]
          cells =NULL
          for (i in sub.idx[-length(sub.idx)]) cells = paste(cells, paste("A", i,",", sep=""), sep="")  
          cells = paste(cells, paste("A",sub.idx[length(sub.idx)], sep=""),sep="")
          excel.set.property(allcurve.filepath, "allcurves", cells, propertyList)
    }
   #excel.sheet.rename(allcurve.filepath, newsheet.name, "allcurves")   
  }#end of if, for need of adding new curve names
}

 #
 ############################################################################## 
 #
 #
 

#
#
##########################################################################################
#
# 
  add.curves <- function(newtable.path, oldtable.path,newtable.sheet, oldtable.sheet, propertyList = NULL) {
   require(RODBC)
   #require(SecDb)
   load.packages("rcom")  
   newtable.con   <- odbcConnectExcel(newtable.path)
   newtable.table <- sqlTables(newtable.con)
   newtable.data  <- sqlFetch(newtable.con, newtable.sheet) 
   odbcCloseAll()
 
   target.xls <- comCreateObject("Excel.Application")   
   target.wb  <- comGetProperty(comGetProperty(target.xls, "Workbooks"), "Open", oldtable.path)
   comSetProperty(target.xls, "Visible", TRUE)
   sheets <- comGetProperty(target.xls, "Sheets")
   target.sheet <- comGetProperty(sheets,"Item", oldtable.sheet)
   
   #new.xls <- comCreateObject("Excel.Application")
   #new.wb <- comGetProperty(comGetProperty(new.xls, "Workbooks"), "Open", newtable.path)
   #new.sheets <- comGetProperty(new.xls, "Sheets")
   #source.sheet <- comGetProperty(old.sheets,"Item", newtable.sheet)
   
   comInvoke(target.sheet, "Activate")
   colA <- comGetProperty(target.sheet, "Columns", "A")
   colA.value <- comGetProperty(colA, "Value")
   idx <- which(!is.na(colA.value)) #get the last row where there is a curvename, append new curves after that row
   range.top <-  paste("A", idx[length(idx)]+1, sep ="")
   
   xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))
   range.end <- paste(xls.colnames[ncol(newtable.data)],idx[length(idx)]+nrow(newtable.data), sep ="")
   range <- comGetProperty(target.sheet, "Range", range.top,range.end)
   #range$Select()
   browser()
   excel.add.table(range, newtable.data)
   excel.set.property(range, propertyList)
   #excel.sort(target.sheet)
   #range.data = comGetProperty(range, "Value")
   #comGetObjectInfo(range)
   #browser()
   #save and quit
   comSetProperty(target.wb, "Save", oldtable.path)
   comInvoke(target.xls, "Quit")
   rm(target.xls)
   
  }
  
  
  