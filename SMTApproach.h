#pragma once
#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <cassert>
#include <time.h>
#include "Node.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"
#include "DistributionInference.h"
#include "SetUtil.h"
#include "FileUtil.h"
#include "StringUtil.h"
#include "NodeUtil.h"
#include "SMT2Parse.h"
#include "StringUtil.h"

using namespace std;

class SMTApproach {
public:
	static void monolithicApproach(string benchmarkName) {
		cout << "Test for " + benchmarkName << endl;

		vector<string> RandV;
		vector<string> SecV;
		vector<string> PublicV;
		vector<string> InterV;
		vector<string> CodeV;

		vector<Node > RandNodeVector;
		vector<Node > SecNodeVector;
		vector<Node > PublicNodeVector;
		vector<Node > InterNodeVector;
		vector<Node > ConstantNodeVector;

		string RandFile = "TestBenchmark/" + benchmarkName + "/inRandList.txt";
		string SecFile = "TestBenchmark/" + benchmarkName + "/inSecList.txt";
		string PublicFile = "TestBenchmark/" + benchmarkName + "/inPubList.txt";
		string InterFile = "TestBenchmark/" + benchmarkName + "/inInterList.txt";
		string CodeFile = "TestBenchmark/" + benchmarkName + "/code.cpp";



		ifstream infile;
		string s;
		cout << RandFile << endl;
		infile.open(RandFile.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			RandV = StringUtil::SplitStringBySpace(s);
		}
		infile.close();

		cout << SecFile << endl;
		infile.open(SecFile.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			SecV = StringUtil::SplitStringBySpace(s);
		}
		infile.close();

		cout << PublicFile << endl;
		infile.open(PublicFile.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			PublicV = StringUtil::SplitStringBySpace(s);
		}
		infile.close();

		cout << InterFile << endl;
		infile.open(InterFile.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			InterV = StringUtil::SplitStringBySpace(s);
		}
		infile.close();





		for (unsigned int i = 0; i < RandV.size(); i++) {
			Node n = Node(RandV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
			n.setDistributionEnums(DistributionEnums::RUD);
			RandNodeVector.push_back(n);
		}

		for (unsigned int i = 0; i < SecV.size(); i++) {
			Node n = Node(SecV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::SECRECT);
			n.setDistributionEnums(DistributionEnums::UKD);
			SecNodeVector.push_back(n);
		}

		for (unsigned int i = 0; i < PublicV.size(); i++) {
			Node n = Node(PublicV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::PLAIN);
			n.setDistributionEnums(DistributionEnums::UKD);
			PublicNodeVector.push_back(n);
		}

		for (unsigned int i = 0; i < InterV.size(); i++) {
			Node n = Node(InterV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::INTERMEDIATE);
			InterNodeVector.push_back(n);
		}

		Node t = Node("1", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
		t.setDistributionEnums(DistributionEnums::CST);
		Node f = Node("0", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
		f.setDistributionEnums(DistributionEnums::CST);
		ConstantNodeVector.push_back(t);
		ConstantNodeVector.push_back(f);


		/*
		cout << "node over" << endl;
		cout << InterNodeVector.size() << endl;
		for (unsigned int i = 0; i < InterNodeVector.size(); i++) {
		cout << InterNodeVector[i].getName() << endl;

		}
		for (unsigned int i = 0; i <PublicNodeVector.size(); i++) {
		cout << PublicNodeVector[i].toString() << endl;

		}
		*/



		cout << CodeFile << endl;
		infile.open(CodeFile.data());   //link the file object and the file stream object 
		assert(infile.is_open());   //test if read a file failed
		while (getline(infile, s))
		{
			if (s.find("=") != -1) {
				CodeV.push_back(s);
			}
		}
		infile.close();

		/*
		Node* node = NodeUtil::findNode(InterNodeVector, "n02");
		cout << node->toString() << endl;
		*/

		//cout << CodeV.size() << endl;
		for (unsigned int i = 0; i < CodeV.size(); i++) {
			string str = CodeV[i];
			str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
			StringUtil::trim(str);
			string temp[3];
			vector<string> v1;
			vector<string> v2;

			if (str.find("^") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "^");
				temp[1] = v2[0];
				temp[2] = v2[1];

				Node *leftNode = new Node();
				Node *rightNode = new Node();
				if (NodeUtil::findNode(InterNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(InterNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(SecNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(RandNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(ConstantNodeVector, temp[1]);
				}
				else {
					leftNode = NodeUtil::findNode(PublicNodeVector, temp[1]);
				}

				if (NodeUtil::findNode(InterNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(InterNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(SecNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(RandNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(ConstantNodeVector, temp[2]);
				}
				else {
					rightNode = NodeUtil::findNode(PublicNodeVector, temp[2]);
				}

				NodeUtil::findNode(InterNodeVector, temp[0])->setLeftChild(leftNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setRightChild(rightNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setOperatorEnums(OperatorEnums::XOR);

			}
			else if (str.find("&") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");

				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "&");
				temp[1] = v2[0];
				temp[2] = v2[1];

				Node *leftNode = new Node();
				Node *rightNode = new Node();
				if (NodeUtil::findNode(InterNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(InterNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(SecNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(RandNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(ConstantNodeVector, temp[1]);
				}
				else {
					leftNode = NodeUtil::findNode(PublicNodeVector, temp[1]);
				}

				if (NodeUtil::findNode(InterNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(InterNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(SecNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(RandNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(ConstantNodeVector, temp[2]);
				}
				else {
					rightNode = NodeUtil::findNode(PublicNodeVector, temp[2]);
				}

				NodeUtil::findNode(InterNodeVector, temp[0])->setLeftChild(leftNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setRightChild(rightNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setOperatorEnums(OperatorEnums::AND);

			}
			else if (str.find("|") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "|");
				temp[1] = v2[0];
				temp[2] = v2[1];

				Node *leftNode = new Node();
				Node *rightNode = new Node();
				if (NodeUtil::findNode(InterNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(InterNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(SecNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(RandNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(ConstantNodeVector, temp[1]);
				}
				else {
					leftNode = NodeUtil::findNode(PublicNodeVector, temp[1]);
				}

				if (NodeUtil::findNode(InterNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(InterNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(SecNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(RandNodeVector, temp[2]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[2]) != NULL) {
					rightNode = NodeUtil::findNode(ConstantNodeVector, temp[2]);
				}
				else {
					rightNode = NodeUtil::findNode(PublicNodeVector, temp[2]);
				}

				NodeUtil::findNode(InterNodeVector, temp[0])->setLeftChild(leftNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setRightChild(rightNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setOperatorEnums(OperatorEnums::OR);

			}
			else if (str.find("~") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				str = v1[1];
				str.erase(std::remove(str.begin(), str.end(), '~'), str.end());
				temp[1] = str;

				Node *leftNode = new Node();
				if (NodeUtil::findNode(InterNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(InterNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(SecNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(RandNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(ConstantNodeVector, temp[1]);
				}
				else {
					leftNode = NodeUtil::findNode(PublicNodeVector, temp[1]);
				}

				NodeUtil::findNode(InterNodeVector, temp[0])->setLeftChild(leftNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setRightChild(NULL);
				NodeUtil::findNode(InterNodeVector, temp[0])->setOperatorEnums(OperatorEnums::NOT);
			}
			else {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				temp[1] = v1[1];

				Node *leftNode = new Node();
				if (NodeUtil::findNode(InterNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(InterNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(SecNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(SecNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(RandNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(RandNodeVector, temp[1]);
				}
				else if (NodeUtil::findNode(ConstantNodeVector, temp[1]) != NULL) {
					leftNode = NodeUtil::findNode(ConstantNodeVector, temp[1]);
				}
				else {
					leftNode = NodeUtil::findNode(PublicNodeVector, temp[1]);
				}


				NodeUtil::findNode(InterNodeVector, temp[0])->setLeftChild(leftNode);
				NodeUtil::findNode(InterNodeVector, temp[0])->setRightChild(NULL);
				//NodeUtil::findNode(InterNodeVector, temp[0])->setOperatorEnums(OperatorEnums::NOT);
				cout << "we don't add now" << endl;
			}

		}


		cout << "AST finish" << endl;
		cout << "" << endl;
		
		/*
		for (unsigned int i = 0; i < InterNodeVector.size(); i++) {
			cout << SMT2Parse::getFirstString(InterNodeVector[i]) << endl;
			cout << SMT2Parse::parseNode(InterNodeVector[i]) << endl;
			cout << "" << endl;
			cout << "" << endl;
			cout << "" << endl;
			cout << "" << endl;
		}
		*/

		
		
		for (unsigned int i = 0; i < InterNodeVector.size(); i++) {
			string fileName = "TestBenchmark\\" + benchmarkName + "\\smt_result\\node_";
			fileName.append(InterNodeVector[i].getName() + ".smt2");
			SMT2Parse::nodeToSmtFile(InterNodeVector[i], fileName);
			string outFileName = "  >> TestBenchmark\\" + benchmarkName + "\\smt_result\\sat.txt";
			string path = "z3\\z3.exe "+fileName+outFileName;
			int b = system(path.data());
			cout << "" << endl;
			cout << "" << endl;
		}
		
		
	}

	static bool needToBeChecked(Node node) {
		set<string> *secret = new set<string>();
		node.getSecretSet(node,*secret);
		if (secret->size() == 0) {
			//cout << "No Secret Variable Check" << endl;
			return false;
		}else if (threeConditionCheck(&node) == true) {
			//cout << "three condition statisfy" << endl;
			return false;
		}
		
		return true;
	}

	static bool threeConditionCheck(Node *node) {
		Node* leftChild;
		Node* rightChild;
		

		if (node->getOperatorEnums() == OperatorEnums::NOT ||node->getOperatorEnums()==OperatorEnums::EQUAL) {
			return true;
		}
		else {
			leftChild = node->getLeftChild();
			rightChild = node->getRightChild();
			if ( leftChild->getDistributionEnums() != DistributionEnums::SID) {
				return false;
			}
			else if (rightChild->getDistributionEnums() != DistributionEnums::SID) {
				return false;
			}
			else if (leftChild->getNodeTypeEnums()==NodeTypeEnums::SECRECT || rightChild->getNodeTypeEnums()==NodeTypeEnums::SECRECT) {
				return false;
			}
			set<string> *leftSupportV = new set<string>();
			set<string> *rightSupportV = new set<string>();
			set<string> *nodeRand = new set<string>();
			set<string> *temp = new set<string>();
			set<string> *temp1 = new set<string>();
			set<string> *temp2 = new set<string>();

			leftSupportV = leftChild->getSupportV();
			rightSupportV = rightChild->getSupportV();
			Node::getRandSet(*node,*nodeRand);
			SetUtil::Intersection(*nodeRand, *leftSupportV, *temp1);
			SetUtil::Intersection(*nodeRand, *rightSupportV, *temp2);
			SetUtil::Intersection(*temp1, *temp2, *temp);
			if (temp->size() == 0) {
				return true;
			}
			else {
				return false;
			}
			
		}
		return true;
	}

	static void incrementalApproach(string benchmarkName) {

		//cout << "Test for " + benchmarkName << endl;
		clock_t start_time = clock();

		vector<string> RandV;
		vector<string> SecV;
		vector<string> PublicV;
		vector<string> InterV;
		vector<string> CodeV;

		vector<Node > RandNodeVector;
		vector<Node > SecNodeVector;
		vector<Node > PublicNodeVector;
		vector<Node > InterNodeVector;
		vector<Node > ConstantNodeVector;

		map<string, Node> nodeMap;

		string RandFile = benchmarkName + "/inRandList.txt";
		string SecFile = benchmarkName + "/inSecList.txt";
		string PublicFile =  benchmarkName + "/inPubList.txt";
		string CodeFile =  benchmarkName + "/code.cpp";

		ifstream infile;
		FileUtil::readFileToNodeVector(RandFile, RandV);
		FileUtil::readFileToNodeVector(SecFile, SecV);
		FileUtil::readFileToNodeVector(PublicFile, PublicV);
		FileUtil::readFileToCodeVector(CodeFile, CodeV);

		double time1=0.0;
		double time2=0.0;

		for (unsigned int i = 0; i < RandV.size(); i++) {
			Node n = Node(RandV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::MASK);
			nodeMap[RandV[i]] = n;
		}

		for (unsigned int i = 0; i < SecV.size(); i++) {
			Node n = Node(SecV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::SECRECT);
			nodeMap[SecV[i]] = n;
		}

		for (unsigned int i = 0; i < PublicV.size(); i++) {
			Node n = Node(PublicV[i], OperatorEnums::NULLOPERATOR, NodeTypeEnums::PLAIN);
			nodeMap[PublicV[i]] = n;
		}


		Node t = Node("1", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
		Node f = Node("0", OperatorEnums::NULLOPERATOR, NodeTypeEnums::CONSTANT);
		nodeMap["1"] = t;
		nodeMap["0"] = f;


		vector<string> v;
		int rudCount = 0;
		int sidCount = 0;
		int cstCount = 0;
		int ukdCount = 0;
		int nullCount = 0;
		int npmCount = 0;
		int nodeSmtCheckCount = 0;
		ofstream fout;
		fout.open(benchmarkName + "/smt_result/smtResult.txt");

		cout<<"start, hold on..."<<endl;
		for (unsigned int i = 0; i < CodeV.size(); i++) {
			string str = CodeV[i];
			str.erase(std::remove(str.begin(), str.end(), ';'), str.end());
			StringUtil::trim(str);

			string temp[3];
			vector<string> v1;
			vector<string> v2;

			//add inter node one by one to nodeMap
			v.clear();
			StringUtil::SplitString(str, v, "=");
			InterV.push_back(v[0]);
			Node n = Node(v[0], OperatorEnums::NULLOPERATOR, NodeTypeEnums::INTERMEDIATE);
			nodeMap[InterV[i]] = n;


			if (str.find("^") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "^");
				temp[1] = v2[0];
				temp[2] = v2[1];

				nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
				nodeMap[temp[0]].setRightChild(&nodeMap[temp[2]]);
				nodeMap[temp[0]].setOperatorEnums(OperatorEnums::XOR);
			}
			else if (str.find("&") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");

				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "&");
				temp[1] = v2[0];
				temp[2] = v2[1];

				nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
				nodeMap[temp[0]].setRightChild(&nodeMap[temp[2]]);
				nodeMap[temp[0]].setOperatorEnums(OperatorEnums::AND);

			}
			else if (str.find("|") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				StringUtil::SplitString(v1[1], v2, "|");
				temp[1] = v2[0];
				temp[2] = v2[1];

				nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
				nodeMap[temp[0]].setRightChild(&nodeMap[temp[2]]);
				nodeMap[temp[0]].setOperatorEnums(OperatorEnums::OR);

			}
			else if (str.find("~") != -1) {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				str = v1[1];
				str.erase(std::remove(str.begin(), str.end(), '~'), str.end());
				temp[1] = str;

				nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
				nodeMap[temp[0]].setRightChild(NULL);
				nodeMap[temp[0]].setOperatorEnums(OperatorEnums::NOT);
				nodeMap[temp[0]].setDistributionEnums(nodeMap[temp[1]].getDistributionEnums());
			}
			else {
				v1.clear();
				v2.clear();
				StringUtil::SplitString(str, v1, "=");
				temp[0] = v1[0];
				temp[1] = v1[1];

				nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
				nodeMap[temp[0]].setRightChild(NULL);
				nodeMap[temp[0]].setOperatorEnums(OperatorEnums::EQUAL);
				nodeMap[temp[0]].setDistributionEnums(nodeMap[temp[1]].getDistributionEnums());
			}

			NodeUtil::getAuxiliaryTable(&nodeMap[InterV[i]]);

			if(nodeMap[InterV[i]].getOperatorEnums()==OperatorEnums::NOT || nodeMap[InterV[i]].getOperatorEnums()==OperatorEnums::EQUAL){
				nodeMap[InterV[i]].setDistributionEnums(nodeMap[InterV[i]].getLeftChild()->getDistributionEnums());

			}
            if(nodeMap[InterV[i]].getDistributionEnums()==DistributionEnums::NULLDISTRIBUTION || nodeMap[InterV[i]].getDistributionEnums()==DistributionEnums::UKD){
				Node tempNode;
				NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


				if(nodeMap[InterV[i]].getUniqueM()->size()>0 && needToBeChecked(tempNode)){
					NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);

				}

				if (needToBeChecked(tempNode)) {
					nodeSmtCheckCount = nodeSmtCheckCount + 1;
					DistributionEnums dis;
					clock_t t1 = clock();
					set<string> *tempNotCareRand=new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*tempNotCareRand);
					clock_t t2= clock();

					clock_t t3 = clock();
					dis=SMT2Parse::getDistributionByZ3(tempNode,*tempNotCareRand);
					clock_t t4 = clock();

					time1=time1+(t2-t1);
					time2=time2+(t4-t3);
					nodeMap[InterV[i]].setDistributionEnums(dis);
				}
				else {
					nodeMap[InterV[i]].setDistributionEnums(DistributionEnums::SID);

				}


			}


			if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::RUD) {
				rudCount = rudCount + 1;
			}
			else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::SID) {
				sidCount = sidCount + 1;
			}
			else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::UKD) {
				ukdCount = ukdCount + 1;
			}
			else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::CST) {
				cstCount = cstCount + 1;
			}
			else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::NPM) {
				npmCount = npmCount + 1;
			}
			else {
				nullCount = nullCount + 1;
			}


			string s = "";
			s = "Node: " + InterV[i] + " , Distribution: " + EnumUtil::distributionToString(nodeMap[InterV[i]].getDistributionEnums());
			fout << s << "\n";
		}


		clock_t end_time = clock();
		//cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
		double timeCount = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
		//cout << timeCount << endl;
		string s = "";
		s = "All node count is: " + StringUtil::getString(InterV.size());
		fout << s << "\n";
		s = "Node SMT check count: " + StringUtil::getString(nodeSmtCheckCount);
		fout << s << "\n";
		s = "RUD count: " + StringUtil::getString(rudCount);
		fout << s << "\n";
		s = "SID count: " + StringUtil::getString(sidCount);
		fout << s << "\n";
		s = "CST count: " + StringUtil::getString(cstCount);
		fout << s << "\n";
		s = "NPM count: " + StringUtil::getString(npmCount);
		fout << s << "\n";
		s = "UKD count: " + StringUtil::getString(ukdCount);
		fout << s << "\n";
		s = "NULL count: " + StringUtil::getString(nullCount);
		fout << s << "\n";
		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";

		s = "Time1 is : " + std::to_string(time1/1000) + "ms";
		fout << s << "\n";

		s = "Time2 is : " + std::to_string(time2/1000) + "ms";
		fout << s << "\n";

		fout << flush;
		fout.close();


	}

	

};