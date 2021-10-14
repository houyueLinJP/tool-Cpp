/*******************************************************************
*  Copyright(c) 2017-2020 
*  All rights reserved.
*
*  文件名称:
*  简要描述:
*
*  作者:
*  日期:
*  说明:
******************************************************************/

#ifndef __AI_IMAGE_PROC_FILE_H__
#define __AI_IMAGE_PROC_FILE_H__
 
#include <list>
#include <string>
using namespace std;

extern "C" {
	/**
	*  功能描述: 深度优先递归遍历当前目录及子目录下的JPG图片文件
	*  @param path 待遍历的目录绝对路径
	*  @param layer 当前目录的层数，0表示从当前目录下开始搜索，1表示从当前目录的子目录开始搜索，以此类推
	*  @param suffixName 待筛选的后缀名，若为null则不筛选，需传输".jpg"、".txt"格式
	*  @param filePath 输出参数，找到的图片文件列表
	*
	*  @return
	*/
	void GetImageFilesByRecur(const string folderPath, int layer, const char * suffixName, list<string>& filePath);
}

#endif
