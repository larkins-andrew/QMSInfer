//Example 2 in Provably

bool compute(bool k1, bool k2, bool r1, bool r2){


bool n01;
bool n02;
bool n03;
bool n04;

n01 = r1;
n02 = r2;
n03 = k1 ^ r1;
n04 = k2 ^ r2;



return(n02);
}