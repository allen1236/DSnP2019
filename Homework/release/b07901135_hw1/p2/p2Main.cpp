/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include "p2Json.h"

using namespace std;

int main()
{
	Json json;

   // Read in the csv file. Do NOT change this part of code.
	string jsonFile;
	cout << "Please enter the file name: ";
	cin >> jsonFile;
	if (json.read(jsonFile))
		cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
	else {
		cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
		exit(-1); // jsonFile does not exist.
	}

	// TODO read and execute commands
	cout << "Enter command: ";
	while (true) {

		string cmd;
		cin >> cmd;
		if ( cmd == "PRINT" ) {
			json.print();
		} else if ( cmd == "SUM" ) {
			if ( ! json.empty() ) 
				cout << "The summation of the values is: " << json.sum() << "." << endl;
		} else if ( cmd == "AVE" ) {
			if ( ! json.empty() ) 
				cout << "The average of the values is: " << fixed << setprecision(1) << json.ave() << "." << endl;
		} else if ( cmd == "MAX" ) {
			if ( ! json.empty() )
				cout << "The maximum element is: " << json.max() << "." << endl;
		} else if ( cmd == "MIN"  ) {
			if ( ! json.empty() ) 
				cout << "The minimum element is: " << json.min() << "." << endl;
		} else if ( cmd == "ADD" ) {
			string key, valueStr, line;
			getline( cin, line );
			stringstream ss;
			ss << line;
			if( ss >> key >> valueStr ){
				json.add( key, stoi(valueStr) );
			}
		} else if ( cmd == "EXIT" ) break;
		cout << "Enter command: ";
	}
	return 0;
}
