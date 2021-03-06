#ifndef __LIB_COLOR_H__
#define __LIB_COLOR_H__
#include "stdafx.h"

#define COLOR_ARGB(a, r, g, b) (((a)<<24)+((r)<<16)+((g)<<8)+(b))
#define COLOR_ALPHA(color) (((color)>>24)&0xFF)
#define COLOR_RED(color) (((color)>>16)&0xFF)
#define COLOR_GREEN(color) (((color)>>8)&0xFF)
#define COLOR_BLUE(color) ((color)&0xFF)

namespace lib {
class Color {
public:
	Color(void): m_r(0),m_g(0),m_b(0),m_a(0) {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a): m_r(r),m_g(g),m_b(b),m_a(a) {}
	Color(uint8_t r, uint8_t g, uint8_t b): m_r(r),m_g(g),m_b(b),m_a(0) {}
	Color(const Color &color) {
		SetColor(color);
	}
	~Color(void) {}

	void SetColor(const Color &color)
	{
		m_r = color.GetR();
		m_g = color.GetG();
		m_b = color.GetB();
		m_a = color.GetA();
	}
	void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}
	void SetColor(uint8_t r, uint8_t g, uint8_t b)
	{
		m_r = r;
		m_g = g;
		m_b = b;
	}
	void SetR(uint8_t r) { m_r = r; }
	void SetG(uint8_t g) { m_g = g; }
	void SetB(uint8_t b) { m_b = b; }
	void SetA(uint8_t a) { m_a = a; }

	uint8_t GetR() const { return m_r; }
	uint8_t GetG() const { return m_g; }
	uint8_t GetB() const { return m_b; }
	uint8_t GetA() const { return m_a; }

private:
	uint8_t m_r;
	uint8_t m_g;
	uint8_t m_b;
	uint8_t m_a;
};
}	// namespace lib
#endif // __LIB_COLOR_H__