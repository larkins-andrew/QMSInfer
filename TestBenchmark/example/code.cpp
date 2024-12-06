//Example 2 in Provably

bool compute(bool k1, bool r1, bool r2){


bool n01;
bool n02;
bool n03;

n01 = r1 ^ r2;
n02 = k1 ^ r1;
n03 = n02 ^ r2;



return(n03);
}