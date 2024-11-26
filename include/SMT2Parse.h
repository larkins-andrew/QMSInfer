#pragma once
#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <map>
#include "Node.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"

class SMT2Parse {
public:
	static int exportInt;
	static std::string & replace_all(std::string &str, const std::string &old_value, const std::string &new_value);
	static std::string & replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value);
	static std::string getSatFirstString(Node n);
	static std::string getSatString(Node n);
	static std::string getFirstString(Node n);
	static std::string parseNode(Node n);
	static void nodeToSmtFile(Node node, std::string outFilePath);
	static DistributionEnums getDistributionByZ3(Node node);
	static DistributionEnums getDistributionByZ3(Node node,std::set<std::string> notCareRandSet);
};