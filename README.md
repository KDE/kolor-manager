# KDE KolorManager README

![](http://www.oyranos.org/images/oyranos_logo.svg)

The Color Management System (CMS) Oyranos allows the coordination of
device specific Informations (ICC profiles) und system wide settings.
KolorManager is a KDE Systemssettings component for Oyranos.

## Modules
* kmdevices (Kolor Management Devices - Monitor/Printer/etc. profile setup)
* kminfo (Kolor Management Information - Displays specific profile information)
* kmsettings (Kolor Management Settings - Change behavior and policy settings)

## Dependencies
* The v1.1.0 tagged release builds fine using Oyranos-0.9.6 .
* Actual git should the latest Oyranos from it's git repository. KolorManager gets updated pretty quick to Oyranos changes.
* [Synnefo](http://www.oyranos.org/synnefo/)'s libOyranosSynnefo is used for the program logic


## Installation
Enter the following commands:

    mkdir build && cd build/
    cmake ..
    make
    make install


## Run
This build should install a "Kolor Management" icon in the KF5 settings window, under "Computer Administration".  An alternative method for running the individual modules would be to type this in a terminal:

    kcmshell5 [KMMODULE]     (Where KMMODULE is "kcm_kmdevices", "kcm_kminfo", or "kcm_kmsettings")  

This is handy for displaying debug information through the console.


## Build Notes

The following files are installed under each module directory.  They are shown here for reference:

MODULE.cpp (source code)
MODULE.h (header)
MODULE.ui (Qt Designer file)
ui_MODULE.h (Qt designer header file - for hand coding purposes)
CMakeLists.txt (build instructions for each module)


## References

* [Home Page@www.oyranos.org](http://www.oyranos.org/kolormanager)
* [@ColourWiki](http://www.oyranos.org/wiki/index.php?title=Kolor-manager)

## Development

    # local debug build
    mkdir build && cd build
    cmake -DCMAKE_INSTALL_PREFIX=~/.local -DCMAKE_BUILD_TYPE=debugfull -DCMAKE_CXX_FLAGS="-g -Wall" ../kolor-manager
    #Possibly prepend the PKG_CONFIG_PATH variable to reflect Oyranos...
    kbuildsycoca5 # update KDEs internal icon and module cachings
    kcmshell5 kminfo & # start

### Debugging
    echo $!  # obtain process id from bashs most recent process - kcmshell5
    KDE_DEBUG=1 gdb kcmshell5  # the variable avoid KDE's crash handler
    gdb > attach to process id
    gdb > breakpoint kminfo.cpp:line_number
    gdb > continue

### create tar ball from git
    sha=`cat .git/refs/heads/master | head -c 8`
    git archive --format=tar --prefix=kolor-manager-$sha/ HEAD | bzip2 > kolor-manager-$sha.tar.bz2

### list modules
    qdbus org.kde.kded5 /kded org.kde.kded5.loadedModules
### restart kded (omit kdeinit5_wrapper to see LD_LIBRARY_PATH)
    kquitapp kded5; sleep 1s; kdeinit5_wrapper kded5
    # kdebugdialog --fullmode   might be useful
### unload kolorserver
    qdbus org.kde.kded /kded unloadModule kolorserver

## History
* started by [Joe Simon](http://jsimon3.wordpress.com/category/synnefo/) in summer 2008-2009
* KolorManager maintained by [Kai-Uwe Behrmann](http://www.behrmann.name) 2010-2016
* KolorServer was maintained by Casian Andrei 2012 (It is now a separate project.)

## Known Issues

### KMSettings
- The configuration has to be ported to a Oyranos configure object to manipulate
settings and then apply at once. This is currently not possible, as each setting
is imediately applied to the Oyranos settings DB.  

### KMDevices
- Camera support needs to be outlined first
