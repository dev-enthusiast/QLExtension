# The function returns the number of days in a month.
# Where: data is the first day of the month, of class "Date"  
#
# Written by Zhendong Xia on 5-May-2004

days.in.month <- function(data){
    m <- months(data)
    for (j in 31:1){
        if (months(data+j-1) == m){
            break
        }
    }
    return(j)
  }
