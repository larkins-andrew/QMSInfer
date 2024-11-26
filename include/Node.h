#pragma once
#include <iostream>
#include <string>
#include <set>
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"

class Node {

private:
	std::string name;
	double qms;
	DistributionEnums distribution;
	OperatorEnums operation;
	NodeTypeEnums type;
	std::set<std::string> *dependences;  //structural dependency
	std::set<std::string> *dominant;     //Dom set

	std::set<std::string> *supportV;
	std::set<std::string> *uniqueM;
	std::set<std::string> *perfectM;

	std::set<std::string> *parentNodeNames;
	std::set<std::string> *semd;

	Node *parent;
	Node *leftChild;
	Node *rightChild;

public:
	Node();
	~Node();
	Node(std::string nodeName, OperatorEnums nodeOp, NodeTypeEnums nodeType);
	double getQms();
	void setQms(double nodeQms);
	std::string getName();
	void setName(std::string nodeName);
	DistributionEnums getDistributionEnums();
	void setDistributionEnums(DistributionEnums dis);
	OperatorEnums getOperatorEnums();
	void setOperatorEnums(OperatorEnums op);
	NodeTypeEnums getNodeTypeEnums();
	void setNodeTypeEnums(NodeTypeEnums t);
	std::set<std::string>* getDependences();
	void setDependences(std::set<std::string>* dep);
	std::set<std::string>* getDominant();
	void setDominant(std::set<std::string>* dom);
	std::set<std::string>* getSupportV();
	void setSupportV(std::set<std::string>* SupportV);
	std::set<std::string>* getUniqueM();
	void setUniqueM(std::set<std::string>* UniqueM);
	std::set<std::string>* getPerfectM();
	void setPerfectM(std::set<std::string>* PerfectM);
	std::set<std::string>* getParentNodeNames();
	void setParentNodeNames(std::set<std::string>* parentNames);
	std::set<std::string>* getSemd();
	void setSemd(std::set<std::string>* semd);
	Node* getParent();
	void setParent(Node* node);
	Node* getLeftChild();
	void setLeftChild(Node* node);
	Node *getRightChild();
	void setRightChild(Node* node);
	static std::string setToString(std::set<std::string> s);
	std::string toString();
	static void printTreeAtNode(Node n);
	static void getRandSet(Node n, std::set<std::string> &rand);
	static bool hasKey(Node n);
	static void getRandSet2(Node n, std::set<std::string> &rand);
	static void getSecretSet(Node n, std::set<std::string> &rand);
	static void getPlainSet(Node n, std::set<std::string> &plain);
};