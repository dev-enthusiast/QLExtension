# Calculate hourly shapes by month.  Do a Trellis plot and save the table. 
# Where data is a data frame with columns:
#    Year Month Day Hour x        - Hours in the day go from 0 to 23.
# If save$Analysis$all=1, provide save$Plots$filename
#                                 save$Tables$filename
#                                 options$main  for the title
#
# Written by Adrian Dragulescu on 1-Sep-2004

calculate.hourly.shapes <- function(data, options, save){

h.days   <- as.Date(paste(data[,1],data[,2],data[,3], sep="-"))
uDays    <- unique(days)
D        <- length(uDays)
h.months <- months(uDays)
uMonths  <- unique(h.months)

hMatrix <- matrix(NA, nrow=D, ncol=24)  # hourly matrix
for (d in 1:D){
  ind <- which(h.days == uDays[d])
  hours <- data[ind, 4] + 1
  hMatrix[d, hours] <- data[ind,5]
}
sumFun <- function(x){sum(x, na.rm=TRUE)}
meanFun <- function(x){mean(x, na.rm=TRUE)}
quantileFun <- function(x){quantile(x, probs=c(0.25,0.75), na.rm=TRUE)}
mean.X  <- apply(hMatrix, 1, meanFun)
shapeM  <- hMatrix/mean.X              # daily shape matrix, size [D,24]
shapeM[1:10,]

shape.Month=shape.Month.variation=NULL
for (m in uMonths){
  ind <- which(h.months==m)
  meanShape   <- apply(shapeM[ind,], 2, meanFun)
  meanShape   <- meanShape/meanFun(meanShape) # normalize the mean to 1
  shape.Month <- rbind(shape.Month, meanShape)
  variation   <- apply(shapeM[ind,], 2, quantileFun)
  shape.Month.variation <- rbind(shape.Month.variation,
                                 variation[2,]-variation[1,])
}
colnames(shape.Month) = colnames(shape.Month.variation) <- 1:24
rownames(shape.Month) = rownames(shape.Month.variation) <- uMonths
shape.Month
#shape.Month.variation # the difference between the 75 and 25 percentile

#-----------------------------------------------do the Trellis plot 
df.shapeM <- data.frame( Hour  = rep(1:24, times=D),
       Month = ordered(rep(h.months, each=24), levels=month.name),
       Shape = as.numeric(t(shapeM)));
df.shapeM[1:10,]

library(lattice)
bkg.col <- trellis.par.get("background")
bkg.col$col <- "white"
trellis.par.set("background", bkg.col)

if (save$Analysis$all){pdf(save$Plots$filename, width=7.25, heigh=6.5)}
print(xyplot( Shape ~ Hour | Month, data=df.shapeM, 
   panel=function(x,y, subscripts){
     hF    <- factor(x)
     mthPanel <- as.character(df.shapeM[subscripts[1],"Month"])
     meanS    <- shape.Month[mthPanel,] 
     panel.xyplot(x,y, col="grey", pch=".")
     panel.xyplot(levels(hF), meanS, col="blue", pch=20)
   },
   layout=c(6,2),    
   xlab="Hour of day",
   ylab="Shape factor",
   main=options$main))
if (save$Analysis$all){dev.off()}

#----------------------------------------------write the table
ordered.shape.Month <- shape.Month
for (m in 1:12){
  ind <- which(rownames(shape.Month)==month.name[m])
  ordered.shape.Month[m,] <- shape.Month[ind,]
}
rownames(ordered.shape.Month) <- month.name
ordered.shape.Month <- cbind(month.name, ordered.shape.Month)
colnames(ordered.shape.Month) <- c(options$point, paste("h",(1:24),sep=""))
ordered.shape.Month <- data.frame(ordered.shape.Month)
ordered.shape.Month

if (save$Analysis$all){
  write.table(ordered.shape.Month, file=save$Tables$filename,
              row.names=FALSE, quote=FALSE, sep=",")}

return(ordered.shape.Month[,-1])
}
