#ifndef __LIB_FILE_H__
#define __LIB_FILE_H__
#include "stdafx.h"

namespace lib {
namespace file {
class FileList;
class FileData;

void getFileList(FileList *filelist, const char *dir, const char *ext="*.*", bool recursive=true);
void getPathFilename(std::string *filename, const std::string &filepath);
void getFileExtention(std::string *ext, const std::string &filepath);

class FileList {
public:
	FileList();
	~FileList();
	void clear();
	void addFile(const char *filename);
	void reset();
	bool nextFile();
	const char *getFileName();
private:
	int m_file_index;
	std::vector<std::string> m_file_list;
};

class FileData {
public:
	FileData(const char *filename);
	FileData();
	~FileData();

	void load(const char *filename);
	void release();
	uint32_t getSize() { return m_size; }
	uint8_t *getData() { return m_data; }
	const char *getFileName() { return m_filename.c_str(); }

private:
	std::string m_filename;
	uint32_t m_size;
	uint8_t *m_data;
};
} // namespace file
} // namespace lib
#endif	// __LIB_FILE_H__