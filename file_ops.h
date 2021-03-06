#include <dirent.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "tagger.h"
#include "string_helpers.h"
#include "ugly_global_vars.h"

#ifndef __FILE_OPS_H__
#define __FILE_OPH_H__

#define _D(msg, ...) if (logger) printf(msg, ##__VA_ARGS__)

using namespace std;

enum corpus_type {BROWN, NKJP};
Tagger* read_corpus(corpus_type corpus, string path);
#endif
