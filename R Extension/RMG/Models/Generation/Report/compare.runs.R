# Compare different TLP runs side by side
#
#
# Written by Adrian Dragulescu on 27-Apr-2005

options$runs   <- c(4819, 4820, 4821)
options$case   <- c("Base", "Low", "High")
options$layout <- c(8,2)
source("C:/R/Work/Energy/Models/Valuation/Generation/set.parms.gen.model.R")
aux  <- set.parms.gen.model(save,options)
save <- aux[[1]]; options <- aux[[2]]

MM=NULL
for (r in 1:length(options$runs)){
  filename <- paste(save$dir$main, "Data/results.by.year.",
                  options$runs[r],".csv", sep="")
  M  <- read.csv(filename); M <- M[,-ncol(M)]
  MM <- rbind(MM, cbind(M, case=options$case[r])) 
}
MM$year <- factor(MM$year)
MM$total.net <- MM$net.peak + MM$net.offpeak


filename  <- paste(save$dir$plots, "net_total_cases.pdf", sep="")
pdf(filename, width=10, height=6)
bkg.col <- trellis.par.get("background")
bkg.col$col <- "white"
trellis.par.set("background", bkg.col)
trellis.par.set("background", bkg.col)
aux <- trellis.par.get("box.rectangle"); aux$col <- "blue"
trellis.par.set("box.rectangle", aux)
aux <- trellis.par.get("box.umbrella");aux$col <- "blue"; aux$lty <- 1
trellis.par.set("box.umbrella", aux)
aux <- trellis.par.get("plot.symbol");aux$col <- "white"; aux$cex <- 0
trellis.par.set("plot.symbol", aux)
print(bwplot(total.net/1000000 ~ case | year, data=MM,
      layout=options$layout, ylab="Total Net PV in M$",
      main=options$unit.name))
dev.off()

