# Copied the xtable.summary.lm from the xtable package
#
# Adrian Dragulescu on 20-Jan-2005


xtable.summary.rlm <- function (x, caption = NULL, label = NULL,
                                align = NULL, vsep = NULL, 
    digits = NULL, display = NULL, ...) 
{
    x <- data.frame(x$coef, check.names = FALSE)
    class(x) <- c("xtable", "data.frame")
    caption(x) <- caption
    label(x) <- label
    align(x) <- switch(1 + is.null(align), align, c("r", "r", 
        "r", "r"))
    vsep(x) <- switch(1 + is.null(vsep), vsep, rep("", ncol(x) + 
        2))
    digits(x) <- switch(1 + is.null(digits), digits, c(0, 4, 
        4, 2))
    display(x) <- switch(1 + is.null(display), display, c("s", 
        "f", "f", "f"))
    return(x)
}
