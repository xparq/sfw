#!/bin/bash

# NOTE: When called from the GH Action, that may pass --dry-run and/or -Werror, too.

clang-format $(find src/lib -type f) -i $*
echo "Done"
