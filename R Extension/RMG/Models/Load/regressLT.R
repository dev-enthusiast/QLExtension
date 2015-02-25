# The regression of load to temperature, daily load vs daily temperature
# options.m       - month, numeric 1-12 
#        .dayType = "WE", or "WD"
#
# written by Adrian Dragulescu on 12-Feb-2004

regressLT <- function(load,temp,save,options){

  if (options$dayType=="WE"){
      exp.day <- "Weekend"
  } else {exp.day <- "Weekday"}

  res <- rlm(load ~ temp)
  if ((save$Analysis$all)&(save$Analysis$LTplots)){
    pName <- paste("LT--",options$dayType,"--",options$m,".pdf", sep="") 
    pName <- paste(save$dir$plots, pName, sep="") # plot name
    pdf(pName)
    plot(temp,load, xlab="Daily temperature",
       ylab="Daily load") 
    abline(res, col="blue", lwd=2)
    par(cex.main=1)
    title(paste(options$loadName, month.abb[options$m], exp.day, sep=", "))
    dev.off()
  }  
  return(res)
}
