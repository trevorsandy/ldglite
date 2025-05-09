                              LdGLite
					        LPub3D Build
               (a platform independent version of LdLite)

Contents of readme.txt:
  Introduction
  LdGLite for Windows README
  LdGLite for linux README
  LdGLite for DOS README
  LdGLite for MacOS README
  Extra Stuff
    Command line options
    Hot Keys
    LEDIT extensions
    LEDIT emulation quirks
  Notes 
    Producing image files
    Tiled rendering
    Off screen background rendering


			     Introduction

Ldglite is a program that lets you create, edit, and view Lego brick
models stored in LDRAW format. It was created by connecting the LDLite
DAT file parser to an OpenGL rendering engine and the GLUT toolkit,
making it portable to many operating systems.  It currently runs in
Windows, Linux, and MacOS, DOS, and BSD unix, and operates in several
compatibility modes.

The default is the viewer mode which is compatible with LDLite and
ldraw, but also contains many added features such as the ability to
function as a cheesy scene modeler for l3p and POV.

Ldglite contains an LEdit emulation mode which gives you the ability
to create and edit models.  If you rename the program something with
"ledit" in it such as ldgledit or l3gledit it will start up
automatically in LEDIT emulation mode.  This can also be accomplished
with the -le or -lE command line option.  In editing mode ldglite
can be configured to use plugins from the LDDP project to generate
hoses, pose minifigs, create train track layouts, etc.

Several internet sites use ldglite as a scripted offscreen renderer,
generating many pictures of LDRAW parts for inventory lists and
such. 

Possibly the most important feature of ldglite is that all of the
source code is available.

This instance of ldglite has been substantially modified specifically
for LPub3D. 

                        LdGLite for Windows README

You must have a copy of opengl32.dll installed.  This comes with
Windows 95 OSR2, Windows 98, Windows 2000, and Windows NT.  If you use
an early version of Windows 95 then you must get opengl95.exe from
Microsoft and install it.

    ftp://ftp.microsoft.com/Softlib/MSLFILES/opengl95.exe

You must also install the GLUT dll in the appropriate directory.  For
Windows 95 and 98 copy glut32.dll to the windows\system directory.
For Windows NT and 2000 copy glut32.dll to the winnt\system32
directory.

You must also install the ldraw parts files from www.ldraw.org.

    c:
    cd \
    mkdir c:\ldraw
    cd c:\ldraw
    ldraw.exe -y
    complete.exe -y

For ldglite to find the parts you can use the ldglite argument - see
arguments below - or perform one of the following:

1. Set the environment variable LDRAWDIR.
	Add this line to your C:\AUTOEXEC.BAT file and
	reboot your PC.  

    SET LDRAWDIR=C:\LDRAW

2. Alternatively, ldglite will look in C:\ProgramData and
   %USERPROFILE%, if not found, for the LDraw parts folder.

As of version 7.1 ldglite can also use the BaseDirectory setting in 
ldraw.ini to find the parts files.  The ldraw.ini file is created by
LDAO and various other ldraw type programs.

To use ldglite from MS Internet Explorer do this:

    1) Open an Explorer window (or any folder window).
    2) Select the "View->Folder Options" menu selection.
       ("Tools->Folder Options" on 2000.)
    3) Go to the "File Types" tab.
    4) Scroll down and double click on the "ldlite Document" line.  
       If there is no "ldlite Document" line, click the "New Type" button
       and enter "ldlite Document" in the "Description of type" field
       and ".dat" for the "Associated extension".
    5) In the "Content Type" field, type in "application/x-ldraw"
    6) In the "Actions" area, Click on the "New..." button.  
       For the Action name, type "open".  
       For the filename, browse to the location of ldglite.exe

Or on the command line of windows 2000 type something like this:
    
    FTYPE ldlite.document=C:\some\directory\l3glite.exe -v3 "%1"
    ASSOC .dat=ldlite.document
    ASSOC .mpd=ldlite.document
    ASSOC .ldr=ldlite.document
    SET PATHEXT=.ldr;.mpd;.dat;%PATHEXT%

The last bit allows you to skip the .ldr extension and just "run" .ldr files
    

For Netscape:

    1) Go to the Netscape menu item Edit/Preferences/Navigator/Applications.
    2) Select "ldlite Document".   
       If you don't find "ldlite Document" then add it.
    3) Edit, and set the mime type to application/x-ldraw.
    4) Browse for the ldglite.exe executable.



                        LdGLite for linux README

You must install the basic ldraw files from www.ldraw.org

    cd /usr/local
    mkdir ldraw
    cd ldraw
    unarj x ldraw.exe 
    unarj x complete.exe

The default directory for ldraw dat files is /usr/local/ldraw.
If you wish to install the files elsewhere, set the environment
variable LDRAWDIR

    LDRAWDIR=/var/ldraw
    export LDRAWDIR

Alternatively, ldglite will look in $HOME - if LDraw was not
found - for the LDraw parts folder.

Lastly, you can use the ldglite argument - see
arguments below to specify the LDraw folder
   
The default case for dat files in the p and parts subdirectories
is lowercase.  If you want to use uppercase set the environment
variable LDRAWIRCASE

    LDRAWDIRCASE=UPPERCASE
    export LDRAWDIR

Unzip the ldglite distribution.  I zipped it with infozip which is free
on the web.  (I'm skipping the usual .tgz file since geocities seems to
choke on it.)  The Mandrake 6.1 linux distribution came with an unzip
utility preinstalled in /usr/bin. 

You must build ldglite

    make -f makefile.linux
    cp ldglite /usr/local/bin
    ln -s /usr/local/bin/ldglite /usr/local/bin/l3glite

Of course this assumes you have Mesa or some other OpenGL installed.
I had to make some softlinks for the Mesa libs on my Mandrake 6.1
linux distribution before the ldglite would link.

    cd /usr/X11R6/lib
    ln -s libMesaGLU.so libGLU.so
    ln -s libMesaGL.so libGL.so

I think the makefile also assumes you have libpng and libz installed.
Check makefile.linux for instructions on how to skip them if you have 
trouble linking.

For Netscape, go to the Netscape menu item
Edit/Preferences/Navigator/Applications, select "ldlite Document",
Edit, and set the mime type to application/x-ldraw.  If you don't find 
"ldlite Document" then add it.
Set the executable like this.  

    /usr/local/bin/ldglite %s

Feel free to substitute whatever path you use for the executable.  The
%s is required to pass the temporary filename of the dat file to
ldglite.  If you want a 640x480 window with shading turned on try something
like this.  See ldlite docs for help with command line options. 

    /usr/local/bin/ldglite -v3 -fh %s



                        LdGLite for DOS README

You must install the basic ldraw files from www.ldraw.org

    c:
    cd \
    mkdir c:\ldraw
    cd c:\ldraw
    ldraw.exe -y
    complete.exe -y

For ldglite to find the parts you must set the environment variable
LDRAWDIR.  Add this line to your C:\AUTOEXEC.BAT file and reboot your
PC.  

    SET LDRAWDIR=C:\LDRAW

You need to run the csdpmi5b DOS extender for this to work unless you
are using Windows 95 or 98 DOS mode.  You also seem to need at least
8MB of RAM (16MB recommended) and a 16bpp Video card.  You may also
need the Scitech Display Doctor (univbe.exe) if video bios does not
support any of the video modes available with the -v command line
option.  Finally you may need to redirect stdout to a file to avoid
some garbage at the top of the screen.

    ldglite -l3 -v3 myfile.dat >messages.txt



                        LdGLite for MacOS README

Installation instructions are online at http://ldglite.sourceforge.net.
Scroll down to the installation section and follow the link to the 
instructions for your flavor of MacOS.



                             Extra Stuff


Command line options (see also the original ldlite docs for more):
-& windows mode.  Detach from the console window.  Windows version only.
-- if no DAT filename is given on the command line ldglite will read
   from stdin.  (l3glite does not do this)  Try this:
     cat file.dat | ldglite -v3 -ms -- 
   Kinda neat, but I don't know if this is good for much.  Perhaps if you
   have an application that generates dat files you can pipe the output
   through ldglite and produce an image file.  For example ldrawmode for
   emacs could possibly be extended to use this to send a dat file in the
   buffer out to the printer.  (especially if I add postscript output)
+X,Y sets the initial window position offset to (X,Y).-caN sets the camera FOV angle in degrees.
-2g downsamples output images by 2 with a Gaussian blur filter (png only).
-2g,2x will also scale up the image (and edge lines) by 2 first.
   The result is a nice pretty antialiased image.  Edges are smooth.
   Transparent parts are blended instead of dithered.  (like l3p, default = 67.38)
-ccX,Y,Z sets the camera location.  (like l3p, except default = 0,0,1000)
-cg<la>,<lo>,<r> sets the camera location on globe.  (like l3p)
-coX,Y,Z sets the model origin for the camera to look at.  (default = 0,0,0)
-claX,Y,Z same as -coX,Y,Z
-cuX,Y,Z sets the camera up vector.  (default = 0,1,0)-fh turns on shading mode.
-eN.NN fixes polyline offset driver errors.  Try -e0.95 if lines look bad.
-fs turns off stud drawing. (l3glite only)
-fsl draws studs as lines. (l3glite only)
-fssN draws stud style. 'N' can be 0-7, 0 no style is the default value.
    style 6 and 7 are high contrast and can be fine tuned using the
    -lhS -lhP -lhB and -lhD command line options. These options can
	be disabled using -lhSd -lhPd -lhBd and -lhDd.
-fe turns off edge lines 
-fr same as -fe (Render mode?  Rough draft?  I don't know what R stands for.)
-ffT,E,S,R,G,B turns on fogging 
   T = fogging type (1 = linear, 2 = exp2, 3 = exp3)
   E,S = end/far, start/near fog clip planes in linear mode.
   E,S = fog density, S= ignored param in exp mode.
   R,G,B = fog color (default = fade to white, try 0,0,0 for night scene)
   All fog params except T are optional.
   Try these:  -ff3,0.0004    -ff3,0.0003,0,0.8,0.8,1.0
-fmN sets default mouse spin drawing mode bitmask (eg -fm0x80).  Bits are:
   TYPE_F_NORMAL 		0x0000
   TYPE_F_LOW_RES 		0x0001
   TYPE_F_NO_POLYGONS	 	0x0002
   TYPE_F_NO_LINES		0x0004
   TYPE_F_BBOX_MODE		0x0008
   TYPE_F_INVISIBLE		(TYPE_F_NO_POLYGONS | TYPE_F_NO_LINES)
   TYPE_F_STUDLESS_MODE 	0x0010
   TYPE_F_STUDONLY_MODE 	0x0020
   TYPE_F_STUDLINE_MODE 	0x0040
   TYPE_F_SHADED_MODE	 	0x0080
-iN picks output image type. 1 = PNG, 2 = Transparent PNG, 3 = BMP8, 4 = BMP24 
    5 = PPM.  Use -N to turn off output image cropping.
-j picks the orthographic projection.
-J picks the perspective projection.
-ms Saves an image for each STEP and quits.
-mS Does the same, but without opening a window (on most systems).
    You can also include an optional output filename.
    eg. -mSpicture.png
-n2 renders each part in the backbuffer before transfering it to the screen.
-n4 renders the whole model in the backbuffer and then copies it to the screen.
-p turns on polling mode.
-l logs messages to the ldglite.log file instead of stdout.
-ld forces the program to use the ldlite parser.
-ldcF<ldconfig.ldr file> Include an LDConfig.ldr file as a command line argument. 
   e.g. -ldcF/usr/local/foo/CustomLDConfig.ldr. If not used, ldglite will attempt  
   to load the LDConfig.ldr from the default locations (LDRAWDIR and CWD).
-ldiR<LDraw path> LDraw part library path as a command line argument.
   e.g. -ldiR/usr/local/ldraw. If not used, ldglite will attempt to load the
   the library from the LDRAWDIR environment variable, the default location or
   the user profile path.
-ldsE<LDraw search path(s)> Include a | delimited string of extra LDraw search paths.
   e.g. C:/Users/LPub3D/LDraw/Unoff/helper|C:/Users/LPub3D/LDraw/Unoff/LSynth. If
   not used, ldglite will attempt the load extra search directories from the
   LDRAWSEARCH environment variable. NOTE: This environment variable renders
   ldglite unstable on Windows. The problem seems to originate from the LDrawIni
   module which is used to process the search paths.
-l3 forces the program to use the l3 parser.
-le starts the program in LEDIT emulation mode. 
-lE starts the program in LEDIT emulation mode with solid moving pieces.
-ledit or -LEDIT do the same but with draw-to-current and stud-as-line on.
-lcX,Y,Z sets the light location.  (like l3p, except default = -1000,1000,1000)
-lcX,Y,Z,R,G,B sets the light location and color.
-lCR,G,B sets the ambient lighting color.
-lhSR,G,B,A sets the high contrast stud cylinder color - default is 27,42,52,255.
-lhPR,G,B,A sets the high contrast edge color for parts - default is 0,0,0,255.
-lhBR,G,B,A sets the high contrast edge color for black parts - default
    is 255,255,255,255.
-lhDR,G,B,A sets the high contrast edge color for dark color parts - default
    is 27,42,52,255.
-laCN.NN sets the color contrast, valid values are 0.0f to 1.0f. This
    setting is used when the automate edge colors command option (-laA) is present.
-laIN.NN sets the point where color values are classified as light or dark when,
    high contrast stud styles (-fss6/7) are enabled. This setting also provides
    the saturation value when automate edge colors (-laA) is enabled. valid
    values are between 0.0f and 1.0f.
-laA enable automate edge colors. This flag enables the application to
    automatically adjust part edge colors using the user configurable light/
    dark colour value, and contrast settings.
-q quality lines (antialiased)
-uC Use upper case LDraw part and file output strings. If not used, ldglite will
   attempt to populate this flag using the LDRAWDIRCASE environment variable.
-uX,Y saves a huge output image from window sized tiles.  (see notes below)
-v0 displays in a 320x200 window.
-v-1 displays in fullscreen with no decorations.
-v-2 displays in fullscreen gamemode (no menus in gamemode).
-v7 displays in 1600x1024
-v8 displays in 1600x1200
-vX,Y displays in an X wide by Y high window.
-wN sets linewidth to N.
-x displays the x,y,z axis.
-zN sets the near clipping plane.  (default = 10.  100 if GL_DEPTH_BITS < 24)
-ZN sets the far clipping plane.  (default = 4000)

Hot Keys (especially useful in fullscreen gamemode):
NOTE:  A few keys were changed in 0.9.0 to accomodate LEDIT emulation mode.
The arrow key combinations were redefined in 0.9.6.  CTRL key moves camera.
Some mouse key combos were redefined in 1.1.3.  CTRL mouse moves camera.

F10 enters fullscreen gamemode.  (May be faster on some video cards)
F9 exits fullscreen gamemode.

Right Mouse button displays the popup menu.
Left Mouse Drag will rotate the model (defaults to solid bounding box mode)
Alt Left Mouse Drag rotates the model in solid studless mode.
Alt Shift Left Mouse rotates the model with everything drawn. (good for parts)

Arrows keys move the current part in LEDIT mode, the model in LDRAW mode.
Shifted Arrow keys always move the model (in LEDIT or LDRAW mode).
Alt and Arrow keys spin the model by 5 degrees.
Shift, Alt, and Arrows spin the model by 45 degrees.

Ctrl Arrow keys move the camera position left, right, up, down.
Ctrl Shift Arrow keys turn the camera 5 degrees left, right, up, down.
Ctrl Alt Arrow keys roll the camera left, right, up, down.
PGUP and PGDN move the camera in and out (only visible in perspective mode)

0-9 keys pick one of the preset viewpoints.
+ scales the model up to double size. (ALT +  zooms in 10%)
- shrinks the model by half.  (ALT -  zooms out 10%)
S attempts to scale the model to fit the window.  Needs work though.
s toggles step/continuous mode
g toggles polling mode.
j picks the orthographic projection.
J picks the perspective projection.
r toggles the parser (LdLite or L3)
n turns on normal (no) shading.
h turns on shading.
l turns on line only wireframe mode.
f toggles stud drawing.
V toggles visible spin mode.
b prints a BMP file. (use ALT-b for 24 bit BMP)
B prints a PNG file. (use ALT-B for Alpha transparency)
q toggles line quality (antialiasing)
Esc quits.

INSERT key toggles LEDIT emulation mode.  
(use CTRL-INSERT if you want to render the moving part solid)

An excellent description of the LEDIT Hot Keys can be found at:
  http://library.thinkquest.org/20551/keys.html

			   LEDIT extensions

The LEDIT emulation currently contains an extension to the linetype menu
to allow adding primitive linetypes 2-5.  The parser for this ignores
commas and parenthesis so you can enter the points for a type 2 line
like this if you wish:

Enter Coords for Type 2 Primitive: (0.0, 0.0, 0.0) (10.0, 10.0, 10.0)

The color defaults to 16 for filled primitives and 24 for line primitives.

Also, in addition to the x,y,z keys to translate the piece along an axis,
you can now use the v key to translate by an (x,y,z) vector.

The Piece menu now also contains:
  an (x,y,z) location option 
  an (x,y,z) scale option (use negative numbers to mirror)
  a matrix replacement option.
  a part inliner option.

The Turn menu now also contains:
  an (x,y,z) origin option to select an arbitrary turn center.
  an option to toggle display of the three axis through the turn center.
  a rotate option (uses the part origin instead of the turn center)

The Edit menu now also contains:
  a hoser function.  Insert 2 hose end parts.  Type /EH50 at the 2nd part.
  Hose ends are 750.dat, 752.dat, 755.dat, 76.dat, 79.dat, and stud3a.dat.

A Rotate menu has been added:  Like the rotate option on the Turn menu.

			LEDIT emulation quirks

Part lookup is not currently supported.  Try the printable parts catalog.
  http://moon.pr.erau.edu/~sayrew/lego.html
Windows users can also use ldlist or the VEC from LDAO for part lookup.
  http://user.tninet.se/~hbh828t/ldlist.htm
  http://www.ldraw.org/download/software/ldao

Empty lines are discarded by the L3 parser.  Use a blank comment instead.


				Notes

Lugnet news article http://news.lugnet.com/cad/?n=5344 describes
how linux users can use ldglite to convert DAT or MPD files to GIF 
and JPEG.

LDGLite (see http://www.ldraw.org/reference/linux for installation
instructions on Linux) can convert DAT and MPD files to PNG:

  ldglite -i2 -ms model.mpd

Using netpbm/pbmplus (and tcsh) it is easy to convert the PNG files
to GIF and JPEG:

  foreach file ( /usr/local/share/ldraw/bitmap/model*.png )
    pngtopnm < ${file} | cjpeg > `basename ${file} .png`.jpeg
    pngtopnm < ${file} | ppmtogif > `basename ${file} .png`.gif
  end

And if you want to improve the looks of the model, you can render
the model in double size, and then scale down the images:

  ldglite -i2 -ms -S2 -w2 model.mpd

  foreach file ( /usr/local/share/ldraw/bitmap/model*.png )
    pngtopnm < ${file} | pnmscale 0.5 | cjpeg > `basename ${file} .png`.jpeg
    pngtopnm < ${file} | pnmscale 0.5 | ppmtogif > `basename ${file} .png`.gif
  end

New as of version 1.2.10:
Ldglite will render double size and then decimate for you with -2g,2x
No need for any external image decimation program.  (png output only)

  ldglite -i2 -ms -2g,2x model.mpd


		       -----------------------

Use -uX,Y on the command line to build and save an image larger than
the screen made out of several tiles, each the size of the ldglite
window.  This can be used in combination with the -W option for wide
lines and/or the -Q switch for antialiased lines to generate high
quality instructions.  Currently this dumps PPM files instead of BMP
files.  PNG works, but transparent backgrounds and cropping are not
supported in tiled mode.  Ldglite automatically exits after saving the
images (much like the -MS mode).  The following command renders
the model in double size and saves a 2000 by 2000 PNG file.  It uses
the l3 parser so it only has to parse the mpd file once for the multiple
rendering passes required for the tiles.

  ldglite -l3 -i2 -s2 -w2 -u2000,2000 model.mpd

		       -----------------------

Use -mS (uppercase S) to save images without even opening a window.
This is nice for background rendering processes.  Use this in
combination with -vX,Y to set the image size.  This currently does NOT
work in conjunction with the tiled rendering mode, but perhaps by the
next release.  To get large images with the offscreen MESA driver
you'll need to edit the MESA config file src/config.h and change
MAX_WIDTH and MAX_HEIGHT, then recompile MESA.  Linux users can also
use the X Virtual FrameBuffer (xvfb) to render offscreen.  I suspect
xvfb may also require the changes to the MESA config file to produce
large images.

		       -----------------------

Here's a batch file for nifty fogged perspective rendering of datsville.

ldglite v5 -fs -fe -J -mc -s0.2 town.dat -ff3,0.0003,0,0.8,0.8,1.0
