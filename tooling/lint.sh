#!/bin/bash

clang-format $(find src/impl -type f) -i
echo "Done"
