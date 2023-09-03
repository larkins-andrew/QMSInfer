#pragma once
#include <iostream>
#include <string>
#include <set>
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
using namespace std;
class EnumUtil
{
public:
	static string distributionToString(DistributionEnums dis) {

		if (dis == DistributionEnums::CST) {
			return "CST";
		}
		else if (dis == DistributionEnums::SID) {
			return "SID";
		}
		else if (dis == DistributionEnums::RUD) {
			return "RUD";
		}
		else if (dis == DistributionEnums::UKD) {
			return "UKD";
		}
		else if (dis == DistributionEnums::NPM) {
			return "NPM";
		}
		else if (dis == DistributionEnums::NULLDISTRIBUTION) {
			return "NULLDISTRIBUTION";
		}
		return "";
	}

	static string operationToString(OperatorEnums op) {
		if (op == OperatorEnums::XOR) {
			return "XOR";
		}
		else if (op == OperatorEnums::AND) {
			return "AND";
		}
		else if (op == OperatorEnums::OR) {
			return "OR";
		}
		else if (op == OperatorEnums::NOT) {
			return "NOT";
		}
		else if (op == OperatorEnums::EQUAL) {
			return "EQUAL";
		}
		else if (op == OperatorEnums::NULLOPERATOR) {
			return "NULLOPERATOR";
		}
		return "";
	}

	static string typeToString(NodeTypeEnums t) {
		if (t == NodeTypeEnums::INTERMEDIATE) {
			return "INTERMEDIATE";
		}
		else if (t == NodeTypeEnums::MASK) {
			return "MASK";
		}
		else if (t == NodeTypeEnums::SECRECT) {
			return "SECRECT";
		}
		else if (t == NodeTypeEnums::PLAIN) {
			return "PLAIN";
		}
		else if (t == NodeTypeEnums::CONSTANT) {
			return "CONSTANT";
		}
		else if (t == NodeTypeEnums::NULLNODETYPE) {
			return "NULLNODETYPE";
		}
		return "";
	}
};

