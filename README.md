**IPTC keyword editor**


Installation
------------

iptckwed is built using the commands:

    $ make
    # make install

Please note, that the latter one requires root privileges.
By default, iptckwed is installed using the prefix "/usr/local", so the full
path of the executable will be "/usr/local/bin/iptckwed".

You can install iptckwed into a directory of your choice by changing the second
command to:

    # make PREFIX="/your/dir" install


Usage
-----

    iptckwed [-chilq] [-aMmrXx KEYWORDS] [FILES]...

**Options:**

    -a KEYWORDS     Add keywords to files
    -c              Remove all keywords from files
    -h              Print help screen and exit
    -l              Print files and their keywords
    -i              Read file names from stdin
    -M/-m KEYWORDS  Print files which match all/any keyword(s)
    -r KEYWORDS     Remove keywords from files
    -q              Be quiet, disable warnings
    -v              Print version information and exit
    -X/-x KEYWORDS  Exclude files from printing which match all/any keyword(s)

KEYWORDS is a comma-separated list. Options can be combined in any form,
except that only one of the `-l`/`-M`/`-m` options and one of the `-X`/`-x`
options can be used. Options `-a` and `-r` can be used multiple times.

