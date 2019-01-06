CC=g++
CFLAGS=-std=c++11 -I. -lboost_filesystem -lboost_serialization
DEPS=ugly_global_vars.h file_ops.h tagger.h string_helpers.h
OBJ=bigram_tagger.o file_ops.o tagger.o string_helpers.o

all: bigram_tagger

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bigram_tagger: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o bigram_tagger
