#
#
#
#

get.calculators <- function(file.name) {
    cat("Get Calculctor Starts ... ")
    require(xlsReadWrite) 
    require(SecDb) 
    #file <- "//ceg/cershare/All/Risk/Software/R/prod/Energy/Tools/BasisVaR/BasisVaRCalculator.xls"
    file <- "H:/user/R/RMG/Energy/Tools/BasisVaR/BasisVaRCalculator.xls"
    #file <-  file.name
    input.type <-  c(rep("character", 8), rep("isodate", 2) )
                   
    inputs <- read.xls(file, colClasses=input.type, sheet="InputToModel")

    calculators <- get.calcs(group=inputs[1,"group"], trader=inputs[1,"trader"], calc="")
    head(calculators)
    cat("Get Calculctor Ends ... ")
    return(calculators)
}


get.calcs <- function(group="CPS Marketing Group", trader=c("DC PEP"), calc="Credit"){
  require(SecDb)
  source("//ceg/cershare/All/Risk/Software/R/prod/Energy/VaR/Tools/utils.combine.VaR.R")
  #source("//ceg/cershare/All/Risk/Software/R/prod/Utilities/Interfaces/SecDb/R/SecDb.R")

  calcs <- NULL
  for (i in 1:length(trader)) {
    all.calcs <- calculator.list(group, trader[i])
    for (j in 1:length(calc)) {
      ind <- grep(calc[j], all.calcs)
      calc.names <- all.calcs[ind]
      calcs <- rbind(calcs,cbind(rep(group, length(calc.names)), rep(trader[i], length(calc.names)), calc.names))
    }
  }
  colnames(calcs) <- c("group", "trader", "calc")
  return(as.data.frame(calcs))
}
