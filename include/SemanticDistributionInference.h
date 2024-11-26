#pragma once
#include "Node.h"
class SemanticDistributionInference {
public:
	static int hasSemd;
	static int noSemd;

	static void synInference(Node* node);
	static void synRule(Node* node);
	static void semRule(Node* node, Node* originNode);
};