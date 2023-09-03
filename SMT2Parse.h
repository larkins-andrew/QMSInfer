#pragma once
#include <iostream>
#include <string>
#include <set>
#include <sstream>
#include <map>
#include "Node.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"
#include "DistributionInference.h"
#include "SetUtil.h"
#include "StringUtil.h"
#include "z3++.h"
using namespace std;

class SMT2Parse {
public:
	static int exportInt;


	static string&   replace_all(string&   str,const   string&   old_value,const   string&   new_value)
	{
		while(true)   {
			string::size_type   pos(0);
			if(   (pos=str.find(old_value))!=string::npos   )
				str.replace(pos,old_value.length(),new_value);
			else   break;
		}
		return   str;
	}


	static string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value)
	{
		for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length())   {
			if(   (pos=str.find(old_value,pos))!=string::npos   )
				str.replace(pos,old_value.length(),new_value);
			else   break;
		}
		return   str;
	}


	static string getSatFirstString(Node n){
		string s="";
		string leftName, rightName;
		if (n.getLeftChild() != NULL) {
			leftName = n.getLeftChild()->getName();
		}
		if (n.getRightChild() != NULL) {
			rightName = n.getRightChild()->getName();
		}

		if (n.getOperatorEnums() == OperatorEnums::AND) {
			s.append("( " + leftName + " & " + rightName + " )");
		}else if (n.getOperatorEnums() == OperatorEnums::OR) {
			s.append("( " + leftName + " | " + rightName + " )");
		}else if (n.getOperatorEnums() == OperatorEnums::XOR) {
			s.append("( ( ! " + leftName + " & " + rightName + " ) | ( "+leftName+" & ! "+rightName+" ) )");
		}else if (n.getOperatorEnums() == OperatorEnums::NOT) {
			s.append("( ! " + leftName + " " + rightName + " )");
		}
		else {
			s.append(" " + leftName + " " + rightName + " ");
		}
		return s;
	}


	static string getSatString(Node n) {
		string s = getSatFirstString(n);
		if (n.getNodeTypeEnums() == NodeTypeEnums::MASK || n.getNodeTypeEnums() == NodeTypeEnums::SECRECT || n.getNodeTypeEnums()==NodeTypeEnums::PLAIN || n.getNodeTypeEnums()==NodeTypeEnums::CONSTANT) {
			return n.getName();
		}
		if (n.getLeftChild() != NULL) {
			string ss = getSatString(*n.getLeftChild());
			replace_all_distinct(s,n.getLeftChild()->getName(),ss);
		}
		if (n.getRightChild() != NULL) {
			string ss = getSatString(*n.getRightChild());
			replace_all_distinct(s,n.getRightChild()->getName(),ss);
		}
		return s;
	}


	static string getFirstString(Node n) {
		string s="";
		string leftName, rightName;
		if (n.getLeftChild() != NULL) {
			leftName = n.getLeftChild()->getName();
		}
		if (n.getRightChild() != NULL) {
			rightName = n.getRightChild()->getName();
		}
		if (n.getOperatorEnums() == OperatorEnums::AND) {
			s.append("( and  " + leftName + "  " + rightName + "  )");
		}else if (n.getOperatorEnums() == OperatorEnums::OR) {
			s.append("( or  " + leftName + "  " + rightName + "  )");
		}else if (n.getOperatorEnums() == OperatorEnums::XOR) {
			s.append("( xor  " + leftName + "  " + rightName + "  )");
		}else if (n.getOperatorEnums() == OperatorEnums::NOT) {
			s.append("( not  " + leftName + "  " + rightName + "  )");
		}
		else {
			s.append("  " + leftName + "  " + rightName + "  ");
		}
		return s;
	}


	static string parseNode(Node n) {
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


	static void nodeToSmtFile(Node node, string outFilePath) {

		ofstream fout;
		fout.open(outFilePath);

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
		int num = 1;
		for (int i = 0; i < randCount; i++) {
			num = num * 2;
		}

		string first = "This is node " + node.getName() + " smt2 file.";
		fout << "; " + first << "\n";


		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun r");
			s.append(StringUtil::getString(i) + "() Bool )");
			fout << s << "\n";
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun rr");
			s.append(StringUtil::getString(i) + "() Bool )");
			fout << s << "\n";
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun n");
			s.append(StringUtil::getString(i) + "() Int )");
			num1.push_back("n" + StringUtil::getString(i));
			fout << s << "\n";
		}
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun nn");
			s.append(StringUtil::getString(i) + "() Int )");
			num2.push_back("nn" + StringUtil::getString(i));
			fout << s << "\n";
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun k");
			s.append(StringUtil::getString(i) + "() Bool )");
			secretMap1[secretVector[i]] = " k" + StringUtil::getString(i)+" ";
			fout << s << "\n";
		}
		for (int i = 0; i < secretCount; i++) {
			string s = "";
			s.append("(declare-fun kk");
			s.append(StringUtil::getString(i) + "() Bool )");
			secretMap2[secretVector[i]] = " kk" + StringUtil::getString(i)+" ";
			fout << s << "\n";
		}
		for (int i = 0; i < plainCount; i++) {
			string s = "";
			s.append("(declare-fun ");
			s.append(plainVector[i] + "() Bool )");
			fout << s << "\n";
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
			fout << s << "\n";
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
			fout << s << "\n";
		}


		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (or  (and (= n" + StringUtil::getString(i) + " 1)");
			s.append(" r" + StringUtil::getString(i) + ")   (and (= n" + StringUtil::getString(i) + " 0)");
			s.append("(not r" + StringUtil::getString(i) + "))  ))");
			fout << s << "\n";
		}


		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert (or  (and (= nn" + StringUtil::getString(i) + " 1)");
			s.append(" rr" + StringUtil::getString(i) + ")   (and (= nn" + StringUtil::getString(i) + " 0)");
			s.append("(not rr" + StringUtil::getString(i) + "))  ))");
			fout << s << "\n";
		}

		string s = "(assert(not (= ";
		string s1, s2;
		StringUtil::VectorLogicAdd(num1, s1);
		StringUtil::VectorLogicAdd(num2, s2);
		s.append(s1 + " ");
		s.append(s2);
		s.append(")))");
		fout << s << "\n";
		fout << "(check-sat)" << "\n";
		fout << flush;
		fout.close();

	}


	static DistributionEnums getDistributionByZ3(Node node) {
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
		int num = 1;
		for (int i = 0; i < randCount; i++) {
			num = num * 2;
		}

		string first = "This is node " + node.getName() + " smt2 file.";


		string finalString="";
		z3::context contextUse;
		z3::solver solverUse(contextUse);

        cout<<"hello3"<<endl;
        cout<<num<<endl;
		//declare variable for smt2
		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(declare-fun r");
			s.append(StringUtil::getString(i) + "() Bool )");


			finalString.append(s+"\n");
		}
        cout<<"hello4"<<endl;
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

		string s = "(assert(not (= ";
		string s1, s2;
		StringUtil::VectorLogicAdd(num1, s1);
		StringUtil::VectorLogicAdd(num2, s2);
		s.append(s1 + " ");
		s.append(s2);
		s.append(")))");


		finalString.append(s+"\n");
		solverUse.from_string(finalString.data());

		DistributionEnums dis;
		switch (solverUse.check()) {
			case z3::unsat:
				dis=DistributionEnums ::SID; break;
			case z3::sat:
				dis=DistributionEnums ::NPM; break;
			case z3::unknown:
				dis=DistributionEnums ::UKD; break;
		}
		return dis;

	}


	static DistributionEnums getDistributionByZ3(Node node,set<string> notCareRandSet) {

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
		int num = 1;
		for (int i = 0; i < randCount; i++) {
			num = num * 2;
		}

		string finalString="";
		z3::context contextUse;
		z3::solver solverUse(contextUse);


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
			s.append("(assert(= r" + StringUtil::getString(i)+" ");
			string ss = parseNode(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap1);
			vector<int> temp;
			StringUtil::IntToBinaryVector(i, temp, randCount);
			StringUtil::BitToBoolVector(randVector, temp, randMap);
			StringUtil::replaceThroughMap(ss, randVector, randMap);

			for(unsigned int m=0;m<notCareRandSet.size();m++){
				StringUtil::replace_all(ss, notCareRandVector[m], " false ");
			}

			s.append(ss + " ))");
			StringUtil::replace_all(s, " 0 ", " false ");
			StringUtil::replace_all(s, " 1 ", " true ");


			finalString.append(s+"\n");
		}


		for (int i = 0; i < num; i++) {
			string s = "";
			s.append("(assert(= rr" + StringUtil::getString(i)+" ");
			string ss = parseNode(node);
			StringUtil::replaceThroughMap(ss, secretVector, secretMap2);
			vector<int> temp;
			StringUtil::IntToBinaryVector(i, temp, randCount);
			StringUtil::BitToBoolVector(randVector, temp, randMap);
			StringUtil::replaceThroughMap(ss, randVector, randMap);
			for(unsigned int m=0;m<notCareRandSet.size();m++){
				StringUtil::replace_all(ss, notCareRandVector[m], " false ");
			}
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

		string s = "(assert(not (= ";
		string s1, s2;
		StringUtil::VectorLogicAdd(num1, s1);
		StringUtil::VectorLogicAdd(num2, s2);
		s.append(s1 + " ");
		s.append(s2);
		s.append(")))");

		finalString.append(s+"\n");
		solverUse.from_string(finalString.data());


		DistributionEnums dis;
		switch (solverUse.check()) {
			case z3::unsat:
				dis=DistributionEnums ::SID; break;
			case z3::sat:
				dis=DistributionEnums ::NPM; break;
			case z3::unknown:
				dis=DistributionEnums ::UKD; break;
		}

		solverUse.reset();

		return dis;


	}

};

int SMT2Parse::exportInt = 1;

