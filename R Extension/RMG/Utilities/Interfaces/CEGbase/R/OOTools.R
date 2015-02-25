################################################################################
# OOTools.R
#
# A suite of utilities to manipulate and work with OpenOffice files & data
#
# Author: e14600
#
require( odfWeave )


################################################################################
# File Namespace
OOTools = new.env(hash=TRUE)


################################################################################
# Convert various document formats using OpenOffice & the JODConverter tool.
#
# The conversion happens based on the extension of the output file argument.
#
OOTools$convertDocument <- function( inputFile, outputFile, verbose = FALSE )
{
    OOFFICE_PATH = "C:/Program Files/OpenOffice.org 3/program/soffice"
    JOD_CONVERTER_PATH = "//nas-omf-01/cpsshare/Risk/Software/jodconverter-2.2.1/lib/jodconverter-cli-2.2.1.jar"
    
    OOFFICE_START_COMMAND = paste( 
            '"', OOFFICE_PATH, '" -headless ',
            '-accept="socket,host=127.0.0.1,port=8100;urp;" ', 
            '-nofirststartwizard', sep="" )
    OOFFICE_STOP_COMMAND = paste('"', OOFFICE_PATH,'" -unaccept=all', sep="" )
    
    # Start the OpenOffice listener    ?sweave
    system( OOFFICE_START_COMMAND, show.output.on.console = verbose )
    
    # Run the java converter to produce the files
    command = paste("java -jar ", JOD_CONVERTER_PATH, 
            " \"", inputFile, "\" ", "\"", outputFile, "\"", sep="")
    system( command, show.output.on.console = verbose )
    
    # Shutdown the OpenOffice listener
    system( OOFFICE_STOP_COMMAND, show.output.on.console = verbose )
    
    invisible( outputFile )
}


################################################################################
# Convert a formatted number string back into a numeric
#
OOTools$convertStringToNumber <- function( x )
{
    cleanData = gsub( "(", "-", x, fixed=TRUE)
    cleanData = gsub( ")", "", cleanData, fixed=TRUE)
    cleanData = gsub( ",", "", cleanData, fixed=TRUE)
    cleanData = gsub( "$", "", cleanData, fixed=TRUE)
    cleanData = as.numeric( cleanData )
    
    return( cleanData )
}


################################################################################
# When odfWeaving an OpenOffice Spreadsheet, the numerics are formatted as 
# strings. This function fixes that issue. 
#
OOTools$fixODSFile <- function ( file, dest, 
        workDir = odfTmpDir(), control = odfWeaveControl() )
{
    if (file.exists(dest)) {
        if (file.access(dest, mode = 2) < 0) 
            stop(paste("cannot write to", dest))
    }
    currentLoc <- getwd()
    zipCmd <- control$zipCmd
    zipCmd <- gsub("$$file$$", odfWeave:::shellQuote(basename(file)), zipCmd, 
            fixed = TRUE)
    if (dirname(dest) == ".") 
        dest <- paste(currentLoc, "/", dest, sep = "")
    verbose <- control$verbose
    if (all(zipCmd == c("zip -r $$file$$ .", "unzip -o $$file$$"))) {
        errorText <- paste("unzip could not be found.", "If installed, check your path.", 
                "If not installed, either go to", "www.info-zip.org and install or", 
                "use another utility (like jar)")
        if (.Platform$OS.type == "windows") {
            zipTest <- class(try(system("unzip", intern = TRUE, 
                                    invisible = TRUE), silent = TRUE))
            if (class(zipTest) == "try-error") 
                stop(errorText)
        }
        else {
            zipTest <- system("unzip", intern = TRUE)[1]
            if (is.na(zipTest) || length(grep("UnZip", zipTest)) == 
                    0) 
                stop(errorText)
        }
    }
    if (!file.exists(workDir)) {
        announce(verbose, "  Creating ", workDir, "\n")
        dir.create(workDir, showWarnings = TRUE, recursive = FALSE)
        if (!file.exists(workDir)) 
            stop("Error creating working directory")
    }
    workingCopy <- basename(file)
    if (!file.exists(file)) 
        stop(paste(file, "does not exist"))
    announce(verbose, "  Copying ", file, "\n")
    if (!file.copy(file, paste(workDir, "/", workingCopy, sep = ""), 
            overwrite = TRUE)) 
        stop("Error copying odt file")
    announce(verbose, "  Setting wd to ", workDir, "\n")
    setwd(workDir)
    on.exit(setwd(currentLoc))
    announce(verbose, "  Unzipping ODF file using", zipCmd[2], 
            "\n")
    if (.Platform$OS.type == "windows") {
        if (system(zipCmd[2], invisible = TRUE) != 0) 
            stop("Error unzipping file")
    }
    else {
        if (system(zipCmd[2]) != 0) 
            stop("Error unzipping odt file")
    }
    announce(verbose, "\n  Removing ", workingCopy, "\n")
    file.remove(workingCopy)
    if (file.exists(workingCopy)) 
        stop("Error removing original file")
    
    
    if (!file.exists("content.xml")) 
        stop("Error removing xml file")
    announce(verbose, "\n  Updating the numerics\n")
    OOTools$.updateNumerics("content.xml")
    
    
    announce(verbose, "\n  Packaging file using", zipCmd[1], 
            "\n")
    if (.Platform$OS.type == "windows") {
        if (system(zipCmd[1], invisible = TRUE) != 0) 
            stop("Error zipping file")
    }
    else {
        if (system(zipCmd[1]) != 0) 
            stop("Error zipping file")
    }
    if (!file.exists(workingCopy)) 
        stop(paste(workingCopy, "does not exist"))
    announce(verbose, "  Copying ", workingCopy, "\n")
    if (!file.copy(workingCopy, dest, overwrite = TRUE)) 
        stop("Error copying odt file")
    announce(verbose, "  Resetting wd\n")
    setwd(currentLoc)
    if (control$cleanup) {
        announce(verbose, "  Removing ", workDir, "\n")
        unlink(workDir, recursive = TRUE)
        if (file.exists(workDir)) 
            unlink(shQuote(workDir), recursive = TRUE)
        if (file.exists(workDir)) 
            unlink(odfWeave:::shellQuote(workDir), recursive = TRUE)
        if (file.exists(workDir)) 
            stop("Error removing work dir")
    }
    else {
        announce(verbose, " Not removing ", workDir, "\n")
    }
    announce(verbose, "\n  Done\n")
    invisible(NULL)
}


################################################################################
# We want to replace:
# 	office:value-type="string"><text:p>32844439.1396691</text:p>
# with
# 	office:value-type="float" office:value="32844439.1396691"><text:p>32,844,439 </text:p>
#
OOTools$.updateNumerics <- function( inputFile )
{
    data = readLines( inputFile, warn=FALSE )
    
    regex = 'office:value-type="string"><text:p>-*\\$*\\(*[0-9.,]+\\)*</text:p>'
    
    results = gregexpr( regex, data, perl=TRUE)
    for( resultIndex in seq_along( results ) )
    {
        result = results[[resultIndex]]
        
        resultList = NULL
        for( matchIndex in seq_along( result ) )
        {
            match = result[[matchIndex]]
            if( match == -1 ) next
            match.length = attr(result, "match.length")[[matchIndex]]
            oldString = substr( data[[resultIndex]], match, match+match.length-1 )
            print( oldString )
            
            value = gsub( 'office:value-type="string"><text:p>', '', 
                    oldString, fixed = TRUE )
            value = gsub( '</text:p>', '', value )
            
            if( length( grep( "$", value, fixed=TRUE ) ) > 0 )
            {
                newString = paste( 'office:value-type="currency" office:currency="USD" office:value="', 
                        OOTools$convertStringToNumber( value ),
                        '"><text:p>', value, ' </text:p>', sep = "" )
            }
            else
            {
                newString = paste( 'office:value-type="float" office:value="', 
                        OOTools$convertStringToNumber( value ),
                        '"><text:p>', value, ' </text:p>', sep = "" )
            }
            print( newString )
            
            resultList = append( resultList, oldString )
            resultList = append( resultList, newString )
        }
        
        for( index in which( seq_along( resultList ) %% 2 == 1 ) )
        {
            data[[resultIndex]] = gsub( resultList[index], resultList[index+1], 
                    data[[resultIndex]], fixed = TRUE )
        }
    }
    
    writeLines( data, inputFile )
}

