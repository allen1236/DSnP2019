/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
 ****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//#define DEBUG

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream&);
ParseChar getChar(istream&);

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void
CmdParser::readCmd()
{
	if (_dofile.is_open()) {
		readCmdInt(_dofile);
		_dofile.close();
	}
	else
		readCmdInt(cin);
}

void
CmdParser::readCmdInt(istream& istr)
{
	resetBufAndPrintPrompt();

	while (1) {
		ParseChar pch = getChar(istr);
		if (pch == INPUT_END_KEY) break;
		switch (pch) {
			case LINE_BEGIN_KEY :
			case HOME_KEY       : moveBufPtr(_readBuf); break;
			case LINE_END_KEY   : 
			case END_KEY        : moveBufPtr(_readBufEnd); break;
			case BACK_SPACE_KEY : if ( moveBufPtr(_readBufPtr-1) ) deleteChar();
								  else mybeep();
								  break;
			case DELETE_KEY     : deleteChar(); break;
			case NEWLINE_KEY    : addHistory();
								  moveBufPtr(_readBufEnd);
								  cout << char(NEWLINE_KEY);
								  resetBufAndPrintPrompt(); break;
			case ARROW_UP_KEY   : moveToHistory(_historyIdx - 1); break;
			case ARROW_DOWN_KEY : moveToHistory(_historyIdx + 1); break;
			case ARROW_RIGHT_KEY: if ( ! moveBufPtr( _readBufPtr + 1 ) ) mybeep(); break;
			case ARROW_LEFT_KEY : if ( _readBufPtr != _readBuf ) moveBufPtr( _readBufPtr - 1 );
								  else mybeep();
								  break;
			case PG_UP_KEY      : moveToHistory(_historyIdx - PG_OFFSET); break;
			case PG_DOWN_KEY    : moveToHistory(_historyIdx + PG_OFFSET); break;
			case TAB_KEY        : {
								  int num_of_space = TAB_POSITION - ( _readBufPtr - _readBuf ) % TAB_POSITION;
								  if ( num_of_space == 0 ) num_of_space = TAB_POSITION;
								  while( num_of_space -- ) insertChar(' ');
								  }
								  break;
			case INSERT_KEY     : // not yet supported; fall through to UNDEFINE
			case UNDEFINED_KEY:   mybeep(); break;
			default:  // printable character
								  insertChar(char(pch)); break;
		}
#ifdef TA_KB_SETTING
		taTestOnly();
#endif
	}
}


// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool
CmdParser::moveBufPtr(char* const ptr)
{
	if ( ptr > _readBufEnd || ptr < _readBuf ) return false;
	int distance = ptr - _readBufPtr;
	if ( distance < 0 ) {
		while ( distance ++ ) {
			cout << '\b';
			_readBufPtr --;
		}
	} else {
		while ( distance --  ) {
			cout << *_readBufPtr;
			_readBufPtr ++;
		}
	}
	_readBufPtr = ptr;
	return true;
}


// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool
CmdParser::deleteChar() {
	if ( _readBufPtr == _readBufEnd ) {
		mybeep();
		return false;
	} else {
		strcpy( _readBufPtr, _readBufPtr + 1 );
		cout << _readBufPtr;
		cout << ' ';
		int len = _readBufEnd - _readBufPtr; 
		_readBufEnd --;
		while( len -- ) cout << '\b';
		return true;
	}
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void
CmdParser::insertChar(char ch, int repeat) {
	assert(repeat >= 1);

	strcpy( _readBufPtr+repeat, _readBufPtr );

	_readBufEnd += repeat;

	while( repeat-- ) {
		*(_readBufPtr++) = ch;
		cout << ch;
	}
	cout << _readBufPtr;

	int len = strlen( _readBufPtr );
	while( len-- ) cout << '\b';
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^      https://zh.wikipedia.org/wiki/%E8%87%AA%E7%94%B1%E5%BA%A6_(%E7%BB%9F%E8%AE%A1%E5%AD%A6)          (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void
CmdParser::deleteLine() {
	int distance_to_ptr = _readBufPtr - _readBuf;
	while( distance_to_ptr -- ) cout << '\b';
	int len = _readBufEnd - _readBuf;
	for (int i = 0; i < len; ++i) cout <<  ' ';
	while( len -- ) cout << '\b';
	_readBufPtr = _readBufEnd = _readBuf;
	*_readBufPtr = 0;
}


// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
//
void
CmdParser::moveToHistory(int index) {
	if ( index < 0 ) index = 0;
	else if ( index >= (int)_history.size() ) {
		if ( _tempCmdStored ) index = _history.size() - 1;
		else return;
	}
	if ( !_tempCmdStored ) {
		_history.push_back( _readBuf );
		_tempCmdStored = true;
	}
	deleteLine();

	strcpy( _readBuf, _history[index].c_str() );
	cout << _readBuf;
	_readBufPtr = _readBufEnd = _readBuf + _history[index].size();

	if ( index == (int)_history.size() - 1 ) {
		_tempCmdStored = false;
		_history.pop_back();
	}
	_historyIdx = index;
}


// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether 
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void
CmdParser::addHistory() {
	string cmd = _readBuf;
	if ( cmd.find_first_not_of( ' ' ) != string::npos ) {
		cmd.erase( 0, cmd.find_first_not_of( ' ' ) );
		cmd.erase( cmd.find_last_not_of( ' ' ) + 1 );
		if ( _tempCmdStored ) {
			_history[_history.size()-1] = cmd;
			_tempCmdStored = false;
		} else _history.push_back( cmd );
		_historyIdx = _history.size();
	} else if ( _tempCmdStored ) {
		_tempCmdStored = false;
		_history.pop_back();
		_historyIdx = _history.size();
	}

#ifdef DEBUG
	cout << endl << "history: " << endl;
	for (int i = 0; i < (int)_history.size(); ++i) {
		cout << _history[i] << endl;
	}
#endif
}


// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void
CmdParser::retrieveHistory() {
	strcpy(_readBuf, _history[_historyIdx].c_str());
	cout << _readBuf;
	_readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx].size();
}
