###############################################################################
# sparkline.R
#
# Author: e14600
#
sparkline <- function( y, width = 5, height = 1, line=NULL, plotType=windows, ... )
{
    y = na.omit( as.numeric(y) )
    
    if( length(y)==0 ) return(NULL)
    
    LIMIT = line
    
    WINDOW_WIDTH = width
    WINDOW_HEIGHT = height
    POINT_TYPE = 20
    MIN_MAX_COLOR = "blue"
    START_END_COLOR =  "red"  
    VAR_LINE_COLOR = "light grey"
    PLOT_LINE_COLOR = "dark grey"
    
    x = 1:length(y)
    
    data = data.frame(x=x, y=y)
    
    plotType(width=WINDOW_WIDTH, height=WINDOW_HEIGHT, ...)

    par(bty="n", mai=c(0,0,0,0), omi=c(0.05,0.05,0.05,0.05), xpd=NA)
    plot( data, type="l", xaxt='n', yaxt='n', 
            xlab="", ylab="", col=PLOT_LINE_COLOR )
    
    if( !is.null(LIMIT) )
    {
        lines( y = rep(LIMIT, length(data$x)),  x = data$x, 
                col = VAR_LINE_COLOR )
        lines( y = data$y, x = data$x ) 
    }
    
    
    minY = min(data$y)
    maxY = max(data$y)
    startY = data$y[1]
    endY = data$y[length(data$x)]
    
    #browser()
    
    points( x=data$x[which(data$y==minY)[1]], y=minY, 
            col=MIN_MAX_COLOR, pch=POINT_TYPE )
    points( x=data$x[which(data$y==maxY)[1]], y=maxY, 
            col=MIN_MAX_COLOR, pch=POINT_TYPE )
    
    points( x=data$x[1], y=startY, 
            col=START_END_COLOR, pch=POINT_TYPE )
    points( x=data$x[length(data$x)], y=endY, 
            col=START_END_COLOR, pch=POINT_TYPE )
    
    return( c(start=startY, min=minY, max=maxY, end=endY) )
}


sparkline2<-function(ydata=rnorm(100,500,50),width=1.5,height=0.5,sigfigs=4) {
    
# ydata = vector of data to be plotted
# width = width of sparlkline in inches, including text
# height = height of sparkline in inches
# sigfigs = number of significant figures to round min, max, and last values to
    
    temppar<-par(no.readonly = TRUE) # store default graphics parameters
    par(mai=c(0.10,0.05,0.10,0.05),fin=c(width,height)) # adjust graphics parameters for sparklines
    len<-length(ydata) # determine the length of the data set
    ymin<-min(ydata) # determine the minimum
    tmin<-which.min(ydata) # and its index
    ymax<-max(ydata) # determine the maximum
    tmax<-which.max(ydata) # and its index
    yfin<-signif(ydata[len],sigfigs) #determine most recent data point
    plotrange=c(ymin-0.3*(ymax-ymin),ymax+0.3*(ymax-ymin)) # define plot range to leave enough room for min and max circles and text
    plot(x=1:len,y=ydata,type="l",xlim=c(1,len*1.5),ylim=plotrange,col="gray",lwd=0.5,ann=FALSE,axes=FALSE) # plot sparkline
    points(x=c(tmin,tmax),y=c(ymin,ymax),pch=19,col=c("red","blue"),cex=0.5) # plot min and max points
    text(x=len,y=ymin,labels=signif(ymin,sigfigs),cex=0.5,pos=4,col="red") # show minimum value
    text(x=len,y=ymax,labels=signif(ymax,sigfigs),cex=0.5,pos=4,col="blue") # show maximum value
    text(x=len,y=(ymin+ymax)/2,labels=yfin,cex=0.5,pos=4) # show most recent value
    par(temppar) # restore graphics defaults
}

