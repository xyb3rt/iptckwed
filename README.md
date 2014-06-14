**IPTC keyword editor**


Installation
------------

iptckwed is built using the commands:

    $ make
    # make install

Please note, that the latter one requires root privileges.
By default, sxiv is installed using the prefix "/usr/local", so the full path
of the executable will be "/usr/local/bin/sxiv".

You can install sxiv into a directory of your choice by changing the second
command to:

    # make PREFIX="/your/dir" install


Usage
-----

    iptckwed [-chlq] [-aMmr KEYWORDS] FILES...

**Options:**

    -a KEYWORDS  Add given keywords to files
    -c           Remove all keywords from files
    -h           Print help screen and exit
    -l           Print the list of keywords in the given files
    -M KEYWORDS  Print all files which match all given keywords
    -m KEYWORDS  Print all files which match any given keyword
    -r KEYWORDS  Remove given keywords from files
    -q           Be quiet, disable warnings
    -v           Print version information and exit

KEYWORDS is a comma-separated list. Options can be combined in any form,
except that only one of the `-l`/`-M`/`-m` options can be used. Options `-a`
and `-r` can be used multiple times.

