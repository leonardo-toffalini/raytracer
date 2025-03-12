CC = clang++
CFLAGS = -std=c++23 -O3
SCENE ?= 1

main:
	$(CC) $(CFLAGS) src/main.cc -o a.out

run: main
	./a.out $(SCENE) > test.ppm && open test.ppm

clean:
	rm -f a.out test.ppm

