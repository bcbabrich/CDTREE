// CDTREE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "decisionTree.h"
#include "vector"
using namespace std;

int main()
{
    // training loop driver
    // for now, this is a manual building of the toy tree
    // eventually it will get converted to a single recursive training function and moved to decisionTree
    
    // read in data
    string filename = "C:/Users/Berett/Projects/CDTREE/Data/toyDataBinLabels.csv";
    decisionTree myDecTree(filename);
    
    // generate initial binary mask
    // represents root node in this driver code
    vector<vector<int>> emptyMask;
    vector<vector<int>> initMask = myDecTree.gen_bin_mask(0, 0, emptyMask);

    // Since we're doing recursion manually, we have to create a bunch of copies of binMask
    vector<vector<int>> intermMask0;
    vector<vector<int>> intermMask1;

    // get first column to split on
    // should be 2 - "sys"
    int col0 = myDecTree.col_to_split_on(initMask);

    // int to store second column to split on
    // should be 1 - "ai"
    int col1;

    // int to store guess from first split
    // should be 76 - "L"
    int guess0;

    // add a new child node to root for each value of col0
    for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col0).size(); colValIdx++) {
        intermMask0 = myDecTree.gen_bin_mask(col0, myDecTree.colVals.at(col0).at(colValIdx), initMask);
        int guess = myDecTree.is_base_case(intermMask0);
        if (guess == NULL) {
            col1 = myDecTree.col_to_split_on(intermMask0);

            // save this version of intermMask for use in next for loop
            intermMask1 = intermMask0;
        }
        else {
            guess0 = guess;
        }
    }

    /*
    We should now be done with the first level of the tree.
    That is, root has been created and has two children.
    One is a guess and one is a new split.
    Luckily, the second level of the tree is identical to the first in behavior.
    */

    // int to store third column to split on
    // should be 4 - "morn"
    int col2;

    // int to store guess from second split
    // should be 68 - "D"
    int guess1;

    // There needs to be a new copy of intermMask in between each for loop call!
    vector<vector<int>> intermMask2;
    vector<vector<int>> intermMask3;

    // add a new child node to root for each value of col1
    for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col1).size(); colValIdx++) {
        intermMask2 = myDecTree.gen_bin_mask(col1, myDecTree.colVals.at(col1).at(colValIdx), intermMask1);
        int guess = myDecTree.is_base_case(intermMask2);
        if (guess == NULL) {
            col2 = myDecTree.col_to_split_on(intermMask2);
            intermMask3 = intermMask2;
        }
        else {
            guess1 = guess;
        }
    }
    
    /*
    We should now be done with the second level of the tree.
    That is, the root has been created along with two children,
    and we've added two children for one of the root's children.
    At this next level we will see a new base case: the mask leaves no data
    */

    // int to store fourth column to split on
    // should be 3 - 'thy'
    int col3;

    // int to store guess from third split
    // should be random - 69 for now lel
    int guess2;

    // new mask for current split
    vector<vector<int>> intermMask4;
    vector<vector<int>> intermMask5;

    // add a new child node to root for each value of col2
    for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col2).size(); colValIdx++) {
        intermMask4 = myDecTree.gen_bin_mask(col2, myDecTree.colVals.at(col2).at(colValIdx), intermMask3);
        int guess = myDecTree.is_base_case(intermMask4);
        if (guess == NULL) {
            col3 = myDecTree.col_to_split_on(intermMask4);
            intermMask5 = intermMask4;
        }
        else {
            guess2 = guess;
        }
    }

    /*
    We should now be done with the third level of the tree.
    That is, the root has been created along with two children,
    we've added two children for one of the root's children,
    and we've added two children to that child.
    There should only be one split left.
    At this next level we will see a new base case: no columns left.
    */

    // there is no fourth column to split on
    // both children should be bases here, i.e., guesses
    // this should not get updated in debugger
    int col4;

    // int to store guesses from fourth split
    // two values should get pushed in here: 76 and 68
    // 76 for "only one label left" base case
    // 68 for "no columns left"
    vector<int> final_guesses;

    // new mask for current split
    vector<vector<int>> intermMask6;
    vector<vector<int>> intermMask7; // this should not get updated in debugger

    for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col3).size(); colValIdx++) {
        intermMask6 = myDecTree.gen_bin_mask(col3, myDecTree.colVals.at(col3).at(colValIdx), intermMask5);
        int guess = myDecTree.is_base_case(intermMask6);
        if (guess == NULL) {
            col4 = myDecTree.col_to_split_on(intermMask6);
            intermMask7 = intermMask6;
        }
        else {
            final_guesses.push_back(guess);
        }
    }


    return 0;
}