#
# options <- NULL
# options$portfolio.name <- "RISK UK PM PORTFOLIO"
# options$portfolio.name <- "RISK UK MNEALE PORTFOLIO"
# options$dir <- "S:/Risk/Reports/UK VaR SAS/WORK/"
# options$portfolio.name  <- "FREIGHT ACCRUAL PORTFOLIO"
# options$dir <- "S:/Risk/Reports/TEST/"
# source("h:/user/R/RMG/Energy/Tools/get.var.history.R")
# 
get.var.history <- function(options) {

  file.names <- list.files(options$dir )

  res <- NULL
  
  for (i in 1:length(file.names)) {
     wk <- NULL
     x <- unlist(strsplit(file.names[i], "_"))
     
     if (length(x) > 1) {
       pricing.date <- unlist(strsplit(x[1],"R"))        
       if (length(pricing.date) > 1) {
          wk$pricing.date <- unlist(strsplit(x[1],"R"))[2]
       } else {
          wk$pricing.date <- x[1]
       } 
          
       wk$portfolio <- unlist(strsplit(x[2], "\\."))[1]
  
       if ((wk$pricing.date > "20061231" ) & (options$portfolio.name == toupper(wk$portfolio))) {
          csv.file.name <- paste(options$dir, file.names[i], sep="")
          csvcontents <- read.csv(file=csv.file.name,head=TRUE,sep=",")
          wk$Y2007 <- as.character(csvcontents$X_2007)[1]
          if ( is.na(wk$Y2007)) {
              wk$Y2007 = 0 
          }
          wk$Y2008 <- as.character(csvcontents$X_2008)[1]
          if ( is.na(wk$Y2008)) {
              wk$Y2008 = 0 
          }
          wk$Y2009 <- as.character(csvcontents$X_2009)[1]
          if ( is.na(wk$Y2009)) {
              wk$Y2009 = 0 
          }
          wk$Y2010 <- as.character(csvcontents$X_2010)[1]
          if ( is.na(wk$Y2010)) {
              wk$Y2010 = 0 
          }
          wk$Y2011 <- as.character(csvcontents$X_2011)[1]
          if ( is.na(wk$Y2011)) {
              wk$Y2011 = 0 
          }
          wk$Y2012 <- as.character(csvcontents$X_2012)[1]
          if ( is.na(wk$Y2012)) {
              wk$Y2012 = 0 
          }
          wk$Y2013 <- as.character(csvcontents$X_2013)[1] 
          if ( is.na(wk$Y2013)) {
              wk$Y2013 = 0 
          }
          #wk$Y2014 <- as.character(csvcontents$X_2014)[1] 
          #if ( is.na(wk$Y2014)) {
          #    wk$Y2014 = 0 
          #}
          #wk$Y2015 <- as.character(csvcontents$X_2015)[1] 
          #if ( is.na(wk$Y2015)) {
          #    wk$Y2015 = 0 
          #}
          #wk$Y2016 <- as.character(csvcontents$X_2016)[1] 
          #if ( is.na(wk$Y2016)) {
          #    wk$Y2016 = 0 
          #}
          wk$TOTAL <- as.character(csvcontents$Total)[1] 
          if ( is.na(wk$TOTAL)) {
              wk$TOTAL = 0 
          }
          wk <- data.frame(wk)
          res <- data.frame(rbind(res, wk))
       }
     }
     
  }
  
  res <- data.frame(res)
  return(res)
}