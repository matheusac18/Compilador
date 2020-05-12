#!/bin/bash
flex scanner.l
bison -d parser.y
gcc -c *.c
gcc -g -o cmenos *.o -ly -lfl
g++ tradutor.cpp -o tradutor.out
g++ tradutorBinario.cpp -o tradutorBinario.out
./cmenos
./tradutor.out
./tradutorBinario.out
