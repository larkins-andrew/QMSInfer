#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Node.h"

class StringUtil {
public:
	//erase all space in string
	static void trim(std::string &s);
	static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);
	static std::vector<std::string> SplitStringBySpace(const std::string& s);
	static std::string getString(int n);
	// replace all occurance of t in s to w  
	static void replace_all(std::string & s, std::string const & t, std::string const & w);
	//replace string in vector and map
	static void  replaceThroughMap(std::string &s, std::vector<std::string> &v, std::map<std::string, std::string> &m);
	static void IntToBinaryVector(int n, std::vector<int> &v,int len);
	static void BitToBoolVector(std::vector<std::string> &rand,std::vector<int> &tag, std::map<std::string,std::string> &m);
	static void VectorLogicAdd(std::vector<std::string> &v, std::string &s);
};