#pragma once

#include <ctype.h>
#include "../types.c"
#include "../raylib.h"
#include "../utils/arena.c"
#include "../utils/timer.c"

typedef struct Console
{
	u32 m_cursorPos;
	char m_typedText[100];
	Timer m_inputHoldTimer;
	bool m_drawable : 1;
} Console;

void ConsoleDrawDebug(Rectangle _container)
{
	DrawRectangleLinesEx(_container, 1.f, RED);	 
}

void ConsoleDrawText(Font font, const char* text, Vector2 position, float fontSize, float spacing, Color tint, u32 _cursorPos)
{
	if (font.texture.id == 0) font = GetFontDefault();  // Security check in case of not valid font

	int size = TextLength(text);    // Total size in bytes of the text, scanned by codepoints in loop
	int textLineSpacing = 2;
	float textOffsetY = 0;          // Offset between lines (on linebreak '\n')
	float textOffsetX = 0.0f;       // Offset X to next character to draw

	float scaleFactor = fontSize / font.baseSize;         // Character quad scaling factor

	for (int i = 0; i < size;)
	{
		// Get next codepoint from byte string and glyph index in font
		int codepointByteCount = 0;
		int codepoint = GetCodepointNext(&text[i], &codepointByteCount);
		int index = GetGlyphIndex(font, codepoint);

		// Draw Cursor
		if (i == _cursorPos)
		{
			DrawRectangle(position.x + textOffsetX, position.y + textOffsetY, 2, fontSize, ORANGE);
		}

		if (codepoint == '\n')
		{
			// NOTE: Line spacing is a global variable, use SetTextLineSpacing() to setup
			textOffsetY += (fontSize + textLineSpacing);
			textOffsetX = 0.0f;
		}
		else
		{
			if ((codepoint != ' ') && (codepoint != '\t'))
			{
				DrawTextCodepoint(font, codepoint, (Vector2) { position.x + textOffsetX, position.y + textOffsetY }, fontSize, tint);
			}

			if (font.glyphs[index].advanceX == 0) textOffsetX += ((float)font.recs[index].width * scaleFactor + spacing);
			else textOffsetX += ((float)font.glyphs[index].advanceX * scaleFactor + spacing);
		}

		i += codepointByteCount;   // Move text bytes counter to next codepoint
	}

	// Draw Cursor if exceed the length of the text
	if (_cursorPos >= size)
	{
		DrawRectangle(position.x + textOffsetX, position.y + textOffsetY, 2, fontSize, ORANGE);
	}
}

void ConsoleDrawCursor(Rectangle _container, const char* _text, Font _font, int _fontSize, u32 _cursorPos)
{
	const Vector2 position = { .x = _container.x, .y = _container.y };
	ConsoleDrawText(_font, _text, position, _fontSize, 1, WHITE, _cursorPos);
}

void ConsoleDrawInputBox(Rectangle _container, Font _font, int _fontSize)
{
	Color dark = { .r = 30, .g = 30, .b = 30, .a = 255 };
	Color light = dark;
	light.a = 190;
	DrawRectangleGradientV(_container.x, _container.y, _container.width, _container.height, dark, light);
}

void ConsoleDrawContainer(Rectangle _container, u32 _cursorPos, const char* _text)
{
	Color dark = { .r = 30, .g = 30, .b = 30, .a = 190 };
	Color light = dark;
	light.a = 100;
	DrawRectangleGradientEx(_container, light, dark, dark, light);

	Font defaultFont = GetFontDefault();
	int fontSize = 16;

	Rectangle containerInput = _container;
	containerInput.y += _container.height - fontSize;
	containerInput.height = fontSize;
	ConsoleDrawInputBox(containerInput, defaultFont, fontSize);

	ConsoleDrawCursor(containerInput, _text, defaultFont, fontSize, _cursorPos);
}

void ConsoleRemoveCharAtCursor(char* _text, const u32 _textLen, const u32 _cursor)
{
	if (_cursor == 0)
		return;

	if (_textLen == 0)
		return;

	if (_cursor >= _textLen)
		return;

	for (u32 i = _cursor - 1; i < _textLen - 1; ++i)
	{
		_text[i] = _text[i + 1];
	}

	// Add new \0 at the end, to mark the new end character
	_text[_textLen - 1] = '\0';
}

void ConsoleInsertCharAtCursor(char* _text, const u32 _textLen, const u32 _textCapacity, const u32 _cursor, const char _character)
{
	if (_textLen >= _textCapacity)
		return;

	if (_cursor > _textLen)
		return;

	if (_cursor >= _textCapacity)
		return;

	if (_textLen > 0)
	{
		// Shift the characters to the right
		for (u32 i = _textLen; i >= _cursor; --i)
		{
			_text[i] = _text[i - 1];
		}
	}

	// Snsert the character
	_text[_cursor] = _character;

	// Add new \0 at the end, to mark the new end character
	_text[_textLen + 1] = '\0';
}

void ConsoleRender(const Console* _console, const f32 _dt)
{
	if (_console->m_drawable == false)
		return;

	Rectangle container;
	container.x = 0.f;
	container.y = 0.f;
	container.width = GetScreenWidth();
	container.height = GetScreenHeight() / 2.f;
	ConsoleDrawContainer(container, _console->m_cursorPos, _console->m_typedText);
}

void ConsoleUpdate(Console* _current, const f32 _dt)
{
	bool inputHold = false;

	if (IsKeyPressed(KEY_F2))
	{
		_current->m_drawable = !_current->m_drawable;
		strncpy_s(_current->m_typedText, sizeof(_current->m_typedText), "test aussi test", 16);
	}

	if (IsKeyPressed(KEY_RIGHT))
	{
		if (_current->m_cursorPos < TextLength(_current->m_typedText))
			_current->m_cursorPos += 1;
	}
	else if (IsKeyPressed(KEY_LEFT))
	{
		if (_current->m_cursorPos > 0)
			_current->m_cursorPos -= 1;
	}
	else if (IsKeyPressed(KEY_BACKSPACE))
	{
		const u32 textLength = TextLength(_current->m_typedText);
		if (textLength > 0 && _current->m_cursorPos > 0)
		{
			const char* begin2 = _current->m_typedText + _current->m_cursorPos;

			if (begin2[0] == '\0')
			{
				_current->m_typedText[_current->m_cursorPos - 1] = '\0';
			}
			else
			{
				ConsoleRemoveCharAtCursor(_current->m_typedText, textLength, _current->m_cursorPos);
			}
		}

		if (_current->m_cursorPos > 0)
			_current->m_cursorPos -= 1;
	}
	else if (IsKeyPressed(KEY_SPACE))
	{
		const u32 textLength = TextLength(_current->m_typedText);
		const u32 capacity = sizeof(_current->m_typedText) / sizeof(_current->m_typedText[0]);
		ConsoleInsertCharAtCursor(_current->m_typedText, textLength, capacity, _current->m_cursorPos, ' ');
		
		if (_current->m_cursorPos < capacity)
			_current->m_cursorPos += 1;

		inputHold = true;
	}

	const i32 charPressed = GetCharPressed();
	if (charPressed == 0)
		return;

	const i32 charToLower = tolower(charPressed);

	{
		const u32 textLength = TextLength(_current->m_typedText);
		const u32 capacity = sizeof(_current->m_typedText) / sizeof(_current->m_typedText[0]);
		ConsoleInsertCharAtCursor(_current->m_typedText, textLength, capacity, _current->m_cursorPos, charToLower);
		
		if (_current->m_cursorPos < capacity)
			_current->m_cursorPos += 1;
	}

}