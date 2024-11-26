#pragma once
#include <iostream>  
#include <set>  
#include <algorithm>  
#include <string>
#include <vector>

class SetUtil {
public:
	static void put_out(std::set<std::string> &p);
	static std::string getFirstElement(std::set<std::string> &p);
	static void Intersection(std::set<std::string> &A, std::set<std::string> &B, std::set<std::string> &result);
	static void Union(std::set<std::string> &A, std::set<std::string> &B, std::set<std::string> &result);
	static void Difference(std::set<std::string> &A, std::set<std::string> &B, std::set<std::string> &result);
	static bool Contain(std::set<std::string> &A, std::string temp) ;
	static bool containValue(std::set<std::string> &A, std::string temp);
	static void SetToVector(std::set<std::string> &s, std::vector<std::string> &v);
	static std::string SetToString(std::set<std::string> &s);
};
