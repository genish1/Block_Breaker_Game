#include "G2D.h"

const Color Color::Black = Color(0, 0, 0);
const Color Color::White = Color(1, 1, 1);
const Color Color::Red = Color(1, 0, 0);
const Color Color::Green = Color(0, 1, 0);
const Color Color::Blue = Color(0, 0, 1);
const Color Color::Magenta = Color(1, 0, 1);
const Color Color::Cyan = Color(0, 1, 1);
const Color Color::Yellow = Color(1, 1, 0);
const Color Color::Gray = Color(0.5, 0.5, 0.5);


Color ColorFrom255(int r, int g, int b)
{
	return Color(r / 255.0, g / 255.0, b / 255.0);
}


Color ColorFromHex(int hexValue)
{
	float r = ((hexValue >> 16) & 0xFF) / 255.0;  // Extract the RR byte
	float g = ((hexValue >> 8) & 0xFF) / 255.0;   // Extract the GG byte
	float b = ((hexValue) & 0xFF) / 255.0;        // Extract the BB byte
	return Color(r, g, b);
}

