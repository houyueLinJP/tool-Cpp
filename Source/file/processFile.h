/*************************************************************************
*  Copyright(c) 2017-2020
*  All rights reserved.
*
*  文件名称:
*  简要描述:
*
*  作者:
*  日期:
*  说明:
**************************************************************************/
#ifndef __AI_IMAGE_PROC_FILE_H__
#define __AI_IMAGE_PROC_FILE_H__
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>  
#include <stdio.h> 

#ifdef WIN64
#include <Windows.h>
#include <direct.h>
#else
#include <dirent.h>
#include <string.h>
#include <queue>
#endif 


/*******************************************************************************
*	@Func: 文件操作 之 移动
*	@class: FileMover、RenameMover、StreamMover
********************************************************************************/
// method 1
//C
// move file by API:move
struct FileMover
{
	virtual bool move(const std::string& src, const std::string& dst) const = 0;
	bool move(const std::vector<std::string>& src, const std::vector<std::string>& dst) const;
	FileMover() {}
	virtual ~FileMover() {}
};
bool FileMover::move(const std::vector<std::string>& src, const std::vector<std::string>& dst) const {
	if (src.size() != dst.size()) {
		std::cerr << "src and dst files number not equal" << std::endl;
		return false;
	}
	bool ret = true;
	for (int i = 0; i < src.size(); ++i)
		ret &= move(src[i].c_str(), dst[i].c_str());
	return ret;
}

// method 2
// C 
// move file by API:rename. 
// This is an operation performed directly on a file;
// No streams are involved in the operation.
struct RenameMover :public FileMover {
	bool move(const std::string& src, const std::string& dst) const override;
};
bool RenameMover::move(const std::string& src, const std::string& dst) const {
	if (0 == rename(src.c_str(), dst.c_str()))
		return true;
	perror("Error renaming file");
	return false;
}

// method 3
// C++
// move flle by read and write using std::ios::stream.
struct StreamMover :public FileMover {
	StreamMover(bool keep = false) :keep_src(keep) {}
	~StreamMover() {}
	bool keep_src;
	bool move(const std::string& src, const std::string& dst) const override;
};
bool StreamMover::move(const std::string& src, const std::string& dst) const {
	std::ifstream ifs(src, std::ios::binary);
	std::ofstream ofs(dst, std::ios::binary);
	if (!ifs.is_open()) {
		std::cout << "open src file fail: " + src << std::endl;
		return false;
	}
	ofs << ifs.rdbuf();
	ifs.close();
	ofs.close();
	if (!keep_src && 0 != remove(src.c_str())) {
		std::cerr << "remove src file fail: " + src << std::endl;
	}
	return true;
}


/*******************************************************************************
*	@Func: 文件操作 之 读写
*	@class: FileReader、FileWriter
*
********************************************************************************/
struct FileReader {
	bool read(const std::string src, std::string& dst) const;
	bool read(const char *pszPath, char **pszContent) const;
	~FileReader() {}
};
bool FileReader::read(const std::string src, std::string& dst) const
{
	std::fstream jsonFile(src, std::ios::in | std::ios::out);
	if (!jsonFile.is_open()) return false;

	dst.assign((std::istreambuf_iterator<char>(jsonFile)), std::istreambuf_iterator<char>());
	jsonFile.close();

	return true;
}
bool FileReader::read(const char *pszPath, char **pszContent) const
{
	FILE *fp = NULL;
	fp = fopen(pszPath, "r");
	if (fp == NULL) return false;

	fseek(fp, 0L, SEEK_END);
	int fileLength = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	*pszContent = (char*)malloc(sizeof(char)*fileLength);
	fread((*pszContent), fileLength, 1, fp);

	fclose(fp);
	return true;
}

struct FileWriter {
	bool write(const char *pszPath, char *content) const;
	~FileWriter() {}
};
bool FileWriter::write(const char *pszPath, char *content) const
{
	std::fstream file;
	file.open(pszPath, std::ofstream::out | std::ios::app);
	if (!file.is_open()) return false;

	//time_t now = time(0);
	//file << "log date: " << now << std::endl << std::endl;
	file << content << std::endl;
	file.close();

	return true;
}


/*******************************************************************************
*	@Func: 文件名(字符串)操作 之 切割
*	@class: FileNameSpliter
*
********************************************************************************/
void splitFilename(std::string& str, int method)
{
	std::size_t found = str.find_last_of("/\\");
	std::string out;
	if (method == 0) {
		//get path
		//D:\test\1.jpg ==> D:\test
		out.append(str.substr(0, found));
		//std::cout << " path: " << str.substr(0, found) << '\n';
	}
	else {
		//get file
		//D:\test\1.jpg ==> 1.jpg
		out.append(str.substr(found + 1));
		//std::cout << " file: " << str.substr(found + 1) << '\n';
	}
	str.assign(out.begin(), out.end());
}
std::string splitPathOfFileOrName(std::string str, int method)
{
	std::size_t found = str.find_last_of("/\\");
	std::string out;
	if (method == 0) {
		//get path
		//D:\test\1.jpg ==> D:\test
		out.append(str.substr(0, found));
		//std::cout << " path: " << str.substr(0, found) << '\n';
	}
	else {
		//get file
		//D:\test\1.jpg ==> 1.jpg
		out.append(str.substr(found + 1));
		//std::cout << " file: " << str.substr(found + 1) << '\n';
	}
	return out;
}

struct FileNameSpliter
{
	bool split(std::string& srcStr, int method = 1) const;
	std::string split(std::string srcStr, int method = 1) const;

	bool split(std::string srcStr, std::vector<std::string>& vecStr, const std::string separator) const;
	std::vector<std::string> split(std::string srcStr, const std::string separator) const;

	~FileNameSpliter() {}
};
bool FileNameSpliter::split(std::string& srcStr, int method) const
{
	if (srcStr.empty()) return false;

	std::size_t found = srcStr.find_last_of("/\\");
	if (found == srcStr.npos) return false;

	std::string dstStr;
	if (method == 0) {
		dstStr.assign(srcStr.substr(0, found));
	}
	else if (method == 1) {
		dstStr.assign(srcStr.substr(found + 1));
	}
	else {
		return false;
	}

	return true;
}
std::string FileNameSpliter::split(std::string srcStr, int method) const
{
	if (srcStr.empty()) return std::string();

	std::size_t found = srcStr.find_last_of("/\\");
	if (found == srcStr.npos) return std::string();

	std::string dstStr;
	if (method == 0) {
		dstStr.assign(srcStr.substr(0, found));
	}
	else if (method == 1) {
		dstStr.assign(srcStr.substr(found + 1));
	}
	else {
		return std::string();
	}

	return dstStr;
}
bool FileNameSpliter::split(std::string srcStr, std::vector<std::string>& vecStr, const std::string separator) const
{
	std::string::size_type posSeparator = srcStr.find(separator);//分隔符位置
	if (posSeparator == srcStr.npos) return false;

	std::string::size_type posSubstringStart = 0;//子串开始位置
	while (std::string::npos != posSeparator)
	{
		vecStr.push_back(srcStr.substr(posSubstringStart, posSeparator - posSubstringStart));

		posSubstringStart = posSeparator + separator.size();
		posSeparator = srcStr.find(separator, posSubstringStart);
	}

	if (posSubstringStart != srcStr.length())//截取最后一段数据
		vecStr.push_back(srcStr.substr(posSubstringStart));

	return true;
}
std::vector<std::string> FileNameSpliter::split(std::string srcStr, const std::string separator) const
{
	std::string::size_type posSeparator = srcStr.find(separator);//分隔符位置
	if (posSeparator == srcStr.npos) return std::vector<std::string>();

	std::string::size_type posSubstringStart = 0;//子串开始位置
	std::vector<std::string> vecStr;
	while (std::string::npos != posSeparator)
	{
		vecStr.push_back(srcStr.substr(posSubstringStart, posSeparator - posSubstringStart));

		posSubstringStart = posSeparator + separator.size();
		posSeparator = srcStr.find(separator, posSubstringStart);
	}

	if (posSubstringStart != srcStr.length())//截取最后一段数据
		vecStr.push_back(srcStr.substr(posSubstringStart));

	return vecStr;
}


/*******************************************************************************
*	@Func: 文件操作 之 递归目录结构 [不稳定]
*   @class: FilesFinder
*	@parameter lpPath: path of upper-layer
*	@parameter separator: 分隔符;  = "." 、 ".*"
*	@parameter vecStr: outPut
********************************************************************************/
struct FilesFinder
{
	bool find(const char* lpPath, std::vector<std::string>& vecStr, const char* separator = ".*") const;
	std::vector<std::string> find(const char* lpPath, const char* separator = ".*") const;
	~FilesFinder() {}
};
bool FilesFinder::find(const char* lpPath, std::vector<std::string>& vecStr, const char* separator) const
{
#ifdef WIN64
	char szFind[1024] = { 0 };
	char szFile[1024] = { 0 };
	//std::vector<std::string> file_lists;
	WIN32_FIND_DATA FindFileData;

	std::strcpy(szFind, lpPath);
	std::strcat(szFind, "\\*");
	std::strcat(szFind, separator);

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);


	if (INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << "Empty folder!" << std::endl;
		return false;
	}

	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				std::strcpy(szFile, lpPath);
				std::strcat(szFile, "\\");
				std::strcat(szFile, FindFileData.cFileName);
				FilesFinder::find(szFile);
			}
		}
		else
		{
			if (szFile[0])
			{
				std::string filePath = lpPath;
				filePath += "\\";
				filePath += FindFileData.cFileName;
				vecStr.push_back(filePath);
			}
			else
			{
				std::string filePath = szFile;
				filePath += FindFileData.cFileName;
				vecStr.push_back(filePath);
			}
		}

	} while (::FindNextFile(hFind, &FindFileData));

	::FindClose(hFind);
	return true;
#else
	(void)secName;

	//std::vector<std::string> result;
	std::queue<std::string> queue;
	std::string dirname;

	DIR *dir;
	if (!(dir = opendir(lpPath))) {
		return false;
	}
	queue.push(lpPath);

	struct dirent *ent;
	while (!queue.empty())
	{

		dirname = queue.front();
		dir = opendir(dirname.c_str());
		queue.pop();
		if (!dir) { continue; }

		while (ent = readdir(dir))
		{

			if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
			{
				continue;
			}
			if (ent->d_type == DT_DIR)
			{
				queue.push(dirname + "/" + ent->d_name);
			}
			else
			{
				vecStr.push_back(dirname + "/" + ent->d_name);
			}

		}

		closedir(dir);
	}

	return true;
#endif
}
std::vector<std::string> FilesFinder::find(const char* lpPath, const char* separator) const
{
#ifdef WIN64
	char szFind[1024] = { 0 };
	char szFile[1024] = { 0 };
	std::vector<std::string> file_lists;
	WIN32_FIND_DATA FindFileData;

	std::strcpy(szFind, lpPath);
	std::strcat(szFind, "\\*");
	std::strcat(szFind, separator);

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);


	if (INVALID_HANDLE_VALUE == hFind)
	{
		std::cout << "Empty folder!" << std::endl;
		return file_lists;
	}

	do
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				std::strcpy(szFile, lpPath);
				std::strcat(szFile, "\\");
				std::strcat(szFile, FindFileData.cFileName);
				FilesFinder::find(szFile);
			}
		}
		else
		{
			if (szFile[0])
			{
				std::string filePath = lpPath;
				filePath += "\\";
				filePath += FindFileData.cFileName;
				file_lists.push_back(filePath);
			}
			else
			{
				std::string filePath = szFile;
				filePath += FindFileData.cFileName;
				file_lists.push_back(filePath);
			}
		}

	} while (::FindNextFile(hFind, &FindFileData));

	::FindClose(hFind);
	return file_lists;

#else
	(void)secName;

	std::vector<std::string> result;
	std::queue<std::string> queue;
	std::string dirname;

	DIR *dir;
	if (!(dir = opendir(lpPath))) {
		return result;
	}
	queue.push(lpPath);

	struct dirent *ent;
	while (!queue.empty())
	{

		dirname = queue.front();
		dir = opendir(dirname.c_str());
		queue.pop();
		if (!dir) { continue; }

		while (ent = readdir(dir))
		{

			if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0)
			{
				continue;
			}
			if (ent->d_type == DT_DIR)
			{
				queue.push(dirname + "/" + ent->d_name);
			}
			else
			{
				result.push_back(dirname + "/" + ent->d_name);
			}

		}

		closedir(dir);
	}

	return result;

#endif 
}

#endif
