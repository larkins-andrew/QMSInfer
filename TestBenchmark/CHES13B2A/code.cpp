require import Byte.
op Oxff byte
op Ox00 byte
module M = {
  proc main(x:byte): byte = {

    var r,x1,t,n1,r1;

    (* Presharing x *)
    r = $distr;
    x1 = x ^ r; n01

    r1 = $distr;
    t = x1 ^ r1; n02
    t = t - r1; n03
    t = t ^ x1; n04
    n1 = r1 ^ r; n05
    a = x1 ^ n1; n06
    a = a - n1; n07
    a = a ^ t; n08

  }
}.

bool compute(bool x1, bool x2, bool r1, bool r2, bool k1){


bool n01;
n01 = x2 ^ r1;
n02 = x1 ^ r2;
n03 = n02 - r2;
n04 = n03 ^ n01;
n05 = r2 ^ r1;
n06 = n01 ^ n05;
n07 = n06 - n05;
n08 = n07 ^ n04;


return(n08);
}