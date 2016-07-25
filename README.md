# DSP
- hw1 - discrete HMM in c++
 - **train.cpp**
    - Given O and an initial model λ=(A, B, π), adjust λ to minimize P(O|λ)
    - Baum-Welch algorithm 
  
  - **test.cpp** 
    - Given model λ and O, find the best state sequenceto minimize P(O|λ,q)
    - Viterbi Algorithm
  
 - iterating 155 times will have the highest accuarcy in 1~400 times


- hw2 - shell script
  - modify shell script to train data, test data in order to get higher accuracy
  
  - using Hidden Markov Toolkit (HTK) 


- hw3 - in c++
  - build a character-based language model with toolkit SRILM
  
  - **mapping.cpp**
    - turn Big5-ZhuYin.map to ZhuYin-Big5.map
    
  - **disambig.cpp**
    - Viterbi algorithm to find out the most possible sequence
