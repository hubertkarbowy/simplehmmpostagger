#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <cmath>
#include <climits>
#include <vector>
#include "ugly_global_vars.h"

#ifdef __BOOST_SERIALIZATION__
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#endif

#ifndef __TAGGER_H__
#define __TAGGER_H__

using namespace std;
#ifdef __BOOST_SERIALIZATION__
namespace boost {
namespace serialization {
class access;
}
}
#endif
class Tagger {
    private:
        int n_gram;
        int total_tokens_count;
        map<string, int> unigram_tag_counts; // these maps store n-gram tag counts. The keys are strings and for n>1 the tags are separated by two backslashes (\\). This is to avoid having maps of maps.
        map<string, int> bigram_tag_counts;
        map<string, int> trigram_tag_counts;
        map<string, map<string, float>> tags_per_word_counts;
        #ifdef __BOOST_SERIALIZATION__
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive& ar, const unsigned version) {
            ar & n_gram & total_tokens_count & unigram_tag_counts & bigram_tag_counts & trigram_tag_counts & tags_per_word_counts;  // Simply serialize the data members of Obj
        }
        #endif
        
    public:
        Tagger();
        Tagger(int n_gram);
        void addTag(string tags[], int ngram);
        void addObservation(string token, string tag);
        template <typename GenericTuple> void addAndIncrement (std::map<GenericTuple, int> &my_map, GenericTuple my_tuple);
        map<string, int> getTags();
        static string tagsCollapser(string extended_tag);
        static vector<string> tokenize(string s);
        
        float getTagFreq(string tag[], int ngram);
        int getTagCounts(string tag[], int ngram);
        int getTagsPerWord(string tag, string token);
        float getObservationLikelihood(string tag, string token);
        vector<string> tag(vector<string> tokens);
        #ifdef __NATIVE_SERIALIZATION__
        void serialize_native();
        void deserialize_native();
        #endif
        
};
#endif
