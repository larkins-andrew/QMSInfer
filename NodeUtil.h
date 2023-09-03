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
#include "SMT2Parse.h"


using namespace std;
class NodeUtil {
public:

	static int semdCalls;
	static int dontcareCalls;
	static double semdTime;
	static double dontcareTime;

	static Node* findNode(vector<Node> &v, string nodeName) {
		Node *node = NULL;
		for (unsigned int i = 0; i < v.size(); i++) {
			if (v[i].getName() == nodeName) {
				node = &v[i];
			}
		}
		return node;
	}

	static void nodeCopy(Node *n, Node copyNode) {
		n->setName(copyNode.getName());
		n->setDistributionEnums(copyNode.getDistributionEnums());
		n->setNodeTypeEnums(copyNode.getNodeTypeEnums());
		n->setOperatorEnums(copyNode.getOperatorEnums());
		n->setDependences(copyNode.getDependences());
		n->setDominant(copyNode.getDominant());
		n->setSupportV(copyNode.getSupportV());
		n->setPerfectM(copyNode.getPerfectM());
		n->setUniqueM(copyNode.getUniqueM());
		n->setParent(copyNode.getParent());
		n->setParentNodeNames(copyNode.getParentNodeNames());
		n->setSemd(copyNode.getSemd());
		if (copyNode.getLeftChild() != NULL) {
			n->setLeftChild(new Node());
			nodeCopy(n->getLeftChild(), *copyNode.getLeftChild());
		}
		else {
			n->setLeftChild(NULL);
		}
		if (copyNode.getRightChild()!=NULL) {
			n->setRightChild(new Node());
			nodeCopy(n->getRightChild(),*copyNode.getRightChild());
		}
		else {
			n->setRightChild(NULL);
		}
	}

	static bool hasChildrenUKD(Node* node, deque<Node* > UKDNds, int &count) {
		count++;
	    if(node == NULL)
	    	return false;
	    if(findUKD(node, UKDNds))
	    	return true;

	    if(hasChildrenUKD(node->getLeftChild(), UKDNds, count) ||
				hasChildrenUKD(node->getRightChild(), UKDNds, count))
	    	return true;

		return false;

	}

	static bool findUKD(Node* node, deque<Node* > UKDNds) {

		deque<Node* >::iterator pos;
		pos = find(UKDNds.begin(), UKDNds.end(), node);
		if(pos != UKDNds.end()) {
		    return true;
		}
		else {
		    return false;
		}

	}

	static void getAuxiliaryTable(Node *node) {
		if ((*node).getLeftChild() == NULL && (*node).getRightChild() == NULL) {
			if ((*node).getNodeTypeEnums() == NodeTypeEnums::MASK) {
				node->getSupportV()->insert(" " + node->getName() + " ");
				node->getUniqueM()->insert(" " + node->getName() + " ");
				node->getPerfectM()->insert(" " + node->getName() + " ");
				node->getDominant()->insert(" " + node->getName() + " ");
				return;
			}
			else {
				node->getSupportV()->insert(" " + node->getName() + " ");
				return;
			}
		}
		else {

			if ((*node).getLeftChild() != NULL) {
				if (node->getLeftChild()->getSupportV()->size() == 0) {
					getAuxiliaryTable((*node).getLeftChild());
				}
			}
			if ((*node).getRightChild() != NULL) {
				if (node->getRightChild()->getSupportV()->size() == 0) {
					getAuxiliaryTable((*node).getRightChild());
				}
			}

			if (node->getOperatorEnums() == OperatorEnums::NOT || node->getOperatorEnums() == OperatorEnums::EQUAL) {
				node->setSupportV(node->getLeftChild()->getSupportV());
				node->setUniqueM(node->getLeftChild()->getUniqueM());
				node->setPerfectM(node->getLeftChild()->getPerfectM());
				node->setDominant(node->getLeftChild()->getDominant());
			}
			else {
				SetUtil::Union(*node->getLeftChild()->getSupportV(), *node->getRightChild()->getSupportV(), *node->getSupportV());
				set<string> *temp1 = new set<string>();
				set<string> *temp2 = new set<string>();
				SetUtil::Union(*node->getLeftChild()->getUniqueM(), *node->getRightChild()->getUniqueM(), *temp1);
				SetUtil::Intersection(*node->getLeftChild()->getSupportV(), *node->getRightChild()->getSupportV(), *temp2);
				SetUtil::Difference(*temp1, *temp2, *node->getUniqueM());
				if (node->getOperatorEnums() == OperatorEnums::XOR) {
					set<string> *temp3 = new set<string>();
					SetUtil::Union(*node->getLeftChild()->getPerfectM(), *node->getRightChild()->getPerfectM(), *temp3);
					SetUtil::Intersection(*node->getUniqueM(), *temp3, *node->getPerfectM());

					set<string> *temp4 = new set<string>();
					SetUtil::Intersection(*node->getLeftChild()->getDominant(), *node->getRightChild()->getDominant(), *temp4);
					set<string> *temp5 = new set<string>();
					SetUtil::Union(*node->getLeftChild()->getDominant(), *node->getRightChild()->getDominant(), *temp5);
					SetUtil::Difference(*temp5, *temp4, *node->getDominant());
				}
			}

		}
	}

	static void getRegion(Node *node, set<string>* uniqueMATi, map<string, Node> nodeMap) {

		set<string> *perfectM = new set<string>();
		perfectM = node->getPerfectM();
		set<string> *temp = new set<string>();
		SetUtil::Intersection(*perfectM, *uniqueMATi, *temp);


		if (temp->size()>0 && uniqueMATi->size()>0) {

            string s = SetUtil::getFirstElement(*temp);
            StringUtil::trim(s);
            NodeUtil::nodeCopy(node, nodeMap[s]);
            return;
        }

		if (node->getLeftChild() != NULL && uniqueMATi->size()>0) {
			getRegion(node->getLeftChild(), uniqueMATi, nodeMap);
		}
		if (node->getRightChild() != NULL  && uniqueMATi->size()>0) {
			getRegion(node->getRightChild(), uniqueMATi, nodeMap);
		}

	}

	static  bool isNotCareRand(Node node, string checkRand) {

		clock_t c1 = clock();

		set<string> rand;
		set<string> secret;
		set<string> plain;
		vector<string> randVector;
		vector<string> secretVector;
		vector<string> plainVector;

		Node::getRandSet(node, rand);
		Node::getSecretSet(node, secret);
		Node::getPlainSet(node, plain);

		SetUtil::SetToVector(rand, randVector);
		SetUtil::SetToVector(secret, secretVector);
		SetUtil::SetToVector(plain, plainVector);

		int randCount = rand.size();
		int secretCount = secret.size();
		int plainCount = plain.size();


		string first = "This is node " + node.getName() + " to get not care random smt2 file.";
		string finalString="";
		z3::context contextUse;
		z3::solver solverUse(contextUse);

		//declare variable for smt2
		for (int i = 0; i < randCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(randVector[i] + " () Bool )");

			finalString.append(s+"\n");
		}


		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(secretVector[i] + " () Bool )");
			finalString.append(s+"\n");
		}

		for (int i = 0; i < plainCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(plainVector[i] + "() Bool )");
			finalString.append(s+"\n");
		}

		for (int i = 0; i < 2; i++) {
			string s = "";
			s.append("(declare-fun num");
			s.append(StringUtil::getString(i) + " () Bool )");
			finalString.append(s+"\n");
		}


		for (int i = 0; i < 2; i++) {
			string s = "";
			s.append("(assert(= num" + StringUtil::getString(i));
			string ss = SMT2Parse::parseNode(node);
			s.append(" " + ss + " ))");
			if (i == 0) {
				StringUtil::replace_all(s, checkRand, "  false  ");
			}
			else {
				StringUtil::replace_all(s, checkRand, "  true  ");
			}
			StringUtil::replace_all(s, " 0 ", "  false  ");
			StringUtil::replace_all(s, " 1 ", "  true  ");

			finalString.append(s+"\n");
		}

		string s = "(assert(not (= num0 num1 ))) ";
		finalString.append(s+"\n");
		solverUse.from_string(finalString.data());

		bool flag=false;
		switch (solverUse.check()) {
			case z3::unsat:   //std::cout << "unsat\n";
			{
				clock_t c2 = clock();
				dontcareCalls++;
				dontcareTime += c2 - c1;
				flag = true;
				break;
			}
			case z3::sat:     //std::cout << "sat\n";
			{
				clock_t c3 = clock();
				semdCalls++;
				semdTime += c3 - c1;
				break;
			}
			case z3::unknown: //std::cout << "unknown\n";
				break;
		}

		return flag;

	}

	static void getNotCareRandomSet(Node node, set<string>& randSet) {

		set<string> rand;
		vector<string> randVector;
		Node::getRandSet(node, rand);
		SetUtil::SetToVector(rand, randVector);

		for(unsigned int i=0;i<randVector.size();i++){
			if(isNotCareRand(node,randVector[i])){
				randSet.insert(randVector[i]);
			}
		}
	}

	static void getNodeMap(Node* node, map<string, Node*>& nodeMap) {
	    if(node == NULL) {
	        return;
	    }
		nodeMap[node->getName()] = node;
	    getNodeMap(node->getLeftChild(), nodeMap);
	    getNodeMap(node->getRightChild(), nodeMap);

	}

	static int getHeight(Node* node) {
		if(node == NULL) {
			return 0;
		}
		return max(getHeight(node->getLeftChild()), getHeight(node->getRightChild())) + 1;
	}

	static void printTree(Node* node, int indent) {
		if(node == NULL)
			return;
		std::cout << regularStringInTree(node, indent) << "\n";
		printTree(node->getLeftChild(), indent + 2);
		printTree(node->getRightChild(), indent + 2);
	}

	static string regularStringInTree(Node* node, int indent) {
		string s = "";
		for(int i = 0; i < indent; i++)
			s += "-";
		s += node->getName();
		if(node->getNodeTypeEnums() == NodeTypeEnums::INTERMEDIATE) {
			switch(node->getOperatorEnums()) {
				case OperatorEnums::XOR:
					s += "(XOR)";
					break;
				case OperatorEnums::OR:
					s += "(OR)";
					break;
				case OperatorEnums::AND:
					s += "(AND)";
					break;
				case OperatorEnums::NOT:
					s += "(NOT)";
					break;
				default:
					s += "(D)";
			}
		}
		return s;
	}


};

int NodeUtil::semdCalls = 0;
int NodeUtil::dontcareCalls = 0;
double NodeUtil::semdTime = 0.0;
double NodeUtil::dontcareTime = 0.0;