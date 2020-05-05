// decision tree class
// 

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "vector"
using namespace std;

class decisionTree {
    // dataset
    // vector of int vectors representing data in the file.
    // filled in constructor
    vector<vector<int>> data;

// all function descriptions provided at actual functions
public:
    

    // all the possible values each column can have
    // filled in constructor
    // only public TEMPORARILY while using CDTREE.cpp as a training loop driver
    // TODO: make colVals private again when training function written
    vector<vector<int>> colVals;

    decisionTree(string);
    int col_to_split_on(); // make this private once training function is written?
    vector<vector<int>> gen_bin_mask(int col, int val, vector<vector<int>> binMask);
};

/*
Constructor for a decision tree object.
- Takes in a string file name corresponding to a csv data file.
- Fill class variable "data"
- Also fills class variable "colVals"
-->  All data is therefore represented as ints.
*/
decisionTree::decisionTree(string filename) {
    // file reading tools
    string line;
    ifstream myfile(filename);

    // colVals helper variables
    bool firstLine = true;
    bool colIdx = 0;

    // begin file reading...
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            // row to be added to data
            vector<int> row;

            // Create a stringstream of the current line
            std::stringstream ss(line);

            // Keep track of the current column index
            int colIdx = 0;

            // Extract each integer
            char c;
            int val;
            bool neg = false;
            while (ss >> c) {
                // read in chars as ints
                if (!neg) {
                    val = c - 0;
                }
                else {
                    // if the last token was a negative sign,
                    // take negative of int value
                    val = 0 - (c - 0);
                    neg = false;
                }

                // place int val in current row
                row.push_back(val);

                // if we're still on the first line,
                // add a new vector to colVals
                // i.e., we're still finding new columns
                if (firstLine) {
                    vector<int> newCol;
                    colVals.push_back(newCol);
                }
                // otherwise add val to corresponding inner vector
                else {
                    // check if val is already accounted for
                    // TODO: this "check if value exists in given vector" should be a utility function
                    bool valFound = false;
                    for (int potVal = 0; potVal < colVals.at(colIdx).size(); potVal++) {
                        if (colVals.at(colIdx).at(potVal) == val) {
                            valFound = true;
                        }
                    }

                    // if not, add it
                    if (!valFound) {
                        colVals.at(colIdx).push_back(val);
                    }
                }

                // If the next token is a comma, ignore it and move on
                if (ss.peek() == ',') ss.ignore();

                // If the next token is a negative sign, ignore but save it
                if (ss.peek() == '-') {
                    neg = true;
                    ss.ignore();
                }

                // increment column index
                colIdx++;
            }
            // push row into data vector
            data.push_back(row);

            // done with the first line, all columns passed over once
            firstLine = false;
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

/*
Finds best column to split on based on column scores
- Takes in nothing. Called during training loop, operates on data.
- Outputs an int corresponding to a column index.
-- Assumptions:
--- data is square
--- labels for data are at the end of their rows
*/
int decisionTree::col_to_split_on() {
    /*
    Initialize colCounts variable, a three-dimensional vector
    - 1st dimension: number of columns in data ... "colCounts"
    - 2nd dimension: possible label values in data ... "labelCounts"
    --  size equal accros all values of the first dimension ... "valCounts"
    - 3rd dimension: possible column values
    --  size unique to second dimension
    */
    
    // grab first two dimensional values for legibility's sake
    int colSize = data.at(0).size() - 1; // -1 here because we don't need counts for labels
    int lablSize = colVals.at(colVals.size() - 1).size();

    // populate colCounts with correct number of zeroes along 3rd dimension
    vector<vector<vector<int>>> colCounts;
    
    for (int col = 0; col < colSize; col++) {
        vector<vector<int>> colVec;
        colCounts.push_back(colVec);

        for (int labl = 0; labl < lablSize; labl++) {
            vector<int> lablVec;
            colCounts.at(col).push_back(lablVec);
            int valSize = colVals.at(col).size(); // this is the variable that might change per column

            for (int val = 0; val < valSize; val++) {
                colCounts.at(col).at(labl).push_back(0);
            }
        }
    }

    /*
    We will now use colCounts to count how many times each column value shows up
    This will then be used to calculate column scores
    */

    // so we need to loop over the rows of data, then over columns within rows
    for (int row = 0; row < data.size(); row++) {

        // -1 here because we don't want the counts for the labels column
        for (int col = 0; col < data.at(row).size() - 1; col++) {
            int val = data.at(row).at(col);

            // get index of val within colVals
            // TODO: utility function for finding index of element in a vector of ints
            int valIdx;
            for (int potValIdx = 0; potValIdx < colVals.at(col).size(); potValIdx++) {
                if (colVals.at(col).at(potValIdx) == val) {
                    valIdx = potValIdx;
                }
            }

            // look ahead in row to grab label
            int lbl = data.at(row).at(data.at(row).size() - 1);

            // get index of label within colVals
            int lblIdx;
            for (int potLblIdx = 0; potLblIdx < colVals.at(colVals.size() - 1).size(); potLblIdx++) {
                if (colVals.at(colVals.size() - 1).at(potLblIdx) == lbl) {
                    lblIdx = potLblIdx;
                }
            }

            // increment corresponding index in colCounts
            // VS throws an "uninitialized" error here,
            // even if I initialize the second two indices...
            colCounts.at(col).at(lblIdx).at(valIdx)++;
        }
    }

    /*
    Now we can use colCounts to calculate the scores for each column.
    */
    vector<double> colScores;

    /*
    The score of a column is the sum of the max of each valCounts vector
    for every index of the labelCounts vector over the number of active rows
    ...
    see the documentation for a better explanation
    */
    int numRows = data.size();
    for (int col = 0; col < colCounts.size(); col++) {
        int maxSum = 0;
        for (int lbl = 0; lbl < colCounts.at(col).size(); lbl++) {
            // find max of valCounts
            int max = 0;

            for (int val = 0; val < colCounts.at(col).at(lbl).size(); val++) {
                if (colCounts.at(col).at(lbl).at(val) > max) {
                    max = colCounts.at(col).at(lbl).at(val);
                }
            }

            // add it to maxSum
            maxSum += max;

        }

        // push score for column onto colScores
        colScores.push_back(double(maxSum) / double(numRows));
    }
    
    /*
    Finally, we return the index of the maximum element in colScores...
    I.e., the column to split on.
    */

    // find the max element of colScores
    double max = 0;
    int maxIdx;
    for (int scridx = 0; scridx < colScores.size(); scridx++) {
        if (colScores.at(scridx) > max) {
            max = colScores.at(scridx);
            maxIdx = scridx;
        }
    }

    return maxIdx;
}

/*
Set binary mask given current binary mask and new column to "split" on
- a binary mask is used to represent splits of data
- first vector is indices of ommitted columns
- second vector is indices of ommitted rows
- this function is called before a col_to_split_on call
*/
vector<vector<int>> decisionTree::gen_bin_mask(int col, int val, vector<vector<int>> binMask) {
    // when function is first called, binMask will be empty.
    // push back two empty vectors onto it to be filled with ommitted columns and rows respectively.
    if (binMask.empty()) {
        vector<int> ommittedCols;
        binMask.push_back(ommittedCols);
        vector<int> ommittedRows;
        binMask.push_back(ommittedRows);
    }

    // since we've already scored for column col, it gets ommitted
    binMask.at(0).push_back(col);

    // any row that has a val in column col NOT equal to val gets omitted
    for (int row = 0; row < data.size(); row++) {
        if (data.at(row).at(col) != val) {
            binMask.at(1).push_back(row);
        }
    }

    return binMask;
}