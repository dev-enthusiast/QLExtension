# convert a Goldman Sachs date into regular date
# dateOut <- dateGS(dateIn) so "K03" --> 2003-05-01
# dateIn  is a concatenation of character strings 
# dateOut is POSIX time, the format of strptime
# if direction=+1 from Goldman to POSIX
#    direction=-1 from POSIX to Goldman
#    options$chron=1 if dateIn or dateOut is in chron format
#
# written by Adrian Dragulescu on 13-May-2003

dateGS <- function(dateIn,direction,options){

  if (nargs()==2){options <- list()}
  if (length(options$chron)==0){options$chron=0}
  if ((options$chron==1) & (direction==-1)){
     dateIn <- strptime(as.character(dateIn),format="%m/%d/%y")
  }
  
  month=c("F","G","H","J","K","M","N","Q","U","V","X","Z")
  lmonth=c("Jan","Feb","Mar","Apr","May","Jun","Jul",
           "Aug","Sep","Oct","Nov","Dec")
  dateOut <- character()
  nD <- length(as.character(dateIn))   # number of dates
  for (n in 1:nD){
    if (direction==1){                 # Goldman -> POSIX
       aux <- substr(dateIn[n],1,1)
       bux <- grep(aux,month)
       mth <- lmonth[bux]
       yr <-  substr(dateIn[n],2,3)
       date <- paste("1",mth,yr,sep="-")
       dateOut <- c(dateOut,as.character(strptime(date,format='%d-%b-%y')))
    }else { if (direction == -1){     # POSIX -> GOLDMAN
       aux <- strftime(dateIn[n], format="%m%Y")
       mth <- month[as.numeric(substr(aux,1,2))]
       yr <- substr(aux,5,6)
       dateOut <- c(dateOut,paste(mth,yr,sep=""))
           } else {
                 sprintf("Unknown direction, check inputs.")
     }}
  } # close the for loop
  if (direction==1){
     dateOut <- strptime(dateOut, format="%Y-%m-%d")
     if (options$chron==1){
        dateOut <- strftime(dateOut, format="%m/%d/%y")
        dateOut <- chron(as.character(dateOut))
     }
  }
  dateOut
}

  
