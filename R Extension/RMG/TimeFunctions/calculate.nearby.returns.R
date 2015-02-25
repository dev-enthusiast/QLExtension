# Calculate the nearby log-returns given a data.frame of forward prices
# Where data is a data.frame with rows = pricing.dates
#                          and columns = contract dates.
#       ind.nearby is an array of indices with the nearbys that you need.
#
# Written by Adrian Dragulescu on 22-Jun-2005

calculate.nearby.returns <- function(data, ind.nearby){

 if (nargs()==1){ind.nearby <- 1:ncol(data)}
 #----------------------------------------Remove cash prices------
 fpricing.dt <- format(as.Date(rownames(data)), "%b%y")
 for (col in 1:36){
   ind <- which(fpricing.dt==colnames(data)[col])
   if (length(ind)>0){data[ind,col] <- NA}
 }
 
 #----------------------------------------Remove columns with NA's only
 ind <- which(apply(is.na(data), 2, all))     
 if (length(ind)>0){data <- data[,-ind]}      

 rdata <- NULL
 while (length(which(is.na(data[,1])))>0){     
   ind <- which(!is.na(data[,1]))
   N   <- length(ind)
   r   <- data.frame(log(data[ind[2:N],ind.nearby]/data[ind[1:(N-1)],ind.nearby]))
   colnames(r) <- ind.nearby
   r   <- cbind(from=rownames(data)[ind[1:(N-1)]], to=rownames(data)[ind[2:N]], r)
   rownames(r) <- 1:nrow(r)
   rdata <- rbind(rdata, r)
   
   data <- data[-ind,]; data <- data[,-1]
 }
# colnames(rdata) <- ind.nearby
 
 return(as.data.frame(rdata))
}
