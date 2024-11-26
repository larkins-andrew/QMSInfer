#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <cassert>
#include <map>
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
#include "SemanticDistributionInference.h"
#include "SymbolicApproach.h"
using namespace std;

void SymbolicApproach::symbolicApproach(string benchmarkName) {
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

	string RandFile =  benchmarkName + "/inRandList.txt";
	string SecFile =  benchmarkName + "/inSecList.txt";
	string PublicFile = benchmarkName + "/inPubList.txt";
	string CodeFile =  benchmarkName + "/code.cpp";

	ifstream infile;
	FileUtil::readFileToNodeVector(RandFile, RandV);
	FileUtil::readFileToNodeVector(SecFile, SecV);
	FileUtil::readFileToNodeVector(PublicFile, PublicV);
	FileUtil::readFileToCodeVector(CodeFile, CodeV);

	
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

	set<string> M;
	for (unsigned int i = 0; i < RandV.size(); i++) {
		M.insert(RandV[i]);
	}



	set<string>  randSet;

	vector<string> v;
	int rudCount = 0;
	int isdCount = 0;
	int cstCount = 0;
	int ukdCount = 0;
	int nullCount = 0;

	ofstream fout;
	fout.open( benchmarkName + "/symbolic_result/symbolicResult.txt");

	cout << "start, hold on..." << endl;
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

			randSet.insert(temp[0]);
			randSet.insert(temp[1]);
			randSet.insert(temp[2]);

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

			randSet.insert(temp[0]);
			randSet.insert(temp[1]);
			randSet.insert(temp[2]);

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

			randSet.insert(temp[0]);
			randSet.insert(temp[1]);
			randSet.insert(temp[2]);

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

			randSet.insert(temp[0]);
			randSet.insert(temp[1]);
			randSet.insert(temp[2]);

			nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
			nodeMap[temp[0]].setRightChild(NULL);
			nodeMap[temp[0]].setOperatorEnums(OperatorEnums::NOT);
		}
		else {
			v1.clear();
			v2.clear();
			StringUtil::SplitString(str, v1, "=");
			temp[0] = v1[0];
			temp[1] = v1[1];

			randSet.insert(temp[0]);
			randSet.insert(temp[1]);
			//randSet.insert(temp[2]);

			nodeMap[temp[0]].setLeftChild(&nodeMap[temp[1]]);
			nodeMap[temp[0]].setRightChild(NULL);
		}


		DistributionInference::inference(&nodeMap[InterV[i]], M);
		if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::RUD) {
			rudCount = rudCount + 1;
		}
		else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::SID) {
			isdCount = isdCount + 1;
		}
		else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::UKD) {
			ukdCount = ukdCount + 1;
		}
		else if (nodeMap[InterV[i]].getDistributionEnums() == DistributionEnums::CST) {
			cstCount = cstCount + 1;
		}
		else {
			nullCount = nullCount + 1;
		}

		
		string s = "";
		s = "Node: " + nodeMap[InterV[i]].getName() + " , Distribution: " + EnumUtil::distributionToString(nodeMap[InterV[i]].getDistributionEnums());
		fout << s << "\n";

	}

	clock_t end_time = clock();
	double timeCount = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	string s = "";
	s = "All node count is: " + StringUtil::getString(InterV.size());
	fout << s << "\n";
	s = "RUD count: " + StringUtil::getString(rudCount);
	fout << s << "\n";
	s = "ISD count: " + StringUtil::getString(isdCount);
	fout << s << "\n";
	s = "CST count: " + StringUtil::getString(cstCount);
	fout << s << "\n";
	s = "UKD count: " + StringUtil::getString(ukdCount);
	fout << s << "\n";
	s = "NULL count: " + StringUtil::getString(nullCount);
	fout << s << "\n";
	s = "Running time is : " + std::to_string(timeCount) + "ms";
	fout << s << "\n";
	fout << flush;
	fout.close();

}