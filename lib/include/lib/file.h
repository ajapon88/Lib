#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__
#include "stdafx.h"

namespace lib {
namespace file {
typedef std::vector<std::string> FileList;
void getFileList(FileList *filelist, const char *dir, const char *ext="*.*", bool recursive=true);
void getPathFilename(std::string *filename, const std::string &filepath);
void getFileExtention(std::string *ext, const std::string &filepath);

class FileData {
public:
	FileData(const char *filename);
	FileData();
	~FileData();

	void load(const char *filename);
	void release();
	int getSize();
	char *getData() { return m_data; }
	const char *getFileName() { return m_filename.c_str(); }

private:
	std::string m_filename;
	long m_size;
	char *m_data;
};
} // namespace file
} // namespace lib
#endif	// __LIB_FILE_H__