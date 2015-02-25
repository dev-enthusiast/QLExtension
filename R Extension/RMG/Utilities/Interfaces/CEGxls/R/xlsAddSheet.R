xlsAddSheet <- function(parentfile, parent.sheetname, child.sheetname)
{   
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
