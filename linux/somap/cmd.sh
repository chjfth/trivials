############# METHOD 1 #################

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -L . -lfoo2 -Wl,-Map=foo1.map,--cref

gcc -c -o main.o main.c
gcc -o main main.o -L . -lfoo1 -Wl,-Map=main.map,--cref



############# METHOD 2 (USE -rpath) #################

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -L . -lfoo2 -Wl,-rpath,. -Wl,-Map=foo1.map,--cref

gcc -c -o main.o main.c
gcc -o main main.o -L . -lfoo1 -Wl,-Map=main.map,--cref



############# METHOD 3 (USE -rpath-link) #################

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -L . -lfoo2 -Wl,-Map=foo1.map,--cref

gcc -c -o main.o main.c
gcc -o main main.o -L . -lfoo1 -Wl,-rpath-link,. -Wl,-Map=main.map,--cref


############# METHOD 4.1 (USE -rpath-link , use bar.c) #################

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -L . -lfoo2 -Wl,-Map=foo1.map,--cref

gcc -c -o bar.o bar.c
gcc -shared -o libbar.so bar.o -Wl,-Map=bar.map,--cref

gcc -c -o main.o main.c
gcc -o main main.o -L . -lbar -lfoo1 -Wl,-rpath-link,. -Wl,-Map=main.map,--cref



############# METHOD 4.2 (USE -rpath-link, use bar.c) #################

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -L . -lfoo2 -Wl,-Map=foo1.map,--cref

gcc -c -o bar.o bar.c
gcc -shared -o libbar.so bar.o -Wl,-Map=bar.map,--cref

gcc -c -o main.o main.c
gcc -o main main.o -L . -lfoo1 -lbar -Wl,-rpath-link,. -Wl,-Map=main.map,--cref



############ METHOD 0 (discard early so? No! It's so, not archive) ###############

gcc -c -o foo2.o foo2.c
gcc -shared -o libfoo2.so foo2.o -Wl,-Map=foo2.map,--cref

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o -Wl,-Map=foo1.map,--cref

gcc -c -o bar.o bar.c
gcc -shared -o libbar.so bar.o -Wl,-Map=bar.map,--cref

gcc -c -o main.o main.c
gcc -o dmain main.o -L . -lbar -lfoo1 -lfoo2 -Wl,-Map=dmain.map,--cref


############ METHOD A (discard early -lbar, actually use -lfoo2) ###############

gcc -c -o foo2.o foo2.c
ar crs libfoo2.a foo2.o

gcc -c -o foo1.o foo1.c
ar crs libfoo1.a foo1.o

gcc -c -o bar.o bar.c
ar crs libbar.a bar.o

gcc -c -o main.o main.c
gcc -o amain main.o -L . -lbar -lfoo1 -lfoo2 -Wl,-Map=amain.map,--cref


################ same FooX in two dlls, only one effective ####################
gcc -c bar.c use_bar.c foo2.c use_foo2.c vmain.c

gcc -shared -o libbar.so bar.o
gcc -shared -o libuse_bar.so use_bar.o -L . -lbar

gcc -shared -o libfoo2.so foo2.o
gcc -shared -o libuse_foo2.so use_foo2.o -L . -l foo2

gcc -o vmain vmain.o -L . -luse_bar -luse_foo2 -Wl,-rpath-link,.

LD_LIBRARY_PATH=. ./vmain


############### .so and .a mix (1), no name clash reported ###############
gcc -c -o foo2.o foo2.c
ar crs libfoo2.a foo2.o

gcc -c -o foo1.o foo1.c
gcc -shared -o libfoo1.so foo1.o

gcc -c -o bar.o bar.c
gcc -shared -o libbar.so bar.o

gcc -c -o main.o main.c
gcc -o amain main.o -L . -lbar -lfoo1 -lfoo2 -Wl,-Map=amain.map,--cref

############### .so and .a mix (2), no name clash reported ###############
gcc -c -o foo2.o foo2.c
ar crs libfoo2.a foo2.o

gcc -c -o foo1.o foo1.c
ar crs libfoo1.a foo1.o

gcc -c -o bar.o bar.c
gcc -shared -o libbar.so bar.o

gcc -c -o main.o main.c
gcc -o amain main.o -L . -lbar -lfoo1 -lfoo2 -Wl,-Map=amain.map,--cref


