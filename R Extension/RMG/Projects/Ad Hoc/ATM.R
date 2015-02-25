######################################################################
#Code for ad hoc ATM data reorganization
#
#Author: Sean McWilliams
#
#July 11, 2008

#connect to excel file and load librarires
tableNames <- c("ANCILLARIES", "COAL","EMISSION","GAS","ICAP","MIX","POWER","TENOR","WEATHER","ENTRY TYPE","FREIGHT TIME","FREIGHT VOYAGE","INTERNATIONAL COAL","INTERNATIONAL GAS","OIL CRUDE","OIL PRODUCT GALS","OIL PRODUCT MT","URANIUM KGU","URANIUM LB","URANIUM SWU")
helper <- function(tableNames,channel){
	data <- sqlFetch(channel,tableNames[1])[-1]
	data[,3:length(data)]<- lapply(data[,3:length(data)],as.character)
	data$CEG_ID <- as.character(data$CEG_ID)
	data$TRADER_NAME <- tolower(as.character(data$TRADER_NAME))
	data <- melt(data,id=c("CEG_ID","TRADER_NAME"))
	data$variable <- as.character(data$variable)
	data$value <- as.character(data$value)
	data$variable <- paste(data$variable,tableNames[1],sep="")
	data <- data.frame(data)
	
	
	i <- 2
	
	while(i <= 20){
		temp <- sqlFetch(channel,tableNames[i])[-1]
		temp[,3:length(temp)] <- lapply(temp[,3:length(temp)],as.character)
		temp$CEG_ID <- as.character(temp$CEG_ID)
		temp$TRADER_NAME <- tolower(as.character(temp$TRADER_NAME))
		temp <- melt(temp,id=c("CEG_ID","TRADER_NAME"))
		temp$variable <- as.character(temp$variable)
		temp$value <- as.character(temp$value)
		temp$variable <- paste(temp$variable,tableNames[i],sep="")
		temp <- data.frame(temp)
		data <- rbind(data,temp)
		i <- i + 1
		
	}
	data <- cast(data, CEG_ID + TRADER_NAME ~ variable,function(x){x[1]})
	data <- data.frame(data)
	data[,3:length(data)] <- lapply(data[,3:length(data)],as.character)
	return(data)
}
data <- helper(tableNames, channel)
changeFalse <- function(col){
	return(unlist(lapply(col,function(x){if(is.na(x)){x <- NA} else if(x==FALSE){x <- "F"}else{x <- x}})))	
}
helper <- function(){
	i <- 1
	while(i <= 20){
		assign(paste("data",i,sep""),5)
		i <- i + 1
		
	}
}