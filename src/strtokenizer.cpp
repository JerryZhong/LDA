#include <string>
#include <vector>
#include "strtokenizer.h"
 
using namespace std;

strtokenizer::strtokenizer(string str, string seperators) {
    parse(str, seperators);
}

void strtokenizer::parse(string str, string seperators) {
    int n = str.length();
    int start, stop;
    
    start = str.find_first_not_of(seperators);
    while (start >= 0 && start < n) {
	stop = str.find_first_of(seperators, start);
	if (stop < 0 || stop > n) {
	    stop = n;
	}
	
	tokens.push_back(str.substr(start, stop - start));	
	start = str.find_first_not_of(seperators, stop + 1);
    }
    
    start_scan();
}

int strtokenizer::count_tokens() {
    return tokens.size();
}

void strtokenizer::start_scan() {
    idx = 0;
}

string strtokenizer::next_token() {    
    if (idx >= 0 && idx < tokens.size()) {
	return tokens[idx++];
    } else {
	return "";
    }
}

string strtokenizer::token(int i) {
    if (i >= 0 && i < tokens.size()) {
	return tokens[i];
    } else {
	return "";
    }
}

