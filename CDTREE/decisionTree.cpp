#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "vector"
using namespace std;


vector<vector<int>> get_data(std::string filename)
{
    // initialize stuff
    vector<vector<int>> data;
    string line;
    ifstream myfile(filename);

    // convert each line to a vector of ints
    // then load that vector into data
    // eventually return vector of data
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
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

                // If the next token is a comma, ignore it and move on
                if (ss.peek() == ',') ss.ignore();

                // If the next token is a negative sign, ignore but save it
                if (ss.peek() == '-') {
                    neg = true;
                    ss.ignore();
                }
            }
            // push row into data vector
            data.push_back(row);
        }
        myfile.close();
    }

    else cout << "Unable to open file";

    return data;
}