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
#include <cmath>

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
class Classifier{
    private:
        int n_o_p = 0; //number of posts
        map<string, int> n_o_p_containing_word;
        map<string,int> num_of_content_with_tag;
        set<string>total_words;
        map<pair<string,string>,int> pair_of_cw_count;
    public:
        void train(const string &filename, bool trainOnly){
            csvstream csvin(filename);
            map<string, string> row;

            while (csvin >> row) {
                n_o_p++;
                if (trainOnly){
                    cout << "  label = " << row["tag"]
                    << ", content = " << row["content"] << endl;
                }
                set<string>uw_set = unique_words(row["content"]);
                for (string s:uw_set) {
                    n_o_p_containing_word[s]+=1;
                    pair_of_cw_count[{row["tag"],s}]++;
                }
                num_of_content_with_tag[row["tag"]]++;            
                //BOW[row["tag"]].insert(uw_set.begin(),uw_set.end());
                total_words.insert(uw_set.begin(),uw_set.end());
            }
                if (trainOnly) cout << endl;
        }
        void print_results(bool trainOnly){
            cout << "trained on " << n_o_p << " examples" << endl;
            if (trainOnly){
                cout << "vocabulary size = " << total_words.size() <<endl<<endl;
                cout << "classes:"<<endl;
                for (const auto &A: num_of_content_with_tag){
                string tag = A.first;
                int count = A.second;
                double log_temp = log((double)count / n_o_p);
                cout << "  " << tag << ", " << count
                     << " examples, log-prior = " << log_temp << endl;

                }
                cout <<endl;
                cout << "classifier parameters:"<<endl;
                for (const auto &A: pair_of_cw_count){
                string tag = A.first.first;
                string word = A.first.second;
                int count = A.second;
                double log_temp = log((double)count / num_of_content_with_tag.at(tag));
                cout << "  " << tag<< ":" << word
                     << ", count = " << count
                     << ", log-likelihood = " << log_temp << endl;
            }
             cout << endl;
            }
            
        }

    double find_Score(string tag, const set<string> &words){
        double score = log((double)num_of_content_with_tag.at(tag) / n_o_p);
        for (const string &A : words) {
            int posts_w_word = 0;
            if (n_o_p_containing_word.count(A) > 0){
            posts_w_word = n_o_p_containing_word.at(A);
            }
            int count = 0;
        if (pair_of_cw_count.count({tag, A}) > 0) count = pair_of_cw_count.at({tag, A});
            double log_count;
            if (posts_w_word == 0) {
                log_count = log(1.0 / n_o_p);
            }
            else if (count == 0) {
                log_count = log((double)posts_w_word / n_o_p);
            }
            else {
                log_count = log((double)count /
                                num_of_content_with_tag.at(tag));
            }
            score += log_count;
        }
        return score;
    }

        string predict(const string &content){
            set<string> words = unique_words(content);
            string label;
            bool first = true;
            double score = -INFINITY;
            for (const auto& A : num_of_content_with_tag){
                const string &tag = A.first;
                double temp = find_Score(tag, words);
                if (first){
                    score = temp;
                    label = tag;
                    first = false;
                }
                if (temp>score) {
                    score = temp;
                    label = tag;
                } else if (score == temp && tag < label){
                    label = tag;
                }
            }
            return label;
        }
};
int main(int argc, char* argv[]) {
    cout.precision(3);
    if (argc!=2 && argc!=3) {
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }
    ifstream input; ifstream input2;
    input.open(argv[1]);
    if (argc==3) input2.open(argv[2]);
    if(!input.is_open() || (!input2.is_open() && argc==3)){
        cout <<"Error opening file: "<< argv[1] << endl;
        return 1;
    }
    string filename = argv[1];
    Classifier classifier;
    bool testOnly = argc==2;
    if (testOnly) cout << "training data:" <<endl;
    classifier.train(filename, testOnly);
    classifier.print_results(testOnly);
    if (argc==3) {
    string test_file = argv[2];
        cout << "test data:" << endl;
        csvstream csvin(test_file);
        map<string, string> row;
        int correct = 0;
        int total = 0;
        while (csvin >> row) {
            string clabel = row["tag"];
            string content = row["content"];
            string predicted = classifier.predict(content);
            double score = classifier.find_Score(predicted, unique_words(content));
            cout << "  correct = " << clabel
                 << ", predicted = " << predicted
                 << ", log-probability score = " << score << endl;
            cout << "  content = " << content << endl<<endl;
            total++;
                if (predicted == clabel) correct++;
            }
            cout << "performance: " << correct << " / " <<total
                 << " posts predicted correctly" <<endl;
        }
        return 0;
}
