//Example 2 in Provably

bool compute( bool k1, bool k2, bool r1, bool r2, bool r3){


bool n01;
bool n02;
bool n03;
bool n04;
bool n05;
bool n06;
bool n07;
bool n08;
bool n09;
bool n10;
bool n11;


n01 = k1 ^ r1;
n02 = k2 ^ r2;
n03 = n01 & n02;

n04 = k1 ^ r1;
n05 = r2 & n04;


n06 = r1 & r2;
n07 = n06 ^ r3;

n08 = n05 | n07;

n09 = k2 ^ r2;
n10 = r1 & n09;


n11 = n03 ^ n10;


return(n11);
} 
