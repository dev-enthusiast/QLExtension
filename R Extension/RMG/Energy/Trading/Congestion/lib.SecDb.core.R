# Keep an environment with key SecDb functions at the top search space
# to use by the SecDb package. 
# Need to do this to use the .Call function without the PACKAGE argument
# in R 3.1.2
#
# There should be no need to change this function unless SecDb API changes
# which is very unlikely.
#
# Adrian Dragulescu, 12/11/2014
 

.SDB <- new.env(hash=TRUE)

.SDB$call_function <- function(libraryName, functionName, arguments) {
  .Call( "CallFunction", libraryName, functionName, arguments )
}

.SDB$get_symbol_info <- function(curve) {
  .Call("GetSymbolInfo", curve)
}

.SDB$get_value_type <- function(security, valueType) {
  .Call("GetValueType", security, valueType)
}

.SDB$evaluate_slang <- function(slang) {
  .Call("EvaluateSlang", slang)
}

.SDB$set_database <- function(name) {
  .Call("SetSecDbDatabase", name)
}
