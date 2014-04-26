#include "stdafx.h"
#include <lib/file/file.h>
#define NOMINMAX
#include <windows.h>

namespace lib {
namespace file {
void getFileList(FileList *filelist, const char *dir, const char *ext, bool recursive)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	std::string path;
	path = dir;
	if (path.back() != '\\' && path.back() != '/')  path += '/';
	std::string filename = path;
	filename += "*";
	hFind = FindFirstFile(filename.c_str(), &fd);
	if(hFind == INVALID_HANDLE_VALUE) {
		// 失敗
		return;
	}
	do {
		std::string filepath = path;
		filepath += fd.cFileName;
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// ディレクトリ
			if (recursive) {
				if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
					getFileList(filelist, filepath.c_str(), ext, recursive);
				}
			}
		} else {
			if (!ext || strcmp(ext, "*") == 0) {
				filelist->addFile(filepath.c_str());
			} else {
				std::string filext;
				getFileExtention(&filext, filepath);
				if (filext.compare(ext) == 0) {
					filelist->addFile(filepath.c_str());
				}
			}
		}
	} while(FindNextFile(hFind, &fd));
	FindClose(hFind);
}

void getPathFilename(std::string *filename, const std::string &filepath)
{
	std::string::size_type pos = filepath.find_last_of("\\/");
	if (pos == std::string::npos) {
		*filename = filepath;
	} else {
		*filename = filepath.substr(pos+1);
	}
}

void getFileExtention(std::string *ext, const std::string &filepath)
{
	*ext = "";
	std::string filename;
	getPathFilename(&filename, filepath);
	std::string::size_type pos = filepath.find_last_of(".");
	if (pos != std::string::npos) {
		*ext = filepath.substr(pos+1);
	}
}

FileList::FileList()
: m_file_index(-1)
{
}
FileList::~FileList()
{
}
void FileList::clear()
{
	m_file_index = -1;
	m_file_list.clear();
}
void FileList::addFile(const char *filename)
{
	std::string str = filename;
	m_file_list.push_back(str);
}
void FileList::reset()
{
	m_file_index = -1;
}
bool FileList::nextFile()
{
	m_file_index++;
	if (0 <= m_file_index && m_file_index < static_cast<int>(m_file_list.size())) {
		return true;
	}
	return false;
}
const char *FileList::getFileName()
{
	if (0 <= m_file_index && m_file_index < static_cast<int>(m_file_list.size())) {
		return m_file_list[m_file_index].c_str();
	}
	return NULL;
}

FileData::FileData(const char *filename)
: m_data(NULL)
, m_size(0)
{
	load(filename);
}
FileData::FileData()
: m_data(NULL)
, m_size(0)
{
}
FileData::~FileData()
{
	release();
}

void FileData::release()
{
	SAFE_DELETE_ARRAY(m_data);
	m_size = 0;
	m_filename = "";
}

void FileData::load(const char *filename)
{
	release();
	m_filename = filename;
	FILE *fp = fopen(filename, "rb");
	if (fp) {
		fseek(fp, 0, SEEK_END);
		m_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		m_data = NEW_ARRAY("FileData::Data") uint8_t[m_size+1];
		memset(m_data, 0, sizeof(char)*(m_size+1));
		fread(m_data, sizeof(char), m_size, fp);
		fclose(fp);
	}
}

} // namespace file
} // namespace lib