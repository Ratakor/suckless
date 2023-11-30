dwmblocks
=========
Asynchronous modular status bar for dwm written in C.

Notes
-----
- **DEPRECATED** see [sb](https://github.com/Ratakor/suckless/tree/master/sb) instead.

- If you don't use my build of dwm you will need the
  [statuscolors](https://dwm.suckless.org/patches/statuscolors/) patch and
  [that fix](https://github.com/anurag3301/my-dwm/blob/main/patches/dwm-statucolours-fix.diff)
  from anurag3301 for the colors to work with dwm.

- Blocks are installed with dwmblocks and can be activated with a config file
  located in `$XDG_CONFIG_HOME/dwmblocks/config` (if XDG_CONFIG_HOME is not set,
  `$HOME/.config/dwmblocks/config` is used instead).

- It is possible to manually refresh a block with
  `kill -sig $(pidof dwmblocks)`, see Configuration for the list of signals.
  e.g. `kill -34 $(pidof dwmblocks)` will refresh the music block.

- Restart dwmblocks with `kill -10 $(pidof dwmblocks)`.

Installation
------------
Make sure to have these dependencies: libX11, libconfig, pthread and run

    # make install

After that you can put dwmblocks in your xinitrc or other startup script to
have it start with dwm.

Configuration
-------------
default configuration:
```
# Name       Active  Signal
music      = false   #  34
cputemp    = false   #  35
fanspeed   = false   #  36
memory     = false   #  37
battery    = false   #  38
wifi       = false   #  39
ethernet   = false   #  40
localip    = false   #  41
publicip   = false   #  42
volume     = false   #  43
mic        = false   #  44
news       = false   #  45
weather    = false   #  46
daypercent = false   #  47
date       = false   #  48
time       = true    #  49
```
