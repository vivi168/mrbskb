# MRUBY SOKOBAN

Sokoban on PSX, but all the game logic is implemented using mruby.

## Building

You need:

- mipsel-none-elf gcc and binutils
- mkpsxiso
- psyq2elf (for convert the Psy-Q library to elf format and use it with modern gcc toolchain)
- the patch for the Psy-Q header files
- the playstation.rb build_config

then, clone mruby

set `PSYQ_PATH` to where you put the files converted with `psyq2elf`

you can then build mruby with

```
MRUBY_CONFIG=/path/to/build_config/playstation.rb rake
```

Set `MRUBY_PATH` to where you cloned mruby. You can then build the game with:

```
make
```
