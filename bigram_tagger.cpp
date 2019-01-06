#include <iostream>
#include <fstream>
#include "file_ops.h"
#include "string_helpers.h"
#include "ugly_global_vars.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>

using namespace std;
int logger = 0; // declared extern in ugly_global_vars.h

int main (int argc, char** argv) {
    Tagger* p;
    goto readymodel; // Ehh... how bad can it get with one 'goto'? https://xkcd.com/292/
                     // Comment this out if you don't want to use serialized data and build the model first.
    try {
        p = read_corpus(BROWN, "brown"); // First parameter is enum specifying corpus type, the second param gives the directory with corpus files. Only the Brown corpus is supported for now.
                                         // Model building happens while files are being loaded.
        cout << endl << endl;
    }
    catch (const invalid_argument &e) {
        cout << e.what();
        exit (-1);
    }
    
    // save serialized data
    {
        std::ofstream ofs("serialized.txt");
        boost::archive::text_oarchive ar(ofs);
        ar & p;
    }

readymodel:

    Tagger* x;
    // load serialized data
    {
        // Create and input archive
        std::ifstream ifs( "serialized.txt" );
        boost::archive::text_iarchive ar(ifs);
 
        // Load the data
        ar & x;
    }
    
    // Some examples - I really should have used vectors instead of string arrays to save myself the pain... Sorry.
    string tags[1] = {"bez"};
    string tag_dt[1] = {"dt"}; string tag_nn[1] = {"nn"};
    string tag_dt_nn[2] = {"dt", "nn"}; // P(nn | dt): tag transition probability: first element is the preceeding tag, second is the following tag.
    string tag_to_vb[2] = {"to", "vb"}; // P(to | vb)
    string tag_to_bos[2] = {"_BOS_", "``"}; // _BOS_ is a beginning-of-sentence symbol.
    cout << "Simple HMM POS tagger - some data from the Brown corpus (note: below are probabilities, but the tagger uses logprobs):\n\n";
    cout << "Number of BEZ tags is " << x->getTagFreq(tags, 1) << endl;
    cout << "Number of 'is' tokens with BEZ tags is " << x->getTagsPerWord("bez", "is") << endl;
    cout << "P(is | bez) = " << x->getObservationLikelihood("bez", "is") << endl;
    cout << "P(race | nn) = " << x->getObservationLikelihood("nn", "race") << endl;
    cout << "P(race | vb) = " << x->getObservationLikelihood("vb", "race") << endl;
    cout << "c(DT) = " << x->getTagCounts(tag_dt, 1) << ", c(NN) = " << x->getTagCounts(tag_nn, 1) << endl;
    cout << "c(DT, NN) = " << x->getTagCounts(tag_dt_nn, 2) << endl;
    cout << "P(NN | DT) = " << x->getTagFreq(tag_dt_nn, 2) << endl;
    cout << "P(VB | TO) = " << x->getTagFreq(tag_to_vb, 2) << endl;
    cout << "P(`` | _BOS_) = " << x->getTagFreq(tag_to_bos, 2) << endl;
    cout << "c(_BOS_, ``) = " << x->getTagCounts(tag_to_bos, 2) << endl;
    cout << "P(I | ppss) = " << x->getObservationLikelihood("ppss", "I") << endl;
    cout << "There are " << x->getTags().size() << " unique tags." << endl;
    cout << "\nSample sentence: 'I want to race .'\n";
    vector<string> sequence = {"I", "want", "to", "race", "."};
    x->tag(sequence); // also prints to console
    cout << "============== USAGE: ==============\n";
    cout << "/t dt nn   - prints P(dt | nn)\n";
    cout << "/w vb want - prints P(want | vb)\n";
    cout << "/q         - quit\n";
    cout << "Type a sentence to run the tagger or use one of the commands (tags must be lowercase, punctuation signs must have a space before them).\n";
    sequence.clear();
    string line;
    while (true) {
        getline(cin, line);
        if (line.substr(0,2)=="/t") {
            stringstream explore(line.substr(3));
            string tag1, tag2;
            explore >> tag1; explore >> tag2;
            string transition[2] = {tag1, tag2};
            cout << "P(" << tag1 << " -> " << tag2 << ") = " << x-> getTagFreq(transition, 2) << endl;
        }
        else if (line.substr(0,2)=="/w") {
            stringstream explore(line.substr(3));
            string oword, otag;
            explore >> otag; explore >> oword;
            cout << "P(" << oword << " | " << otag << ") = " << x-> getObservationLikelihood(otag, oword) << endl;
        }
        else if (line == "/q") break;
        else if (line == "") continue;
        else {
            cout << line << endl;
            sequence = x->tokenize(line);
            x->tag(sequence);
        }
    }
    
    delete (x);
}
