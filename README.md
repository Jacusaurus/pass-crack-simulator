# Password Cracking "Simulator"

A simple program that "simulates" brute force password cracking.  It's not a real password cracker, it's just a program to showcase several C++ concepts, like multi-threading, mutexes, checksums, vectors, and more.

To compile, run `gcc -lphtread -o main main.cpp` in the cloned directory.  Then run `./main <password>`, replacing password with whatever password you want to check.

Please note that the runtime of this program is exponential in the length of the password, so it will take a long time to crack a long password.  Also, the program will not terminate until it has found the password, so if you want to stop it, you'll have to kill it with Ctrl+C.

For a quick demo, try something that's four character or less, like 't3ST' or 'f0RK'.  For something that will take about a minute, you can try something one character longer, like 'P45_W' or 'C++++'.  Anything longer than that will take **much** longer.  You've been warned.
