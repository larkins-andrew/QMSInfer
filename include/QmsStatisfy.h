#pragma once
#include <string>
#include <set>
#include "Node.h"

class QmsStatisfy {
public:
	static std::string getFirstString(Node n);
	static std::string parseNode(Node n);
	static bool isQmsStatisfy(Node node,double qms);
	static bool isQmsStatisfyWithNotCares(Node node, std::set<std::string> notCareRandSet,double qms);
};