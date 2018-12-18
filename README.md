Smileycoin fork with an REPL(read evaluate print loop) command line
-------------------
This is a fork of smileycoin that implements an interactive command line
with command history and tab completion.

Building
-------------------
Simply follow the instructions provided for your operating system.
No extra steps are necessary to build this fork.

Running
-------
To run the interactive version of the command line, run the smileycoin-cli
with "interactive" as its argument.
```
./smileycoin-cli interactive
```

Development
-------------------
We decided to create this fork because operation on the command line felt
tendious to us, and having an REPL is the first step in a less tedious direction.
Having command history and tab completion makes for a better experience when
using this tool.

Initally the idea was also to add some interactivity to some commands but due to
time mismanagement and the fact that implementing the command history and tab
completion was headache until we found a library that did the job, we had to
cut some corners.
There are 2 extra commands provided, clear and history. Clear clears the terminal screen and history prints a list of recent commands.
The library used for command history and tab completion is not written to be
compatable with c++, so some edits had to be made in order to get it to compile.

All of the code changes can be found in bitcoin-cli.cpp

Community
---------

https://bitcointalk.org/index.php?topic=845761.0;all
