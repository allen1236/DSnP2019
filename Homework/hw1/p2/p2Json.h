/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem {
public:
	JsonElem() {}
	JsonElem(const string& k, int v): _key(k), _value(v) {}

	friend ostream& operator << (ostream&, const JsonElem&);
	string getKey() { return _key; }
	int getValue() { return _value; }

private:
	string  _key;   // DO NOT change this definition. Use it to store key.
	int     _value; // DO NOT change this definition. Use it to store value.
};

class Json {
public:
	bool read(const string&);

	void print();
	bool empty();
	int sum();
	double ave();
	JsonElem max();
	JsonElem min();
	void add( const string&, const int& );

private:
	vector<JsonElem>       _obj;  // DO NOT change this definition.
	                              // Use it to store JSON elements.
	int _size;
	
};

#endif // P2_TABLE_H
