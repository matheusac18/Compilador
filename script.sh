#!/bin/bash
flex scanner.l
bison -d parser.y
gcc -c *.c
gcc -g -o cmenos *.o -ly -lfl
