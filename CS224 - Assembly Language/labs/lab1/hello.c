/* filename: hello.c
   purpose:  demonstrate the process of compilation to execution 

   Step 1. translate source code hello.c into assembly code (hello.s)
      $ gcc -S hello.c         # gcc compiler stops after translation phase
   Step 2. assemble hello.s into machine code (hello.o)
      $ gcc -c hello.s         # gcc calls assembler 'as' with appropriate flags
   Step 3. link object code hello.o into executable (hello)
      $ gcc -o hello hello.o   # gcc calls linker 'ld' with appropriate flags
   Step 4. Execute code        #  OS calls the loader
      $ ./hello           

   Or you can call each program yourself which is a bit of a hassle since
   you have to manually tell the linker what libraries to link in:

   $ gcc -S hello.c
   $ as -o hello.o hello.s
   $ ld /usr/lib/crt1.o  /usr/lib/crti.o /usr/lib/crtn.o  -o hello hello.o -lc
   $ ./hello 

*/

int main()
{
   int x = 5;
   write(1,"hello\n",6);
   return 0;
}
