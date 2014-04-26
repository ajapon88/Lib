#ifndef __LIB_TEXTURE_TEXTURE_H__
#define __LIB_TEXTURE_TEXTURE_H__
#include "stdafx.h"

#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace lib {
namespace texture {

class Texture {
public:
	Texture();
	Texture(uint32_t width, uint32_t height);
	~Texture();
	
	void create(uint32_t width, uint32_t height);
	void destroy();
	
	uint32_t getWidth() { return m_width; }
	uint32_t getHeight() { return m_height; }
	HDC getHandle() { return m_hDC; }
	void clear(COLORREF color);

private:
	
	bool m_bCreated;
	uint32_t m_height;
	uint32_t m_width;
    HDC m_hDC;
	HBITMAP m_hBitmap;
};

} // namespace texture
} // namespace lib
#endif	// __LIB_TEXTURE_TEXTURE_H__