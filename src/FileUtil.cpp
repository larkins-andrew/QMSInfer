#include <iostream>
#include <fstream>
#include <cassert>
#include "FileUtil.h"
#include "StringUtil.h"

using namespace std;

//read a line
void FileUtil::readTxtByLine(string file)
{
	ifstream infile;
	infile.open(file.data());   //link the file object and the file stream object 
	assert(infile.is_open());   //test if read a file failed

	string s;
	while (getline(infile, s))
	{
		cout << s << endl;
	}
	infile.close();           
}

//read a char, don't cout space and tab
void FileUtil::readTxtBySpace(string file) {
	ifstream infile;
	infile.open(file.data());  
	assert(infile.is_open());  

	char c;
	while (!infile.eof())
	{
		infile >> c;
		cout << c << endl;

	}
	infile.close();             
}
	
//read a char
void FileUtil::readTxtByChar(string file)
{
	ifstream infile;
	infile.open(file.data());   
	assert(infile.is_open());   

	char c;
	infile >> noskipws;
	while (!infile.eof())
	{
		infile >> c;
		cout << c << endl;

	}
	infile.close();             
}

void FileUtil::readFileToNodeVector(string filePath, vector<string> &vec) {
	ifstream infile;
	string s;
	cout << filePath << endl;
	infile.open(filePath.data());   //link the file object and the file stream object 
	assert(infile.is_open());   //test if read a file failed
	while (getline(infile, s))
	{
		vector<string> temp;
		temp = StringUtil::SplitStringBySpace(s);
		for (unsigned int i = 0; i < temp.size();i++) {
			vec.push_back(temp[i]);
		}
	}
	infile.close();
}

void FileUtil::readFileToCodeVector(string filePath, vector<string> &vec) {
	ifstream infile;
	string s;
	cout << filePath << endl;
	infile.open(filePath.data());   //link the file object and the file stream object 
	assert(infile.is_open());   //test if read a file failed
	while (getline(infile, s))
	{
		string temp;
		if(s.size()>0){
			temp=s.substr(0, s.size() -1 );
			if (temp.find("=") != -1) {
				vec.push_back(temp);
			}
		}

	}
	infile.close();
}