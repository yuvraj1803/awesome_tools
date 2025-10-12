make: Compiles all files in src/ and dumps them into bin/
make install: Adds symlinks of all executables in bin/ to /usr/local/bin/
make uninstall: Removes all symlinks created in 'make install' step
make clean: Removes all executables from bin/ (does not remove installed binaries)
