# General settings
#
#
# Written by Adrian Dragulescu on 15-Apr-2005

set.parms.model <- function(save, options){
  
    require(lattice); require(Simple); require(xtable); require(RODBC)

    save$Results <- 1             # save final results to file
    save$Analysis$all <- 1        # save all the intermediate results
    save$dir=list()
    save$dir$main    <- paste(save$dir.main.root, "Analysis/", sep="")
    save$dir$plots   <- paste(save$dir$main, "Plots/", sep="")
    save$dir$tables  <- paste(save$dir$main, "Rtables/", sep="")
    if (file.access(save$dir$main)!=0)  {dir.create(save$dir$main)}
    if (file.access(save$dir$plots)!=0) {dir.create(save$dir$plots)}
    if (file.access(save$dir$tables)!=0){dir.create(save$dir$tables)}
   
    options$main   <- paste(options$unit.name, "unit") 
    if (length(options$scale.factor)==0){options$scale.factor <- 1000000} 
    options$path.G <- "H:/user/R/RMG/Models/Generation/Report/"
    options$lib.TF <- "C:/R/Work/MyLibrary/TimeFunctions/"
    
    source(paste(options$lib.TF,"summarize.monthly.R", sep=""))
    source("H:/user/R/RMG/Statistics/quantiles.by.factor.R")
    source("H:/user/R/RMG/Utilities/Database/CPSPROD/get.discount.factor.R")
    source("H:/user/R/RMG/Utilities/Math/extrapolate.R")

    return(list(save, options))
  }
