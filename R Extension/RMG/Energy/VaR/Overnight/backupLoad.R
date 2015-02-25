source("H:/user/R/RMG/Utilities/load.R")


################################################################################
# Restore a file from the backup directories
#
restoreFromBackup <- function( file, filehash=NULL )
{
    snapshotDir = paste(dirname(file), "/~snapshot", sep="")
    
    backupDirs = c(
            "hourly.0", "hourly.1", "hourly.2", "hourly.3",
            "hourly.4", "hourly.5", "hourly.6", "hourly.7",
            "hourly.8", "hourly.9", "hourly.10", "hourly.11",
            "hourly.12", "hourly.13", "hourly.14", "hourly.15",
            "hourly.16", "hourly.17", "hourly.18", "hourly.19",
            "hourly.20", "hourly.21", "hourly.22", 
            "nightly.0", "nightly.1", "nightly.2", "nightly.3", 
            "nightly.4", "nightly.5",
            "weekly.0", "weekly.1", "weekly.2", "weekly.3" )

    backupPaths = file.path( snapshotDir, backupDirs )
    
    loadedOK = FALSE
    for( directory in backupPaths )
    {
        fileToLoad = file.path( directory, basename(file) )
        if (!is.null(filehash))  # need to copy it for the filehash
          file.copy( fileToLoad, file, overwrite=TRUE )
        
        loadedOK = safeLoad( fileToLoad, filehash=filehash )
        if( loadedOK )
        {
            cat("Restoring file:", file, "from backup:", fileToLoad, "\n")
            file.copy( fileToLoad, file, overwrite=TRUE )
            break
        }
    }
    
    if( !loadedOK )
    {
        cat("Unable to restore backup for file:", file, "\n")
    }
}


################################################################################
# If the file can't be loaded, restore it from the backup and load that
#
backupLoad <- function( file, filehash=NULL )
{
    loadedOK = safeLoad( file, envir=parent.frame(), filehash=filehash )
    
    if( !loadedOK )
    {
        restoreFromBackup( file, filehash )
        loadedOK = safeLoad( file, envir=parent.frame(), filehash=filehash) 
    }

    invisible( loadedOK )
}

