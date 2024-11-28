#pragma once
#include <string>
#include <vector>
#include <map>
#include "Node.h"

class HammingDistance {
private:
	std::string absoluteBenchmarkPath;
	std::map<std::string, Node> nodeMap;
	std::vector<std::string> RandV;
	std::vector<std::string> SecV;
	std::vector<std::string> PublicV;
	std::vector<std::string> InterV;
	std::vector<std::string> CodeV;
	std::vector<std::string> DistLeakV; // TODO: rename?
	int rudCount;
	int sidCount;
	int cstCount;
	int ukdCount;
	int nullCount;
	int npmCount;
	int nodeSmtCheckCount;
	int ukdBySemdR;
	int ukdToNpm;
	int ukdToSid;
	double time1;
	double time2;
	double time3;
	double timeCount;

	void add_node(std::string &str);
	void distAnalysis();
	void evalNode(std::string n);
public:
	HammingDistance(std::string absPath);
	~HammingDistance();
	void scInferApproach();
	void outputResults();
};