################################################################################
#function that takes the first day of a month and returns the number of days
#in the month
#
#Author: Sean McWilliams
#
################################################################################

daysInMonth <- function(day){

	year <- as.integer(substring(as.character(day),1,4))
	month <- as.integer(format(as.Date(day), "%m"))
	if(month == 1){
		return(31)
	}
	else if(month == 2){
		if(((year %% 4 == 0) && (year %% 100 != 100)) || (year %% 400 == 0))
			return(29)
		else{
			return(28)
		}
	}
	else if(month == 3){
		return(31)
	}
	else if(month == 4){
		return(30)
	}
	else if(month == 5){
		return(31)
	}
	else if(month == 6){
		return(30)
	}
	else if(month == 7){
		return(31)
	}
	else if(month == 8){
		return(31)
	}
	else if(month == 9){
		return(30)
	}
	else if(month == 10){
		return(31)
	}
	else if(month == 11){
		return(30)
	}
	else if(month == 12){
		return(31)
	}
}