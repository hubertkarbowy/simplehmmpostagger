CC=g++
CFLAGS=-Wall -std=c++11 -I.
LDFLAGS=-Llib
LDLIBS=-lboost_filesystem -lboost_serialization

HMM_TAGGER_CLASSES_SRC=bigram_tagger.cpp file_ops.cpp tagger.cpp string_helpers.cpp
HMM_TAGGER_CLASSES_OBJ=bigram_tagger.o file_ops.o tagger.o string_helpers.o

all: $(HMM_TAGGER_CLASSES_OBJ)
	$(CC) $(LDFLAGS) $^ -o bigram_tagger $(LDLIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -o $@ $(LDLIBS)

.PHONY: clean

clean:
	rm -f *.o bigram_tagger
