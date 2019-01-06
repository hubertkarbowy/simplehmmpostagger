#include "string_helpers.h"

auto _isspace = [](int ch) {
    return !isspace(ch);
};

void ltrim (string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), _isspace));
}

void rtrim (string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), _isspace).base(), s.end());
}
void lrtrim (string &s) {
    ltrim(s);
    rtrim(s);
}
