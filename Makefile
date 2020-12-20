######## Set the flags below to build the tagger optional serialization functions:

# SER_OPTS="-D__BOOST_SERIALIZATION__"
# SER_OPTS="-D__NATIVE_SERIALIZATION__"  # this one is slow, unreliable and untested

CC=g++
CFLAGS=-Wall -std=c++11 -I.
LDFLAGS=-Llib
ifeq (,$(findstring BOOST, $(SER_OPTS)))
	LDLIBS=
else
	LDLIBS=-lboost_filesystem -lboost_serialization
endif

HMM_TAGGER_CLASSES_SRC=bigram_tagger.cpp file_ops.cpp tagger.cpp string_helpers.cpp
HMM_TAGGER_CLASSES_OBJ=bigram_tagger.o file_ops.o tagger.o string_helpers.o


all: $(HMM_TAGGER_CLASSES_OBJ)
	$(CC) $(SER_OPTS) $(LDFLAGS) $^ -o bigram_tagger $(LDLIBS)

%.o: %.cpp
	$(CC) $(SER_OPTS) $(CFLAGS) $(LDFLAGS) -c $^ -o $@ $(LDLIBS)

.PHONY: clean

clean:
	rm -f *.o bigram_tagger
