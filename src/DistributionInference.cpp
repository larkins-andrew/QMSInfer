#include "DistributionInference.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "EnumUtil.h"
#include "SetUtil.h"

using namespace std;

void DistributionInference::inference(Node* node, set<string> &M) {
	//cout << node->getName() << endl;
	if ((*node).getLeftChild() == NULL && (*node).getRightChild() == NULL) {
		if ((*node).getNodeTypeEnums() == NodeTypeEnums::SECRECT) {
			(*node).setDistributionEnums(DistributionEnums::UKD);
			(*(*node).getDependences()).insert((*node).getName());
			return;
		}
		else if ((*node).getNodeTypeEnums() == NodeTypeEnums::MASK) {
			(*node).setDistributionEnums(DistributionEnums::RUD);
			(*(*node).getDependences()).insert((*node).getName());
			(*(*node).getDominant()).insert((*node).getName());
			return;
		}
		else if ((*node).getNodeTypeEnums() == NodeTypeEnums::PLAIN) {
			(*node).setDistributionEnums(DistributionEnums::UKD);
			(*(*node).getDependences()).insert((*node).getName());
			return;
		}
		else if ((*node).getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
			(*node).setDistributionEnums(DistributionEnums::CST);
			(*(*node).getDependences()).insert((*node).getName());
			return;
		}
	}
	else {
		if ((*node).getLeftChild() != NULL && node->getLeftChild()->getDistributionEnums()==DistributionEnums::NULLDISTRIBUTION) {
			inference((*node).getLeftChild(), M);
		}
		if ((*node).getRightChild() != NULL && node->getRightChild()->getDistributionEnums()==DistributionEnums::NULLDISTRIBUTION) {
			inference((*node).getRightChild(), M);
		}

		ruleInference(node, (*node).getOperatorEnums(), M);
	}
}
	

void DistributionInference::ruleInference(Node *n, OperatorEnums op, set<string> &M) {
	if (n->getLeftChild()!=NULL && n->getRightChild()!=NULL) {
		SetUtil::Union(*n->getLeftChild()->getDependences(), *n->getRightChild()->getDependences(), *n->getDependences());
	}
	else if (n->getLeftChild() == NULL) {
		set<string>* temp = new set<string>();
		SetUtil::Union(*temp, *n->getRightChild()->getDependences(), *n->getDependences());
	}
	else if (n->getRightChild() == NULL) {
		set<string>* temp = new set<string>();
		SetUtil::Union(*n->getLeftChild()->getDependences(), *temp, *n->getDependences());
	}
	
	bool isTag = false;

	if (op == OperatorEnums::XOR) {
		//SetUtil::Union(*n->getLeftChild()->getDominant(), *n->getRightChild()->getDominant(), *n->getDominant());

		/**/
		set<string> *temp1= new set<string>();
		SetUtil::Intersection(*n->getLeftChild()->getDominant(), *n->getRightChild()->getDominant(), *temp1);
		set<string> *temp2 = new set<string>();
		SetUtil::Union(*n->getLeftChild()->getDominant(), *n->getRightChild()->getDominant(), *temp2);
		SetUtil::Difference(*temp2,*temp1, *n->getDominant());
		

		//rule1
		if (n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD && n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD) {
			bool flag = false;
			set<string>::iterator it;
			it = M.begin();
			while (it != M.end()) {
				if ((SetUtil::Contain(*n->getLeftChild()->getDominant(),*it) && !SetUtil::Contain(*n->getRightChild()->getDependences(),*it)) || (SetUtil::Contain(*n->getRightChild()->getDominant(),*it) && !SetUtil::Contain(*n->getLeftChild()->getDependences(),*it)     )) {
					flag = true;
					break;
				}
				it++;
			}
			if (flag == true) {
				n->setDistributionEnums(DistributionEnums::RUD);
				isTag = true;
				return;
			}
		}

		//rule 2,3
		if (n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD && (n->getRightChild()->getDistributionEnums() == DistributionEnums::SID || n->getRightChild()->getDistributionEnums() == DistributionEnums::UKD)) {
			bool flag = false;
			set<string>::iterator it;
			it = M.begin();
			while (it != M.end()) {
				if (SetUtil::Contain(*n->getLeftChild()->getDominant(), *it) && !SetUtil::Contain(*n->getRightChild()->getDependences(), *it) ) {
					flag = true;
					break;
				}
				it++;
			}
			if (flag == true) {
				n->setDistributionEnums(DistributionEnums::RUD);
				isTag = true;
				return;
			}
		}

		if (n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD && (n->getLeftChild()->getDistributionEnums() == DistributionEnums::SID || n->getLeftChild()->getDistributionEnums() == DistributionEnums::UKD)) {
			bool flag = false;
			set<string>::iterator it;
			it = M.begin();
			while (it != M.end()) {
				if (SetUtil::Contain(*n->getRightChild()->getDominant(), *it) && !SetUtil::Contain(*n->getLeftChild()->getDependences(), *it)) {
					flag = true;
					break;
				}
				it++;
			}
			if (flag == true) {
				n->setDistributionEnums(DistributionEnums::RUD);
				isTag = true;
				return;
			}
		}

		//rule 4
		if (n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD && n->getRightChild()->getDistributionEnums() == DistributionEnums::CST ) {
			n->setDistributionEnums(DistributionEnums::RUD);
			isTag = true;
			return;
		}

		if (n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD && n->getLeftChild()->getDistributionEnums() == DistributionEnums::CST) {
			n->setDistributionEnums(DistributionEnums::RUD);
			isTag = true;
			return;
		}

		//rule 5
		if (n->getRightChild()->getDistributionEnums() == DistributionEnums::SID && n->getLeftChild()->getDistributionEnums() == DistributionEnums::SID) {
			set<string> temp, result;
			SetUtil::Intersection(*n->getLeftChild()->getDependences(), *n->getRightChild()->getDependences(), temp);
			SetUtil::Intersection(M, temp, result);
			if (result.size()==0) {
				n->setDistributionEnums(DistributionEnums::SID);
				isTag = true;
				return;
			}
		}

	}
	else if (op==OperatorEnums::AND || op==OperatorEnums::OR) {
		//rule 1
		if (n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD && n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD) {
			//condition 1
			set<string> temp, result;
			SetUtil::Intersection(*n->getLeftChild()->getDependences(), *n->getRightChild()->getDependences(), temp);
			SetUtil::Intersection(M, temp, result);
			if (result.size() == 0) {
				n->setDistributionEnums(DistributionEnums::SID);
				isTag = true;
				return;
			}
			//condition 2
			bool flag = false;
			set<string>::iterator it;
			it = M.begin();
			while (it != M.end()) {
				if ((SetUtil::Contain(*n->getLeftChild()->getDominant(), *it) && !SetUtil::Contain(*n->getRightChild()->getDependences(), *it)) || (SetUtil::Contain(*n->getRightChild()->getDominant(), *it) && !SetUtil::Contain(*n->getLeftChild()->getDependences(), *it))) {
					flag = true;
					break;
				}
				it++;
			}
			if (flag == true) {
				n->setDistributionEnums(DistributionEnums::SID);
				isTag = true;
				return;
			}
		}

		//rule 2,4
		if ((n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD || n->getLeftChild()->getDistributionEnums() == DistributionEnums::SID) && n->getRightChild()->getDistributionEnums() == DistributionEnums::SID) {
			set<string> temp, result;
			SetUtil::Intersection(*n->getLeftChild()->getDependences(), *n->getRightChild()->getDependences(), temp);
			SetUtil::Intersection(M, temp, result);
			if (result.size() == 0) {
				n->setDistributionEnums(DistributionEnums::SID);
				isTag = true;
				return;
			}
		}

		if ((n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD || n->getRightChild()->getDistributionEnums() == DistributionEnums::SID) && n->getLeftChild()->getDistributionEnums() == DistributionEnums::SID) {
			set<string> temp, result;
			SetUtil::Intersection(*n->getLeftChild()->getDependences(), *n->getRightChild()->getDependences(), temp);
			SetUtil::Intersection(M, temp, result);
			if (result.size() == 0) {
				n->setDistributionEnums(DistributionEnums::SID);
				isTag = true;
				return;
			}
		}

		//rule 3
		if (n->getLeftChild()->getDistributionEnums() == DistributionEnums::RUD && n->getRightChild()->getDistributionEnums() == DistributionEnums::CST) {
			n->setDistributionEnums(DistributionEnums::SID);
			isTag = true;
			return;
		}

		if (n->getRightChild()->getDistributionEnums() == DistributionEnums::RUD && n->getLeftChild()->getDistributionEnums() == DistributionEnums::CST) {
			n->setDistributionEnums(DistributionEnums::SID);
			isTag = true;
			return;
		}
	}
	else if (op == OperatorEnums::NOT) {
		if (n->getLeftChild() != NULL) {
			n->setDistributionEnums(n->getLeftChild()->getDistributionEnums());
			isTag = true;
			return;
		}
		if (n->getRightChild() != NULL) {
			n->setDistributionEnums(n->getRightChild()->getDistributionEnums());
			isTag = true;
			return;
		}
	}
	else if (op == OperatorEnums::NULLOPERATOR) {
		if (n->getLeftChild() != NULL) {
			n->setDistributionEnums(n->getLeftChild()->getDistributionEnums());
			isTag = true;
			return;
		}
		if (n->getRightChild() != NULL) {
			n->setDistributionEnums(n->getRightChild()->getDistributionEnums());
			isTag = true;
			return;
		}
	}
	else {
		cout <<"We don't add not now" << endl;
	}
	
	if (isTag == false) {
		n->setDistributionEnums(DistributionEnums::UKD);
		return;
	}	
}