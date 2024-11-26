#pragma once
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
class EnumUtil
{
public:
	static std::string distributionToString(DistributionEnums dis);
	static std::string operationToString(OperatorEnums op);
	static std::string typeToString(NodeTypeEnums t);
};