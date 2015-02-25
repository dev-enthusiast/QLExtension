
xlsDeleteSheet <- function(filepath=NULL, sheetname=NULL)
{
  xls <- comCreateObject("Excel.Application")
  wb  <- comGetProperty(comGetProperty(xls, "Workbooks"), "Open", filepath)
  sheets <- comGetProperty(xls, "Sheets") 
  comSetProperty(xls, "Visible", TRUE) 
  sheet.to.delete <- comGetProperty(sheets, "Item", sheetname)
  if( !is.null(sheet.to.delete) )
  {
      sheet.to.delete$Activate()
      sheet.to.delete$Delete()
  }
  comSetProperty(wb, "Save", filepath)      
  comInvoke(xls, "Quit")
  rm(xls)  
}
