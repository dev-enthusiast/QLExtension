# Map a date to a season.  
# Where data is a data frame with columns [year month]
# options$season can take "season year" = calendar season + year
#                         "season"      = calendar season
#                         "shoulder"    = peak 1,2,6,7,8,12, shoulder 3,4,5,9,10,11  
#                         "gas"         = Nov-Mar and Apr-Oct
#
# Last modified by AAD on 19-Apr-2006

calculate.season <- function(data, options){

  if (nargs()==1){options <- list()}
  if (length(options$season)==0){options$season <- "season year"}
  if (!is.numeric(data$year)){data$year <- as.numeric(as.matrix(data$year))}
  if (!is.numeric(data$month)){data$month <- as.numeric(as.matrix(data$month))}

  Season.Year=count=NULL
  if ((options$season=="season year")|(options$season=="season")){
    Season=NULL
    ind <- which(data[,"month"] %in% c(3,4,5));   Season[ind] <- "Spring"
    ind <- which(data[,"month"] %in% c(6,7,8));   Season[ind] <- "Summer"
    ind <- which(data[,"month"] %in% c(9,10,11)); Season[ind] <- "Fall"
    ind <- which(data[,"month"] %in% c(12,1,2));  Season[ind] <- "Winter"
    seasonal.year <- data[,"year"]
    ind <- which(data[,"month"] %in% c(1,2))      # bump the year index for 
    seasonal.year[ind] <- seasonal.year[ind] - 1  # Jan and Feb
    Season.Year <- paste(Season, seasonal.year)
    Season.Year <- ordered(Season.Year, levels=unique(Season.Year))

    # count the number of observations in each season
    t.Season <- as.numeric(table(Season.Year))
    for (s in 1:length(t.Season)){count <- c(count, 1:t.Season[s])}
    if (options$season=="season"){Season.Year <- Season}
    
  }
  if (options$season=="shoulder"){
    Season.Year <- matrix("Peak", nrow=nrow(data), ncol=1)
    ind <- which(data$month %in% c(3, 4, 5, 9, 10, 11))
    Season.Year[ind,1] <- "Shoulder"
    count=0  # not supported yet
  }
  if (options$season=="gas"){
    Season.Year <- matrix("Summer", nrow=nrow(data), ncol=1)
    ind <- which(data$month %in% c(1, 2, 3, 11, 12))
    Season.Year[ind,1] <- "Winter"
    count=0  # not supported yet
  }
    
  return(cbind(data, count=count, season=Season.Year))
}
