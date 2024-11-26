#include <iostream>
#include <string>
#include <set>
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"
#include "Node.h"
#include "NodeUtil.h"
#include "SetUtil.h"
#include "SemanticDistributionInference.h"

using namespace std;

int SemanticDistributionInference::hasSemd = 0;
int SemanticDistributionInference::noSemd = 0;

void SemanticDistributionInference::synInference(Node* node) {

	Node* leftChild = node->getLeftChild();
	Node* rightChild = node->getRightChild();
	if ( leftChild == NULL && rightChild == NULL) {
		if ((*node).getNodeTypeEnums() == NodeTypeEnums::SECRET || (*node).getNodeTypeEnums() == NodeTypeEnums::PLAIN) {
			(*node).setDistributionEnums(DistributionEnums::UKD);
			return;
		}
		else if ((*node).getNodeTypeEnums() == NodeTypeEnums::MASK) {
			(*node).setDistributionEnums(DistributionEnums::RUD);
			return;
		}
		else if ((*node).getNodeTypeEnums() == NodeTypeEnums::CONSTANT) {
			(*node).setDistributionEnums(DistributionEnums::CST);
			return;
		}
	}
	else {
		if (leftChild != NULL && leftChild->getDistributionEnums()==DistributionEnums::NULLDISTRIBUTION) {
			synInference(leftChild);
		}
		if (rightChild != NULL && rightChild->getDistributionEnums()==DistributionEnums::NULLDISTRIBUTION) {
			synInference(rightChild);
		}

		synRule(node);
		
	}
}

//add new rule
void SemanticDistributionInference::synRule(Node* node) {
	Node* leftChild = node->getLeftChild();
	Node* rightChild = node->getRightChild();

	set<string>* secretSet = new set<string>();
	Node::getSecretSet(*node,*secretSet);


	if (node->getOperatorEnums() == OperatorEnums::XOR) {
		if (leftChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* leftDominant = new set<string>();
			set<string>* rightSupportV = new set<string>();
			set<string>* temp = new set<string>();
			leftDominant = leftChild->getDominant();
			rightSupportV = rightChild->getSupportV();
			SetUtil::Difference(*leftDominant,*rightSupportV,*temp);

			if (temp->size()>0) {
				node->setDistributionEnums(DistributionEnums::RUD);
				return;
			}
		}
		if (rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* rightDominant = new set<string>();
			set<string>* leftSupportV = new set<string>();
			set<string>* temp = new set<string>();
			rightDominant = rightChild->getDominant();
			leftSupportV = leftChild->getSupportV();
			SetUtil::Difference(*rightDominant, *leftSupportV, *temp);

			if (temp->size()>0) {
				node->setDistributionEnums(DistributionEnums::RUD);
				return;
			}
		}
		if (leftChild->getDistributionEnums() == DistributionEnums::SID && rightChild->getDistributionEnums() == DistributionEnums::SID) {

			set<string>* leftSupportV = new set<string>();
			set<string>* rightSupportV = new set<string>();
			leftSupportV = leftChild->getSupportV();
			rightSupportV = rightChild->getSupportV();
			set<string>* leftRand = new set<string>();
			set<string>* rightRand = new set<string>();
			Node::getRandSet(*leftChild,*leftRand);
			Node::getRandSet(*rightChild,*rightRand);
			set<string>* temp1 = new set<string>();
			set<string>* temp2 = new set<string>();
			set<string>* temp = new set<string>();
			SetUtil::Intersection(*leftRand,*leftSupportV,*temp1);
			SetUtil::Intersection(*rightRand,*rightSupportV,*temp2);
			SetUtil::Intersection(*temp1, *temp2, *temp);
			if (temp->size()==0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}
		}
		if (secretSet->size() == 0) {
			node->setDistributionEnums(DistributionEnums::SID);
			return;
		}

	}
	else if (node->getOperatorEnums()==OperatorEnums::AND || node->getOperatorEnums()==OperatorEnums::OR) {


		if (leftChild->getDistributionEnums() == DistributionEnums::RUD && rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* leftDominant = new set<string>();
			set<string>* rightSupportV = new set<string>();
			set<string>* temp1 = new set<string>();
			leftDominant = leftChild->getDominant();
			rightSupportV = rightChild->getSupportV();
			SetUtil::Difference(*leftDominant, *rightSupportV, *temp1);
			if (temp1->size()>0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}

			set<string>* rightDominant = new set<string>();
			set<string>* leftSupportV = new set<string>();
			set<string>* temp2 = new set<string>();
			rightDominant = rightChild->getDominant();
			leftSupportV = leftChild->getSupportV();
			SetUtil::Difference(*rightDominant, *leftSupportV, *temp2);

			if (temp2->size()>0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}

		}


		if (leftChild->getDistributionEnums() == DistributionEnums::RUD || rightChild->getDistributionEnums() == DistributionEnums::RUD) {
			if(leftChild->getDistributionEnums() != DistributionEnums::UKD && rightChild->getDistributionEnums() != DistributionEnums::UKD && leftChild->getDistributionEnums() != DistributionEnums::NPM && rightChild->getDistributionEnums() != DistributionEnums::NPM){
				set<string>* leftSupportV = new set<string>();
				set<string>* rightSupportV = new set<string>();
				leftSupportV = leftChild->getSupportV();
				rightSupportV = rightChild->getSupportV();
				set<string>* leftRand = new set<string>();
				set<string>* rightRand = new set<string>();
				Node::getRandSet(*leftChild, *leftRand);
				Node::getRandSet(*rightChild, *rightRand);
				set<string>* temp1 = new set<string>();
				set<string>* temp2 = new set<string>();
				set<string>* temp = new set<string>();
				SetUtil::Intersection(*leftRand, *leftSupportV, *temp1);
				SetUtil::Intersection(*rightRand, *rightSupportV, *temp2);
				SetUtil::Intersection(*temp1, *temp2, *temp);
				if (temp->size() == 0) {
					node->setDistributionEnums(DistributionEnums::SID);
					return;
				}
			}

		}
		if (leftChild->getDistributionEnums() == DistributionEnums::SID && rightChild->getDistributionEnums() == DistributionEnums::SID) {

			set<string>* leftSupportV = new set<string>();
			set<string>* rightSupportV = new set<string>();
			leftSupportV = leftChild->getSupportV();
			rightSupportV = rightChild->getSupportV();
			set<string>* leftRand = new set<string>();
			set<string>* rightRand = new set<string>();
			Node::getRandSet(*leftChild, *leftRand);
			Node::getRandSet(*rightChild, *rightRand);
			set<string>* temp1 = new set<string>();
			set<string>* temp2 = new set<string>();
			set<string>* temp = new set<string>();
			SetUtil::Intersection(*leftRand, *leftSupportV, *temp1);
			SetUtil::Intersection(*rightRand, *rightSupportV, *temp2);
			SetUtil::Intersection(*temp1, *temp2, *temp);

			if (temp->size() == 0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}
		}

		if (secretSet->size() == 0) {

			node->setDistributionEnums(DistributionEnums::SID);
			return;
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::RUD && rightChild->getDistributionEnums() == DistributionEnums::NPM) {

			set<string>* leftSupportV = new set<string>();
			set<string>* rightSupportV = new set<string>();
			leftSupportV = leftChild->getSupportV();
			rightSupportV = rightChild->getSupportV();
			set<string>* leftRand = new set<string>();
			set<string>* rightRand = new set<string>();
			Node::getRandSet(*leftChild, *leftRand);
			Node::getRandSet(*rightChild, *rightRand);
			set<string>* temp1 = new set<string>();
			set<string>* temp2 = new set<string>();
			set<string>* temp = new set<string>();
			SetUtil::Intersection(*leftRand, *leftSupportV, *temp1);
			SetUtil::Intersection(*rightRand, *rightSupportV, *temp2);
			SetUtil::Intersection(*temp1, *temp2, *temp);

			if (temp->size() == 0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::NPM && rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* leftSupportV = new set<string>();
			set<string>* rightSupportV = new set<string>();
			leftSupportV = leftChild->getSupportV();
			rightSupportV = rightChild->getSupportV();
			set<string>* leftRand = new set<string>();
			set<string>* rightRand = new set<string>();
			Node::getRandSet(*leftChild, *leftRand);
			Node::getRandSet(*rightChild, *rightRand);
			set<string>* temp1 = new set<string>();
			set<string>* temp2 = new set<string>();
			set<string>* temp = new set<string>();
			SetUtil::Intersection(*leftRand, *leftSupportV, *temp1);
			SetUtil::Intersection(*rightRand, *rightSupportV, *temp2);
			SetUtil::Intersection(*temp1, *temp2, *temp);

			if (temp->size() == 0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::RUD && rightChild->getDistributionEnums() == DistributionEnums::NPM) {

			set<string>* leftPerfectM = new set<string>();
			set<string>* rightSupportV = new set<string>();
			leftPerfectM = leftChild->getPerfectM();
			rightSupportV = rightChild->getSupportV();

			set<string>* temp = new set<string>();
			SetUtil::Difference(*leftPerfectM,*rightSupportV,*temp);

			if (temp->size() > 0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::NPM && rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* leftSupportV = new set<string>();
			set<string>* rightPerfectM  = new set<string>();
			leftSupportV = leftChild->getSupportV();
			rightPerfectM = rightChild->getPerfectM();

			set<string>* temp = new set<string>();
			SetUtil::Difference(*rightPerfectM,*leftSupportV,*temp);

			if (temp->size() > 0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}



	}
	else if (node->getOperatorEnums() == OperatorEnums::NOT || node->getOperatorEnums()==OperatorEnums::EQUAL) {

		node->setDistributionEnums(leftChild->getDistributionEnums());
		return;
	}

	node->setDistributionEnums(DistributionEnums::UKD);
}

void SemanticDistributionInference::semRule(Node* node, Node* originNode) {
	Node* leftChild = node->getLeftChild();
	Node* rightChild = node->getRightChild();

	set<string>* secretSet = new set<string>();
	Node::getSecretSet(*node,*secretSet);

	if (node->getOperatorEnums()==OperatorEnums::XOR) {

		if (leftChild->getDistributionEnums()==DistributionEnums::RUD) {
			set<string>* leftDominant = new set<string>();
			leftDominant = leftChild->getPerfectM();

			set<string>* randSet = new set<string>();
			Node::getRandSet(*rightChild, *randSet);

			set<string>* sed = rightChild->getSemd();

			if(sed == NULL) {
				set<string> *notCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*rightChild, *notCareRandSet);
				sed = new set<string>();
				SetUtil::Difference(*randSet, *notCareRandSet, *sed);
				originNode->getRightChild()->setSemd(sed);
				noSemd++;
			} else {
				hasSemd++;
			}

			set<string>* temp = new set<string>();
			SetUtil::Difference(*leftDominant,*sed,*temp);

			if (temp->size()>0) {
				node->setDistributionEnums(DistributionEnums::RUD);
				return;
			}
		}
		if (rightChild->getDistributionEnums() == DistributionEnums::RUD) {
			set<string>* rightDominant = new set<string>();
			rightDominant = rightChild->getPerfectM();

			set<string>* randSet = new set<string>();
			Node::getRandSet(*leftChild, *randSet);

			set<string>* sed = leftChild->getSemd();
			if(sed == NULL) {
				set<string> *notCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*leftChild, *notCareRandSet);
				sed = new set<string>();
				SetUtil::Difference(*randSet, *notCareRandSet, *sed);
				originNode->getLeftChild()->setSemd(sed);
				noSemd++;
			} else {
				hasSemd++;
			}


			set<string>* temp = new set<string>();
			SetUtil::Difference(*rightDominant, *sed, *temp);
			if (temp->size()>0) {
				node->setDistributionEnums(DistributionEnums::RUD);
				return;
			}
		}

		if (secretSet->size() == 0) {

			node->setDistributionEnums(DistributionEnums::SID);
			return;
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::SID && rightChild->getDistributionEnums() == DistributionEnums::SID) {

			set<string>* leftSed = leftChild->getSemd();
			set<string>* leftSet = new set<string>();
			Node::getRandSet(*leftChild, *leftSet);
			if(leftSed == NULL) {
				set<string>* leftNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*leftChild, *leftNotCareRandSet);
				leftSed = new set<string>();
				SetUtil::Difference(*leftSet, *leftNotCareRandSet, *leftSed);
				originNode->getLeftChild()->setSemd(leftSed);
				noSemd++;
			} else {
				hasSemd++;
			}


			set<string>* rightSed = rightChild->getSemd();
			set<string>* rightSet = new set<string>();
			Node::getRandSet(*rightChild, *rightSet);
			if(rightSed == NULL) {
				set<string>* rightNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*rightChild, *rightNotCareRandSet);
				rightSed = new set<string>();
				SetUtil::Difference(*rightSet, *rightNotCareRandSet, *rightSed);
				originNode->getRightChild()->setSemd(rightSed);
				noSemd++;
			} else {
				hasSemd++;
			}

			set<string>* temp = new set<string>();
			set<string>* temp1 = new set<string>();
			set<string>* temp2 = new set<string>();
			SetUtil::Intersection(*leftSed,*rightSed,*temp1);
			SetUtil::Union(*leftSet,*rightSet,*temp2);
			SetUtil::Intersection(*temp1,*temp2,*temp);
			if (temp->size()==0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}
		}

	}
	else if (node->getOperatorEnums() == OperatorEnums::AND || node->getOperatorEnums() == OperatorEnums::OR) {
		if (leftChild->getDistributionEnums() == DistributionEnums::RUD || rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			if(leftChild->getDistributionEnums() != DistributionEnums::UKD && rightChild->getDistributionEnums() != DistributionEnums::UKD && leftChild->getDistributionEnums() != DistributionEnums::NPM && rightChild->getDistributionEnums() != DistributionEnums::NPM){

				set<string>* leftSed = leftChild->getSemd();
				set<string>* leftSet = new set<string>();
				Node::getRandSet(*leftChild, *leftSet);
				if(leftSed == NULL) {
					set<string>* leftNotCareRandSet = new set<string>();
					NodeUtil::getNotCareRandomSet(*leftChild, *leftNotCareRandSet);
					leftSed = new set<string>();
					SetUtil::Difference(*leftSet, *leftNotCareRandSet, *leftSed);
					originNode->getLeftChild()->setSemd(leftSed);
					noSemd++;
				} else {
					hasSemd++;
				}


				set<string>* rightSed = rightChild->getSemd();
				set<string>* rightSet = new set<string>();
				Node::getRandSet(*rightChild, *rightSet);
				if(rightSed == NULL) {
					set<string>* rightNotCareRandSet = new set<string>();
					NodeUtil::getNotCareRandomSet(*rightChild, *rightNotCareRandSet);
					rightSed = new set<string>();
					SetUtil::Difference(*rightSet, *rightNotCareRandSet, *rightSed);
					originNode->getRightChild()->setSemd(rightSed);
					noSemd++;
				} else {
					hasSemd++;
				}


				set<string>* temp = new set<string>();
				set<string>* temp1 = new set<string>();
				set<string>* temp2 = new set<string>();
				SetUtil::Intersection(*leftSed, *rightSed, *temp1);
				SetUtil::Union(*leftSet, *rightSet, *temp2);
				SetUtil::Intersection(*temp1, *temp2, *temp);
				if (temp->size() == 0) {
					node->setDistributionEnums(DistributionEnums::SID);
					return;
				}
			}


		}
		if (leftChild->getDistributionEnums() == DistributionEnums::SID && rightChild->getDistributionEnums() == DistributionEnums::SID) {

			set<string>* leftSed = leftChild->getSemd();
			set<string>* leftSet = new set<string>();
			Node::getRandSet(*leftChild, *leftSet);
			if(leftSed == NULL) {
				set<string>* leftNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*leftChild, *leftNotCareRandSet);
				leftSed = new set<string>();
				SetUtil::Difference(*leftSet, *leftNotCareRandSet, *leftSed);
				originNode->getLeftChild()->setSemd(leftSed);
				noSemd++;
			} else {
				hasSemd++;
			}


			set<string>* rightSed = rightChild->getSemd();
			set<string>* rightSet = new set<string>();
			Node::getRandSet(*rightChild, *rightSet);
			if(rightSed == NULL) {
				set<string>* rightNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*rightChild, *rightNotCareRandSet);
				rightSed = new set<string>();
				SetUtil::Difference(*rightSet, *rightNotCareRandSet, *rightSed);
				originNode->getRightChild()->setSemd(rightSed);
				noSemd++;
			} else {
				hasSemd++;
			}

			set<string>* temp = new set<string>();
			SetUtil::Intersection(*leftSed, *rightSed, *temp);
			if (temp->size() == 0) {
				node->setDistributionEnums(DistributionEnums::SID);
				return;
			}
		}

		if (secretSet->size() == 0) {

			node->setDistributionEnums(DistributionEnums::SID);
			return;
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::RUD && rightChild->getDistributionEnums() == DistributionEnums::NPM) {

			set<string>* leftSed = leftChild->getSemd();
			set<string>* leftSet = new set<string>();
			Node::getRandSet(*leftChild, *leftSet);
			if(leftSed == NULL) {
				set<string>* leftNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*leftChild, *leftNotCareRandSet);
				leftSed = new set<string>();
				SetUtil::Difference(*leftSet, *leftNotCareRandSet, *leftSed);
				originNode->getLeftChild()->setSemd(leftSed);
				noSemd++;
			} else {
				hasSemd++;
			}


			set<string>* rightSed = rightChild->getSemd();
			set<string>* rightSet = new set<string>();
			Node::getRandSet(*rightChild, *rightSet);
			if(rightSed == NULL) {
				set<string>* rightNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*rightChild, *rightNotCareRandSet);
				rightSed = new set<string>();
				SetUtil::Difference(*rightSet, *rightNotCareRandSet, *rightSed);
				originNode->getRightChild()->setSemd(rightSed);
				noSemd++;
			} else {
				hasSemd++;
			}

			set<string>* temp1 = new set<string>();
			SetUtil::Intersection(*leftSed, *rightSed, *temp1);

			set<string>* leftDominant = new set<string>();
			leftDominant = leftChild->getPerfectM();
			set<string>* temp2 = new set<string>();
			SetUtil::Difference(*leftDominant,*rightSed,*temp2);


			if (temp1->size() == 0 || temp2->size()>0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}

		if (leftChild->getDistributionEnums() == DistributionEnums::NPM && rightChild->getDistributionEnums() == DistributionEnums::RUD) {

			set<string>* leftSed = leftChild->getSemd();
			set<string>* leftSet = new set<string>();
			Node::getRandSet(*leftChild, *leftSet);
			if(leftSed == NULL) {
				set<string>* leftNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*leftChild, *leftNotCareRandSet);
				leftSed = new set<string>();
				SetUtil::Difference(*leftSet, *leftNotCareRandSet, *leftSed);
				originNode->getLeftChild()->setSemd(leftSed);
				noSemd++;
			} else {
				hasSemd++;
			}


			set<string>* rightSed = rightChild->getSemd();
			set<string>* rightSet = new set<string>();
			Node::getRandSet(*rightChild, *rightSet);
			if(rightSed == NULL) {
				set<string>* rightNotCareRandSet = new set<string>();
				NodeUtil::getNotCareRandomSet(*rightChild, *rightNotCareRandSet);
				rightSed = new set<string>();
				SetUtil::Difference(*rightSet, *rightNotCareRandSet, *rightSed);
				originNode->getRightChild()->setSemd(rightSed);
				noSemd++;
			} else{
				hasSemd++;
			}

			set<string>* temp1 = new set<string>();
			SetUtil::Intersection(*leftSed, *rightSed, *temp1);

			set<string>* rightDominant = new set<string>();
			rightDominant = rightChild->getPerfectM();
			set<string>* temp2 = new set<string>();
			SetUtil::Difference(*rightDominant,*leftSed,*temp2);

			if (temp1->size() == 0 || temp2->size()>0) {
				node->setDistributionEnums(DistributionEnums::NPM);
				return;
			}
		}
	}
}