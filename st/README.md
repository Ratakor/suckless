# Ratakor's build of st

## patch added and changes iirc
- alpha
- Xresources but default to dracula theme
- scrolling with mouse and alt + j k u d
- anysize
- clipboard
- copyurl (useful for newsboat + dmenuhandler)
- selectioncolors with alpha
- workingdir
- CSI 22, 23
- reload Xresources with `kill -10 $(pidof st)`

st - simple terminal
--------------------
st is a simple terminal emulator for X which sucks less.


Requirements
------------
In order to build st you need the Xlib header files.


Installation
------------
Edit config.mk to match your local setup (st is installed into
the /usr/local namespace by default).

Afterwards enter the following command to build and install st (if
necessary as root):

    make clean install


Running st
----------
If you did not install st with make clean install, you must compile
the st terminfo entry with the following command:

    tic -sx st.info


See the man page for additional details.

Also add something like that to your .zshenv / .bash_profile
export TERMINFO_DIRS="PREFIX/share/terminfo:/usr/share/terminfo"
where PREFIX is your PREFIX

Credits
-------
Based on Aurélien APTEL <aurelien dot aptel at gmail dot com> bt source code.

