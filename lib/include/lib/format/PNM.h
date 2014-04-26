#ifndef __LIB_FORMAT_PNM_H__
#define __LIB_FORMAT_PNM_H__
#include "stdafx.h"
#include <stdint.h>
#include <lib/texture/texture.h>

namespace lib {
namespace format {

class PNM
{
public:
	enum FORMAT{
		FORMAT_INVALID = -1,
		FORMAT_P1 = 1,
		FORMAT_P2,
		FORMAT_P3,
		FORMAT_P4,
		FORMAT_P5,
		FORMAT_P6,
	};

public:
	PNM(void);
	PNM(uint8_t* data, uint32_t size);
	~PNM(void);

	bool createPNM(uint8_t* data, uint32_t size);
	void destroy();
	
	bool isPBM() { return (FORMAT_P1 == m_format || FORMAT_P4 == m_format); }
	bool isPGM() { return (FORMAT_P2 == m_format || FORMAT_P5 == m_format); }
	bool isPPM() { return (FORMAT_P3 == m_format || FORMAT_P6 == m_format); }

	bool isAsciiFormat() {return (FORMAT_P1 <= m_format && m_format <= FORMAT_P3);}
	bool isBinaryFormat() {return (FORMAT_P4 <= m_format && m_format <= FORMAT_P6);}

	bool isValid() { return m_bValid; }
	lib::texture::Texture* getTexture() { return m_pTexture; }
	
	int getCommentCount() { return m_comments.size(); }
	const std::string& getComment(int idx) { return m_comments[idx]; }

private:
	int getFormatHeaderCount();
	int32_t searchData(uint8_t* data, uint32_t size, uint8_t offset, uint8_t search);
	void setPpmParam(int* dst, int size, std::string& param);
	bool copyPPMTextureBinary(lib::texture::Texture* tex, uint8_t* data, uint32_t size);

	std::vector<std::string> m_comments;
	
	bool m_bValid;
	// PNM情報
	FORMAT m_format;
	int m_width;
	int m_height;
	int m_brightness;

	// テクスチャ
	lib::texture::Texture *m_pTexture;
};

} // namespace format
} // namespace lib

#endif // __LIB_FORMAT_PNM_H__