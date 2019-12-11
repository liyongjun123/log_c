CFLAGS = -Wall

all:
	gcc *.c -o main -l pthread $(CFLAGS)
clean:
	rm main
