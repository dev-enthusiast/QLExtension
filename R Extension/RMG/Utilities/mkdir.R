# dir.create has problems creating directories on network drives
# i:\mks\mksnt\mkdir.exe works only with "\" slashes,
# and is recursive.  shell is much slower than system ...
# 
# Let me know if you found a work around for dir.create (Adrian).


mkdir <- function(dir){
  shell(paste("mkdir", dir), translate=TRUE, invisible=TRUE)
}
