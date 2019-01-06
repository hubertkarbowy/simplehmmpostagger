#include "file_ops.h"

template<typename GenericType>
bool is_in_vector(vector<GenericType> v, GenericType obj) {
    return (std::find(v.begin(), v.end(), obj) == v.end()) ? false : true;
}

// Reads the corpus files and builds an HMM model.
Tagger* read_corpus(corpus_type corpus, string path) {
    DIR *dp;
    struct dirent *dirp;
    vector<corpus_type> supported_corpora {BROWN};
    Tagger* x = new Tagger();
    
    if (!is_in_vector(supported_corpora, corpus)) throw invalid_argument("Corpus not supported");
    if (corpus == BROWN) {
        vector<string> excluded_names {".", "..", "README", "cats.txt", "CONTENTS"};
        if((dp  = opendir(path.c_str())) == NULL) throw invalid_argument("Cannot open directory " + path);
        int count = 0;
        while ((dirp = readdir(dp)) != NULL) {
            if (is_in_vector(excluded_names, string(dirp -> d_name))) continue;
            string file_name = path + "/" + dirp -> d_name;
            string line;
            ifstream f(file_name);
            int empty_lines_count = 0;
            while (getline(f, line)) { // In the Brown corpus we assume that each line contains a single sentence.
                if (count > 0 && count % 1000 == 0) cout << "Processing line" << count << endl;
                lrtrim(line);
                if (line == "") { empty_lines_count++; continue; } // Some lines are empty - skip them.
                stringstream trimmed_line(line);
                string token;
                string token_1 = "_BOS_"; string singleto[1] = {"_BOS_"};
                string token_2 = "_BOS_";
                x->addTag(singleto, 1); // Don't forget to add _BOS_ to unigram counts.
                while (getline(trimmed_line, token, ' ')) { // Read each line token by token.
                    if (std::count(token.begin(), token.end(), '/') != 1) { // Ignore tokens with slashes that do not separate tags (e.g. 1/12)
                        _D("ABNORMAL TOKEN: %s\n", token.c_str());
                        continue;
                    }
                    string tag, token_content;
                    size_t slash_pos = token.find("/"); // Find the slash that separates the token from the tag
                    token_content = token.substr(0, slash_pos);
                    tag = token.substr(slash_pos+1);
                    tag = x->tagsCollapser(tag); // Reduce the number of tags. E.g. nn-ttl => nn. You can comment this out if you want the full tagset.
                    x->addObservation(token_content, tag);
                    
                    string singleton[1] = {tag};                      x->addTag(singleton, 1);   // add unigrams
                    string bigram_tags[2] = {token_1, tag};           x->addTag(bigram_tags, 2); // add bigrams
                    string trigram_tags[3] = {token_2, token_1, tag}; x->addTag(trigram_tags, 3);// add trigrams
                    token_2 = token_1;
                    token_1 = tag;
                }
                count++;
            }
            f.close();
            _D("Processed %d lines in file %s of which empty %d\n", count, file_name.c_str(), empty_lines_count);
        }
        closedir(dp);
    }
    return x;
}
