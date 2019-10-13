/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "p2Json.h"

using namespace std;

/**********
*  Json  *
**********/

bool Json::read(const string& jsonFile) {

	ifstream inf( jsonFile );

	string key, valueStr; 
	int value;

	inf >> key;

	while ( inf >> key >> valueStr >> valueStr ) {

		// get the key
		key = key.substr( 1, key.size()-2 );

		//check if there's a comma in the value string
		bool comma = true;
		if ( valueStr.find( ',' ) != string::npos ) {
			// if there's a comma, delete it
			valueStr = valueStr.substr( 0, valueStr.size()-1 );
			comma = false;
		}
		// convert the value to integer
		value = stoi( valueStr );

		// creat and add the new element to the JsonElem vector
		JsonElem newJsonElem( key, value );
		_obj.push_back( newJsonElem );

		// read the comma if it hasn't been read
		if( comma ) inf >> key;
	}
	return true;
}
void Json::print() {

	const int len = _obj.size();
	cout << '{' << endl;

	for ( int i=0; i < len; ++i ) {
		cout << "  " << _obj[i];
		if ( i != len - 1 ) cout << ',';
		cout << endl;
	}
	cout << '}' << endl;
}
bool Json::empty() {
	if ( _obj.empty() ) {
		cout << "Error: No element found!!" << endl;
		return true;
	}
	return false;
}
int Json::sum() {
	vector<JsonElem>::iterator it = _obj.begin(), ter = _obj.end();
	int sum=0;
	for ( ; it != ter; it++ ) {
		sum += it -> getValue();
	}
	return sum;
}
double Json::ave() {
		return ( (double)sum() ) / _obj.size();
}
JsonElem Json::max() {
	vector<JsonElem>::iterator it = _obj.begin(), ter = _obj.end(), result = it;
	int max = it -> getValue();
	for ( ; it != ter; it++ ) {
		if ( it -> getValue() > max ) {
			max = it -> getValue();
			result = it;
		}	
	}
	return *result;
}
JsonElem Json::min() {
	vector<JsonElem>::iterator it = _obj.begin(), ter = _obj.end(), result = it;
	int min = it -> getValue();
	for ( ; it != ter; it++ ) {
		if ( it -> getValue() < min ) {
			min = it -> getValue();
			result = it;
		}
	}
	return *result;
}
void Json::add( const string &key, const int &value ) {
	JsonElem newJsonElem( key, value );
	_obj.push_back( newJsonElem );
}
ostream& operator << (ostream& os, const JsonElem& j) {
	return (os << "\"" << j._key << "\" : " << j._value);
}

