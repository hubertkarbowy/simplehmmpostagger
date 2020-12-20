#include "tagger.h"

Tagger::Tagger() {
    n_gram = 2;
    unigram_tag_counts = map<string, int>();
    tags_per_word_counts = map<string, map<string, float>>();
}

Tagger::Tagger(int n_gr) {
    this->n_gram == n_gr;
    unigram_tag_counts = map<string, int>();
    tags_per_word_counts = map<string, map<string, float>>();
}

// Insert a tag sequence into its relevant map (unigrams, bigrams, trigrams) and increase its count.
template <typename GenericTuple>
void Tagger::addAndIncrement (std::map<GenericTuple, int> &my_map, GenericTuple my_tuple) {
    int current_count = my_map[my_tuple];
    my_map[my_tuple] = current_count == 0 ? 1 : current_count + 1;
}

void Tagger::addTag(string tags[], int ngram) {
    if (ngram == 1) {
        addAndIncrement(unigram_tag_counts, tags[0]);
    }
    else if (ngram == 2) {
        string bigram = tags[0] + "\\" + tags[1]; // double slashes are used to avoid having maps of maps
        addAndIncrement(bigram_tag_counts, bigram);
    }
    else if (ngram == 3) {
        string trigram = tags[0] + "\\" + tags[1] + "\\" + tags[2];
        addAndIncrement(trigram_tag_counts, trigram);
    }
    else throw invalid_argument("N-gram model not supported");
}

// Adds a word observation given its tag and keeps the word-given-tag counts.
// The structure here is a map of maps. First key is tag, second key is word.
void Tagger::addObservation(string token, string tag) {
    int current_tags_per_word_counts = tags_per_word_counts[tag][token];
    tags_per_word_counts[tag][token] = current_tags_per_word_counts == 0 ? 1 : current_tags_per_word_counts + 1;
    total_tokens_count++;
}

map<string, int> Tagger::getTags() {
    return unigram_tag_counts;
}

// Returns tag frequencies. First argument is the tag sequence, second argument is the n-gram model.
// The number of elements in the first array must match the integer in the ngram parameter.
float Tagger::getTagFreq(string tag[], int ngram) {
    if (ngram==1) return (float)unigram_tag_counts[tag[0]] / total_tokens_count;
    else if (ngram==2) {
        string two_tags = tag[0] + "\\" + tag[1];
        int bigrams_count = bigram_tag_counts[two_tags];
        int unigrams_count = unigram_tag_counts[tag[0]];
        return (float)bigrams_count / unigrams_count;
    }
    else if (ngram==3) {
        string three_tags = tag[0] + "\\" + tag[1] + "\\" + tag[2];
        string two_tags = tag[0] + "\\" + tag[1];
        int trigrams_count = trigram_tag_counts[three_tags];
        int bigrams_count = bigram_tag_counts[two_tags];
        return (float)trigrams_count / bigrams_count;
    }
    else return -1;
}

// Returns the tag counts. First argument is the tag sequence, second argument is the n-gram model.
// The number of elements in the first array must match the integer in the ngram parameter.
int Tagger::getTagCounts(string tag[], int ngram) {
    if (ngram==1) return unigram_tag_counts[tag[0]];
    else if (ngram==2) {
        string two_tags = tag[0] + "\\" + tag[1];
        return bigram_tag_counts[two_tags];
    }
    else if (ngram==3) {
        string three_tags = tag[0] + "\\" + tag[1] + "\\" + tag[2];
        return trigram_tag_counts[three_tags];
    }
}

// Returns the number of occurrences of `token` in the corpus given `tag`.
int Tagger::getTagsPerWord(string tag, string token) {
    return tags_per_word_counts[tag][token];
}

// Returns the probability of `token` given `tag`.
float Tagger::getObservationLikelihood(string tag, string token) {
    return (float)tags_per_word_counts[tag][token] / unigram_tag_counts[tag];
}

// A very primitive way to reduce the number of tags.
string Tagger::tagsCollapser(string extended_tag) {
    size_t dash = extended_tag.find("-"); // we collapse every tag that has a dash into its more general form, e.g. nn-tl => nn
    if (dash != string::npos) {
        return extended_tag.substr(0, dash);
    }
    else return extended_tag;
}

// Viterbi algorithm. Todo: merge the first and subsequent iterations into a single loop.
std::vector<string> Tagger::tag(vector<string> tokens) {
    map<string, double> previous_column;
    map<string, double> this_column;
    vector<string> backpointers;
    double max_probability_from_prev_step = -1000000.0; // initialize to something around minus infinity (not zero, since we use log probabilities)
    string most_likely_tag = "";
    
    // Initialize the first column with transition probabilities from the start state:
    for (auto const& tag : unigram_tag_counts) {
        previous_column.insert(std::pair<string, double>(tag.first, 1.0));
    }
    
    for (auto const& tag : unigram_tag_counts) {
        string bos_tag[2] = {"_BOS_", tag.first};
        double word_probability = getObservationLikelihood(tag.first , tokens[0]);
        double transition_probability = getTagFreq(bos_tag, 2);
        double combined_probability = log(previous_column[tag.first] * word_probability * transition_probability);
        this_column.insert(std::pair<string, double>(tag.first, combined_probability)); // save the Viterbi path
        if (combined_probability > max_probability_from_prev_step) {
            max_probability_from_prev_step = combined_probability;
            most_likely_tag = tag.first;
        }
    }
    cout << tokens[0] << " -> " << most_likely_tag << ", logprob: " << max_probability_from_prev_step << endl;
    backpointers.push_back(most_likely_tag); // save the most likely tag
    previous_column = this_column; // copy the column containing Viterbi paths and...
    this_column.clear(); // discard everything else.

    for (size_t i=1; i<tokens.size(); i++) { // For every token in the sentence starting from the second
        most_likely_tag = "";
        max_probability_from_prev_step = -1000000.0;
        for (auto const& this_tag : unigram_tag_counts) { // Go over all the tags in _this_ column and ...
            double max_prob_this_col = -10000000.0;
            for (auto const& prev_tag : unigram_tag_counts) { // ... compute the new Viterbi paths from the _previous_ column.
                string prev_next_tag[2] = {prev_tag.first, this_tag.first};
                double previous_viterbi_path = previous_column[prev_tag.first];
                double word_probability = getObservationLikelihood(this_tag.first, tokens[i]);
                double transition_probability = getTagFreq(prev_next_tag, 2);
                /* if (prev_tag.first == "ppss" || prev_tag.first == "vb" || prev_tag.first == "nn" || prev_tag.first == "to")  {
                    if (this_tag.first == "ppss" || this_tag.first == "vb" || this_tag.first == "nn" || this_tag.first == "to") {
                        // cout << "Prev viterbi path [" << prev_tag.first << "]: " << previous_viterbi_path << ", P( " << prev_tag.first << " -> " << this_tag.first << ") = " << transition_probability << ", P(" << this_tag.first << " | " << tokens[i] << ") = " << word_probability << endl;
                    }
                }
                */
                
                double combined_probability = previous_viterbi_path + log(word_probability * transition_probability); // Do not save it yet!
                if (combined_probability > max_prob_this_col) max_prob_this_col = combined_probability; // Find the path with the max value.
                if (combined_probability > max_probability_from_prev_step) { // However, we can spot the most promising tag inside this loop.
                    max_probability_from_prev_step = combined_probability;
                    most_likely_tag = this_tag.first;
                }
            }
            this_column.insert(std::pair<string, double>(this_tag.first, max_prob_this_col)); // Only save the Viterbi path after going over all the tags in the _previous_ column.
        }
        cout << tokens[i] << " -> " << most_likely_tag << ", logprob: " << max_probability_from_prev_step << endl;
        backpointers.push_back(most_likely_tag); // Save the tag
        previous_column = this_column; // Copy this column
        this_column.clear(); // Discard old data
    }
    return backpointers; 
}

// Just split the incoming string by spaces. It's primitive, but that's not the point.
std::vector<string> Tagger::tokenize(string s) {
    string token;
    vector<string> tokens;
    stringstream ss(s);
    while (getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    return tokens;
}

#ifdef __NATIVE_SERIALIZATION__
void Tagger::serialize_native() {
    cout << "Serializing data to `serialized_native.txt`...\n";
    stringstream text_archive("");
    text_archive << "=== SIMPLEHMMPOSTAGGER_HEADER ===\n";
    text_archive << "n_gram\t" << (this->n_gram) << "\n";
    text_archive << "total_tokens_count\t" << (this->total_tokens_count) << "\n";
    text_archive << "=== BEGIN UNIGRAM_TAG_COUNTS_DATA ====\n";
    for (auto it = this->unigram_tag_counts.begin(); it != this->unigram_tag_counts.end(); it++) {
        text_archive << it->first << "\t" << it->second << "\n";
    }
    text_archive << "=== BEGIN BIGRAM_TAG_COUNTS_DATA ====\n";
    for (auto it = this->bigram_tag_counts.begin(); it != this->bigram_tag_counts.end(); it++) {
        text_archive << it->first << "\t" << it->second << "\n";
    }
    text_archive << "=== BEGIN TRIGRAM_TAG_COUNTS_DATA ====\n";
    for (auto it = this->trigram_tag_counts.begin(); it != this->trigram_tag_counts.end(); it++) {
        text_archive << it->first << "\t" << it->second << "\n";
    }
    text_archive << "=== BEGIN TAGS_PER_WORD_COUNTS_DATA ====\n";
    for (auto it = this->tags_per_word_counts.begin(); it != this->tags_per_word_counts.end(); it++) {
        text_archive << it->first << "\n";
        for (auto it_inner = it->second.begin(); it_inner != it->second.end(); it_inner++) {
            text_archive << it_inner->first << "\t" << it_inner->second << "\t";
        }
        text_archive << "\n";
    }
    ofstream f("serialized_native.txt", ofstream::out);
    f << text_archive.str();
    f.flush();
    f.close();
}

void Tagger::deserialize_native() {
    cout << "Restoring data from `serialized_native.txt`...\n";
    ifstream f("serialized_native.txt");
    cout << "Validating file information: ";
    unsigned char is_valid_file = 31;
    string line;
    while (getline(f, line)) {
        if (line.find("=== SIMPLEHMMPOSTAGGER_HEADER") == 0) { cout << "HEADER "; is_valid_file ^= 16; }
        if (line.find("=== BEGIN UNIGRAM_TAG_COUNTS_DATA") == 0) { cout << "UNIGRAM "; is_valid_file ^= 8; }
        if (line.find("=== BEGIN BIGRAM_TAG_COUNTS_DATA") == 0) { cout << "BIGRAM "; is_valid_file ^= 4; }
        if (line.find("=== BEGIN TRIGRAM_TAG_COUNTS_DATA") == 0) { cout << "TRIGRAM "; is_valid_file ^= 2; }
        if (line.find("=== BEGIN TAGS_PER_WORD_COUNTS_DATA") == 0) { cout << "TAG_COUNTS "; is_valid_file ^= 1; }
    }
    cout << endl;
    if (is_valid_file != 0) {
        cout << "Archive appears corrupted\n";
        exit(1);
    }
    cout << "Archive appears correct\n";
    f.clear();
    f.seekg(0);
    string mode;
    while (getline(f, line)) {
        if (line.find("=== SIMPLEHMMPOSTAGGER_HEADER") == 0) { mode = "header"; }
        else if (line.find("=== BEGIN UNIGRAM_TAG_COUNTS_DATA") == 0) { mode = "unigram"; }
        else if (line.find("=== BEGIN BIGRAM_TAG_COUNTS_DATA") == 0) { mode = "bigram"; }
        else if (line.find("=== BEGIN TRIGRAM_TAG_COUNTS_DATA") == 0) { mode = "trigram"; }
        else if (line.find("=== BEGIN TAGS_PER_WORD_COUNTS_DATA") == 0) { mode = "counts"; }
        else {
            if (mode == "header") {
                string token;
                string param_name;
                int param_value;
                stringstream params_tuple = stringstream(line);
                params_tuple >> param_name >> param_value;
                if (param_name == "n_gram") this->n_gram = param_value;
                if (param_name == "total_tokens_count") this->total_tokens_count;
            }
            else if (mode == "unigram" || mode == "bigram" || mode == "trigram") {
                string tag;
                int tag_count;
                stringstream params_tuple = stringstream(line);
                params_tuple >> tag >> tag_count;
                auto ngram_counts = (mode == "trigram" ? this->trigram_tag_counts : (mode == "bigram" ? this->bigram_tag_counts : this->unigram_tag_counts));
                ngram_counts[tag] = tag_count;
                cout << tag << "*" << tag_count << endl;
            }
            else if (mode == "counts") {
                string tag = line;
                getline(f, line); // read second line
                stringstream token_counts = stringstream(line);
                int tuple_idx = 0;
                string token;
                float cnt;
                while (!token_counts.eof()) {
                    if (token_counts.tellg() == -1) { // last tab
                        break;
                    }
                    token_counts >> token >> cnt;
                    this->tags_per_word_counts[tag][token] = cnt;
                }
            }
        }
    }
}
#endif
