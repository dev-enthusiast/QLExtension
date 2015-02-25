#
#
# source("H:/user/R/RMG/Models/Gas/Gas.Products/Storage/fea.wrap.R")
#
#
fea.wrap <- function(file.name) {
    require(xlsReadWrite)
    source("H:/user/R/RMG/Models/Gas/Gas.Products/Storage/create.FEA.file.r")
  
    #file <- "H:/user/R/RMG/Models/Gas/Gas.Products/Storage/FEAFiles.xls"
    file <- file.name
    input.type <-  c("isodate", "isodate", "character", "character", "character")
                   
    inputs <- read.xls(file, colClasses=input.type, sheet="InputsToModel")

    create.FEA.file(secSymbol=inputs$SecDBSymbol, 
                    GDM=inputs$GDM, 
                    dateSt=as.Date(inputs$StartDate), 
                    dateEnd=as.Date(inputs$EndDate),
                    output=inputs$OutputDir, 
                    write=TRUE)
    return("Completed")
    
}