How to compile the LIM C dll for R.  
2006-05-01 
Wang Yu and Adrian Dragulescu 

1) Wang modified some of the C code provided by LIM, removing some
   error messages and some of the warnings.  This is the RMGxmimR.c
   file.  

2) Use pexports on the LIM dll xmim4.dll to create the xmim4.def file.
   Get pexports from
   http://www.emmestech.com/software/cygwin/pexports-0.43/download_pexports.html 
   pexports xmim4.dll > xmim4.def

3) Use the dlltool to create the library
   dlltool -d xmim4.def -l xmim4.a

4) Modify the Makevars file to link the xmim4.dll.  Add this line: 
   PKG_LIBS=-L. -lxmim4  

5) Run R CMD SHLIB RMGxmimR.c   It should produce the RMGxmimR.dll.  



   See also:
   http://www.stats.uwo.ca/faculty/murdoch/software/compilingDLLs/index.html
   http://www.nag.co.uk/numeric/RunderWindows.asp
   
How to compile the LIM C dll for R.  
2006-08-29 
Wang Yu and Adrian Dragulescu 

A make file named as xmimR.mak is created to build the R->Lim library.  
1. Verify the location of header file and libs files are correct
2. Type: NMAKE /f "xmimR.mak" CFG="xmimR - Win32 Release" 
to invoke the microsoft nmake to build the library.

A stripped down verion of the c source code, named as xmimR_Minimal.c is also
created and used for starting point of migrating into UNIX.

    
   