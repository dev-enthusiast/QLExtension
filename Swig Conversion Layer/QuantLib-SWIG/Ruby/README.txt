
The C++ wrappers for the QuantLib-Ruby extension module are created
by means of SWIG (Simplified Wrapper and Interface Generator) available
from <http://swig.sourceforge.net/>. SWIG version 1.3.31 is needed.

The wrappers are generated on all supported platforms by issuing the command
    ruby setup.rb wrap
The above assumes that the SWIG executable is named "swig" and can be found
in the system path. However, this step is only necessary if you are compiling
from sources checked out from the CVS repository. It is not required if you
are using a distributed tarball.

The building and installation process consists of the following commands:
    ruby setup.rb build
    ruby setup.rb test
    ruby setup.rb install

The build step requires that the QuantLib headers and library can be
found by the compiler. On Unix-like platforms, this should already
hold if you ran "make install" from your QuantLib directory. On the
Windows platform, this can be achieved by defining a QL_DIR environment
variable pointing to your QuantLib directory (e.g., "C:\Lib\QuantLib".)

The install step might require superuser privileges.
An alternate install location can be specified with the command:
    ruby setup.rb install --prefix=/home/johndoe

The test suite is implemented on top of the Test::Unit framework, included
in the standard Ruby distribution since version 1.8.0. A version of Test::Unit 
for Ruby 1.6.x is available from <http://testunit.talbott.ws>. 

