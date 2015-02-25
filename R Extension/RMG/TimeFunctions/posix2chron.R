# Convert a Posix date to a Chron date
#
#
# written by Adrian Dragulescu on 8-Oct-2003

posix2chron <- function(date){

   library("chron")
   dateFormated <- strftime(date, format="%m/%d/%y")
   return(chron(dateFormated))  
}
