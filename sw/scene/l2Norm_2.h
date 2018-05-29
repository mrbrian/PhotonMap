#pragma once

#include <photon.h>

//l2 norm squared

struct L2Norm_2{
   L2Norm_2(){}
   float operator () (const photon &n1,const photon &n2)const{
      return n1.dist2(n2);
   }
};

struct GetDim{
   GetDim(){}
   float operator () (const photon &n1,unsigned dim)const{
    switch (dim)
    {
    case 0:
        return n1.x;
    case 1:
        return n1.y;
    case 2:
        return n1.z;
    default:
        printf("incorrect dim parameter\n");
        throw -1;
    }
   }
};
