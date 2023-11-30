# Ratakor's build of dwm

## Important notes
For default keybinds read the man pages, it is up to date, there are no default
keybinds for st or dmenu to allow the use of other terminal or application
launcher without recompiling.

Thus this build of dwm can be used as a
[package](https://github.com/ratakor/ratakor-repo) and modified only through
[sxhkd](https://github.com/baskerville/sxhkd) and
[Xresources](https://wiki.archlinux.org/title/X_resources).

Gaps are disabled by default, it's possible to enable them via Xresources by
setting dwm.enablegaps to +1 or by setting enablegaps to 1 in config.h, it's
even possible to enable them on the fly with super + shift + g.

## patch added and changes iirc
- attach aside
- rainbow tab
- Xresources but default to dracula theme
- alpha (transparency)
- quitprompt (super + shift + e)
- bartab (all app are shown in status bar and have an optional border)
- 3 layouts, tile, bottom stack and floating
- true fullscreen (super + f)
- colors in status bar
- status bar on all monitors
- cycle layouts (super + shift + space)
- smartborders
- gaps (also in bar, everything can be toggled on/off with super + shift + g)
- restart with `kill -10 $(pidof dwm)`

## TODO

- Add rofi or any application launcher support to quitprompt
- Fix ugly hard code of font because icons are too big or too small
- Add an azerty configuration switch maybe or autodetect layout idk

---

dwm - dynamic window manager
============================
dwm is an extremely fast, small, and dynamic window manager for X.


Requirements
------------
In order to build dwm you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (dwm is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install dwm (if
necessary as root):

    make clean install


Running dwm
-----------
Add the following line to your .xinitrc to start dwm using startx:

    exec dwm

In order to connect dwm to a specific display, make sure that
the DISPLAY environment variable is set correctly, e.g.:

    DISPLAY=foo.bar:1 exec dwm

(This will start dwm on display :1 of the host foo.bar.)

In order to display status info in the bar, you can do something
like this in your .xinitrc:

    while xsetroot -name "`date` `uptime | sed 's/.*,//'`"
    do
    	sleep 1
    done &
    exec dwm


Configuration
-------------
The configuration of dwm is done by creating a custom config.h
and (re)compiling the source code.
