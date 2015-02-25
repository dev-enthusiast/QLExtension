# Starting with the universe of about 50,000 unique paths 
#


rm(list=ls())
# load the NEPOOL environment
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.NEPOOL.iso.R")
# load the RPT environment
source("H:/user/R/RMG/Energy/Trading/Congestion/NEPOOL/lib.analyze.paths.R")

options <- NULL
options$auction <- as.Date("2008-05-01")
options$bids$total.expense  <- 250000      # total amount of dollars I want to spend
options$bids$max.mw.path    <- 50
options$bids$max.daily.loss.cluster <- 0.2*options$bids$total.expense



setwd("S:/All/Structured Risk/NEPOOL/FTRs/Analysis/2008-05")

Omega <- function(x, r=0){
  x <- sort(x[!is.na(x)])
  N <- length(x)
  numerator   <- sum(x[x>r]-r)
  denominator <- sum(x[x<r]-r)/N
  #not sure yet
}


uAWD <- unique(NEPOOL$AWD[,c("customer.name", "source.name",
  "sink.name", "buy.sell", "class.type", "auction")])

dim(NEPOOL$AWD)[1]==dim(uAWD)[1]  # check if you have everything...

uAWD$value   <- 1
uAWD$auction <- NULL
res <- cast(uAWD, ... ~ ., sum)
names(res)[ncol(res)] <- "count"

res <- res[order(res$count, decreasing=T),]
#res <- subset(res, count>=25)
#write.csv(res, file="C:/Temp/recurrent.paths.csv", row.names=F)

res$method <- "copycat"
Paths <- data.frame(res[,c("source.name", "sink.name", "class.type")])
Paths <- unique(Paths)

Paths <- ut.FTR$.PathNamesToPTID(Paths, NEPOOL, "to.ptid")
Paths <- na.omit(Paths)
Paths$path.no <- 1:nrow(Paths)

CP  <- ut.FTR$get.CP.for.paths(Paths, NEPOOL)
CP <- CP[-grep("-", CP$auction),]    # keep only montly auctions

SP <- ut.FTR$calc.SP.for.paths( Paths )

res <- merge(CP, SP)
res$Payoff.per.hour <- res$SP - res$CP

res$value <- (res$SP - res$CP)
aux <- cast(res, path.no ~ ., function(x){length(which(x>=0))/length(x)})
names(aux)[2] <- "winning.percentage"
aux <- aux[order(aux$winning.percentage), ]

## bux <- cast(res, path.no ~ ., length)
## hist(bux[,"(all)"])

winners <- merge(Paths, aux)

# add path names
aux <- ut.FTR$.PathNamesToPTID(winners[,c("path.no", "sink.ptid",
  "source.ptid")], NEPOOL, "to.names")
winners <- merge(winners, aux)
winners <- winners[order(winners$winning.percentage, decreasing=T), ]

ind <- which(duplicated(winners[,2:4]))
winners <- winners[-ind, ]
write.csv(winners, "all.paths.csv", row.names=F)

windows()
hist(winners$winning.percentage, col="wheat",
     xlab="Winning percentage in the past 27 monthly auctions",
     main="Histogram")

# find the paths that have been winning in more than 90% times
winners <- subset(winners, winning.percentage >= 0.90)

# get the historical settle prices
hSP <- ut.FTR$get.hSP.for.paths(winners, NEPOOL)

# remove incomplete paths
aux     <- ut.FTR$dropIncompleteHistory( winners, hSP )
hSP     <- aux[[2]]
winners <- aux[[1]]


winners$log.ratio.RL <- log((winners$p95 - winners$p50)/
                            (winners$p50 - winners$p5))





write.csv(winners, "all.paths.csv", row.names=F)


#----------------------------------------------------------------------
# load all recurrent paths
winners <- read.csv("all.paths.csv")
winners <- winners[,1:7]

#select some very good paths ...
paths <- subset(winners, ratio.EWin.ELoss > 4 & mean > 0)
write.csv(paths, "all.paths_B.csv", row.names=F)

#select some paths to sell ...
paths <- subset(winners, ratio.EWin.ELoss < 0.25 & mean < 0)
paths <- paths[order(paths$ratio.EWin.ELoss, decreasing=T),]

paths <- paths[1:30,]
#paths <- winners[sample(1:nrow(winners), 30),]


#-------------------------------------------------------------
# Select paths to buy ...
#
winners <- read.csv("all.paths.csv")

#select some very good paths ...
paths <- subset(winners, ratio.EWin.ELoss > 4 & mean > 0)
paths <- paths[order(paths$ratio.EWin.ELoss, decreasing=T),]
write.csv(paths, "subset.paths_B.csv", row.names=F)

paths <- subset(paths, winning.percentage >= 0.9)
write.csv(paths, "subset.paths_BA.csv", row.names=F)

# get the historical settle prices
hSP <- ut.FTR$get.hSP.for.paths(paths, NEPOOL, melt=TRUE)

windows()
bwplot(as.character(path.no) ~ value, aux,
       xlab="Hourly settle price, $", main="Paths to buy", 
       type="p")


#-------------------------------------------------------------
# Select paths to sell ...
#
paths <- read.csv("all.paths.csv")

paths <- subset(paths, ratio.EWin.ELoss < 0.25 & mean < 0)
paths <- paths[order(paths$ratio.EWin.ELoss, decreasing=T),]
write.csv(paths, "subset.paths_C.csv", row.names=F)

paths <- subset(paths, winning.percentage <= 0.1)
write.csv(paths, "subset.paths_CA.csv", row.names=F)


#--------------------------------------------------------------
# Focus on a set of paths
#
paths <- read.csv("subset.paths_B.csv")
paths <- paths[,1:7]

hSP <- ut.FTR$get.hSP.for.paths(paths, ISO.env, melt=F)

paths <- RPT$basic.stats(paths, hSP, NEPOOL)

paths <- subset(paths, days.gt.0 > 10 & days.gt.0/days.lt.0 > 2)

write.csv(paths, "subset.paths_BB.csv", row.names=F)


#--------------------------------------------------------------
paths <- read.csv("subset.paths_BB.csv")
paths <- paths[,1:7]

#paths <- paths[1:50,]  # just for playing with the portfolio...


hSP   <- ut.FTR$get.hSP.for.paths(paths, NEPOOL, melt=F)
hSP   <- ut.FTR$filter.hSP( hSP, auction )
paths <- RPT$basic.stats(paths, hSP, NEPOOL)

score <- RPT$calc.score(paths)
paths$score <- score[, "Total"]
paths <- subset(paths, score>=3)
paths <- paths[order(paths$score, decreasing=T), ]

write.csv(paths, "subset.paths_BBA.csv", row.names=F)


#==============================================================
paths <- read.csv("subset.paths_BBA.csv")

aux <- paths[,c("path.no", "source.ptid", "sink.ptid")]
aux <- ut.FTR$.PathNamesToZone(aux, NEPOOL)
paths <- merge(paths, aux)

#--------------------------------get the CP, SP and compare---
aux  <- RPT$propose.bid.price( paths, NEPOOL, options$auction )
CSP  <- aux[[1]]
bids <- aux[[2]]

aux <- CSP
aux$value <- CSP$return
aux <- cast(aux[,c("path.no", "value")], path.no ~ .,
            function(x){length(x[x>0])})
names(aux)[2] <- "count"
# select only the paths that have been successful in 5 out of
# 6 past auctions. 
good.paths <- aux$path.no[aux$count>=5]


paths <- subset(paths, path.no %in% good.paths)
write.csv(paths, "subset.paths_BBAA.csv", row.names=F)

#==============================================================
paths <- read.csv("subset.paths_BBAA.csv")
options$run.name <- "BBAA"

#--------------------------------get the CP, SP and compare---
#aux  <- RPT$propose.bid.price( paths, NEPOOL, options$auction )
aux  <- RPT$propose.bid.price( paths, NEPOOL, NULL )
CSP  <- aux[[1]]
bids <- aux[[2]]

options$no.plots <- 8    # number of plots per page
RPT$plot.CP.vs.SP( CSP, options )

#----------------------------------plot the network of paths---
paths$mw <- 1

options$map$by.zone <- TRUE
options$map$save <- TRUE
options$run.name <- "BBAA"
FTR.env$plot.map(paths, options)


BC <- NEPOOL$getBindingConstraints()

paths.BC <- ut.FTR$countBindingConstraints( paths[1:2,], BC )

#==============================================================
paths <- read.csv("subset.paths_B.csv")
paths <- paths[, c(1,3:26,2)]
paths.0 <- paths

paths <- paths.0
paths <- subset(paths, ratio.EWin.ELoss > 10)
dim(paths)
paths <- subset(paths, days.gt.0 > 20)
dim(paths)
paths <- subset(paths, days.gt.0/days.lt.0 > 8)
dim(paths)
paths <- subset(paths, p0 > -0.02)
dim(paths)

write.csv(paths, "subset.paths_BC.csv", row.names=F)


#==============================================================
paths <- read.csv("subset.paths_BC.csv")
paths <- paths[, c(1:4, 6:7)]
paths$strategy <- "BC: Agressive constraints"
all.paths <- paths

paths <- read.csv("subset.paths_BBAA.csv")
paths <- paths[, c(1:4, 6:7)]
paths$strategy <- "BBAA: Mild constraints"
all.paths <- rbind(all.paths, paths)

paths <- read.csv("subset.paths_BAA.csv")
paths <- paths[, c(1:4, 6:7)]
paths$strategy <- "BAA: Lottery tickets"
all.paths <- rbind(all.paths, paths)

paths <- all.paths
paths <- paths[order(paths$path.no), ]
paths <- paths[!duplicated(paths$path.no), ]

options$map$by.zone <- TRUE
options$map$save <- TRUE
options$run.name <- "BC"
FTR.env$plot.map(paths, options)

paths.BC <- ut.FTR$countBindingConstraints( paths, BC )

options$no.plots <- 8   # number of plots per page
RPT$plot.CP.vs.SP( CSP, options )

#----------------------------------how similar are these paths---
hSP <- ut.FTR$get.hSP.for.paths(paths, NEPOOL, melt=T)

aux <- RPT$cluster.paths(hSP, percent=0.01)
cuts     <- aux[[1]]
clusters <- aux[[2]]

write.csv(cuts, "BBAA+BAA+BC_clusters.csv", row.names=F)

#----------------------------------make bids---------------------
aux <- RPT$get.bid.price.limits( paths, NEPOOL, options$auction, enh=3)
CSP <- aux[[1]]
bids.lim <- aux[[2]]

bids <- RPT$make.bids( bids.lim, cuts )
#paths <- subset(paths, path.no %in% cuts$path.no)  # some may have dropped

aux <- RPT$find.bid.quantity( bids, paths, hSP )
paths.xls <- aux[[1]]
QQ        <- aux[[2]]


options$run.name <- "BBAA+BAA+BC"
RPT$prepare.xls( paths.xls, options )


# What happened to path 23241 ?!  Did it have missing history?











#CSP <- melt(CSP, id=1:2)

p1 <- xyplot(CP ~ auction, data=CSP, aspect=1,
#        subset=class.type == "ONPEAK",
        panel=function(x,y, ...){
          panel.xyplot(x,y,...)
          panel.abline(0,1, col="gray")
        },
        scales="free",
#             layout=c(6,5),
        xlab="Cleared price", ylab="Settle price",
##         key=simpleKey(text = unique(aux$zone),
##           columns = length(unique(aux$zone)))
      )
plot(p1)



CSP <- CSP[order(CSP$month), ]

xyplot(CP ~ month|sink.zone, data=CSP, groups=path.no,
       type="o", scales="free")


hSP <- ut.FTR$get.hSP.for.paths(paths, NEPOOL, melt=T)
hSP <- ut.FTR$filter.hSP( hSP, options$auction )


aux <- cast(CSP, month ~ path.no, I)
g <- ggobi(aux)



bwplot(as.character(path.no) ~ value, hSP,
       xlab="Hourly settle price, $", main="Paths to buy", 
       type="p")














##########################################################################
##########################################################################
##########################################################################
##########################################################################
##########################################################################
##########################################################################

#hist(paths$ratio.EWin.ELoss)

#paths <- read.csv("subset.paths_AB.csv")


windows()
plot(as.Date(index(hSP$ONPEAK[,"88"])), hSP$ONPEAK[,"88"], type="l")


aux=NEPOOL$hP$ONPEAK[,c("432","4001")]
aux <- cbind(aux, spread=aux[,2]-aux[,1])
index(aux) <- as.Date(index(aux))

aux <- window(aux, start = as.Date("2006-01-01"))
windows(); plot(aux[,3])

t(apply(aux, 2, quantile, prob=probs))

winners[,c(1:7,18)]




hSP <- ut.FTR$get.hSP.for.paths(Paths[3,], NEPOOL)
plot(hSP$ONPEAK)

write.csv(winners, file="C:/temp/junk.csv", row.names=F)
winners <- read.csv(file="C:/temp/junk.csv")



class(index(aux)) <- "Date"

secdb.getBucketMask( "nepool", "5x16", Sys.Date()-1, Sys.Date() )


res <- rbind(CP, SP)
res$auction <- factor(res$auction, levels=ut.FTR$.LevelsAuctionTerm(
  unique(res$auction)))
                
xyplot(value ~ auction |class.type*variable, data = res, groups=path.no,
       type="l")

xyplot(value ~ auction |class.type, data = res, groups=path.no,
       subset = variable=="SP", 
       type="b")










NEPOOL$SP$auction <- ut.FTR$.AuctionTerm(NEPOOL$SP$month, NEPOOL$SP$month)
aux <- merge(NEPOOL$CP, NEPOOL$SP)

aux <- aux[,c("class.type", "auction", "name", "clearing.price.per.hour",
              "settle.price")]
names(aux)[4:5] <- c("CP", "SP")
#aux <- melt(aux, id=1:3)
aux$auction <- factor(aux$auction,
  levels=ut.FTR$.LevelsAuctionTerm(unique(aux$auction)))

aux <- merge(aux, NEPOOL$MAP[,c("name","zone")])


p1 <- xyplot(SP ~ CP | auction, data=aux, groups=zone, aspect=1,
        subset=class.type == "ONPEAK",
        panel=function(x,y, ...){
          panel.xyplot(x,y,...)
          panel.abline(0,1, col="gray")
        },
        scales="free", layout=c(6,5),
        xlab="Cleared price", ylab="Settle price",
        key=simpleKey(text = unique(aux$zone),
          columns = length(unique(aux$zone)))
      )
plot(p1)



xyplot(SP ~ CP | class.type, data=aux, aspect=1 )

        key=list(columns = length(unique(aux$zone)),
                 text = list(unique(aux$zone)))


Paths=winners
aux=Paths[grep("UN.MYSTIC", Paths$sink.name), ]
aux=aux[grep("Z.NEMA", aux$source.name), ]
bux <- ut.FTR$.PathNamesToPTID(aux, NEPOOL, "to.ptid", keep=TRUE)


cux <- ut.FTR$.PathNamesToPTID(bux, NEPOOL, "to.names")


aux <- ut.FTR$.PathNamesToPTID(winners[10247, c("path.no", "sink.ptid", "source.ptid")], NEPOOL, "to.names")



