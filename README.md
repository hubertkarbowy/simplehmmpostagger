# A simple POS tagger using Hidden Markov Model (HMM)

This is my implementation of a Part-Of-Speech tagger for English.

It is based on the Brown corpus and can use both the raw corpus files (included in the brown/ subdirectory) as well as the pre-computed model loaded with the Boost serialization mechanism.

At the moment the tagger only works on the bigram tag transition model, but is easily extensible to trigrams. Only the Maximum Likelihood Estimates (MLE) are used - there is no smoothing implemented yet. This means that out-of-vocabulary (OOV) words are not handled and you will get a probability of zero if you attempt to tag such tokens.

# Compilation
You need to have C++ Boost development libraries (for model serialization).

1. Clone this repo.
2. `make`
3. `./bigram_tagger --read-raw`. Works.

# Serialization
You can optionally use boost libraries to serialize and deserialize the tagger object.
To use this feature, please set the SER_OPT flag in the Makefile accordingly and rebuild the
project.

# Usage

The program is interactive and reads input from the console (you can easily tweak that to read input from options if you want). Just run `./bigram_tagger --read-raw` (or --boost-deserialize) and you will see this message:
```
Simple HMM POS tagger - some data from the Brown corpus (note: below are probabilities,
but the tagger uses logprobs):

Number of BEZ tags is 0.00871372
Number of 'is' tokens with BEZ tags is 9997
P(is | bez) = 0.990194
P(race | nn) = 0.000574252
P(race | vb) = 0.000118161
c(DT) = 8969, c(NN) = 167174
c(DT, NN) = 4486
P(NN | DT) = 0.500167
P(VB | TO) = 0.824221
P(`` | _BOS_) = 0.0727451
c(_BOS_, ``) = 4164
P(I | ppss) = 0.371758
There are 182 unique tags.

Sample sentence: 'I want to race .'
I -> ppss, logprob: -3.89224
want -> vb, logprob: -10.0304
to -> to, logprob: -13.4006
race -> vb, logprob: -22.6374
. -> ., logprob: -26.021
============== USAGE: ==============
/t dt nn   - prints P(dt | nn)
/w vb want - prints P(want | vb)
/q         - quit
Type a sentence to run the tagger or use one of the commands (tags must be lowercase,
punctuation signs must have a space before them).
```
Here's what you can do:
* <b>Type a sentence</b> (remember to put spaces before punctuation signs: `Who framed Roger ?`. You will see the output like this:
```
Who framed Roger ?
Who -> wps, prob: -11.0259
framed -> vbd, prob: -21.9129
Roger -> np, prob: -33.4646
? -> ., prob: -38.6352
```
* <b>Find tag transition probability</b>, i.e. P(nn | dt): `/t dt nn`
* <b>Find word-given-tag probability (observation likelihood)</b>, i.e. P(want | vb): `/w vb want`

# Comments and suggestions

Feel free to contact me if you wish: hk at_sign hubertkarbowy dot_sign pl
