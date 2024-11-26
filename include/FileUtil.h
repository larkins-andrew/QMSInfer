#pragma once
#include <string>
#include <vector>

class FileUtil {
public:
	//read a line
	static void readTxtByLine(std::string file);
	//read a char, don't cout space and tab
	static void readTxtBySpace(std::string file);
	//read a char
	static void readTxtByChar(std::string file);
	static void readFileToNodeVector(std::string filePath, std::vector<std::string> &vec);
	static void readFileToCodeVector(std::string filePath, std::vector<std::string> &vec);
};
