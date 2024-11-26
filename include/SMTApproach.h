#pragma once
#include <string>
#include "Node.h"

class SMTApproach {
public:
	static void monolithicApproach(std::string benchmarkName);
	static bool needToBeChecked(Node node);
	static bool threeConditionCheck(Node *node);
	static void incrementalApproach(std::string benchmarkName);
};