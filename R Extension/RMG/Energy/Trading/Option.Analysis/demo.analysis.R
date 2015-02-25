# 
#
#
# Written by Adrian Dragulescu on 7-Sep-2005


options=NULL
options$books     <- c("FORWSTS", "FOREROVL")
options$asOfDate  <- as.Date("2006-09-07")
options$scenarios <- seq(0.8, 1.2, by=0.05)

options$out.dir  <- "S:/All/Risk/Analysis/Option.Analysis/"
options$source.dir <- "H:/user/R/RMG/Energy/Trading/Option.Analysis/"

source(paste(options$source.dir, "main.analysis.R", sep="")) 
aux <- main.analysis(options)
res <- aux[[1]]
MM  <- aux[[2]]

 
