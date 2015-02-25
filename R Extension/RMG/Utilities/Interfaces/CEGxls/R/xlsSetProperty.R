##############################################################################
# Set certain properties to a selected range. Where sheet is a COM object with 
# the sheet that you want to modify.  
# colors : 6 = yellow, 15 = gray
#
xlsSetProperty <- function(sheet = NULL,
  propertyList = list(range=c("A1", "C1"), Font = FALSE, FontSize = 10,
  FontColorIndex = 1, BackgroundColorIndex = 6, Comment = NULL, Value=NULL))
{
  if (!comIsValidHandle(sheet)){
    stop("Cannot set properties, the sheet is not a valid COM object.")
  }
  if (!is.list(propertyList[[1]])){propertyList <- list(propertyList)}
  
  # Loop over the property List
  for (i in 1:length(propertyList)){ 
    CP <- propertyList[[i]]
    r  <- comGetProperty(sheet, "Range", CP$range[1], CP$range[2])
    if (!r$Activate())stop("Cannot activate the range")

    fp <- comGetProperty(r, "Font")
    if ("FontSize" %in% names(CP)){ comSetProperty(fp, "size", CP$FontSize)}
    if ("Font" %in% names(CP)){ comSetProperty(fp, CP$Font, TRUE) }
    if ("FontColorIndex" %in% names(CP)){
      comSetProperty(fp, "ColorIndex", CP$FontColorIndex) }
    if ("BackgroundColorIndex" %in% names(CP)){
      aux <- comGetProperty(r, "Interior")
      aux[["ColorIndex"]] <- CP$BackgroundColorIndex
      #aux[["Color"]] <- CP$BackgroundColorIndex
      aux[["Pattern"]] <- "xlSolid"
    }
    if ("DrawlineSide" %in% names(CP)){   # does not work now...
      aux <- paste("Borders(xlEdge", CP$DrawlineSide, ")", sep="")
      bux <- eval(parse(text=paste('comGetProperty(r, "', aux, '")', sep="")))
    }
    if ("NumberFormat" %in% names(CP))
      comSetProperty(r, "NumberFormat", CP$NumberFormat)

    if (!is.null(CP$Value))
      comSetProperty(r, "Value", CP$Value)
  }  
}
