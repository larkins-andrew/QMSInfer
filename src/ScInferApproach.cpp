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
#include "SemanticDistributionInference.h"
#include "SetUtil.h"
#include "FileUtil.h"
#include "StringUtil.h"
#include "NodeUtil.h"
#include "SMT2Parse.h"
#include "ScInferApproach.h"
#include <deque>
using namespace std;

void ScInferApproach::scInferApproach(string absoluteBenchmarkPath) {
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

	string RandFile = absoluteBenchmarkPath + "/inRandList.txt";
	string SecFile = absoluteBenchmarkPath + "/inSecList.txt";
	string PublicFile = absoluteBenchmarkPath + "/inPubList.txt";
	string CodeFile = absoluteBenchmarkPath + "/code.cpp";

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


	vector<string> v;
	int rudCount = 0;
	int sidCount = 0;
	int cstCount = 0;
	int ukdCount = 0;
	int nullCount = 0;
	int npmCount = 0;
	int nodeSmtCheckCount = 0;
	int ukdBySemdR = 0;
	int ukdToNpm = 0;
	int ukdToSid = 0;

	double time1=0.0;
	double time2=0.0;
	double time3=0.0;

	ofstream fout,fout2;
	fout.open(absoluteBenchmarkPath + "/scInfer_result/scInferResult.txt");

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

			nodeMap[temp[1]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());
			nodeMap[temp[2]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());

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

			nodeMap[temp[1]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());
			nodeMap[temp[2]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());

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

			nodeMap[temp[1]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());
			nodeMap[temp[2]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());

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

			nodeMap[temp[1]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());
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

			nodeMap[temp[1]].getParentNodeNames()->insert(nodeMap[temp[0]].getName());
		}


		NodeUtil::getAuxiliaryTable(&nodeMap[InterV[i]]);

		SemanticDistributionInference::synInference(&nodeMap[InterV[i]]);

		if(nodeMap[InterV[i]].getDistributionEnums()==DistributionEnums::UKD){
			ukdBySemdR += 1;
			Node tempNode;
			NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


			if(nodeMap[InterV[i]].getUniqueM()->size()>0){
				NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);
			}

			clock_t t1 = clock();
			SemanticDistributionInference::semRule(&tempNode, &nodeMap[InterV[i]]);
			clock_t t2 = clock();

			if(tempNode.getDistributionEnums()==DistributionEnums::UKD){

				clock_t t3 = clock();


				set<string> * notCareRandSet=new set<string>();
				set<string> * randSet = new set<string>();
				set<string> * semdSet = new set<string>();
				NodeUtil::getNotCareRandomSet(tempNode,*notCareRandSet);
				Node::getRandSet(tempNode, *randSet);
				SetUtil::Difference(*randSet, *notCareRandSet, *semdSet);
				tempNode.setSemd(semdSet);
				nodeMap[InterV[i]].setSemd(semdSet);

				clock_t t4 = clock();


				nodeSmtCheckCount = nodeSmtCheckCount + 1;
				DistributionEnums dis;
				clock_t t5 = clock();
				dis=SMT2Parse::getDistributionByZ3(tempNode,*notCareRandSet);
				clock_t t6 = clock();
				nodeMap[InterV[i]].setDistributionEnums(dis);
				if(dis == DistributionEnums::NPM)
					ukdToNpm++;
				else if(dis == DistributionEnums::SID)
					ukdToSid++;

				time1=time1+(t2-t1);
				time2=time2+(t4-t3);
				time3=time3+(t6-t5);
			}else{
				nodeMap[InterV[i]].setDistributionEnums(tempNode.getDistributionEnums());
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
	s = "Node SemdR check count: " + StringUtil::getString(ukdBySemdR);
	fout << s << "\n";
	s = "Node ukd to npm count: " + StringUtil::getString(ukdToNpm);
	fout << s << "\n";
	s = "Node ukd to sid count: " + StringUtil::getString(ukdToSid);
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
	s = "SemRuleTime is : " + std::to_string(time1/1000) + "ms";
	fout << s << "\n";
	s = "NotCareSetTime is : " + std::to_string(time2/1000) + "ms";
	fout << s << "\n";
	s = "SMTTime is : " + std::to_string(time3/1000) + "ms";
	fout << s << "\n";
	s = "semdCallTime : " + std::to_string(NodeUtil::semdTime/1000) + "ms";
	fout << s << "\n";
	s = "dontcareTime : " + std::to_string(NodeUtil::dontcareTime/1000) + "ms";
	fout << s << "\n";
	s = "semdCall: " + StringUtil::getString(NodeUtil::semdCalls);
	fout << s << "\n";
	s = "dontcareCall: " + StringUtil::getString(NodeUtil::dontcareCalls);
	fout << s << "\n";
	s = "hasSemd: " + StringUtil::getString(SemanticDistributionInference::hasSemd);
	fout << s << "\n";
	s = "noSemd: " + StringUtil::getString(SemanticDistributionInference::noSemd);
	fout << s << "\n";
	fout << flush;
	fout.close();


}