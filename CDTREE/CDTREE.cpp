// CDTREE.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <queue> 
#include "decisionTree.h"
#include "vector"
using namespace std;

int main()
{
    bool recursiveTest = true;
    bool printTest = false;

    if (recursiveTest) {
        // read in data
        string filename = "C:/Users/Berett/Projects/CDTREE/Data/toyDataStringColumnsBinLabels.csv";
        decisionTree myDecTree(filename);

        // build decision tree
        myDecTree.trainTree();

        // printing test
        if (printTest) {
            queue<decisionTree::Node*> q; // Create a queue 
            q.push(&myDecTree.root); // Enqueue root
            while (!q.empty())
            {
                // for all nodes currently in the queue,
                // print their values and enque their children
                int n = q.size();
                while (n > 0)
                {
                    // Dequeue node from front of queue
                    decisionTree::Node* p = q.front();
                    q.pop();
                    cout << p->colVal << "," << p->colSplitOn << "," << p->guess << "    ";

                    // Enqueue all children of the dequeued item 
                    for (int childIdx = 0; childIdx < p->children.size(); childIdx++)
                        q.push(p->children.at(childIdx));

                    n--;
                }
                cout << endl; // Print new line between two levels 
            }
        }

        // sing row prediction test
        vector<int> datapoint1 = { 0, 2, 1, 0, 0 }; // should set pred1 to 4
        vector<int> datapoint2 = { 0, 1, 1, 1, 1 }; // should set pred2 to 3
        vector<int> datapoint3 = { 0, 1, 1, 2, 1 }; // should set pred3 to 4
        vector<int> datapoint4 = { 0, 1, 1, 0, 2 }; // should set pred4 to random guess
        int pred1 = myDecTree.predictSingleDataPoint(datapoint1);
        int pred2 = myDecTree.predictSingleDataPoint(datapoint2);
        int pred3 = myDecTree.predictSingleDataPoint(datapoint3);
        int pred4 = myDecTree.predictSingleDataPoint(datapoint4);

        // multi row prediction test
        vector<vector<int>> testData;
        testData.push_back(datapoint1);
        testData.push_back(datapoint2);
        testData.push_back(datapoint3);
        testData.push_back(datapoint4);
        vector<int> preds = myDecTree.predictOnDataSet(testData);

    }
    else {
        // non-recursive training loop driver
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

        // we now have all the information we need to set the root node of our decision tree
        myDecTree.root.colSplitOn = col0;

        // for non-recursive run root will have two children
        decisionTree::Node child0;
        decisionTree::Node child1;

        // int to store second column to split on
        // should be 1 - "ai"
        int col1;

        // int to store guess from first split
        // should be 76 - "L"
        int guess0;

        // cycle through all colVals for found column
        for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col0).size(); colValIdx++) {
            // generate appropriate mask for this child
            intermMask0 = myDecTree.gen_bin_mask(col0, myDecTree.colVals.at(col0).at(colValIdx), initMask);
            int guess = myDecTree.is_base_case(intermMask0);

            if (guess == NULL) {
                col1 = myDecTree.col_to_split_on(intermMask0);

                // set properties of child 1 and push a pointer to it onto the root's children
                child1.colVal = myDecTree.colVals.at(col0).at(colValIdx);
                child1.colSplitOn = col1;
                myDecTree.root.children.push_back(&child1);

                // save this version of intermMask for use in next for loop
                intermMask1 = intermMask0;
            }
            else {
                // set properties of child 0 and push a pointer to it onto the root's children
                child0.colVal = myDecTree.colVals.at(col0).at(colValIdx);
                child0.guess = guess;
                myDecTree.root.children.push_back(&child0);

                // save this guess for next level of tree
                guess0 = guess;
            }
        }

        /*
        We should now have a root node with two childen: child0 and child1
        child0 should be a base case node with guess value 76
        child1 should be a common node with colSplitOn value of 1
        */
        int testGuess0;
        int testColSplitOn0;
        // loop through children of root
        for (int childIdx = 0; childIdx < myDecTree.root.children.size(); childIdx++) {
            int tmpColVal = myDecTree.root.children.at(childIdx)->colVal;
            if (tmpColVal == 110) { // if child's colVal is 'n',
                testGuess0 = myDecTree.root.children.at(childIdx)->guess; // we should see '76' here
            }
            else if (tmpColVal == 121) { // if child's colVal is 'y',
                testColSplitOn0 = myDecTree.root.children.at(childIdx)->colSplitOn; // we should see '1' here
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

        // for non-recursive run, child1 will have two children
        decisionTree::Node child2;
        decisionTree::Node child3;

        // loop through children of child1
        for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col1).size(); colValIdx++) {
            // generate appropriate mask for this child
            intermMask2 = myDecTree.gen_bin_mask(col1, myDecTree.colVals.at(col1).at(colValIdx), intermMask1);
            int guess = myDecTree.is_base_case(intermMask2);

            if (guess == NULL) {
                col2 = myDecTree.col_to_split_on(intermMask2);

                // set properties of child3 and push a pointer to it onto the root's children
                child3.colVal = myDecTree.colVals.at(col1).at(colValIdx);
                child3.colSplitOn = col2;
                child1.children.push_back(&child3);

                intermMask3 = intermMask2;
            }
            else {
                // set properties of child2 and push a pointer to it onto the root's children
                child2.colVal = myDecTree.colVals.at(col1).at(colValIdx);
                child2.guess = guess;
                child1.children.push_back(&child2);

                guess1 = guess;
            }
        }

        /*
        child1 should now have two childen: child2 and child3
        child2 should be a base case node with guess value 76
        child3 should be a common node with colSplitOn value of 1
        */
        int testGuess1;
        int testColSplitOn1;
        // loop through children of root
        for (int childIdx = 0; childIdx < child1.children.size(); childIdx++) {
            int tmpColVal = child1.children.at(childIdx)->colVal;
            if (tmpColVal == 110) { // if child's colVal is 'n',
                testGuess1 = child1.children.at(childIdx)->guess; // we should see '68' here
            }
            else if (tmpColVal == 121) { // if child's colVal is 'y',
                testColSplitOn1 = child1.children.at(childIdx)->colSplitOn; // we should see '4' here
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

        // for non-recursive run, child3 will have two children
        decisionTree::Node child4;
        decisionTree::Node child5;

        // add a new child node to root for each value of col2
        for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col2).size(); colValIdx++) {
            // generate appropriate mask for this child
            intermMask4 = myDecTree.gen_bin_mask(col2, myDecTree.colVals.at(col2).at(colValIdx), intermMask3);
            int guess = myDecTree.is_base_case(intermMask4);

            if (guess == NULL) {
                col3 = myDecTree.col_to_split_on(intermMask4);

                // set properties of child 5 and push a pointer to it onto child3's children
                child5.colVal = myDecTree.colVals.at(col2).at(colValIdx);
                child5.colSplitOn = col3;
                child3.children.push_back(&child5);

                intermMask5 = intermMask4;
            }
            else {
                // set properties of child4 and push a pointer to it onto the root's children
                child4.colVal = myDecTree.colVals.at(col2).at(colValIdx);
                child4.guess = guess;
                child3.children.push_back(&child4);

                guess2 = guess;
            }
        }

        /*
        child3 should now have two childen: child4 and child5
        child4 should be a base case node with a random guess
        child5 should be a common node with colSplitOn value of 3
        */
        int testGuess2;
        int testColSplitOn2;
        // loop through children of root
        for (int childIdx = 0; childIdx < child3.children.size(); childIdx++) {
            int tmpColVal = child3.children.at(childIdx)->colVal;
            if (tmpColVal == 110) { // if child's colVal is 'n',
                testGuess2 = child3.children.at(childIdx)->guess; // we should see a random guess here
            }
            else if (tmpColVal == 121) { // if child's colVal is 'y',
                testColSplitOn2 = child3.children.at(childIdx)->colSplitOn; // we should see '3' here
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

        // for non-recursive run, child3 will have two children
        decisionTree::Node child6;
        decisionTree::Node child7;

        for (int colValIdx = 0; colValIdx < myDecTree.colVals.at(col3).size(); colValIdx++) {
            intermMask6 = myDecTree.gen_bin_mask(col3, myDecTree.colVals.at(col3).at(colValIdx), intermMask5);
            int guess = myDecTree.is_base_case(intermMask6);
            if (guess == NULL) {
                col4 = myDecTree.col_to_split_on(intermMask6);
                intermMask7 = intermMask6;
            }
            else {
                // use child6 as final "no" child
                if (myDecTree.colVals.at(col3).at(colValIdx) == 110) {
                    child6.colVal = myDecTree.colVals.at(col3).at(colValIdx);
                    child6.guess = guess;
                    child5.children.push_back(&child6);
                }
                // and child7 as final "yes" child
                else if (myDecTree.colVals.at(col3).at(colValIdx) == 121) {
                    child7.colVal = myDecTree.colVals.at(col3).at(colValIdx);
                    child7.guess = guess;
                    child5.children.push_back(&child7);
                }
                final_guesses.push_back(guess);
            }
        }

        /*
        child5 should now have two childen: child6 and child7
        child6 should be a base case node with guess '68'
        child7 should be a base case node with guess '76'
        */
        int testGuess3;
        int testGuess4;
        // loop through children of root
        for (int childIdx = 0; childIdx < child5.children.size(); childIdx++) {
            int tmpColVal = child5.children.at(childIdx)->colVal;
            if (tmpColVal == 110) { // if child's colVal is 'n',
                testGuess3 = child5.children.at(childIdx)->guess; // we should see '68' here
            }
            else if (tmpColVal == 121) { // if child's colVal is 'y',
                testGuess4 = child5.children.at(childIdx)->guess; // we should see '76' here
            }
        }

        /*
        We will now print the colVal, colSplitOn, and guess values for every node in the tree we just built
        */

        // Standard level order traversal code 
        // using queue 
        queue<decisionTree::Node> q; // Create a queue 
        q.push(myDecTree.root); // Enqueue root .. error here. why?
        while (!q.empty())
        {
            // for all nodes currently in the queue,
            // print their values and enque their children
            int n = q.size();
            while (n > 0)
            {
                // Dequeue node from front of queue
                decisionTree::Node p = q.front();
                q.pop();
                cout << p.colVal << "," << p.colSplitOn << "," << p.guess << "    ";

                // Enqueue all children of the dequeued item 
                for (int childIdx = 0; childIdx < p.children.size(); childIdx++)
                    q.push(*p.children.at(childIdx)); // ..error here.why ?

                n--;
            }

            cout << endl; // Print new line between two levels 
        }
    }
    
    return 0;
}