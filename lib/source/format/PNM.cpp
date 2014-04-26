#include "stdafx.h"
#include <lib/format/PNM.h>

#include <lib/file/file.h>
#include <lib/utility/utility.h>

namespace lib {
namespace format {

PNM::PNM(void)
: m_bValid(false)
, m_pTexture(NULL)
{
	destroy();
}

PNM::PNM(uint8_t* data, uint32_t size)
: m_bValid(false)
, m_pTexture(NULL)
{
	createPNM(data, size);
}

PNM::~PNM(void)
{
	destroy();
}


int PNM::getFormatHeaderCount()
{
	if (isPBM())  return 2;
	if (isPGM())  return 3;
	if (isPPM())  return 3;
	return -1;
}

bool PNM::createPNM(uint8_t* data, uint32_t size)
{
	destroy();
	// ヘッダ
	bool result = true;
	int32_t pos0, pos1;
	pos0 = 0;
	int header_count = 0;
	do {
		pos1 = searchData(data, size, pos0, '\n');
		if (pos1 == -1 || pos0 >= pos1) {
			destroy();
			return false;
		}
		int s = pos1 - pos0;
		char *p = NEW("PNM::createPNM Buffer") char[s+1];
		memcpy(p, data+pos0, s);
		p[s] = '\0';
		std::string param = lib::utility::trim(p);
		SAFE_DELETE(p);
		
		pos0 = pos1+1;

		if (param[0] == '#') {
			// コメント
			m_comments.push_back(param);
			continue;
		}

		switch(header_count) {
		case 0:	// フォーマット
		{
			if (param.compare("P1") == 0)  m_format = FORMAT_P1;
			else if (param.compare("P2") == 0)  m_format = FORMAT_P2;
			else if (param.compare("P3") == 0)  m_format = FORMAT_P3;
			else if (param.compare("P4") == 0)  m_format = FORMAT_P4;
			else if (param.compare("P5") == 0)  m_format = FORMAT_P5;
			else if (param.compare("P6") == 0)  m_format = FORMAT_P6;
			else {
				ASSERT_MES(false, "[PNM] Invalid format. format=%s\n", param.c_str());
				destroy();
				return false;
			}
			break;
		}
		case 1:	// ピクセル数
		{
			int dst[2];
			setPpmParam(dst, 2, param);
			m_width = dst[0];
			m_height = dst[1];
			break;
		}
		case 2:	// 最大輝度
		{
			setPpmParam(&m_brightness, 2, param);
			break;
		}
		default:
			ASSERT(false);
			return false;
		}
		header_count++;

		if (!result) {
			destroy();
			return false;
		}
	} while(header_count < getFormatHeaderCount());
	
	DEBUG_PRINT("------PNM INFO------\n");
	DEBUG_PRINT("Format:     %d\n", m_format);
	DEBUG_PRINT("Pixel:      %d, %d\n", m_width, m_height);
	DEBUG_PRINT("Blightness: %d\n", m_brightness);
	DEBUG_PRINT("--------------------\n");

	// データ
	uint32_t texsize = size-pos0;
	m_pTexture = NEW("PNM Texture") lib::texture::Texture(m_width, m_height);

	bool created_tex = false;
	switch (m_format) {
	case FORMAT_P6:
		created_tex = copyPPMTextureBinary(m_pTexture, data+pos0, texsize);
		break;
	default:
		// 未対応
		created_tex = false;
	}
	if (!created_tex) {
		destroy();
		return false;
	}

	m_bValid = true;

	return true;
}

void PNM::destroy()
{
	SAFE_DELETE(m_pTexture);
	m_format = FORMAT_INVALID;
	m_width = 0;
	m_height = 0;
	m_brightness = 0;
//	m_divX = 0;
//	m_divY = 0;
//	m_maxCount = 0;
//	m_selectCostRate = 0;
//	m_changeCostRate = 0;
	m_bValid = false;
	m_comments.clear();
}

int32_t PNM::searchData(uint8_t* data, uint32_t size, uint8_t offset, uint8_t search)
{
	while(offset < size) {
		if (data[offset] == search) {
			return offset;
		}
		offset++;
	}
	return -1;
}

void PNM::setPpmParam(int* dst, int size, std::string& param)
{
	memset(dst, 0, size*sizeof(int));
	std::string::size_type pos0, pos1;
	pos0 = pos1 = 0;
	int cnt = 0;
	while(cnt < size && (pos0 = param.find_first_not_of(" \t\r\n", pos1)) != std::string::npos) {
		pos1 = param.find_first_of(" \t\r\n", pos0);
		std::string p = param.substr(pos0, pos1-pos0);
		if (p.compare("#") != 0) {
			dst[cnt] = atoi(p.c_str());
			cnt++;
		}
	}
}


bool PNM::copyPPMTextureBinary(lib::texture::Texture* tex, uint8_t* data, uint32_t size)
{
	ASSERT(tex);
	if (size%3 != 0)  return false;

	tex->clear(RGB(0xFF, 0xFF, 0xFF));
	for(int y=0; y<tex->getHeight(); y++) {
		for(int x=0; x<tex->getWidth(); x++) {
			// TODO: 輝度に合わせてカラー計算
			uint8_t r = data[0];
			uint8_t g = data[1];
			uint8_t b = data[2];
			SetPixelV(tex->getHandle(), x, y, RGB(r, g, b));
			data += 3;
		}
	}
	return true;
}

} // namespace format
} // namespace lib