################################################################################
source("H:/user/R/RMG/Utilities/Packages/testPackageForFunctions.R")


################################################################################
package = "xtable"
functions = c(
  "xtable",
  "caption",
  "label",
  "align",
  "digits",
  "display"
)
testPackageForFunctions(package, functions)


################################################################################
package = "RODBC"
functions = c(
  "odbcConnect",
  "odbcConnectExcel",
  "odbcDriverConnect",
  "sqlFetch",
  "sqlQuery",
  "sqlTables"
)
testPackageForFunctions(package, functions)


################################################################################
package = "rcom"
functions = c(
  "comCreateObject",
  "comGetObject",
  "comGetObjectInfo",
  "comGetPicture",
  "comGetProperty",
  "comGetVersion",
  "comInvoke",
  "comIsValidHandle",
  "comRegisterRegistry",
  "comRegisterServer",
  "comSetProperty",
  "comThis",
  "comTrace",
  "comTraceObject",
  "comUnregisterRegistry",
  "comUnregisterServer"
)
testPackageForFunctions(package, functions)


################################################################################
package = "filehash"
functions = c(
  "db2env",
  "dbCreate",
  "dbDelete",
  "dbExists",
  "dbFetch",
  "dbInit",
  "dbInsert",
  "dbLazyLoad",
  "dbList",
  "dbLoad",
  "dbUnlink",
  "filehashFormats",
  "filehashOption"
)
testPackageForFunctions(package, functions)


################################################################################
package = "outliers"
functions = c(
  "chisq.out.test",
  "cochran.test",
  "dixon.test",
  "grubbs.test",
  "outlier",
  "pcochran",
  "pdixon",
  "pgrubbs",
  "qcochran",
  "qdixon",
  "qgrubbs",
  "qtable",
  "rm.outlier",
  "scores"
)
testPackageForFunctions(package, functions)


################################################################################
package = "reshape"
functions = c(
  "add.all.combinations",
  "add.missing.levels",
  "all.vars.character",
  "as.data.frame.cast_df",
  "as.data.frame.cast_matrix",
  "as.matrix.cast_df",
  "as.matrix.cast_matrix",
  "cast",
  "cast_matrix",
  "cast_parse_formula",
  "check_formula",
  "clean.vars",
  "colsplit",
  "colsplit.character",
  "colsplit.factor",
  "combine_factor",
  "compact",
  "compactify",
  "compute.margins",
  "condense",
  "condense.df",
  "decompactify",
  "defaults",
  "dim_names",
  "dims",
  "expand",
  "expand.grid.df",
  "funstofun",
  "iapply",
  "margin.vars",
  "melt",
  "melt.array",
  "melt.cast_df",
  "melt.cast_matrix",
  "melt.data.frame",
  "melt.default",
  "melt.list",
  "melt.matrix",
  "melt.table",
  "melt_check",
  "merge_all",
  "merge_recurse",
  "nested.by",
  "nulldefault",
  "prettyprint",
  "print.cast_df",
  "print.cast_matrix",
  "rbind.fill",
  "rcolnames",
  "rdimnames",
  "recast",
  "reduce",
  "rename",
  "reorder_factor",
  "rescaler",
  "rescaler.data.frame",
  "rescaler.default",
  "rescaler.matrix",
  "reshape1",
  "round_any",
  "rrownames",
  "sort_df",
  "sparseby",
  "stamp",
  "str.cast_df",
  "str.cast_matrix",
  "strip.dups",
  "tidystamp",
  "uniquedefault",
  "untable",
  "updatelist",
  "vdim"
)
testPackageForFunctions(package, functions)

#rstream
#MASS
#Hmisc
#lattice
#Simple
#xlsReadWrite
#chron