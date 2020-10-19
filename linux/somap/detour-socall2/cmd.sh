gcc -c -g bar.c use_bar.c foo2d.c use_foo2d.c vmaind.c local-foox.c

gcc -shared -o libbar.so bar.o
gcc -shared -o libuse_bar.so use_bar.o -L . -lbar

gcc -shared -o libfoo2d.so foo2d.o local-foox.o
gcc -shared -o libuse_foo2d.so use_foo2d.o -L . -l foo2d

gcc -o vmaind2 vmaind.o -L . -luse_bar -luse_foo2d -Wl,-rpath-link,.

