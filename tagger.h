#include <iostream>
#include <sstream>
#include <map>
#include <cmath>
#include <climits>
#include "ugly_global_vars.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

using namespace std;
namespace boost {
namespace serialization {
class access;
}
}
class Tagger {
    private:
        int n_gram;
        int total_tokens_count;
        map<string, int> unigram_tag_counts; // these maps store n-gram tag counts. The keys are strings and for n>1 the tags are separated by two backslashes (\\). This is to avoid having maps of maps.
        map<string, int> bigram_tag_counts;
        map<string, int> trigram_tag_counts;
        map<string, map<string, float>> tags_per_word_counts;
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive& ar, const unsigned version) {
            ar & n_gram & total_tokens_count & unigram_tag_counts & bigram_tag_counts & trigram_tag_counts & tags_per_word_counts;  // Simply serialize the data members of Obj
        }
        
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
        
};
