#pragma once
#include <iostream>
#include <string>
#include <set>
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"
using namespace std;

class Node {

private:
	string name;
	double qms;
	DistributionEnums distribution;
	OperatorEnums operation;
	NodeTypeEnums type;
	set<string> *dependences;  //structural dependency
	set<string> *dominant;     //Dom set

	set<string> *supportV;
	set<string> *uniqueM;
	set<string> *perfectM;

	set<string> *parentNodeNames;
	set<string> *semd;

	Node *parent;
	Node *leftChild;
	Node *rightChild;

public:
	Node() {

	}
	~Node() {
		//cout << "Node Destroyed" << endl;
	}
	Node(string nodeName, OperatorEnums nodeOp, NodeTypeEnums nodeType) {
		name = nodeName;
		distribution=DistributionEnums ::NULLDISTRIBUTION;
		operation = nodeOp;
		type = nodeType;

		dependences = new set<string>();
		dominant = new set<string>();

		supportV = new set<string>();
		uniqueM = new set<string>();
		perfectM = new set<string>();

        parentNodeNames=new set<string>();
        semd = NULL;
		
		parent = NULL;
		leftChild = NULL;
		rightChild = NULL;
	}

	double getQms(){
		return qms;
	}

	void setQms(double nodeQms){
		qms=nodeQms;
	}


	string getName() {
		return name;
	}

	void setName(string nodeName){
		name = nodeName;
	}

	DistributionEnums getDistributionEnums() {
		return distribution;
	}

	void setDistributionEnums(DistributionEnums dis) {
		distribution = dis;
	}

	OperatorEnums getOperatorEnums() {
		return operation;
	}

	void setOperatorEnums(OperatorEnums op) {
		operation = op;
	}

	NodeTypeEnums getNodeTypeEnums() {
		return  type;
	}

	void setNodeTypeEnums(NodeTypeEnums t) {
		type = t;
	}

	set<string>* getDependences() {
		return dependences;
	}

	void setDependences(set<string>* dep) {
		dependences = dep;
	}

	set<string>* getDominant() {
		return dominant;
	}

	void setDominant(set<string>* dom) {
		dominant = dom;
	}

	set<string>* getSupportV() {
		return supportV;
	}

	void setSupportV(set<string>* SupportV) {
		supportV = SupportV;
	}

	set<string>* getUniqueM() {
		return uniqueM;
	}

	void setUniqueM(set<string>* UniqueM) {
		uniqueM = UniqueM;
	}

	set<string>* getPerfectM() {
		return perfectM;
	}

	void setPerfectM(set<string>* PerfectM) {
		perfectM = PerfectM;
	}





	set<string>* getParentNodeNames() {
		return parentNodeNames;
	}


	void setParentNodeNames(set<string>* parentNames) {
		parentNodeNames = parentNames;
	}

	set<string>* getSemd() {
		return semd;
	}


	void setSemd(set<string>* semd) {
		this->semd = semd;
	}



	Node* getParent() {
		return parent;
	}

	void setParent(Node* node) {
		parent = node;
	}

	Node* getLeftChild() {
		return leftChild;
	}

	void setLeftChild(Node* node) {
		leftChild = node;
	}

	Node *getRightChild() {
		return rightChild;
	}

	void setRightChild(Node* node) {
		rightChild = node;
	}



	static string setToString(set<string> s) {
		string str = "";
		set<string>::iterator iter = s.begin();

		str.append("[");
		while (iter != s.end())
		{
			str.append(*iter + ", ");
			++iter;
		}
		str.append("]");
		return str;
	}


	string toString() {
		string s = "";
		s.append("Node [name=" + name);
		s.append(", distribution=" + EnumUtil::distributionToString(distribution));
		s.append(", operator=" + EnumUtil::operationToString(operation));
		s.append(", type=" + EnumUtil::typeToString(type));
		s.append(", dependences=" + setToString(*dependences));
		s.append(", dominant=" + setToString(*dominant));
		s.append(", supportV =" + setToString(*supportV));
		s.append(", uniqueM=" + setToString(*uniqueM));
		s.append(", perfectM=" + setToString(*perfectM));
		if (parent != NULL) {
			s.append(", parent=" + (*parent).toString());
		}
		else {
			s.append(", parent=null" );
		}
		if (leftChild != NULL) {
			s.append(", leftChild=" + (*leftChild).toString());
		}
		else {
			s.append(", leftChild=null");
		}
		if (rightChild != NULL) {
			s.append(", rightChild=" + (*rightChild).toString());
		}
		else {
			s.append(", rightChild=null");
		}
		s.append("]");
		return s;
	}

	static void printTreeAtNode(Node n) {
		cout << n.getName() << endl;
		cout << n.toString() << endl;
		if (n.leftChild != NULL) {	
			printTreeAtNode(*n.leftChild);
		}

		if (n.rightChild != NULL) {
			printTreeAtNode(*n.rightChild);
		}
	}


	static void getRandSet(Node n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK) {
			rand.insert(" " + n.getName() + " ");
		}
		if (n.leftChild != NULL) {
			getRandSet(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getRandSet(*n.rightChild, rand);
		}
	}

	static bool hasKey(Node n) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
		    return true;
		}

		if (hasKey(*(n.getLeftChild()))) {
		    return true;
		}

		if (hasKey(*(n.getRightChild()))) {
		    return true;
		}

		return false;
	}

	static void getRandSet2(Node n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK) {
			rand.insert(n.getName());
		}
		if (n.leftChild != NULL) {
			getRandSet(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getRandSet(*n.rightChild, rand);
		}
	}
	
	static void getSecretSet(Node n, set<string> &rand) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
			rand.insert(" "+n.getName()+" ");
		}
		if (n.leftChild != NULL) {
			getSecretSet(*n.leftChild, rand);
		}

		if (n.rightChild != NULL) {
			getSecretSet(*n.rightChild, rand);
		}
	}

	static void getPlainSet(Node n, set<string> &plain) {
		if (n.getNodeTypeEnums() == NodeTypeEnums::PLAIN) {
			plain.insert(" " + n.getName() + " ");
		}
		if (n.leftChild != NULL) {
			getPlainSet(*n.leftChild, plain);
		}

		if (n.rightChild != NULL) {
			getPlainSet(*n.rightChild, plain);
		}
	}
	
};