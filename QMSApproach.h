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
#include "QmsStatisfy.h"

using namespace std;

class QMSApproach {
public:

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

    /*
     * check qms=0.5 by smt-based
     */
	static void smtBasedCheckQms(string benchmarkName) {

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

		int qmsBySmt = 0;
		int satCount=0;
		int unsatCount=0;
		ofstream fout;
		fout.open(benchmarkName + "/qms_result/smtQmsCheck.txt");

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
					qmsBySmt++;
					DistributionEnums dis;
					set<string> *tempNotCareRand=new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*tempNotCareRand);
					bool isStatisfy=QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*tempNotCareRand,0.5);
					if(isStatisfy== true){
						satCount=satCount+1;
					}else{
						unsatCount=unsatCount+1;
					}
					dis=SMT2Parse::getDistributionByZ3(tempNode,*tempNotCareRand);

					nodeMap[InterV[i]].setDistributionEnums(dis);
				}
				else {
					satCount=satCount+1;
					nodeMap[InterV[i]].setDistributionEnums(DistributionEnums::SID);

				}


			}else{
				satCount=satCount+1;
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
		s = "SAT count: " + StringUtil::getString(satCount);
		fout << s << "\n";
		s = "UNSAT count: " + StringUtil::getString(unsatCount);
		fout << s << "\n";
		s = "QMS by SMT: " + StringUtil::getString(qmsBySmt);
		fout << s << "\n";
		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";
		fout << flush;
		fout.close();


	}

	/*
	 * check qms=0.5cd by scinfer
	 */
    static void scinferBasedCheckQms(string absoluteBenchmarkPath) {
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
        int satCount=0;
        int unsatCount=0;
        int qmsBySmt=0;
        int pmBySmt=0;

        ofstream fout;
        fout.open(absoluteBenchmarkPath + "/qms_result/scinferQmsCheck.txt");

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
            /*
            cout<<"Node: "+InterV[i]+", supp is:";
            SetUtil::put_out(*nodeMap[InterV[i]].getSupportV());

            set<string> * notCareRandSet=new set<string>();
            NodeUtil::getNotCareRandomSet(nodeMap[InterV[i]],*notCareRandSet);
            cout<<"Node: "+InterV[i]+", not care rand is:";
            SetUtil::put_out(*notCareRandSet);
            */

            SemanticDistributionInference::synInference(&nodeMap[InterV[i]]);

            if(nodeMap[InterV[i]].getDistributionEnums()==DistributionEnums::UKD){
                Node tempNode;
                NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


                if(nodeMap[InterV[i]].getUniqueM()->size()>0){
                    NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);
                }


                SemanticDistributionInference::semRule(&tempNode, &nodeMap[InterV[i]]);

                if(tempNode.getDistributionEnums()==DistributionEnums::UKD){

                    set<string> * notCareRandSet=new set<string>();
                    set<string> * randSet = new set<string>();
                    set<string> * semdSet = new set<string>();
                    NodeUtil::getNotCareRandomSet(tempNode,*notCareRandSet);
                    Node::getRandSet(tempNode, *randSet);
                    SetUtil::Difference(*randSet, *notCareRandSet, *semdSet);
                    tempNode.setSemd(semdSet);
                    nodeMap[InterV[i]].setSemd(semdSet);

                    DistributionEnums dis;
                    dis=SMT2Parse::getDistributionByZ3(tempNode,*notCareRandSet);
                    nodeMap[InterV[i]].setDistributionEnums(dis);
                    if(dis == DistributionEnums::NPM) {
                        qmsBySmt++;
                        bool isStatisfy=QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*notCareRandSet,0.5);
                        if(isStatisfy==true){
                            satCount++;
                        } else{
                            unsatCount++;
                        }
                    }
                    else if(dis == DistributionEnums::SID){
                        pmBySmt++;
                        satCount++;
                    }

                }else{
                    satCount=satCount+1;
                    nodeMap[InterV[i]].setDistributionEnums(tempNode.getDistributionEnums());
                }

            } else{
                satCount=satCount+1;
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
        s = "SAT count: " + StringUtil::getString(satCount);
        fout << s << "\n";
        s = "UNSAT count: " + StringUtil::getString(unsatCount);
        fout << s << "\n";
        s = "QMS by SMT: " + StringUtil::getString(qmsBySmt);
        fout << s << "\n";
        s = "PM by SMT: " + StringUtil::getString(pmBySmt);
        fout << s << "\n";
        s = "Running time is : " + std::to_string(timeCount) + "ms";
        fout << s << "\n";
        fout << flush;
        fout.close();


    }


	/*
	 * computer every node's qms by smt-based
	 */
	static void smtBasedComputerQms(string benchmarkName) {

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

		int iter=0;
		double min=1.0;
		double max=1.0;
		double avg=0.0;
		double sum=0.0;

		ofstream fout;
		fout.open(benchmarkName + "/qms_result/smtBasedComputerQms.txt");

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
					DistributionEnums dis;
					set<string> *tempNotCareRand=new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*tempNotCareRand);
					dis=SMT2Parse::getDistributionByZ3(tempNode,*tempNotCareRand);
					nodeMap[InterV[i]].setDistributionEnums(dis);
					if(dis==DistributionEnums::SID){
						nodeMap[InterV[i]].setQms(1.0);
						sum=sum+1.0;
					}else{
                        //cout<<"Node: "+InterV[i]<<endl;
						double low=0.0;
						double high=1.0;
						double mid;
						while(low<=high){
							iter++;
							mid=(low+high)/2.0;
							//cout<<"low: "<<low<<endl;
							//cout<<"high: "<<high<<endl;
							//cout<<"mid: "<<mid<<endl;
							if(QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*tempNotCareRand,mid)){
								low=mid+0.01;
							}else{
								high=high-0.01;
							}

						}
						if(min>low){
							min=low;
						}
						sum=sum+low;
						nodeMap[InterV[i]].setQms(low);
					}
				}
				else {
					sum=sum+1.0;
					nodeMap[InterV[i]].setQms(1.0);
					nodeMap[InterV[i]].setDistributionEnums(DistributionEnums::SID);

				}

			}else{
				sum=sum+1.0;
				nodeMap[InterV[i]].setQms(1.0);
			}

			string s = "";
			s = "Node: " + InterV[i] + " , Distribution: " + EnumUtil::distributionToString(nodeMap[InterV[i]].getDistributionEnums());
			fout << s << "\n";
			s = "Node: " + InterV[i] + " , Qms: " + std::to_string(nodeMap[InterV[i]].getQms());
			fout << s << "\n";
		}

		avg=sum/InterV.size();


		clock_t end_time = clock();
		//cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
		double timeCount = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
		//cout << timeCount << endl;
		string s = "";
		s = "All node count is: " + StringUtil::getString(InterV.size());
		fout << s << "\n";
		s = "Iter is: " + StringUtil::getString(iter);
		fout << s << "\n";
		s = "Min is: " + std::to_string(min);
		fout << s << "\n";
		s = "Max is: " + std::to_string(max);
		fout << s << "\n";
		s = "Avg is: " + std::to_string(avg);
		fout << s << "\n";

		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";
		fout << flush;
		fout.close();


	}

	/*
	 * computer every node's qms by scInfer
	 */
	static void scInferComputerQms(string absoluteBenchmarkPath) {
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
		int iter=0;
        int smtCall=0;
        double qmsTime=0.0;
		double min=1.0;
		double max=1.0;
		double avg=0.0;
		double sum=0.0;

		ofstream fout;
		fout.open(absoluteBenchmarkPath + "/qms_result/scinferComputerQms.txt");

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
				Node tempNode;
				NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


				if(nodeMap[InterV[i]].getUniqueM()->size()>0){
					NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);
				}

				SemanticDistributionInference::semRule(&tempNode, &nodeMap[InterV[i]]);

				if(tempNode.getDistributionEnums()==DistributionEnums::UKD){

					set<string> * notCareRandSet=new set<string>();
					set<string> * randSet = new set<string>();
					set<string> * semdSet = new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*notCareRandSet);
					Node::getRandSet(tempNode, *randSet);
					SetUtil::Difference(*randSet, *notCareRandSet, *semdSet);
					tempNode.setSemd(semdSet);
					nodeMap[InterV[i]].setSemd(semdSet);

					DistributionEnums dis;
					dis=SMT2Parse::getDistributionByZ3(tempNode,*notCareRandSet);
                    smtCall++;
					nodeMap[InterV[i]].setDistributionEnums(dis);
					if(dis == DistributionEnums::NPM) {
						double step=1.0;
						double qms=1.0;
						if(semdSet->size()==0){
							qms=0;
						}else{
							for(unsigned int i=0;i<semdSet->size();i++){
								step=step/2.0;
							}
                            clock_t t1 = clock();
							while(true){
								//cout<<InterV[i]<<endl;
								//cout<<qms<<endl;
								iter++;
                                smtCall++;
								if(QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*notCareRandSet,qms)){
									break;
								}else{
									qms=qms-step;
								}
							}
                            clock_t t2 = clock();
                            qmsTime=t2-t1+qmsTime;
						}
						if(min>qms){
							min=qms;
						}
						sum=sum+qms;
						nodeMap[InterV[i]].setQms(qms);

					}else{
						sum=sum+1.0;
						nodeMap[InterV[i]].setQms(1.0);
					}

				}else{
					nodeMap[InterV[i]].setDistributionEnums(tempNode.getDistributionEnums());
					sum=sum+1.0;
					nodeMap[InterV[i]].setQms(1.0);
				}

			} else{
				sum=sum+1.0;
				nodeMap[InterV[i]].setQms(1.0);
			}


			string s = "";
			//s = "Node: " + InterV[i] + " , Distribution: " + EnumUtil::distributionToString(nodeMap[InterV[i]].getDistributionEnums());
			//fout << s << "\n";
			s = "Node: " + InterV[i] + " , Qms: " + std::to_string(nodeMap[InterV[i]].getQms());
			fout << s << "\n";
		}

		avg=sum/InterV.size();


		clock_t end_time = clock();
		//cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
		double timeCount = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
		//cout << timeCount << endl;
		string s = "";
		s = "All node count is: " + StringUtil::getString(InterV.size());
		fout << s << "\n";
		s = "Iter is: " + StringUtil::getString(iter);
		fout << s << "\n";
		s = "Min is: " + std::to_string(min);
		fout << s << "\n";
		s = "Max is: " + std::to_string(max);
		fout << s << "\n";
		s = "Avg is: " + std::to_string(avg);
		fout << s << "\n";
        s = "SMT calls is: " + StringUtil::getString(smtCall);
        fout << s << "\n";
        s = "Time for qms is: " + std::to_string(qmsTime/1000)+"ms";
        fout << s << "\n";
		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";
		fout << flush;
		fout.close();


	}


	static void minQmsInfer (string absoluteBenchmarkPath) {
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
		map<string, Node> ukdNodeMap;

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
		vector<string> ukdVector;
		int maxCareRand=0;
		double minQms=1.0;
		int iter=0;
		int smtCall=0;
		double qmsTime=0.0;


		ofstream fout;
		fout.open(absoluteBenchmarkPath + "/qms_result/minQmsInfer.txt");

		cout<<"start, hold on..."<<endl;
		for (unsigned int i = 0; i < CodeV.size(); i++) {
			if(minQms==0){
				break;
			}

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
				Node tempNode;
				NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


				if(nodeMap[InterV[i]].getUniqueM()->size()>0){
					NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);
				}


				SemanticDistributionInference::semRule(&tempNode, &nodeMap[InterV[i]]);

				if(tempNode.getDistributionEnums()==DistributionEnums::UKD){

					set<string> * notCareRandSet=new set<string>();
					set<string> * randSet = new set<string>();
					set<string> * semdSet = new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*notCareRandSet);
					Node::getRandSet(tempNode, *randSet);
					SetUtil::Difference(*randSet, *notCareRandSet, *semdSet);
					tempNode.setSemd(semdSet);
					nodeMap[InterV[i]].setSemd(semdSet);

					DistributionEnums dis;
					dis=SMT2Parse::getDistributionByZ3(tempNode,*notCareRandSet);
					smtCall++;
					nodeMap[InterV[i]].setDistributionEnums(dis);
					if(dis == DistributionEnums::NPM) {
						double step=1.0;
						if(semdSet->size()==0){
							minQms=0;
						}else{
							for(unsigned int i=0;i<semdSet->size();i++){
								step=step/2.0;
							}
							clock_t t1 = clock();
							if(minQms>0){
								while(true){
									//cout<<InterV[i]<<endl;
									//cout<<qms<<endl;
									iter++;
									if(minQms>=0 && QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*notCareRandSet,minQms)){
										break;
									}else{
										minQms=minQms-step;
									}
								}
							}

							clock_t t2 = clock();
							qmsTime=t2-t1+qmsTime;
						}

					}

				}
			}

		}



		clock_t end_time = clock();
		//cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
		double timeCount = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
		//cout << timeCount << endl;
		string s = "";
		s = "All node count is: " + std::to_string(InterV.size());
		fout << s << "\n";
        s = "Min qms is: " + std::to_string(minQms);
        fout << s << "\n";
		s = "Iter is: " + std::to_string(iter);
		fout << s << "\n";
		s = "SMT calls is: " + std::to_string(smtCall);
		fout << s << "\n";
		s = "Time for qms is: " + std::to_string(qmsTime/1000)+"ms";
		fout << s << "\n";
		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";
		fout << flush;
		fout.close();


	}

    /*
     * different qms check
     */
	static void scinferCheckQms(string absoluteBenchmarkPath,double qms) {
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
		int satCount=0;
		int unsatCount=0;
		int qmsBySmt=0;
		int pmBySmt=0;


		int qmsInt=int(qms*10);

		ofstream fout;
		fout.open(absoluteBenchmarkPath + "/qms_result/scinferQms"+std::to_string(qmsInt)+".txt");

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
			/*
            cout<<"Node: "+InterV[i]+", supp is:";
            SetUtil::put_out(*nodeMap[InterV[i]].getSupportV());

            set<string> * notCareRandSet=new set<string>();
            NodeUtil::getNotCareRandomSet(nodeMap[InterV[i]],*notCareRandSet);
            cout<<"Node: "+InterV[i]+", not care rand is:";
            SetUtil::put_out(*notCareRandSet);
            */

			SemanticDistributionInference::synInference(&nodeMap[InterV[i]]);

			if(nodeMap[InterV[i]].getDistributionEnums()==DistributionEnums::UKD){
				Node tempNode;
				NodeUtil::nodeCopy(&tempNode, nodeMap[InterV[i]]);


				if(nodeMap[InterV[i]].getUniqueM()->size()>0){
					NodeUtil::getRegion(&tempNode, nodeMap[InterV[i]].getUniqueM(), nodeMap);
				}


				SemanticDistributionInference::semRule(&tempNode, &nodeMap[InterV[i]]);

				if(tempNode.getDistributionEnums()==DistributionEnums::UKD){

					set<string> * notCareRandSet=new set<string>();
					set<string> * randSet = new set<string>();
					set<string> * semdSet = new set<string>();
					NodeUtil::getNotCareRandomSet(tempNode,*notCareRandSet);
					Node::getRandSet(tempNode, *randSet);
					SetUtil::Difference(*randSet, *notCareRandSet, *semdSet);
					tempNode.setSemd(semdSet);
					nodeMap[InterV[i]].setSemd(semdSet);

					DistributionEnums dis;
					dis=SMT2Parse::getDistributionByZ3(tempNode,*notCareRandSet);
					nodeMap[InterV[i]].setDistributionEnums(dis);
					if(dis == DistributionEnums::NPM) {
						qmsBySmt++;
						bool isStatisfy=QmsStatisfy::isQmsStatisfyWithNotCares(tempNode,*notCareRandSet,qms);
						if(isStatisfy==true){
							satCount++;
						} else{
							unsatCount++;
						}
					}
					else if(dis == DistributionEnums::SID){
						pmBySmt++;
						satCount++;
					}

				}else{
					satCount=satCount+1;
					nodeMap[InterV[i]].setDistributionEnums(tempNode.getDistributionEnums());
				}

			} else{
				satCount=satCount+1;
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
		s = "SAT count: " + StringUtil::getString(satCount);
		fout << s << "\n";
		s = "UNSAT count: " + StringUtil::getString(unsatCount);
		fout << s << "\n";
		s = "QMS by SMT: " + StringUtil::getString(qmsBySmt);
		fout << s << "\n";
		s = "PM by SMT: " + StringUtil::getString(pmBySmt);
		fout << s << "\n";
		s = "Running time is : " + std::to_string(timeCount) + "ms";
		fout << s << "\n";
		fout << flush;
		fout.close();


	}

};