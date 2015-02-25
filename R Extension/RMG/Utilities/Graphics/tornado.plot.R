# Draw a tornado plot using basic graph commands.  You may need to order your
# data. Where data is a data frame with the following columns:
#   [x.right, label.right, x.left, label.left]
#   options is a list,
#   options$stack <- TRUE   # to put negatives and positives together.
#
# data <- options <- NULL
# data$x.right <- sort(sample(0:10, 6), decreasing=T) 
# data$x.left  <- sort(sample(-10:0, 6)) 
# data$label.right <- c("PWX", "PWY", "PWJ", "NG", "COL", "WTI")
# data$label.left  <- c("PWXPWXPWX", "PWYPWY", "PWJ", "NG", "COL", "WTI")
# data <- as.data.frame(data)
# tornado.plot(data, NULL, xlab="Contribution", ylab="Market", main="Main title")
#
# Written by Adrian Dragulescu on 26-May-2007


tornado.plot <- function(data, options=NULL, main="", xlab="", ylab="",
                         ann=par("ann"), ...){
  if (length(options$stack)==0){options$stack <- TRUE}
  if (length(options$colors)==0){options$colors <- c("green", "red")}
  opar <- par(no.readonly=TRUE)
  on.exit(par(opar))
  h <- 0.2                    # width of the bars
  y <- nrow(data):1
  plot.new()
  ylim <- c(0, nrow(data)+0.5)
  xlim <- c(min(data$x.left[is.finite(data$x.left)]),
            max(data$x.right[is.finite(data$x.right)]))
  plot.window(xlim, ylim)
  xlim <- c(xlim[1]-1.2*strwidth(data$label.left[1]),
            xlim[2]+1.2*strwidth(data$label.right[1]))
  plot.window(xlim, ylim)  # strwidth depends on the plot xlim values!
  
  rect(0, y-h, data$x.right, y+h, col=options$colors[1])
  rect(data$x.left, y-h, 0, y+h, col=options$colors[2])
  axis(1, at=axTicks(1), pos=0)
  text(x=data$x.right, y, labels=data$label.right, pos=4)
  text(x=data$x.left, y, labels=data$label.left, pos=2)
  if (ann){title(main=main, xlab=xlab, ylab=ylab)}  
}

