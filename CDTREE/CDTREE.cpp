// CDTREE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "decisionTree.h"
#include "vector"
using namespace std;

int main()
{
    // training loop driver 
    
    // read in data
    string filename = "C:/Users/Berett/Projects/CDTREE/Data/toyDataBinLabels.csv";
    decisionTree myDecTree(filename);
    
    // get first column to split on
    int col = myDecTree.col_to_split_on();

    // generate initial binary mask
    vector<vector<int>> initMask;
    // 121 corresponds to "y" value
    

    // generate binary mask for each value at col in colVals
    // that is, generate a split for each value
    for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col).size(); colValIdx++) {
        vector<vector<int>> firstMask = myDecTree.gen_bin_mask(col, myDecTree.colVals.at(col).at(colValIdx), initMask);
    }
    

    return 0;


}