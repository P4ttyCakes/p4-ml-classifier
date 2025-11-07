#include <iostream>
using namespace std;
#include <set>
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <cassert>
#include <array>
#include <string>
#include <vector>
#include <assert.h>
#include <algorithm>
#include  "csvstream.hpp" 

// void BOW(int& n_o_p,map<string,int>&n_o_p_containing_word, 
//     map<string,int>&num_of_content_with_tag, set<string>&total_words,
//     map<pair<string,string>,int> &pair_of_cw_count){

// }
// EFFECTS: Return a set of unique whitespace delimited words
set<string> unique_words(const string &str) {
  istringstream source(str);
  set<string> words;
  string word;
  while (source >> word) {
    words.insert(word);
  }
  return words;
}


int main(int argc, char* argv[]) {

    if (argc!=2 && argc!=3) {
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }
    ifstream input;
    input.open(argv[1]);
    if(!input.is_open()){
        cout <<"Error opening file: "<< argv[1] << endl;
        return 1;
    }

    string filename = argv[1];

    //map<key tag, set of tag> bag of words
    //map<string, set<string>> BOW;
    
    //p = post
    int n_o_p = 0;
    map<string, int> n_o_p_containing_word;
    map<string,int> num_of_content_with_tag;
    set<string>total_words;
    map<pair<string,string>,int> pair_of_cw_count;
    
    csvstream csvin(filename);
    map<string, string> row;

    while (csvin >> row) {
        n_o_p++;
        cout << "tag: " << row["tag"] << endl;
        cout << "content: " << row["content"] << endl;
        set<string>uw_set = unique_words(row["content"]);
        for (string s:uw_set) {
            n_o_p_containing_word[s]+=1;
            pair_of_cw_count[{row["tag"],s}]++;
        }
        num_of_content_with_tag[row["tag"]]++;            
        //BOW[row["tag"]].insert(uw_set.begin(),uw_set.end());
        total_words.insert(uw_set.begin(),uw_set.end());
    }
    cout << n_o_p;



    if (argc==3) { //train and test mode
    ifstream input2; input2.open(argv[2]);
    if(!input2.is_open()){
        cout <<"Error opening file: "<< argv[2] << endl;
        return 1;
    }
}
    return 0;
}




