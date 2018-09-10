/**
@file
Contains the LiquidScreen class definition.
*/

/*
The MIT License (MIT)

Copyright (c) 2016 Vasil Kalchev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "LiquidMenu.h"

LiquidScreen::LiquidScreen()
	: _lineCount(0), _focus(0), _hidden(false) {}

LiquidScreen::LiquidScreen(LiquidLine &liquidLine)
	: LiquidScreen() {
	add_line(liquidLine);
	_maxLineDisplay = 1;
}

LiquidScreen::LiquidScreen(LiquidLine &liquidLine1, LiquidLine &liquidLine2)
	: LiquidScreen(liquidLine1) {
	add_line(liquidLine2);
	_maxLineDisplay = 2;
}

LiquidScreen::LiquidScreen(LiquidLine &liquidLine1, LiquidLine &liquidLine2,
                           LiquidLine &liquidLine3)
	: LiquidScreen(liquidLine1, liquidLine2) {
	add_line(liquidLine3);
	_maxLineDisplay = 3;
}

LiquidScreen::LiquidScreen(LiquidLine &liquidLine1, LiquidLine &liquidLine2,
                           LiquidLine &liquidLine3, LiquidLine &liquidLine4)
	: LiquidScreen(liquidLine1, liquidLine2, liquidLine3) {
	add_line(liquidLine4);
	_maxLineDisplay = 4;
}

bool LiquidScreen::add_line(LiquidLine &liquidLine) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (_lineCount < MAX_LINES) {
		_p_liquidLine[_lineCount] = &liquidLine;
		DEBUG(F("Added a new line (")); DEBUG(_lineCount); DEBUGLN(F(")"));
		_lineCount++;
		_focus++;
		return true;
	}
	_maxLineDisplay = _lineCount;
	DEBUG(F("Adding line ")); DEBUG(_lineCount);
	DEBUG(F(" failed, edit LiquidMenu_config.h to allow for more lines"));
	return false;
}

bool LiquidScreen::set_focusPosition(Position position) {
	print_me(reinterpret_cast<uintptr_t>(this));
	if (position == Position::CUSTOM) {
		DEBUGLN(F("Can't set focus position to 'CUSTOM' for the whole screen at once"));
		return false;
	} else {
		DEBUG(F("Focus position set to : ")); DEBUGLN((uint8_t)position);
		for (uint8_t l = 0; l < _lineCount; l++) {
			_p_liquidLine[l]->set_focusPosition(position);
		}
		return true;
	}
}

void LiquidScreen::set_max_line_display(uint8_t lines)
{
	_maxLineDisplay = lines;
}

void LiquidScreen::hide(bool hide) {
	_hidden = hide;
}

void LiquidScreen::print(DisplayClass *p_liquidCrystal) const {
	uint8_t lOffset = 0;
	uint8_t lineDisplayCount = _maxLineDisplay;
	if (lineDisplayCount == 0)
		lineDisplayCount = _lineCount;
	else if (lineDisplayCount > _lineCount)
		lineDisplayCount = _lineCount;
	DEBUG("MaxLine: ");
	DEBUG(lineDisplayCount);
	DEBUG("\n");

	if (_focus >= lineDisplayCount)
	{
		lOffset = (_focus - lineDisplayCount) + 1;
		if ((lineDisplayCount + lOffset) > _lineCount)
			lOffset = (_lineCount - lineDisplayCount);
	}
	uint8_t offsetRow = 0;
	for (uint8_t l = lOffset; l < lineDisplayCount + lOffset; l++) {
		bool focus = true;
		if (_focus != l) {
			focus = false;
			DEBUG(F("|"));
		} else {
			DEBUG(F("|   -->"));
		}
		DEBUG(F("\tLine ")); DEBUG(l);
		if (lineDisplayCount < _lineCount)
			_p_liquidLine[l]->_row = offsetRow;
		_p_liquidLine[l]->print(p_liquidCrystal, focus);
		offsetRow++;
	}
}

void LiquidScreen::switch_focus(bool forward) {
	print_me(reinterpret_cast<uintptr_t>(this));
	do {
		if (forward) {
			if (_focus < _lineCount) {
				_focus++;
				if (_focus == _lineCount) {
					break;
				}
			} else {
				_focus = 0;
			}
		} else { //else (forward)
			if (_focus == 0) {
				_focus = _lineCount;
				break;
			} else {
				_focus--;
			}
		} //else (forward)
	} while (_p_liquidLine[_focus]->_focusable == false);
	DEBUG(F("Focus switched to ")); DEBUGLN(_focus);
}

bool LiquidScreen::call_function(uint8_t number) const {
	if (_focus != _lineCount) {
		return _p_liquidLine[_focus]->call_function(number);
	}
	return false;
}
