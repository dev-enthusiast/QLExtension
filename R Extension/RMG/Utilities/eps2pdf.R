# converts all eps files from a directory to pdf format 
#
# !!! create an epstopdf.cmd file on R's path, with
# set PATH=C:\Program Files\TeXLive\bin\win32; %PATH%
# epstopdf %*
#
# written by Adrian Dragulescu on 29-May-2003

rm(list = ls(all=TRUE))   # clear the memory
setwd("Z:\\Latex\\CPS_Energy\\TraderIssues\\DeltaInconsitency\\Plots")
dir="epstopdf "

aux=list.files(pattern=".eps")
for (f in 1:length(aux)){
   bux=paste(dir,aux[f], sep="")
   shell(bux)
}

