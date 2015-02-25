##
##
##
############################################################################################
 # This table combination utility allows to combine a child table(as a sheet in Excel or a txt file)
 # to a parent table(an excel sheet), with sheetnames of both pre-defined. The child table may contain different number of
 # columns than those of the parent table. The extra columns of the child table will be added to the end of the parent's columns
 # The tables are merged according to either rows or columns:
 #    
 # Parameters:
 #    --childfile, parentfile: work path of the files. Must include the names of the files.
 #      E.g. parentfile = paste("H:/user/R/RMG/Models/Price/ForwardCurve/Network/allcurves.xls")
 #
 #    --add = "rows"(default): will add the rows of the child table to the parent. The first column of the tables are used as id
 #      add = "columns": will add columns of the child table to the parent. The first rows of the two are used as id.
 #
 #    --tag: used to add an addition column/row in the merged table to put in comments, etc.
 #           
 #    --comments: add to the tag column        
 #  Note: the Excel files must be closed when this function is run.
 #
  excel.combine.tables <- function(childfile, parentfile, child.sheetname, parent.sheetname,
                                filetype ="xls", add = "rows", tag = FALSE, newcol.name =NULL, comments = NULL ){
    require(RODBC)
    require(rcom)
    #require(reshape)
    #read child table
    if (filetype == "xls") {
        child.con <- odbcConnectExcel(childfile)
        child.table <- sqlTables(child.con)
        child.data <- sqlFetch(child.con, child.sheetname)
        }else if(filetype == "txt"){
        #read the child file as a .txt file
        }
    

    #import parent table
    parent.con <- odbcConnectExcel(parentfile)
    parent.table <- sqlTables(parent.con)
    parent.data <- sqlFetch(parent.con, parent.sheetname)
    odbcCloseAll()

    if (add == "rows") { #will add new rows to the parent table

        idx = which(!(colnames(parent.data)%in% colnames(child.data)))
        child.new = child.data
        for (i in idx) {#child doesn't have these columns, will add those columns as blanks
               child.new =data.frame(child.new, newcol="")
               colnames(child.new)[length(child.new)] <- colnames(parent.data)[i]
        }
        if(tag == TRUE){#will add an addtional column called TAG_asOfDate, for adding comments
            if (!(newcol.name %in% colnames(child.new))){
               child.new = data.frame(child.new, newcol = "")
               colnames(child.new)[length(child.new)] <- newcol.name
            }
            child.new[, newcol.name] <- comments
       
        }
        idx2 = which(!(colnames(child.new)%in% colnames(parent.data)))
        parent.new = parent.data
        for (i in idx2) {#parent table doesn't have these columns, will add those columns as blanks
               parent.new =data.frame(parent.new, newcol="")
               colnames(parent.new)[length(parent.new)] <- colnames(child.new)[i]
        }

        parent.new <- parent.new[!is.na(parent.new[,1]),]
        child.new <- child.new[!is.na(child.new[,1]),]
        p_c <- rbind(parent.new, child.new)
        p_c <-as.matrix(p_c)
        newTable = as.list(NULL)
        newTable$table <- data.frame(p_c[order(p_c[,1]),])
        newTable$idx <- which( newTable$table[,1] %in% child.new$SecDbCurve) 
        newTable 
      }
      
  }

#
##############################################################################################
# Will set properties to a selected range. A default propertyList is set for font changes only
  excel.set.property <- function(filepath = NULL, sheetname = NULL, range = NULL, propertyList = list(Font = FALSE, Font.size = 10,
                                  Font.colorIndex = 1, Comment = FALSE, comments = NULL)){
    
    if(is.null(filepath) && is.null(sheetname) && class(range) == "COMObject" ){
      range$Activate()
      # set font
      if(!is.null(propertyList$Font) & propertyList$Font == TRUE){
            Font =comGetProperty(range, "Font")
            Font[["ColorIndex"]] <-propertyList$Font.colorIndex
            Font[["size"]] <- propertyList$Font.size
      }
      #set other properties
      #to be implemented
      #...
      
    }else if (!is.null(filepath) && !is.null(sheetname) && !is.null(range))
    {
      xls <- comCreateObject("Excel.Application")
      wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
      sheets <- comGetProperty(xls, "Sheets")
      comSetProperty(xls, "Visible", TRUE)  
      sheet <- comGetProperty(sheets, "Item", sheetname)
      range <- comGetProperty(sheet, "Range", range)
      range$Activate()
       # set font
      if(!is.null(propertyList$Font) & propertyList$Font == TRUE){
            Font =comGetProperty(range, "Font")
            Font[["ColorIndex"]] <-propertyList$Font.colorIndex
            Font[["size"]] <- propertyList$Font.size
      }
      comSetProperty(wb, "Save", filepath)      
      comInvoke(xls, "Quit")
      rm(xls)
   }#end of else


  }
  
  
#
#
  excel.set.column.property <-function(filepath, sheetname, column, propertyList) {
  
    xls <- comCreateObject("Excel.Application")
    wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
    sheets <- comGetProperty(xls, "Sheets")
    #comSetProperty(xls, "Visible", TRUE)  
    sheet <- comGetProperty(sheets, "Item", sheetname)
    range <- comGetProperty(sheet, "Columns", column)
    if(!is.null(propertyList$Font) & propertyList$Font == TRUE){
      Font <- comGetProperty(range,"Font")
      Font[["ColorIndex"]] <-propertyList$Font.colorIndex
      Font[["size"]] <- propertyList$Font.size
      }
    comSetProperty(wb, "Save", filepath)      
    comInvoke(xls, "Quit")
    rm(xls)  
}
#
########################################################################
#  Function to delete a sheet from a file
#  input filepath, and name of the sheet to be deleted
  excel.delete.sheet <- function(filepath=NULL, sheetname=NULL) {
       

      xls <- comCreateObject("Excel.Application")
      wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
      sheets <- comGetProperty(xls, "Sheets") 
      #comSetProperty(xls, "Visible", TRUE) 
      sheet.to.delete <- comGetProperty(sheets, "Item", sheetname)
      sheet.to.delete$Activate()
      sheet.to.delete$Delete()
      comSetProperty(wb, "Save", filepath)      
      comInvoke(xls, "Quit")
      rm(xls)
    
  }

#
########################################################################
#
  excel.sheet.rename <- function(filepath, oldname, newname) {

      xls <- comCreateObject("Excel.Application")
      wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
      sheets <- comGetProperty(xls, "Sheets")  
      sheet.to.rename <- comGetProperty(sheets, "Item", oldname)
      sheet.to.rename[["Name"]] <- newname
      comSetProperty(wb, "Save", filepath)      
      comInvoke(xls, "Quit")
      rm(xls)
    }
  
#
#########################################################################################
# will sort table on a sheet according to column A by ascending order
  excel.sort <-function(sheet){  #argument "sheet" is an Excel worksheet to be sorted
  #not quite sure how to do this in Excel directly. It's better to use R to do the job.

  }
  
#
#############################################################################
#       This one works the same as write.xls()
  excel.add.newsheet <- function(parentfile, parent.sheetname, child.sheetname) {
     
      #add merged table to a new sheet of the parent file
      xls <- comCreateObject("Excel.Application")
      wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", parentfile)
      comSetProperty(xls, "Visible", TRUE)
      sheets <- comGetProperty(xls, "Sheets")
      new.sheet <- sheets$Add()
      new.sheet[["Name"]] <- child.sheetname
      xls.colnames <- c(LETTERS,t(outer(LETTERS,LETTERS,"paste",sep="")))
      headers = colnames(p_c.new)
      h.range.end =paste(xls.colnames[length(headers)],"1",sep ="")
      h.range <- new.sheet[["Range", "A1", h.range.end]]
      h.range$Select()
      comSetProperty(h.range, "Value", as.matrix(headers))
      
      s.range.end <- paste(xls.colnames[ncol(p_c.new)],nrow(p_c.new)+1, sep ="")
      s.range <- new.sheet[["Range", "A2", s.range.end]]
      s.range$Select()
      
      comSetProperty(s.range, "Value", as.matrix(p_c.new))
      comSetProperty(wb, "Save", parentfile)
      
  }
 
#
#######################################################################################  
# Will add a table (given as dataframe) to a selected range.
  excel.add.table <- function(range, table.data){
      if(class(range) == "COMObject" ){
      if(range$Activate()== TRUE){
         comSetProperty(range, "Value", as.matrix(table.data))
      }
      else{
       rLog("Cannot activate the given range. Operation failed")
       }
    }
  }