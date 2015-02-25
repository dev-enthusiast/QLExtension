# Set all the global parameters of the load model
# for both deconstruct.load and construct.load
#
# Written by Adrian Dragulescu on 13-Feb-2004

set.parms.load.model <- function(save, options){
  
    library(MASS); library(chron); library(lattice); library(Simple)
    library(xtable)
    bkg.col <- trellis.par.get("background")
    bkg.col$col <- "white"
    trellis.par.set("background", bkg.col)

    if (length(options$scale.factor)==0){
      options$scale.factor <- 1} # multiply all loads by this
    if (length(options$digits.table)==0){options$digits.table <- 2}
    if (length(options$forecast$loadgrowth)==0){
      options$forecast$loadgrowth <- "Historical"}# in %/year or Historical
    if (length(options$detrend)==0){options$detrend <- TRUE}
    
    options$loadName <- gsub("\_"," ",options$account.name)
    save$Results <- 1             # save final results to file
    save$Analysis$all <- 1        # save all the intermediate results
    save$Analysis$LTplots <- 1    # save LT regression plots
    save$AsOf <- date()           # when was the analysis done 
    save$dir$plots   <- "Plots/"
    save$dir$Rtables <- "Rtables/"
    if (file.access(save$dir$plots)!=0) {dir.create(save$dir$plots)}
    if (file.access(save$dir$Rtables)!=0){dir.create(save$dir$Rtables)}
    
    options$main   <- paste(options$loadName, "load") 
    options$file.hist.temp <- paste("S:/All/Risk/Data/Weather/Calibrations/parms.",
                options$airport.name, ".Rdata", sep="")
    options$path.L <- "H:/user/R/RMG/Models/Load/"
    options$lib.TF <- "H:/user/R/RMG/TimeFunctions/"
    
    source(paste(options$lib.TF,"chron2posix.R", sep=""))
    source(paste(options$lib.TF,"posix2chron.R", sep=""))
    source(paste(options$lib.TF,"ymd.R", sep=""))
    source(paste(options$lib.TF,"summarize.monthly.R", sep=""))

    source(paste(options$path.L, "deconstruct.load.R", sep=""))
    source(paste(options$path.L, "regressLT.R", sep=""))
    source(paste(options$path.L, "construct.load.R", sep=""))
    source(paste(options$path.L, "hist.simulate.load.R", sep=""))
    source(paste(options$path.L, "trend.functions.R", sep=""))
    source(paste(options$path.L, "plot.hist.load.R", sep=""))
    source(paste(options$path.L, "plot.forecasted.load.R", sep=""))
    source(paste(options$path.L, "plot.vlr.R", sep=""))
    source(paste(options$path.L, "make.pdf.R", sep=""))

    return(list(save, options))
}
