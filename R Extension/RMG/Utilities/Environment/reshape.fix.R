# source("H:/user/R/RMG/Utilities/Environment/reshape.fix.R")

reshape.fix <- function() {
assignInNamespace("add.all.combinations", function(data, vars = list(NULL), fill=NA) {
	if (sum(sapply(vars, length)) == 0) return(data)

	all.combinations <- do.call(expand.grid.df,
		lapply(vars, function(cols) data[, cols, drop=FALSE])
	)
	result <- merge_recurse(list(data, all.combinations))

	# fill missings with fill value
	result$result[sapply(result$result, is.null)] <- fill

	sort_df(result, unlist(vars))
}, "reshape")
  return(0)
}
