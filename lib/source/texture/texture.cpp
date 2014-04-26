#include "stdafx.h"
#include <lib/texture/texture.h>

namespace lib {
namespace texture {
	
Texture::Texture()
: m_bCreated(false)
, m_hDC(NULL)
, m_hBitmap(NULL)
, m_width(0)
, m_height(0)
{
}

Texture::Texture(uint32_t width, uint32_t height)
: m_bCreated(false)
, m_hDC(NULL)
, m_hBitmap(NULL)
, m_width(0)
, m_height(0)
{
	create(width, height);
}

Texture::~Texture()
{
	destroy();
}

void Texture::create(uint32_t width, uint32_t height)
{
	destroy();
	// バッファ作成
	m_width = width;
	m_height = height;
	HDC hDc = GetDC(NULL);
	m_hDC = CreateCompatibleDC(hDc);
	m_hBitmap = CreateCompatibleBitmap(hDc, m_width, m_height);
	SelectObject(m_hDC, m_hBitmap);
	ReleaseDC(NULL, hDc);
	m_bCreated = true;
}

void Texture::destroy()
{
	if (m_bCreated) {
		DeleteDC(m_hDC);
		DeleteObject(m_hBitmap);
		m_width = 0;
		m_height = 0;
		m_bCreated = false;
	}
}

void Texture::clear(COLORREF color)
{
	if (m_bCreated) {
		HPEN hPen = CreatePen(PS_SOLID, 1, color);
		HBRUSH hBrush = CreateSolidBrush(color);
		SelectObject( m_hDC, hPen );
		SelectObject( m_hDC, hBrush );
		Rectangle(m_hDC, 0, 0, m_width, m_height);
		DeleteObject(hPen);
		DeleteObject(hBrush);
	}
}

} // namespace texture
} // namespace lib
