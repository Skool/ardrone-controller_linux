Note to developpers:

This application uses wxWidget as framework for the GUI (see http://www.wxwidgets.org/).
To use the project as it is, you have to create a virtual drive B: that contains the wxWidgets directory (B:\wxWidgets), 
otherwise you will have to change the include directories for headers and libs.

The code is based on cvdrone from puku0x (https://github.com/puku0x/cvdrone). The first version was only for windows, I have added some code
for wifi signal strenght that uses the windows api, so even if it uses wxwidgets, it cannot be used directly on linux or mac :-(

I have also try to create a new class that inherits from the Drone class from cvdrone, my intention was to extend the class, and allow to use new versions
of cvdrone when they are published, but this didn't work, because of internal changes of cvdrone. Finally it's a mix... :-S

Enjoy !

PS: Sorry for my bad english !

