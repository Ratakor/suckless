sb - statusbar
==============
Asynchronous modular status bar for dwm written in C99

![image](sb.png)

Notes
-----
- If you don't use my build of dwm you will need
  [this patch](https://gist.github.com/Ratakor/1a2ebc9a690dea31c19ac419e9e14138)
  for the colors to work with dwm. Also you might want to use
  [this font](https://github.com/Ratakor/dotfiles/raw/master/.local/share/fonts/agave%20regular%20Nerd%20Font%20Complete%20Mono.ttf)
  as fallback as it supports all the icons.

- sb can be configured on the fly with a config file located in
  `$XDG_CONFIG_HOME/sb/config` (if XDG_CONFIG_HOME is not set,
  `$HOME/.config/sb/config` will be used instead).

- It is possible to manually refresh a block with
  `kill -sig $(pidof sb)`, see Configuration for the list of signals.
  e.g. `kill -34 $(pidof sb)` will refresh the music block.

- Restart sb with `kill -10 $(pidof sb)`.

Installation
------------
Make sure to have libX11 and pthread then run

    # make install

After that you can put sb in your xinitrc or other startup script to have it
start with dwm.

Configuration
-------------
default configuration:
```
# Name       Active  Signal
music      = false   #  34
cputemp    = false   #  35
cpu        = false   #  36
memory     = false   #  37
battery    = false   #  38
wifi       = false   #  39
netspeed   = false   #  40
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

TODO
----
- Make async optional and only activated for certain blocks
- Make icons customizable with config.h and maybe the config file
- Make colors optional (and customizable) at runtime
