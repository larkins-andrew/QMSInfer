#pragma once
#include <string>
#include "Node.h"

class QMSApproach {
public:
	static bool needToBeChecked(Node node);
	static bool threeConditionCheck(Node *node);
    /*
     * check qms=0.5 by smt-based
     */
	static void smtBasedCheckQms(std::string benchmarkName);
	/*
	 * check qms=0.5cd by scinfer
	 */
    static void scinferBasedCheckQms(std::string absoluteBenchmarkPath);
	/*
	 * computer every node's qms by smt-based
	 */
	static void smtBasedComputerQms(std::string benchmarkName);
	/*
	 * computer every node's qms by scInfer
	 */
	static void scInferComputerQms(std::string absoluteBenchmarkPath);
	static void minQmsInfer (std::string absoluteBenchmarkPath);
    /*
     * different qms check
     */
	static void scinferCheckQms(std::string absoluteBenchmarkPath,double qms);
};