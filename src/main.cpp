#include <iostream>
#include <string>
#include <set>
#include <fstream>
#include <cassert>
#include <pthread.h>
#include <map>
#include <time.h>
#include "Node.h"
#include "NodeTypeEnums.h"
#include "DistributionEnums.h"
#include "OperatorEnums.h"
#include "EnumUtil.h"
#include "DistributionInference.h"
#include "SetUtil.h"
#include "FileUtil.h"
#include "SMTApproach.h"
#include "ScInferApproach.h"
#include "SymbolicApproach.h"
#include "QMSApproach.h"
#include "HammingDistance.h"

#include <z3++.h>

using namespace std;
using namespace z3;

int main(int argc, char* argv[]) {

    clock_t start_time = clock();

    string s1,s2,s3;
    char *p1=NULL;
    char *p2=NULL;
    char *p3=NULL;

    if(argv[1]!=NULL){
        p1 = argv[1];
        s1=p1;
    }
    if(argv[2]!=NULL){
        p2 = argv[2];
        s2=p2;
    }
    if(argv[3]!=NULL){
        p3 = argv[3];
        s3=p3;
    }

    if(s1.compare("-help")==0){
        cout<<"111"<<endl;
        string s="";
        s.append("Usage: qmsInfer [options] [qms] testBenchmarkPath\n");
        s.append("Option available is:\n");
        s.append("   -smt            use smt-based approach to check whether test benchmark is perfect masking\n");
        s.append("   -symbolic       use symbolic-based approach to check whether test benchmark is perfect masking\n");
        s.append("   -scInfer        use scInfer-based approach to check whether test benchmark is perfect masking\n");
        s.append("   -smtQms         use smt-based approach to compute the qms of test benchmark\n");
        s.append("   -scInferQms     use scInfer-based approach to compute the qms of test benchmark\n");
        s.append("   -minQms         use scInfer-based approach to compute the min qms of test benchmark\n");
        s.append("   -qmsCheck qms   use scInfer-based approach to check whether test benchmark satisfy the qms value\n");
        s.append("   -crayonEater    break the mold with our groundbreaking new groundbreaking approach");
        cout<<s<<endl;
    }else if(s1.compare("-smt")==0){
        SMTApproach::incrementalApproach(s2);
    }else if(s1.compare("-symbolic")==0){
        SymbolicApproach::symbolicApproach(s2);
    }else if(s1.compare("-scInfer")==0){
        ScInferApproach::scInferApproach(s2);
    }else if(s1.compare("-smtQms")==0){
        QMSApproach::smtBasedComputerQms(s2);
    }else if(s1.compare("-scInferQms ")==0){
        QMSApproach::scInferComputerQms(s2);
    }else if(s1.compare("-minQms")==0){
        QMSApproach::minQmsInfer(s2);
    }else if(s1.compare("-qmsCheck")==0){
        QMSApproach::scinferCheckQms(s2,atof(p3));
    } else if(s1.compare("-crayonEater")==0){
        HammingDistance test(s2);
        test.outputResults();
    }

    clock_t end_time = clock();
    cout << "Running time is: " << static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000 << "ms" << endl;


   return 0;
}

