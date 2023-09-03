#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Node.h"
using namespace std;

class StringUtil
{
public:
	
	//erase all space in string
	static void trim(string &s)
	{
		/*
		if( !s.empty() )
		{
		s.erase(0,s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		}
		*/
		int index = 0;
		if (!s.empty())
		{
			while ((index = s.find(' ', index)) != string::npos)
			{
				s.erase(index, 1);
			}
		}

	}

	static void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
	{
		std::string::size_type pos1, pos2;
		pos2 = s.find(c);
		pos1 = 0;
		while (std::string::npos != pos2)
		{
			v.push_back(s.substr(pos1, pos2 - pos1));

			pos1 = pos2 + c.size();
			pos2 = s.find(c, pos1);
		}
		if (pos1 != s.length())
			v.push_back(s.substr(pos1));
	}
	
	static vector<string> SplitStringBySpace(const std::string& s) {
		vector<string> result;
		int iBegin = 0;
		string::size_type iLatter = 0;
		string::size_type iFormer = string::npos;
		
		while (1)
		{
			iLatter = s.find_first_not_of(' ', iLatter);
			if (string::npos == iLatter)
			{
				break;
			}

			iFormer = s.find_first_of(' ', iLatter + 1);
			if (string::npos == iFormer)
			{
				iFormer = s.length();
			}
			
			// str2, str3, str4
			//string strNew(s, iLatter, iFormer - iLatter);
			result.push_back(s.substr(iLatter, iFormer - iLatter));

			iLatter = iFormer + 1;
			
		}
		return result;
	}

	static string getString( int n)

	{
		return std::to_string(n);

	}


	// replace all occurance of t in s to w  
	static void replace_all(std::string & s, std::string const & t, std::string const & w)
	{
		string::size_type pos = s.find(t), t_size = t.size(), r_size = w.size();
		while (pos != std::string::npos) { // found   
			s.replace(pos, t_size, w);
			pos = s.find(t, pos + r_size);
		}
	}

	//replace string in vector and map
	static void  replaceThroughMap(string &s, vector<string> &v, map<string, string> &m) {
		for (unsigned int i = 0; i < v.size(); i++) {
			replace_all(s, v[i], m[v[i]]);
		}
	}
	

	static void IntToBinaryVector(int n, vector<int> &v,int len)
	{
		int temp;
		temp = n;
		int count = 0;
		while (temp != 0 )
		{
			v.push_back(temp % 2);
			count++;
			temp = temp>>1;
		}
		while (count < len) {
			v.push_back(0);
			count++;
		}
	}

	static void BitToBoolVector(vector<string> &rand,vector<int> &tag, map<string,string> &m) {
		for (unsigned int i = 0; i < rand.size(); i++) {
			if (tag[i] == 1) {
				m[rand[i]] = " true ";
			}
			else {
				m[rand[i]] = " false ";
			}
		}
	}

	static void VectorLogicAdd(vector<string> &v, string &s) {
		string ss = "temp";
		for (unsigned int i = 0; i < v.size(); i++) {
			if (i == 0) {
				s.append("( + " + v[i] +" " +ss+"  )");
			}
			if (i < (v.size() - 1) && i>0) {
				string str = "( + " + v[i] + " " + ss + "  )";
				s = s.replace(s.find(ss),ss.size(), str);
			}
			if (i == (v.size() - 1)) {
				s = s.replace(s.find(ss), ss.size(), v[i]);
			}
		}
	}


	
};


