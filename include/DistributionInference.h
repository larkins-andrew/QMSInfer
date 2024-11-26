#pragma once
#include <iostream>
#include <string>
#include <set>
#include "OperatorEnums.h"
#include "Node.h"

class DistributionInference {
public:
	static void inference(Node* node, std::set<std::string> &M);
	static void ruleInference(Node *n, OperatorEnums op, std::set<std::string> &M);
};