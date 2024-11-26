#include <iostream>
#include <sstream>
#include <map>
#include "QmsStatisfy.h"
#include "DistributionEnums.h"
#include "StringUtil.h"
#include "SetUtil.h"
#include "z3++.h"

using namespace std;

string QmsStatisfy::getFirstString(Node n) {
	string s="";
	string leftName, rightName;
	if (n.getLeftChild() != NULL) {
		leftName = n.getLeftChild()->getName();
	}
	if (n.getRightChild() != NULL) {
		rightName = n.getRightChild()->getName();
	}

	if (n.getOperatorEnums() == OperatorEnums::AND) {
		s.append("( and " + leftName + " " + rightName + " )");
	}else if (n.getOperatorEnums() == OperatorEnums::OR) {
		s.append("( or " + leftName + " " + rightName + " )");
	}else if (n.getOperatorEnums() == OperatorEnums::XOR) {
		s.append("( xor " + leftName + " " + rightName + " )");
	}else if (n.getOperatorEnums() == OperatorEnums::NOT) {
		s.append("( not " + leftName + " " + rightName + " )");
	}
	else {
		s.append(" " + leftName + " " + rightName + " ");
	}
	return s;
}

string QmsStatisfy::parseNode(Node n) {
	string s = getFirstString(n);
	if (n.getNodeTypeEnums() == NodeTypeEnums::MASK || n.getNodeTypeEnums() == NodeTypeEnums::SECRECT || n.getNodeTypeEnums()==NodeTypeEnums::PLAIN || n.getNodeTypeEnums()==NodeTypeEnums::CONSTANT) {
		return n.getName();
	}
	if (n.getLeftChild() != NULL) {
		string ss = parseNode(*n.getLeftChild());
		s = s.replace(s.find(n.getLeftChild()->getName()), n.getLeftChild()->getName().size(), ss);
	}
	if (n.getRightChild() != NULL) {
		string ss = parseNode(*n.getRightChild());
		s = s.replace(s.find(n.getRightChild()->getName()), n.getRightChild()->getName().size(), ss);
	}
	return s;
}


bool QmsStatisfy::isQmsStatisfy(Node node,double qms) {
	cout<<"hello1"<<endl;
	set<string> rand;
	set<string> secret;
	set<string> plain;
	vector<string> randVector;
	vector<string> secretVector;
	vector<string> plainVector;
	vector<string> num1;
	vector<string> num2;
	map<string, string> randMap;
	map<string, string> secretMap1;
	map<string, string> secretMap2;
	map<string, string> plainMap;


	Node::getRandSet(node, rand);
	Node::getSecretSet(node, secret);
	Node::getPlainSet(node, plain);

	SetUtil::SetToVector(rand, randVector);
	SetUtil::SetToVector(secret, secretVector);
	SetUtil::SetToVector(plain, plainVector);



	int randCount = rand.size();
	int secretCount = secret.size();
	int plainCount = plain.size();
	int num = 1;//record for all rand power set size
	for (int i = 0; i < randCount; i++) {
		num = num * 2;
	}

	string first = "This is node " + node.getName() + " smt2 file.";


	string finalString="";
	z3::context contextUse;
	z3::solver solverUse(contextUse);

	for(int i=1;i<=2;i++){
		string s = "";
		s.append("(declare-fun num");
		s.append(StringUtil::getString(i) + "() Int )");


		finalString.append(s+"\n");
	}


	//declare variable for smt2
	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun r");
		s.append(StringUtil::getString(i) + "() Bool )");


		finalString.append(s+"\n");
	}

	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun rr");
		s.append(StringUtil::getString(i) + "() Bool )");


		finalString.append(s+"\n");
	}
	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun n");
		s.append(StringUtil::getString(i) + "() Int )");
		num1.push_back("n" + StringUtil::getString(i));


		finalString.append(s+"\n");
	}
	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun nn");
		s.append(StringUtil::getString(i) + "() Int )");
		num2.push_back("nn" + StringUtil::getString(i));


		finalString.append(s+"\n");
	}
	for (int i = 0; i < secretCount; i++) {
		string s = "";
		s.append("(declare-fun k");
		s.append(StringUtil::getString(i) + "() Bool )");
		secretMap1[secretVector[i]] = " k" + StringUtil::getString(i) + " ";


		finalString.append(s+"\n");
	}
	for (int i = 0; i < secretCount; i++) {
		string s = "";
		s.append("(declare-fun kk");
		s.append(StringUtil::getString(i) + "() Bool )");
		secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i) + " ";


		finalString.append(s+"\n");
	}
	for (int i = 0; i < plainCount; i++) {
		string s = "";
		s.append("(declare-fun ");
		s.append(plainVector[i] + "() Bool )");
		//plainMap[plainVector[i]] = "x" + StringUtil::getString(i);


		finalString.append(s+"\n");
	}



	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert(= r" + StringUtil::getString(i));
		string ss = parseNode(node);
		StringUtil::replaceThroughMap(ss, secretVector, secretMap1);

		vector<int> temp;
		StringUtil::IntToBinaryVector(i, temp, randCount);
		StringUtil::BitToBoolVector(randVector, temp, randMap);
		StringUtil::replaceThroughMap(ss, randVector, randMap);
		s.append(ss + " ))");
		StringUtil::replace_all(s, " 0 ", " false ");
		StringUtil::replace_all(s, " 1 ", " true ");


		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert(= rr" + StringUtil::getString(i));
		string ss = parseNode(node);
		StringUtil::replaceThroughMap(ss, secretVector, secretMap2);

		vector<int> temp;
		StringUtil::IntToBinaryVector(i, temp, randCount);
		StringUtil::BitToBoolVector(randVector, temp, randMap);
		StringUtil::replaceThroughMap(ss, randVector, randMap);
		s.append(ss + "))");
		StringUtil::replace_all(s, " 0 ", " false ");
		StringUtil::replace_all(s, " 1 ", " true ");


		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert (or  (and (= n" + StringUtil::getString(i) + " 1)");
		s.append(" r" + StringUtil::getString(i) + ")   (and (= n" + StringUtil::getString(i) + " 0)");
		s.append("(not r" + StringUtil::getString(i) + "))  ))");


		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert (or  (and (= nn" + StringUtil::getString(i) + " 1)");
		s.append(" rr" + StringUtil::getString(i) + ")   (and (= nn" + StringUtil::getString(i) + " 0)");
		s.append("(not rr" + StringUtil::getString(i) + "))  ))");


		finalString.append(s+"\n");
	}

	string s1, s2;
	StringUtil::VectorLogicAdd(num1, s1);
	StringUtil::VectorLogicAdd(num2, s2);

	string temp1,temp2;
	temp1.append("(assert (= num1 ");
	temp1.append(s1+" ))");
	finalString.append(temp1+"\n");
	temp2.append("(assert (= num2 ");
	temp2.append(s2+" ))");
	finalString.append(temp2+"\n");

	double qmsTemp=qms*num;
	string s = "(assert (or (> (- num1 num2) ";
	s.append("(to_real " + std::to_string(qmsTemp)+")) (> (- num2 num1) ");
	s.append("(to_real "+std::to_string(qmsTemp)+")))) ");

	finalString.append(s+"\n");
	solverUse.from_string(finalString.data());

	cout<<finalString<<endl;

	bool flag=false;
	switch (solverUse.check()) {
		case z3::unsat:
				flag=true; break;
		case z3::sat:
				break;
		case z3::unknown:
				break;
	}

	return flag;


}

bool QmsStatisfy::isQmsStatisfyWithNotCares(Node node,set<string> notCareRandSet,double qms) {
	if(qms==0){
		return true;
	}

	set<string> rand;
	set<string> tempRand;
	set<string> secret;
	set<string> plain;
	vector<string> randVector;
	vector<string> secretVector;
	vector<string> plainVector;
	vector<string> notCareRandVector;
	vector<string> num1;
	vector<string> num2;
	map<string, string> randMap;
	map<string, string> secretMap1;
	map<string, string> secretMap2;
	map<string, string> plainMap;


	Node::getRandSet(node, tempRand);
	SetUtil::Difference(tempRand,notCareRandSet,rand);
	Node::getSecretSet(node, secret);
	Node::getPlainSet(node, plain);


	SetUtil::SetToVector(rand, randVector);
	SetUtil::SetToVector(secret, secretVector);
	SetUtil::SetToVector(plain, plainVector);
	SetUtil::SetToVector(notCareRandSet,notCareRandVector);



	int randCount = rand.size();
	int secretCount = secret.size();
	int plainCount = plain.size();
	int num = 1;//record for all rand power set size
	for (int i = 0; i < randCount; i++) {
		num = num * 2;
	}

	string finalString="";
	z3::context contextUse;
	z3::solver solverUse(contextUse);


	for(int i=1;i<=2;i++){
		string s = "";
		s.append("(declare-fun num");
		s.append(StringUtil::getString(i) + "() Int )");
		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun r");
		s.append(StringUtil::getString(i) + "() Bool )");

		finalString.append(s+"\n");
	}

	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun rr");
		s.append(StringUtil::getString(i) + "() Bool )");

		finalString.append(s+"\n");
	}
	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun n");
		s.append(StringUtil::getString(i) + "() Int )");
		num1.push_back("n" + StringUtil::getString(i));

		finalString.append(s+"\n");
	}
	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(declare-fun nn");
		s.append(StringUtil::getString(i) + "() Int )");
		num2.push_back("nn" + StringUtil::getString(i));

		finalString.append(s+"\n");
	}
	for (int i = 0; i < secretCount; i++) {
		string s = "";
		s.append("(declare-fun k");
		s.append(StringUtil::getString(i) + "() Bool )");
		secretMap1[secretVector[i]] = " k" + StringUtil::getString(i) + " ";

		finalString.append(s+"\n");
	}
	for (int i = 0; i < secretCount; i++) {
		string s = "";
		s.append("(declare-fun kk");
		s.append(StringUtil::getString(i) + "() Bool )");
		secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i) + " ";

		finalString.append(s+"\n");
	}
	for (int i = 0; i < plainCount; i++) {
		string s = "";
		s.append("(declare-fun ");
		s.append(plainVector[i] + "() Bool )");

		finalString.append(s+"\n");
	}



	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert(= r" + StringUtil::getString(i));
		string ss = parseNode(node);
		StringUtil::replaceThroughMap(ss, secretVector, secretMap1);
		vector<int> temp;
		StringUtil::IntToBinaryVector(i, temp, randCount);
		StringUtil::BitToBoolVector(randVector, temp, randMap);
		StringUtil::replaceThroughMap(ss, randVector, randMap);

		for(unsigned int m=0;m<notCareRandSet.size();m++){
			StringUtil::replace_all(ss, notCareRandVector[m], " false ");
		}

		s.append("  "+ss + " ))");
		StringUtil::replace_all(s, " 0 ", " false ");
		StringUtil::replace_all(s, " 1 ", " true ");


		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert(= rr" + StringUtil::getString(i));
		string ss = parseNode(node);
		StringUtil::replaceThroughMap(ss, secretVector, secretMap2);
		vector<int> temp;
		StringUtil::IntToBinaryVector(i, temp, randCount);
		StringUtil::BitToBoolVector(randVector, temp, randMap);
		StringUtil::replaceThroughMap(ss, randVector, randMap);
		for(unsigned int m=0;m<notCareRandSet.size();m++){
			StringUtil::replace_all(ss, notCareRandVector[m], " false ");
		}
		s.append(" "+ss + "))");
		StringUtil::replace_all(s, " 0 ", " false ");
		StringUtil::replace_all(s, " 1 ", " true ");

		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert (or  (and (= n" + StringUtil::getString(i) + " 1)");
		s.append(" r" + StringUtil::getString(i) + ")   (and (= n" + StringUtil::getString(i) + " 0)");
		s.append("(not r" + StringUtil::getString(i) + "))  ))");

		finalString.append(s+"\n");
	}


	for (int i = 0; i < num; i++) {
		string s = "";
		s.append("(assert (or  (and (= nn" + StringUtil::getString(i) + " 1)");
		s.append(" rr" + StringUtil::getString(i) + ")   (and (= nn" + StringUtil::getString(i) + " 0)");
		s.append("(not rr" + StringUtil::getString(i) + "))  ))");

		finalString.append(s+"\n");
	}



	string s1, s2;
	StringUtil::VectorLogicAdd(num1, s1);
	StringUtil::VectorLogicAdd(num2, s2);

	string temp1,temp2;
	temp1.append("(assert (= num1 ");
	temp1.append(s1+" ))");
	finalString.append(temp1+"\n");
	temp2.append("(assert (= num2 ");
	temp2.append(s2+" ))");
	finalString.append(temp2+"\n");


	double qmsTemp=(1-qms)*num;


	string s = "(assert (or (> (- num1 num2) ";
	s.append("(to_real " + std::to_string(qmsTemp)+")) (> (- num2 num1) ");
	s.append("(to_real "+std::to_string(qmsTemp)+")))) ");



	finalString.append(s+"\n");
	solverUse.from_string(finalString.data());

	bool flag=false;
	switch (solverUse.check()) {
		case z3::unsat:
			flag=true; break;
		case z3::sat:
			break;
		case z3::unknown:
			break;
	}
	solverUse.reset();

	return flag;


}