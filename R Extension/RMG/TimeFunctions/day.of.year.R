# Calculate the day of the year.
# Where x is an R date.
#
# Written by Adrian Dragulescu on 17-Feb-2005

day.of.year <- function(x){
  year <- as.numeric(format(x, "%Y"))
  boy  <- as.Date(paste(year,"01-01", sep="-")) # beginning of the year
  return(as.numeric(x-boy+1))
}
