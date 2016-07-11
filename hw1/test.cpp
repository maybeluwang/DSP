#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <fstream>
#include <vector>
#include "hmm.h"
#ifndef _viterbi
#define _viterbi

class viterbi
{
public:
    int N, M, T, num_model, sample_size;
    int **o, *testing_ans;
    double *max_p;
    double **pi;
    double ***a;
    double ***b;  // HMM
    double ***delta;     // viterbi
    vector<string> sample;
    viterbi(HMM*, const int);
    ~viterbi();
    void input_data(const char*);
    void output_testing_ans(const char*);
    void test();


};
#endif


int main(int argc, char* argv[])
{
    if (argc <4)
        cout<<"Missing Arguments."<<endl;
    else
    {
        HMM hmms[5];
        load_models( argv[1], hmms, 5);
        dump_models( hmms, 5);
    
        viterbi test(hmms, 5);
        test.input_data(argv[2]);
        test.test();
        test.output_testing_ans(argv[3]);
        return 0;
    }

}


 ///////////////////////////member functions for viterbi/////////////////////////////////
    viterbi::viterbi(HMM *new_model, const int num):o(NULL), testing_ans(NULL), max_p(NULL), delta(NULL)
    {
        N=new_model[0].state_num;
        M=new_model[0].observ_num;
        num_model=num;


        //////pi initialize
        pi = new double* [num_model];
        for (int i=0; i<num_model; i++)     pi[i] =new double [N];

        for (int n=0; n<num_model; n++)
            for (int i=0; i<N; i++)     
                pi[n][i]=new_model[n].initial[i];

        //////a iniitalize
        a = new double **[num_model];
        for (int n=0; n<num_model; n++)     a[n] = new double* [N];

        for (int n=0; n<num_model; n++)
            for (int i=0; i<N; i++)     
                a[n][i] = new double [N];

        for (int n=0; n<num_model; n++)
            for (int i=0; i<N; i++)
                for (int j=0; j<N; j++)
                    a[n][i][j]=new_model[n].transition[i][j];

        /////b initialize
        b = new double **[num_model];
        for (int n=0; n<num_model; n++)     b[n]= new double* [M];

        for (int n=0; n<num_model; n++)
            for (int i=0; i<M; i++)     
                b[n][i] = new double [N];

        for (int n=0; n<num_model; n++)
            for (int i=0; i<M; i++)
                for (int j=0; j<N; j++)
                    b[n][i][j]=new_model[n].observation[i][j];


    }

    viterbi::~viterbi()
    {
       
        for (int n=0; n<num_model; n++)
            delete pi[n];

        delete [] pi;

        for (int n=0; n<num_model; n++)
            for (int i=0; i<N; i++)
                delete [] a[n][i];

        for (int n=0; n<num_model; n++)
            delete [] a[n];

        delete [] a;

        for (int n=0; n<num_model; n++)
            for (int i=0; i<M; i++)
                delete [] b[n][i];

        for (int n=0; n<num_model; n++)
            delete [] b[n];

        delete [] b;


        if (o!=NULL)
        {
            for (int i=0; i<sample_size; i++)
                delete [] o[i];
            delete [] o;
        }

        if (max_p!=NULL)    delete [] max_p;

        if(testing_ans!=NULL)   delete [] testing_ans;


        if (delta!=NULL)
        {
            for (int n=0; n<num_model; n++)
                for (int i=0; i<T; i++)
                    delete [] delta[n][i];

            for (int n=0; n<num_model; n++)
                delete [] delta[n];

            delete [] delta;
        }
        
    }


    void viterbi::input_data(const char * filename)
    {
        ifstream fin;
        fin.open(filename, ios::in);

        if(fin.is_open())
        {
            string word;
            while (fin >> word) 
            {
                sample.push_back(word);
            }
        }

        fin.close();

        T=(sample[0]).length();
        sample_size=sample.size(); 
        

        o= new int * [sample_size];
        for (int i=0; i<sample_size; i++) o[i] = new int [T];

        for (int i=0; i<sample_size; i++)
            for (int j=0; j<T; j++)
                o[i][j]=(sample[i][j]-'A');

        sample.clear();

        max_p= new double [sample_size];
        for (int i=0; i<sample_size; i++)   max_p[i]=0;
        testing_ans = new int [sample_size];

        delta = new double** [num_model];
        for (int i=0; i<num_model; i++)     delta[i]= new double* [T];
        
        for (int i=0; i<num_model; i++)
            for (int j=0; j<T; j++)
                delta[i][j]=new double [N];


    }


    void viterbi::output_testing_ans(const char * filename)
    {
        ofstream fout;
        fout.open(filename, ios::out);

        if (fout.is_open())
        {
            for (int i=0; i<sample_size; i++)
            {
                if (testing_ans[i]==0)
                    fout<<"model_01.txt";
                else if (testing_ans[i]==1)
                    fout<<"model_02.txt";
                else if (testing_ans[i]==2)
                    fout<<"model_03.txt";
                else if (testing_ans[i]==3)
                    fout<<"model_04.txt";
                else if (testing_ans[i]==4)
                    fout<<"model_05.txt";
                fout<<" "<<max_p[i]<<endl;

            }
        }
        fout.close();
    }

    void viterbi::test()
    {
        
        for (int current_sample=0; current_sample<sample_size; current_sample++)
        {
            for (int n=0; n<num_model; n++)
            {
                for (int t=0; t<T; t++)
                {
                    for (int j=0; j<N; j++)
                    {
                        if (t == 0)
                        {
                            
                            delta[n][t][j] = pi[n][j] * b[n][o[current_sample][t]][j];
                        }
                            
                        else
                        {
                            double p=0, q=0;
                            for (int i=0; i<N; i++)
                            {
                                p = delta[n][t-1][i] * a[n][i][j];
                                if (p > q) q = p;
                            }
                            delta[n][t][j] = q * b[n][o[current_sample][t]][j];
                        }
                    } 
                }  
            }

            for (int n=0; n<num_model; n++)
            {
                for (int i=0; i<N; i++)
                {
                    if (delta[n][T-1][i] > max_p[current_sample])
                    {
                        max_p[current_sample] = delta[n][T-1][i];
                        testing_ans[current_sample]=n;
                    } 
                }
            }
        }
    }

 