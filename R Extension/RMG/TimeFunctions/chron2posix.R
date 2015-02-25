# Convert a Chron date to a Posix date
#
# written by Adrian Dragulescu on 8-Oct-2003

chron2posix <- function(date){

  return(strptime(as.character(date), format="%m/%d/%y")) 
}
