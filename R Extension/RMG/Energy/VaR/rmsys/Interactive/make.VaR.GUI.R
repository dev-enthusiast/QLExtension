# Use tcl/tk for now to create the VaR GUI. 
#
#
# Written by Adrian Dragulescu on 31-Dec-2006

make.VaR.GUI <- function(){

  require(tcltk)  || stop("tcltk support is absent.")
  require(tcltk2) || stop("Please install the tcltk2 package.")
  tclRequire("BWidget")  # for drop-down combo boxes
  #local({})    # TO DO: consider puting everything in a local environment
  no.portfolios <- 5      # how many rows to have
  bg.col <- "lavender"    # background color, not used now
  extra.class <- c("","gasseason", "20072008")
  tt <- tktoplevel()
  tkconfigure(tt)
  tktitle(tt) <- "Rmsys VaR GUI"
  asOfDate <- tclVar(as.character(Sys.Date()))
  useLivePositions <- tclVar("yes")
  runIndividually  <- tclVar("no")
  for (k in 1:no.portfolios){
    assign(paste("w",k, sep=""), tclVar(""))
    assign(paste("p",k, sep=""), tclVar(""))
    assign(paste("b",k, sep=""), tclVar(""))
    assign(paste("s",k, sep=""), tclVar(""))
    assign(paste("t",k, sep=""), tclVar(""))
  }
  fontFixedWidth <- tkfont.create(family="courier",size=10)
  #------------------------------------ Add the menus ---------------------
  topMenu <- tkmenu(tt)
  tkconfigure(tt, menu=topMenu)
  fileMenu <- tkmenu(topMenu, tearoff=FALSE)
  tkadd(fileMenu, "command", label="Quit", command=function()tkdestroy(tt))
  tkadd(topMenu, "cascade", label="File", menu=fileMenu)
  helpMenu <- tkmenu(topMenu, tearoff=FALSE)
  tkadd(helpMenu, "command", label="About", command=function(){
  tkmessageBox(message=paste("Version 1.0, as of 1-Jan-2007. \nPlease ",
              "contact RMG if you have any questions.", sep=""))})
  tkadd(topMenu, "cascade", label="Help", menu=helpMenu)
  tkfocus(tt)
  #------------------------------------ Add the parameters ----------------
  parms <- tk2labelframe(tt, text="Set run parameters:")
  l.asOfDate <- tklabel(parms, text="As of date")
  tk2tip(l.asOfDate, "Enter dates in format yyyy-mm-dd.")
  l.useLive  <- tklabel(parms, text="Use live positions?")
  l.runIndividually <- tklabel(parms, text="Run individually?")
  e.asOfDate <- tkentry(parms, width=10, textvariable=asOfDate)
  e.useLive  <- tkentry(parms, width=3, textvariable=useLivePositions)
  e.runIndividually <- tkentry(parms, width=3, textvariable=runIndividually)
  tkgrid(l.asOfDate, e.asOfDate)
  tkgrid(l.useLive, e.useLive)
  tkgrid(l.runIndividually, e.runIndividually)
  tkgrid.configure(e.asOfDate, e.useLive, e.runIndividually, sticky="w")
  tkgrid.configure(l.asOfDate, l.useLive, l.runIndividually, sticky="e")

  #------------------------------------ Add extra classifications -------
  class <- tk2labelframe(tt, text="Aggregate by:")
  tk2tip(class, "Select from this list if you want additional VaR breakout.")
  tcb <- NULL
  for (k in 1:length(extra.class)){
    tcb[[k]]<-tkwidget(class, "ComboBox", editable=FALSE, values=extra.class)
    tkgrid(tcb[[k]])
  }
  
  tkgrid(parms, class, sticky="n")
  tkgrid.configure(parms, sticky="w")
  tkgrid.configure(class, sticky="e")  
  emptylabel <- tklabel(tt, text="")
  tkgrid(emptylabel)
  #-------------------------- Add portfolios/books/trades ---------------
  pw  <- tkframe(tt)
  wpw <- tk2labelframe(pw, text="+/-")
  tk2tip(wpw, paste("If you want to aggregate items, use + or - in this ",
    "column.\n If empty it will assume +.", sep=""))
  ppw <- tk2labelframe(pw, text="Portfolios:")
  bpw <- tk2labelframe(pw, text="Books:")
  spw <- tk2labelframe(pw, text="Strategy:")
  tpw <- tk2labelframe(pw, text="Trades:")
  entry.w <- entry.p <- entry.b <- entry.s <- entry.t <- NULL
  for (k in 1:no.portfolios){
    entry.w[[k]] <- eval(parse(text=paste("tkentry(wpw, width=1, ",
      "textvariable=w", k , ")", sep="")))
    tkgrid(entry.w[[k]])
    entry.p[[k]] <- eval(parse(text=paste("tkentry(ppw, width=30, ",
      "textvariable=p", k , ")", sep="")))
    tkgrid(entry.p[[k]])
    entry.b[[k]] <- eval(parse(text=paste("tkentry(bpw, width=8, ",
      "textvariable=b", k , ")", sep="")))
    tkgrid(entry.b[[k]])
    entry.s[[k]] <- eval(parse(text=paste("tkentry(spw, width=20, ",
      "textvariable=s", k , ")", sep="")))
    tkgrid(entry.s[[k]])
    entry.t[[k]] <- eval(parse(text=paste("tkentry(tpw, width=6, ",
      "textvariable=t", k , ")", sep="")))
    tkgrid(entry.t[[k]])
  }
  tkgrid(wpw, ppw, bpw, spw, tpw, sticky="n")
  tkgrid(pw, columnspan=2)
  #------------------------- Add the Run! button -------------------------
  pressed.RUN <- function(){
    options <- run <- NULL
    options$asOfDate             <- tclvalue(asOfDate)
    options$run$useLivePositions <- tclvalue(useLivePositions)
    options$run$runIndividually  <- tclvalue(runIndividually)
    options$run$is.overnight <- FALSE
    options$run$extra.classFactors <- NULL
    for (k in 1:length(extra.class)){
      aux <- extra.class[as.numeric(tcl(tcb[[k]], "getvalue"))+1]
      options$run$extra.classFactors <- c(options$run$extra.classFactors, aux)
    }
    RR <- matrix("", nrow=no.portfolios, ncol=5, dimnames=list(1:no.portfolios,
            c("weight", "portfolio", "book", "strategy", "trade")))
    for (r in 1:no.portfolios){
      RR[r,"weight"]    <- eval(parse(text=paste("tclvalue(w",r,")",sep="")))
      RR[r,"portfolio"] <- eval(parse(text=paste("tclvalue(p",r,")",sep="")))
      RR[r,"book"]      <- eval(parse(text=paste("tclvalue(b",r,")",sep="")))
      RR[r,"strategy"]  <- eval(parse(text=paste("tclvalue(s",r,")",sep="")))
      RR[r,"trade"]     <- eval(parse(text=paste("tclvalue(t",r,")",sep="")))
    }
    tkconfigure(tt,cursor="watch")
    err.flag <- try(res <- run.interactive.VaR(RR, options))
    tkconfigure(tt,cursor="arrow")
    if (class(err.flag)=="try-error"){
      msg <- paste("Error: ", geterrmessage(), ". For help, please take ",
       "a snapshot of your screen including the error and the GUI and send ",
       "it to #rmg.analytics@constellation.com.", sep="")
      tkmessageBox(message=msg)
    } 
  }
  run.but <- tkbutton(tt, text="Run!", command=pressed.RUN)
  tkbind(tt, "<Return>", pressed.RUN)  # if you press Enter, you RUN it!
  emptylabel <- tklabel(tt, text="")
  tkgrid(emptylabel)
  tkgrid(run.but, columnspan=2)
  tkfocus(tt)
  tkwait.window(tt)
}
