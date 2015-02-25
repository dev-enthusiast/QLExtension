xlsRenameSheet <- function(filepath, oldname, newname)
{
  xls <- comCreateObject("Excel.Application")
  wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
  sheets <- comGetProperty(xls, "Sheets")  
  sheet.to.rename <- comGetProperty(sheets, "Item", oldname)
  sheet.to.rename[["Name"]] <- newname
  comSetProperty(wb, "Save", filepath)      
  comInvoke(xls, "Quit")
  rm(xls)
}
