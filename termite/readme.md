#Termite on Mac OS X: (tested on 10.8.3)

##First, you have to install the patched vte:

###Dependencies:
- gtk3

###Build:
``tar xzf working-vte.tar.gz``    
``cd vte-0.32.2-old``    
``./configure --prefix=/opt/local --disable-python --disable-Bsymbolic``    
``make``      
``sudo make install``       


###Fetching termite:
``git clone https://github.com/thestinger/termite``     
``cd termite``    
``rm -r util``    
``git clone https://github.com/thestinger/util``    

###Add this to termite.cc, at the beginning:
``#include <errno.h>``      
``#include <math.h>``    

Now make sure to use the makefile from this repository.

``make``       
``sudo make install``     
