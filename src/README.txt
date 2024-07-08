As of writing this, only the lib, test and example dirs are processed by
the build.

Note: any .c and .cpp files will be compied under any subdirs of those three.
Also, the plan is that prefixing files/dirs with . or suffixing them with .tmp
will exclude them, but that hasn't been implemented yet.
