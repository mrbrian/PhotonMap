#include <kdtree/kdtree.h>
//#include "../kdtree2/kdtree.h"
#include <cstdio>
#include <vector>
#include <iostream>
#include <ctime>
#include "timingstats.h"

using namespace std;

typedef float real;

class Vector{
private:
   real m_data[3];
public:
   Vector(){
       m_data[0] = 0, m_data[1] = 0, m_data[2] = 0;
   }
   Vector(real x,real y,real z){
      m_data[0] = x, m_data[1] = y, m_data[2] = z;
   }

   real operator [] (unsigned index)const {
      return m_data[index];
   }

   real &operator [] (unsigned index){
      return m_data[index];
   }

   bool operator == (const Vector &v)const{
      return ((v.m_data[0] == m_data[0]) &&
	      (v.m_data[1] == m_data[1]) &&
	      (v.m_data[2] == m_data[2]));
   }
};

class Photon{
public:
   Vector m_pos;
   char m_data[30];//just a bunch of stuff

public:
   Photon(){}
   Photon(real x,real y,real z) :m_pos(x,y,z){}
   
   real getDim(unsigned dim)const{
      return m_pos[dim];
   }

   bool operator == (const Photon &n)const{
      return (n.m_pos == m_pos);
   }
   
   real dist2(const Photon &n)const{
      real x = n.getDim(0) - getDim(0);
      real y = n.getDim(1) - getDim(1);
      real z = n.getDim(2) - getDim(2);
      return x*x + y*y + z*z;
   }

};

ostream &operator << (ostream &out, const Photon &n){
   out << "(" << n.getDim(0) << "," << n.getDim(1) << "," << n.getDim(2) << ")";
   return out;
}

//l2 norm squared

struct L2Norm_2{
   L2Norm_2(){}
   real operator () (const Photon &n1,const Photon &n2)const{
      return n1.dist2(n2);
   }
};

struct GetDim{
   GetDim(){}
   real operator () (const Photon &n1,unsigned dim)const{
      return n1.getDim(dim);
   }
};

unsigned totalPhotons = 10000;//one million photons
unsigned numToGet = 6;

int main(){

   unsigned the_time = time(NULL);
   // unsigned the_time = 1082917058;
      //1082917058 - 2172
   //1082916187 - 4596
   
   srand48(the_time);
   TimingStats::initTimingStats();

   for(int f = 0; f < 1; ++f){
      totalPhotons = 40000;//1000 + drand48() * 10000;
      numToGet = 100;//1 + drand48() * 200;
      //totalPhotons = 2172;
      //numToGet = 5;
      //totalPhotons = 4596;
      //numToGet = 10;
      cerr << "Total photons: " << totalPhotons << ", Num to get: " << numToGet << endl;
   vector<Photon> nodes;
   

   for(int i = 0; i < totalPhotons; ++i){
      //nodes.push_back(Photon(randomNumbers[3*i],randomNumbers[3*i+1],randomNumbers[3*i+2]));
      nodes.push_back(Photon(drand48(),drand48(),drand48()));
      //      cerr << nodes[i] << endl;
   }
   
   cerr << "Building kd-tree" << endl;
   TimingStats::startLocal();
   KdTree<Photon,L2Norm_2,GetDim,3,real> myTree(nodes);
   TimingStats::stopLocal();
   cerr << "Finished building" << endl;
   TimingStats::printLocal();

   cerr << endl << "Nearest: ";
   //cerr << closest << endl;


   Photon tester(0.5,0.5,0.5);
  
   vector<Photon> closest;
      TimingStats::startLocal();

      for(int i = 0; i < 100000; ++i){
      closest = myTree.getKNearest(tester, numToGet);
      }
   Photon vc = myTree.getNearest(tester);
   cerr << "Very closest" << vc << endl;
      TimingStats::stopLocal();
      TimingStats::printLocal();

   cerr << "Brute force: " << endl;


   //tester.m_pos[1] = 0.25;

   vector<bool> checkedIndex( totalPhotons, false);
   vector<Photon> cl2;
   unsigned k = 0;
   while(k < numToGet){
      real cldist = INFINITY;
      int bestIndex = -1;
      for(int i = 0; i < totalPhotons; ++i){
	
	 if(!checkedIndex[i] && nodes[i].dist2(tester) < cldist){
	    bestIndex = i;
	    cldist = nodes[i].dist2(tester);
	 }
      }
      if(bestIndex == -1){
	 break;
      }
      checkedIndex[bestIndex] = true;
      cl2.push_back(nodes[bestIndex]);
      ++k;
   }
   
   cerr << "Is there a difference?" << endl;

   if(closest.size() != cl2.size()){
      cerr << "Yep!" << endl;
      cerr << closest.size() << " " << cl2.size() << endl;
      cerr << "THE TIME = " << the_time  << endl;;

      return 0;
   }
   else{
      bool print = false;
      for(unsigned i = 0; i < closest.size(); ++i){
	 bool found = false;
	 for(unsigned j = 0; j < cl2.size(); ++j){
	    if(closest[i] == cl2[j]){
	       found = true;
	    }
	 }
	 if(!found){
	    cerr << "Yep!" << endl;
	    cerr << "Closest[" << i << "] was not found: " << closest[i] << endl;
	    print = true;
	    real cval = 0,c2val = 0;
	    for(int j = 0; j < closest.size(); ++j){
	       cval += closest[j].dist2(tester);
	    }
	    for(int j = 0; j < cl2.size(); ++j){
	       c2val += cl2[j].dist2(tester);
	    }
	    cerr << "Cval: " << cval << " " << "C2Val: " << c2val << endl;

	 }
	 if(print){
	    for(int i = 0; i < closest.size(); ++i){
	       cerr << closest[i] << " ";
	    }

	    cerr << "\n" << endl;
	    for(int i = 0; i < cl2.size(); ++i){
	       cerr << cl2[i] << " ";
	    }    
   cerr << "THE TIME = " << the_time  << endl;;
	    return 0;
	 }
      }
   }
   cerr << "Nope" << endl;
    }
   /*
   real distance = 100000;

   for(int i = 0; i < (1 << power) + offset; ++i){
      if(nodes[i].dist2(Node(0.5,0.5)) < distance){
	 distance = nodes[i].dist2(Node(0.5,0.5));
	 closest = nodes[i];
      }
   }
   cerr << closest << endl;
   */			   
   cerr << "THE TIME = " << the_time  << endl;
   return 0;
}
