#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <set>
#include <map>
#include "Ngram.h"
using namespace std;

void input_test(const char *, vector<string> &);
void input_map(const char *, map< string, set<string> > &);
vector<string> viterbi(string &, map< string, set<string> > &, Vocab &, Ngram & );
double getBigramProb(const char*, const char*, Vocab &, Ngram &);

int main(int argc, char * argv[])
{
	int ngram_order = 2;
    Vocab voc;
    Ngram lm( voc, ngram_order );
    vector<string> testfile;
    map<string, set<string> > mapfile;
    
    File lmFile( argv[6], "r" );
    lm.read( lmFile );
    lmFile.close();
    
    input_map( argv[4] , mapfile);
    input_test( argv[2] , testfile);

	for(int i=0; i<testfile.size(); i++) 
	{
        vector<string> ans = viterbi(testfile[i], mapfile, voc, lm);

        for(int j=0; j<ans.size(); j++)
        {
			if (j!=ans.size()-1)
                cout << ans[j] << " ";
            else 
                cout << ans[j];
        }
        cout << endl;
    }
}

void input_map(const char * filename, map<string, set<string> >& mapping)
{
	fstream fin;
	string line;
	fin.open(filename, ios::in|ios::binary);
	while( getline(fin, line) )
	{
        string word = line.substr(0, 2);
        set<string> & current_set = mapping[word];
        line = line.substr(3);
        string newline;
        for(int i=0; i<line.size(); i++) 
        {
            if(line[i]!=' ')    newline += line[i];
        }
        for(int i=0; i<(newline.size()-1); i+=2)
        {
            current_set.insert( newline.substr(i, 2) );
        }
	}
    fin.close();
    
    //put </s> in mapping to compute the probability
    string setence_end = "</s>";
    set<string> & current_set1 = mapping[setence_end];
    current_set1.insert( setence_end );
  

}

void input_test(const char * filename, vector<string> & testing_data)
{
    fstream fin;
	string input_line;
    fin.open(filename, ios::in|ios::binary);
    while( getline(fin, input_line) )
    {
        string line;
        for(int i=0; i<input_line.length(); i++) 
        {
            if( input_line[i]!=' ' ) line += input_line[i];
        }
        testing_data.push_back(line);
    }
    fin.close();
}

// Get P(W2 | W1) -- bigram
double getBigramProb(const char *w1, const char *w2, Vocab & voc, Ngram & lm)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

vector<string> viterbi(string & input, map<string, set<string> > & mapping, Vocab & voc, Ngram & lm)
{
    vector< vector<string> > words( input.size()/2 +1);
    vector< vector<double> > log_prob( input.size()/2 +1);
    vector< vector<int> > previous_index( input.size()/2 +1);

    for(int i=0; i<log_prob.size(); i++) 
    {
        string word;
        if (i==log_prob.size()-1)
            word = "</s>";
        else
        word = input.substr(i*2, 2);

        log_prob[i].resize( mapping[word].size() );
        words[i].resize( mapping[word].size() );
        previous_index[i].resize( mapping[word].size() );

        set<string>::iterator iter = mapping[word].begin();
        for(int j=0; j<words[i].size(); j++) 
        {
            words[i][j] = *iter;
            ++iter;
        }
    }

    // initial 
    for(int i=0; i<log_prob[0].size(); i++) 
    {
        log_prob[0][i] = getBigramProb("<s>", words[0][i].c_str(), voc, lm);
        previous_index[0][i] = 0;
    }
    // veterbi iterative solve
    for(int i=1; i<log_prob.size(); i++) 
    {
        for(int j=0; j<log_prob[i].size(); j++) 
        {
            log_prob[i][j] = -1e9;
            for(int k=0; k<log_prob[i-1].size(); k++)
            {
                double prob;
                prob = getBigramProb(words[i-1][k].c_str(), words[i][j].c_str(), voc, lm);
                prob += log_prob[i-1][k];
                if( prob > log_prob[i][j] ) 
                {
                    log_prob[i][j] = prob;
                    previous_index[i][j] = k;
                }
            }               
        }
    }

    ///backtrack
    double max_prob = -1e9;
    int max_index = 0;
    for(int i=0; i<log_prob.back().size(); i++)
     {
        if( log_prob.back()[i] > max_prob ) 
        {
            max_prob = log_prob.back()[i];
            max_index = i;
        }
    }
    vector<string> output( log_prob.size() );
    vector<string>::iterator it;
    
    for(int i=output.size()-1; i>=0; i--) 
    {
        output[i] = words[i][max_index];
        max_index = previous_index[i][max_index];
    }
    
    //put <s> in front of the sentence, </s> is added in the mapping.
    it = output.begin();
    it = output.insert ( it , "<s>" );
    
    return output;
}