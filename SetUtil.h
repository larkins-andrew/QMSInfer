#pragma once
#include <iostream>  
#include <set>  
#include <algorithm>  
#include <string>
#include <vector>

using namespace std;
class SetUtil {
public:


	static void put_out(set<string> &p) {
		set<string>::iterator it = p.begin();
		while (it != p.end()) {
			cout << *it << " ";
			it++;
		}
		cout << "\n";
	}

	static string getFirstElement(set<string> &p) {
		set<string>::iterator it = p.begin();
		while (it != p.end()) {
			return *it;
			break;
			it++;
		}
		return "not";
	}

	static void Intersection(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) != B.end()) result.insert(*it);
			it++;
		}
	}



	static void Union(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			result.insert(*it);
			it++;
		}

		it = B.begin();
		while (it != B.end()) {
			result.insert(*it);
			it++;
		}
	}

	static void Difference(set<string> &A, set<string> &B, set<string> &result) {
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (B.find(*it) == B.end()) result.insert(*it);
			it++;
		}
	}

	static bool Contain(set<string> &A, string temp) {
		bool flag = false;
		set<string>::iterator it;

		it = A.begin();
		while (it != A.end()) {
			if (*it == temp) {
				flag = true;
				break;
			}
			else {
				it++;
			}
		}

		return flag;
	}

	static bool containValue(set<string> &A, string temp) {
		bool flag = false;

		if(A.find(temp)!=A.end()){
            flag=true;
		}

		return flag;
	}

	static void SetToVector(set<string> &s, vector<string> &v) {
		set<string>::iterator it = s.begin();
		while (it != s.end()) {
			string ss = *it;
			v.push_back(ss);
			it++;
		}
	}

	static string SetToString(set<string> &s) {
		string str="";
		set<string>::iterator it = s.begin();
		while (it != s.end()) {
			string ss = *it;
			str=str+" "+ss;
			it++;
		}
		return str;
	}


};
