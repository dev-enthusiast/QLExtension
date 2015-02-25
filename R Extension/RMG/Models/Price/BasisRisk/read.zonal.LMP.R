# Read zonal LMP prices and bucket them daily.
#
#
#
# Written by Adrian Dragulescu on 3-Mar-2005


read.zonal.LMP <- function(save, options){

 lsave=save; loptions=options
 loptions$HS <- 1
 lsave$dir   <- paste(save$dir.hdata, options$run.name, "/Historical/", sep="")
 if (file.access(lsave$dir)!=0){dir.create(lsave$dir, recursive=T)}
 
 MM  <- tsdbRead(loptions, lsave)
# MM <-  tsdbRead.SecDB(loptions, lsave)
 ind <- pmatch(options$hdata.symbols, colnames(MM))
 colnames(MM)[ind] <- options$short.names

 options$noZones <- length(options$short.names)-1
 ind.H <- which(options$short.names == options$ref.name)
 options$zone.names  <- options$short.names[-ind.H]

 ind      <- which(colnames(MM) %in% c("year","month","day","hour",options$short.names))
 dMM      <- bucket.ts(MM[,ind], "daily")
 dMM$date <- as.Date(paste(dMM$year, dMM$month, dMM$day, sep="-"))
 season   <- calculate.season(dMM[,c("year","month","day")], options)
 dMM      <- cbind(dMM, season=season$season)

 #--------------------------------------------------------------------------
 #                                           Get current forward marks
 options$forward.marks <- array(NA, dim=c(length(options$fdata$months),
   length(options$short.names)), dimnames=list(options$fdata$months,
   options$short.names))
 for (c in 1:nrow(options$futStrip)){
   aux <- options$futStrip   
   res <- futStrip(aux$commod[c],aux$location[c],aux$bucket[c],options$fdata$startDate,
      options$fdata$endDate, options$asOfDate, "Energy", matlab)
   options$forward.marks[,c] <- res$values
 } 
 
 return(list(MM, dMM, options))
}

