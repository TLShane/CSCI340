all: csci340_proj1.o
	gcc -o all csci340_proj1.o

csci340_proj1.o: csci340_proj1.c
	gcc -c csci340_proj1.c
