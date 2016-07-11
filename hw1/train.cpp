#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <iomanip> 
#include "hmm.h"
#ifndef _model
#define _model

class model
{
public:
	int N, M, T, sample_size, num_to_train;
    int **o;
	double *pi;
    double **a;
    double **b;  // HMM
    double **alpha, **beta;        // evaluation problem
    double **gamma, ***epsilon;     // training
    double *total_pi,*total_gamma, *total_transition_gamma, **total_epsilon, **total_gamma_in_an_observ;
    vector<string> sample;
    model(HMM);
    ~model();
    void input_data(const char*);
    void output_model(const char*);
    void train();


};
#endif




int main (int argc, char* argv[])
{
    if (argc <5)
        cout<<"Missing Arguments."<<endl;
    else
    {
        HMM hmm_initial;
        loadHMM( &hmm_initial, argv[2] );
        dumpHMM( stderr, &hmm_initial );

        model model_train(hmm_initial);
    
        model_train.input_data(argv[3]);
        model_train.num_to_train=atoi(argv[1]);
        model_train.train();
        model_train.output_model(argv[4]);

    }
    return 0;
}

 ///////////////////////////member functions for model/////////////////////////////////
    model::model(HMM hmm_initial)
    :alpha(NULL), beta(NULL), gamma(NULL), epsilon(NULL), o(NULL), total_pi(NULL), total_epsilon(NULL), total_gamma(NULL), total_transition_gamma(NULL), total_gamma_in_an_observ(NULL)
    {
        N=hmm_initial.state_num;
        M=hmm_initial.observ_num;



        //////pi initialize
        pi =new double [N];

        for (int i=0; i<N; i++)     pi[i]=hmm_initial.initial[i];



        //////a iniitalize
        a = new double* [N];
        for (int i=0; i<N; i++)     a[i] = new double [N];


        for (int i=0; i<N; i++)
            for (int j=0; j<N; j++)
                a[i][j]=hmm_initial.transition[i][j];


        /////b initialize

        b= new double* [M];
        for (int i=0; i<M; i++)     b[i] = new double [N];

        for (int i=0; i<M; i++)
            for (int j=0; j<N; j++)
                b[i][j]=hmm_initial.observation[i][j];
  

    }

    model::~model()
    {
       
        delete [] pi;

        for (int i=0; i<N; i++)
            delete [] a[i];

        delete [] a;

        for (int i=0; i<M; i++)
            delete [] b[i];

        delete [] b;


        if (alpha!=NULL)
        {
            for (int i=0; i<T; i++)
                delete [] alpha[i];

            delete [] alpha;
        }

        if (beta!=NULL)
        {
            for (int i=0; i<T; i++)
                delete [] beta[i];

            delete [] beta;
        }

        if (o!=NULL)
        {
            for (int i=0; i<sample_size; i++)
                delete [] o[i];
            delete [] o;
        }

        if (gamma!=NULL)
        {
            for (int i=0; i<T; i++)
                delete [] gamma[i];

            delete [] gamma;
        }


        if (epsilon!=NULL)
        {
            for (int i=0; i<T-1; i++)
                for (int j=0; j<N; j++)
                delete [] epsilon[i][j];

            for (int i=0; i<T-1; i++)
                delete [] epsilon[i];

            delete [] epsilon;
        }



        if (total_pi!=NULL)
        {
            delete [] total_pi;
        }



        if (total_transition_gamma!=NULL)
        {
            delete [] total_transition_gamma;
        }



        if (total_epsilon!=NULL)
        {
            for (int i=0; i<N; i++)
                delete [] total_epsilon[i];

            delete [] total_epsilon;
        }


        if (total_gamma!=NULL)
        {
            delete [] total_gamma;
        }

        if (total_gamma_in_an_observ!=NULL)
        {
            for (int i=0; i<M; i++)
                delete [] total_gamma_in_an_observ[i];

            delete [] total_gamma_in_an_observ;
        }

    }


    void model::input_data(const char * filename)
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

        alpha = new double* [T];
        for (int i=0; i<T; i++)     alpha[i] = new double [N];

        beta = new double* [T];
        for (int i=0; i<T; i++)     beta[i] = new double [N];

        gamma = new double* [T];
        for (int i=0; i<T; i++)     gamma[i] = new double [N];

        epsilon = new double ** [T-1];
        for (int i=0; i<T-1; i++)     epsilon[i] = new double* [N];

        for (int i=0; i<T-1; i++)
            for (int j=0; j<N; j++)
                epsilon[i][j]= new double [N];

        total_pi =new double [N];

        total_transition_gamma = new double [N];

        total_gamma = new double  [N];
        
        total_epsilon = new double * [N];
        for (int i=0; i<N; i++)     total_epsilon[i] = new double [N];

        total_gamma_in_an_observ = new double * [M];
        for (int i=0; i<M; i++)     total_gamma_in_an_observ[i] = new double [N];

    }


    void model::output_model(const char * filename)
    {
        
        ofstream fout;
        fout.open(filename, ios::out);

        if (fout.is_open())
        {
 
            fout<<"initial: "<<N<<endl;
            fout<<fixed<<setprecision(5);
            for (int i=0; i<N-1; i++)
                fout<<pi[i]<<' ';
            fout<<pi[N-1];
            
            fout<<endl<<endl<<"transition: "<<N<<endl;

            for (int i=0; i<N; i++)
            {
                for (int j=0; j<N-1; j++)
                    fout<<a[i][j]<<' ';
                fout<<a[i][N-1]<<endl;
            }
            

            fout<<endl<<"observation: "<<M<<endl;

            for (int i=0; i<M; i++)
            {
                for (int j=0; j<N-1; j++)
                    fout<<b[i][j]<<' ' ;
                fout<<b[i][N-1]<<endl;
            }
        }
        fout.close();
        

    }

    void model::train()
    {
    for (int num =0; num< num_to_train; num++)
    {
            for (int current_sample=0; current_sample<sample_size; current_sample++)
        {
            
            //forward
            for (int t=0; t<T; t++)
            {
                for (int j=0; j<N; j++)
                {
                    if (t == 0)
                        alpha[t][j] = pi[j] * b[o[current_sample][t]][j];
                    else
                    {
                        double p = 0;
                        for (int i=0; i<N; i++)
                         p += alpha[t-1][i] * a[i][j];
                        alpha[t][j] = p * b[o[current_sample][t]][j];
                    }
                }   
            }
            

            //backward
            for (int t=T-1; t>=0; t--)
            {
                for (int i=0; i<N; i++)
                {
                
                    if (t == T-1)
                    {
                        beta[t][i] = 1.0;
                    }
                    else
                    {
                        beta[t][i] = 0;
                        for (int j=0; j<N; j++)
                            beta[t][i] += a[i][j] * b[o[current_sample][t+1]][j] * beta[t+1][j];
                    }
                }       
            }

            //gamma[t][i]=P(q_t=i|O, lambda)
            for (int t=0; t<T; t++)
            {
                double p =0;

                for (int i=0; i<N; i++)
                    p += alpha[t][i]*beta[t][i];

                for (int i=0; i<N; i++)
                gamma[t][i] = alpha[t][i] * beta[t][i] / p;
            }


            //epsilon[t][i][j]=P(q_t=i, q_t+1=j|O, lambda)
            for (int t=0; t<T-1; t++)
            {
                double p=0; 
                for (int i=0; i<N; i++)
                    for (int j=0; j<N; j++)
                        p += alpha[t][i] * a[i][j] * b[o[current_sample][t+1]][j] * beta[t+1][j];

                for (int i=0; i<N; i++)
                    for (int j=0; j<N; j++)
                        epsilon[t][i][j]= alpha[t][i] * a[i][j] * b[o[current_sample][t+1]][j] * beta[t+1][j] / p;
            }
            

            if (current_sample==0)//initialize for all matrix to sum
            {
                //pi
                for (int i=0; i<N; i++) 
                    total_pi[i] = gamma[0][i];


                //total_epsilon
                for (int i=0; i<N; i++)
                {
                    for (int j=0; j<N; j++)
                    {
                        for (int t=0; t<T-1; t++)
                        {
                            if (t==0)   total_epsilon[i][j]=epsilon[t][i][j];
                            else        total_epsilon[i][j] += epsilon[t][i][j];
                        }
                    }
                }

                //total_transition_gamma
                for (int i=0; i<N; i++)
                {
                    for (int t=0; t<T-1; t++)
                    {
                        if (t==0)   total_transition_gamma[i]=gamma[t][i];
                        else        total_transition_gamma[i]+=gamma[t][i];
                    }
                }


                //total_gamma_in_an_observ

                for (int k=0; k<M; k++)
                {
                    for (int j=0; j<N; j++)
                    {
                        total_gamma_in_an_observ[k][j]=0;

                        for (int t=0; t<T; t++)
                        {      
                            if(o[current_sample][t]==k)
                            {
                                total_gamma_in_an_observ[k][j]+=gamma[t][j];
                            }
                        }
                    }
                }


                //total_gamma
                for (int j=0; j<N; j++)
                {
                    for (int t=0; t<T; t++)
                    {
                        if (t==0)   total_gamma[j]=gamma[t][j];
                        else        total_gamma[j]+=gamma[t][j];
                    }
                }
        
            }
            else
            {
                
                //total_pi
                for (int i=0; i<N; i++) 
                    total_pi[i] += gamma[0][i];

                //total_epsilon
                for (int i=0; i<N; i++)
                {
                    for (int j=0; j<N; j++)
                    {
                        for (int t=0; t<T-1; t++)
                        {
                            total_epsilon[i][j]+=epsilon[t][i][j];
                        }
                    }
                }

                //total_transition_gamma
                for (int i=0; i<N; i++)
                {
                    for (int t=0; t<T-1; t++)
                    {
                        total_transition_gamma[i]+=gamma[t][i];
                    }
                }


                //total_gamma_in_an_observ
                for (int k=0; k<M; k++)
                {
                    for (int j=0; j<N; j++)
                    {
                        for (int t=0; t<T; t++)
                        {      
                            if(o[current_sample][t]==k)
                            {
                                total_gamma_in_an_observ[k][j]+=gamma[t][j];
                            }
                        }
                    }
                }

                //total_gamma
                for (int j=0; j<N; j++)
                {
                    for (int t=0; t<T; t++)
                    {
                        total_gamma[j]+=gamma[t][j];
                    }
                }

            }
   
        }

        for (int i=0; i<N; i++)
            pi[i]=total_pi[i]/sample_size;

        for (int i=0; i<N; i++)
            for (int j=0; j<N; j++)
                a[i][j]=total_epsilon[i][j]/total_transition_gamma[i];

        for (int k=0; k<M; k++)
            for (int j=0; j<N; j++)
                b[k][j]=total_gamma_in_an_observ[k][j]/total_gamma[j];
    }
        
    }

