#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <set>
#include <z3++.h>
#include <deque>
#include "Node.h"

class NodeUtil {
public:

	static int semdCalls;
	static int dontcareCalls;
	static double semdTime;
	static double dontcareTime;

	static Node* findNode(std::vector<Node> &v, std::string nodeName);
	static void nodeCopy(Node *n, Node copyNode);
	static bool hasChildrenUKD(Node* node, std::deque<Node* > UKDNds, int &count);
	static bool findUKD(Node* node, std::deque<Node* > UKDNds);
	static void getAuxiliaryTable(Node *node);
	static void getRegion(Node *node, std::set<std::string>* uniqueMATi, std::map<std::string, Node> nodeMap);
	static bool isNotCareRand(Node node, std::string checkRand);
	static void getNotCareRandomSet(Node node, std::set<std::string>& randSet);
	static void getNodeMap(Node* node, std::map<std::string, Node*>& nodeMap);
	static int getHeight(Node* node);
	static void printTree(Node* node, int indent);
	static std::string regularStringInTree(Node* node, int indent);
};