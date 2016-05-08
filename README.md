# Extract binary information for the Epiphany

This program is a tool that extracts information on code placement and binary size for Epiphany coprocessor binaries. The tool makes a distinction between local memory and external memory and will show the size of each function in the binary. This allows the user to find which functions might be too large to store in local memory.  Issue `$ make` from the command-line to build the tool. Issue

    epiphany-bin-info -h

for information on how to use the tool.
