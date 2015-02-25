# Lib to compare data.frames and report changes
# This is used to monitor changes between two different time snapshots. 
#
# .change_data_frames
# .compare_data_frames
#
#


##################################################################
# Look at the changes in two data.frames, for example positions or prices ...
#  Data frames x and y have the data in melted format id1, id2, ..., value
# 
#
.change_data_frames <- function(x, y)
{
  colnames(x)[which(colnames(x)=="value")] <- "value.1"
  colnames(y)[which(colnames(y)=="value")] <- "value.2"
  res <- merge(x, y, all=TRUE)

  res$value.1[is.na(res$value.1)] <- 0
  res$value.2[is.na(res$value.2)] <- 0

  res$change <- res$value.2 - res$value.1
  
  
  res
}


##################################################################
# Compare x with y.  y is the "new" observation set.
# Data frames x and y have the data for two timestamps in the "long"  
# melted format id1, id2, ..., variable, value
#
#
.compare_data_frames <- function(x, y)
{
  if (any(colnames(x) != colnames(y)))
    stop("Columns of x and y need to be the same!")

  x$`_x_ind_` <- 1:nrow(x)
  y$`_y_ind_` <- 1:nrow(y)
  yUnch <- merge(x, y)
  
  # get the unchanged y rows
  yNew <- subset(y, !(y$`_y_ind_` %in% yUnch$`_y_ind_`))
  yNew$`_x_ind_` <- NULL
  yNew$`_y_ind_` <- NULL
 
  # get the removed x rows
  xRm <- subset(x, !(x$`_x_ind_` %in% yUnch$`_x_ind_`))
  xRm$`_x_ind_` <- NULL

  # clear up the yUnch
  yUnch$`_x_ind_` <- NULL
  yUnch$`_y_ind_` <- NULL

  list(xRemoved=xRm, yNew=yNew, unchanged=yUnch)
  
}

##################################################################
##################################################################
#
.test <- function()
{
  require(CEGbase)
  require(plyr)
  require(reshape2)

  source("H:/user/R/RMG/Energy/Trading/Congestion/lib.compare.df.R")


  x <- data.frame(code=c("BWI", "BOS", "BOS"),
                  variable=c("Tmax", "Tmin", "Tmax"),
                  temp=c(100, 50, 97))
  
  y <- data.frame(code=c("DCA", "BWI", "BOS", "BOS"),
                  variable=c("Tmin", "Tmax", "Tmin", "Tmax"),
                  temp=c(73, 100, 77, 101))
  
  res <- .compare_data_frames(x, y)
  res


  
  
  
}



