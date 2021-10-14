#include<stdio.h>
#include<string.h>
#include<list>
#include<string>
#include<fstream>
#include<iostream>
#include"file.h"

#ifdef WINDOWS
#include<io.h>
#else
#include <dirent.h>
#endif
using namespace std;

/*******************************************************************
* 功能描述: 深度优先递归遍历当前目录及子目录下的JPG图片文件
* @param folderPath 输入参数，待遍历的目录绝对路径
* @param layer 输入参数，当前目录的层数，0表示从当前目录下开始搜索，1表示从当前目录的子目录开始搜索，以此类推 -- 目前不起作用
* @param suffixName 待筛选的后缀名，若为"", 则不筛选。如传输".jpg"，则只记录jpg文件
* @param filePath 输出参数，找到的图片文件列表
* @return
*******************************************************************/
#ifdef WIN32
void GetImageFilesByRecur(const string folderPath, int layer, const char * suffixName, list<string>& filePath)
{
	if (0 == folderPath.length())
	{
		return;
	}

	string currentPath = folderPath + "\\*.*";
	int suffixLength = strlen(suffixName);
	_finddata_t file_info;
	int handle = _findfirst(currentPath.c_str(), &file_info);
	// 返回值为-1则查找失败
	if (-1 == handle)
	{
		return;
	}

	do
	{
		// 判断是否子目录
		if (file_info.attrib == _A_SUBDIR)
		{
			// 递归遍历子目录
			int layer_tmp = layer;
			// .是当前目录，..是上层目录，必须排除掉这两种情况
			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)
			{
				GetImageFilesByRecur(folderPath + '\\' + file_info.name, layer_tmp + 1, suffixName, filePath);
			}
		}
		else
		{
			if ((suffixName == NULL) ||
				memcmp(file_info.name + strlen(file_info.name) - suffixLength, suffixName, suffixLength) == 0)
			{
				filePath.push_back(folderPath + '\\' + file_info.name);
			}
		}
	} while (!_findnext(handle, &file_info)); //返回0则遍历完

											  // 关闭文件句柄
	_findclose(handle);
}

#elif WIN64
void GetImageFilesByRecur(const string folderPath, int layer, const char * suffixName, list<string>& filePath)
{
	if (0 == folderPath.length())
	{
		return;
	}

	string currentPath = folderPath + "\\*.*";
	int suffixLength = strlen(suffixName);
	struct __finddata64_t file_info;
	__int64 handle = _findfirst64(currentPath.c_str(), &file_info);
	// 返回值为-1则查找失败
	if (-1 == handle)
	{
		return;
	}

	do
	{
		// 判断是否子目录
		if (file_info.attrib == _A_SUBDIR)
		{
			// 递归遍历子目录
			int layer_tmp = layer;
			// .是当前目录，..是上层目录，必须排除掉这两种情况
			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)
			{
				GetImageFilesByRecur(folderPath + '\\' + file_info.name, layer_tmp + 1, suffixName, filePath);
			}
		}
		else
		{
			if ((suffixName == NULL) ||
				memcmp(file_info.name + strlen(file_info.name) - suffixLength, suffixName, suffixLength) == 0)
			{
				filePath.push_back(folderPath + '\\' + file_info.name);
			}
		}
	} while (!_findnext64(handle, &file_info)); //返回0则遍历完

											  // 关闭文件句柄
	_findclose(handle);
}

#else

void GetImageFilesByRecur(const string folderPath, int layer, const char * suffixName, list<string>& filePath)
{
    DIR *dirp = NULL;
    struct dirent *pstDirent = NULL;
	char * p = NULL;
	int suffixLength = strlen(suffixName);
    if (0 == folderPath.length())
    {    
      return; 
    }    

    if( (dirp = opendir(folderPath.c_str())) ==NULL )
    {
        printf("Open dir path:%s fail!!\n", folderPath.c_str());
        return ;
    }

    while((pstDirent = readdir(dirp)) != NULL)
    {
        // 判断是否子目录
        if( pstDirent->d_type == 4 )
        {
            // 递归遍历子目录
            int layer_tmp = layer;
            // .是当前目录，..是上层目录，必须排除掉这两种情况
            if (strcmp( pstDirent->d_name, "..") != 0 && strcmp( pstDirent->d_name, ".") != 0)      
            {
                GetImageFilesByRecur(folderPath + '/' + pstDirent->d_name, layer_tmp + 1, suffixName, filePath);  
            }
        }  
        else
        {
            if( ( suffixName == NULL ) ||
					memcmp( pstDirent->d_name + strlen( pstDirent->d_name ) - suffixLength, suffixName, suffixLength ) == 0 )
            {
                //       printf("name:%s\n", pstDirent->d_name);
                filePath.push_back(folderPath + '/' + pstDirent->d_name);
            }
        }  
    }

    closedir(dirp);
}
#endif
