#include "Node.h"
using namespace std;

Node::Node() {
}
Node::~Node() {
	//cout << "Node Destroyed" << endl;
}
Node::Node(string nodeName, OperatorEnums nodeOp, NodeTypeEnums nodeType) {
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

double Node::getQms(){
	return qms;
}

void Node::setQms(double nodeQms){
	qms=nodeQms;
}


string Node::getName() {
	return name;
}

void Node::setName(string nodeName){
	name = nodeName;
}

DistributionEnums Node::getDistributionEnums() {
	return distribution;
}

void Node::setDistributionEnums(DistributionEnums dis) {
	distribution = dis;
}

OperatorEnums Node::getOperatorEnums() {
	return operation;
}

void Node::setOperatorEnums(OperatorEnums op) {
	operation = op;
}

NodeTypeEnums Node::getNodeTypeEnums() {
	return  type;
}

void Node::setNodeTypeEnums(NodeTypeEnums t) {
	type = t;
}

set<string>* Node::getDependences() {
	return dependences;
}

void Node::setDependences(set<string>* dep) {
	dependences = dep;
}

set<string>* Node::getDominant() {
	return dominant;
}

void Node::setDominant(set<string>* dom) {
	dominant = dom;
}

set<string>* Node::getSupportV() {
	return supportV;
}

void Node::setSupportV(set<string>* SupportV) {
	supportV = SupportV;
}

set<string>* Node::getUniqueM() {
	return uniqueM;
}

void Node::setUniqueM(set<string>* UniqueM) {
	uniqueM = UniqueM;
}

set<string>* Node::getPerfectM() {
	return perfectM;
}

void Node::setPerfectM(set<string>* PerfectM) {
	perfectM = PerfectM;
}

set<string>* Node::getParentNodeNames() {
	return parentNodeNames;
}

void Node::setParentNodeNames(set<string>* parentNames) {
	parentNodeNames = parentNames;
}

set<string>* Node::getSemd() {
	return semd;
}

void Node::setSemd(set<string>* semd) {
	this->semd = semd;
}

Node* Node::getParent() {
	return parent;
}

void Node::setParent(Node* node) {
	parent = node;
}

Node* Node::getLeftChild() {
	return leftChild;
}

void Node::setLeftChild(Node* node) {
	leftChild = node;
}

Node * Node::getRightChild() {
	return rightChild;
}

void Node::setRightChild(Node* node) {
	rightChild = node;
}

string Node::setToString(set<string> s) {
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

string Node::toString() {
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

void Node::printTreeAtNode(Node n) {
	cout << n.getName() << endl;
	cout << n.toString() << endl;
	if (n.leftChild != NULL) {	
		printTreeAtNode(*n.leftChild);
	}

	if (n.rightChild != NULL) {
		printTreeAtNode(*n.rightChild);
	}
}


void Node::getRandSet(Node n, set<string> &rand) {
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

bool Node::hasKey(Node n) {
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

void Node::getRandSet2(Node n, set<string> &rand) {
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

void Node::getSecretSet(Node n, set<string> &rand) {
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

void Node::getPlainSet(Node n, set<string> &plain) {
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