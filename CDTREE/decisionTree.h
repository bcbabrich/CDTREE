// decision tree class
// 

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <stdlib.h> // srand, rand
#include "vector"
using namespace std;

class decisionTree {

    // dataset
    // vector of int vectors representing data in the file.
    // filled in constructor
    vector<vector<int>> data;

public:
    /*
    tree node
    struct with three member variables:
    1. int column value.. refers to column split on by parent node
    -- null in root's case
    2. int column split on.. column this node splits on
    -- value doesn't matter if guess is not null
    3. int guess
    -- null unless node is a base case
    4. vector of node pointers called "children"
    -- pointers to all of the children of this node
    -- also doesn't matter if node is base case
    ...
    this will become private once training function is moved over here
    */
    struct Node
    {
        int colVal = NULL;
        int colSplitOn = NULL;;
        int guess = NULL;
        vector<Node*> children;
    };

    // always instantiate a root node
    // this might change when training loop is moved into decisionTree class...
    Node root;

    // all the possible values each column can have
    // filled in constructor
    // only public TEMPORARILY while using CDTREE.cpp as a training loop driver
    // TODO: make colVals private again when training function written
    vector<vector<int>> colVals;

    // all function descriptions provided at actual functions
    decisionTree(string);
    int col_to_split_on(vector<vector<int>> binMask); // make this private once training function is written
    vector<vector<int>> gen_bin_mask(int col, int val, vector<vector<int>> binMask);
    int is_base_case(vector<vector<int>> binMask);
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
- Takes in a binary mask. 
-- Does not increment values at values in mask.
- Called during training loop, operates on data.
- Outputs an int corresponding to a column index.
-- Assumptions:
--- data is square
--- labels for data are at the end of their rows
*/
int decisionTree::col_to_split_on(vector<vector<int>> binMask) {
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

            /*
            Now we will increment corresponding index in colCounts.
            But we must do so in accordance with the binary mask.
            */
            bool increm = true;

            // these following two for loops could be done with a "check if element is in vector" method, maybe...

            // check if current row is omitted by mask 
            if (!binMask.at(1).empty()) { // during the root splitting, mask will be empty
                for (int maskRowIdx = 0; maskRowIdx < binMask.at(1).size(); maskRowIdx++) {
                    if (row == binMask.at(1).at(maskRowIdx)) {
                        increm = false;
                    }
                }
            }

            // check if current column is omitted by mask
            if (!binMask.at(1).empty()) {
                for (int maskColIdx = 0; maskColIdx < binMask.at(0).size(); maskColIdx++) {
                    if (col == binMask.at(0).at(maskColIdx)) {
                        increm = false;
                    }
                }
            }
            

            // only increment values not in binary mask
            if (increm) {
                colCounts.at(col).at(lblIdx).at(valIdx)++;
            }
            
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
    // return that empty binMask
    if (binMask.empty()) {
        vector<int> ommittedCols;
        binMask.push_back(ommittedCols);
        vector<int> ommittedRows;
        binMask.push_back(ommittedRows);
        return binMask;
    }
    else {
        // since we've already scored for column col, it gets ommitted
        // but we don't want any duplicates in our omitted columns
        // this would mess up our indexing
        // so check if col is already in omitted columns before pushing
        // (we could use a set instead but that would also make indexing harder)
        bool colFound = false;
        for (int maskColIdx = 0; maskColIdx < binMask.at(0).size(); maskColIdx++) {
            if (binMask.at(0).at(maskColIdx) == col) {
                colFound = true;
            }
        }
        if (!colFound) {
            binMask.at(0).push_back(col);
        }

        // any row that has a val in column col NOT equal to val gets omitted
        for (int row = 0; row < data.size(); row++) {
            if (data.at(row).at(col) != val) {
                // identical logic as with columns above: we don't want any duplicates
                bool rowFound = false;
                for (int maskRowIdx = 0; maskRowIdx < binMask.at(1).size(); maskRowIdx++) {
                    if (binMask.at(1).at(maskRowIdx) == row) {
                        rowFound = true;
                    }
                }
                if (!rowFound) {
                    binMask.at(1).push_back(row);
                }
            }
        }
        return binMask;
    }
}

/*
Check if given binary bask creates a base case.
Returns NULL if no base case (default at bottom of function).
Returns int guess if base case (premature returns throughout function).
There are three known base cases.
Here they are in the order we check them:
- "no rows left": the mask eliminates all data values
-- Return a random guess from all labels
- "all labels equal": the mask leaves only one label value. 
-- Return that label value if found.
- "no columns left": all columns have been split on
-- count how many of each label there is left.
--- return the label with the maximum count
--- return a random guess from labels left if all labels counts are equal
*/
int decisionTree::is_base_case(vector<vector<int>> binMask) {
    // the quickest thing to check is the "no data case", so we do that first
    // there is an optimization to be made here... check your notebook
    if (binMask.at(1).size() == data.size()) {
        // in this base case, we return a random guess from all possible labels
        // first we need to generate a random index from the labels vector in colVals
        int lblInd = rand() % colVals.at(colVals.size() - 1).size();

        // then return the value at that index
        return colVals.at(colVals.size() - 1).at(lblInd);
    }

    // next, check for base case: single label left
    // at this point, we know that there are rows left

    // initialize observed labels as set because we only care about unique found labels
    set<int> observedLbls; 

    // initialize and empty label counts vector
    // - to be used in checking third base case
    vector<int> lblCounts;
    for (int lblIdx = 0; lblIdx < colVals.at(colVals.size() - 1).size(); lblIdx++) {
        lblCounts.push_back(0);
    }

    // iterate over all rows left visible by mask
    for (int row = 0; row < data.size(); row++) {
        int lbl = data.at(row).at(data.at(row).size() - 1); // legibility

        // check if row is in mask
        bool rowFound = false;
        for (int maskRowIdx = 0; maskRowIdx < binMask.at(1).size(); maskRowIdx++) {
            if (binMask.at(1).at(maskRowIdx) == row) {
                rowFound = true;
            }
        }

        // if it isn't, add row's label to observedLbls
        if (!rowFound) {
            observedLbls.insert(lbl);

            // additionally, get the index of that label in colVals
            // and increment that index in lblCounts
            if (!rowFound) {
                int lblValIdx;
                for (int lblIdx = 0; lblIdx < colVals.at(colVals.size() - 1).size(); lblIdx++) {
                    if (colVals.at(colVals.size() - 1).at(lblIdx) == lbl) {
                        lblValIdx = lblIdx;
                        // break could go here? as well as above in colCounts?
                    }
                }

                lblCounts.at(lblValIdx)++;
            }
        }
    }

    // if only one unique label was observed, our first base case was hit
    // so return the int that's at the beginning of observed labels,
    // i.e., the only element in the set
    if (observedLbls.size() == 1) {
        set<int>::iterator it = observedLbls.begin();
        return *it;
    }

    /*
    now we know there are at least two types of labels left
    the last case to check for is the "no columns left" case
    data being square is key here
    at this point we know there are rows left and at least two different types of label values
    */

    // -2 here because we don't want to include the label column or the single column that's left
    else if (binMask.at(0).size() == data.at(0).size() - 2) {
        // if there is an equal number of each label value type left over by mask,
        // we return a random guess from whatever labels are left

        /*
        To check if all non-zero lblCounts values are equal: 
        - cycle through all labels counts
        - the first non-zero count gets saved to omni count and the non-zero label counts vector
        - any other non-zero count gets saved to label counts vector as well
        - but if another non-zero count isn't equal to omni count, allEqual is set to false
        */
        int omniCount = -1;
        bool allEqual = true;
        vector<int> nonZeroLblCntIdxs; // save indices of non-zero lable counts
        for (int lblCntIdx = 0; lblCntIdx < lblCounts.size(); lblCntIdx++) {
            if (lblCounts.at(lblCntIdx) > 0 and omniCount == -1) {
                omniCount = lblCounts.at(lblCntIdx);
                nonZeroLblCntIdxs.push_back(lblCntIdx);
            }
            else if (lblCounts.at(lblCntIdx) > 0 && lblCounts.at(lblCntIdx) != omniCount) {
                allEqual = false;
                // break?
            }
            else if (lblCounts.at(lblCntIdx) > 0) {
                nonZeroLblCntIdxs.push_back(lblCntIdx);
            }
        }

        // if the counts were all equal, randomly choose one of the ones that was left
        if (allEqual) {
            // generate a random index into nonZeroLblCntIdxs
            int randInd = rand() % nonZeroLblCntIdxs.size();

            // get that index
            int lblIdx = nonZeroLblCntIdxs.at(randInd);

            // return label at lblIdx in cols
            return colVals.at(colVals.size() - 1).at(lblIdx);
        }
        // if the counts were not all equal, return the label with the maximum count
        else {
            // get index of max element
            int max = 0;
            int maxIdx;
            for (int cntIdx = 0; cntIdx < lblCounts.size(); cntIdx++) {
                if (lblCounts.at(cntIdx) > max) {
                    maxIdx = cntIdx;
                }
            }

            // return label at maxIdx in cols
            return colVals.at(colVals.size() - 1).at(maxIdx);
        }
    }

    // no base cases were hit
    return NULL;
}