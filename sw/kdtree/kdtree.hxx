#include <cstring>

//////////////////////////////////////////////////////////////////
//Constructor
//////////////////////////////////////////////////////////////////
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
KdTree<position_t,metric,dimget,dim,real>::KdTree(const std::vector<position_t> &data) : m_size(data.size()){
   m_data = new position_t[m_size];
   std::memcpy(m_data,&data[0],m_size * sizeof(position_t));

   m_splits = new KdSplit[m_size + 1];
   m_sortMe = new position_t*[m_size];

   register position_t **k = m_sortMe + m_size; 
   for(position_t **i = m_sortMe, *j = m_data; i < k; ++i, ++j){
      *i = j;
   }
   
   createTree(m_sortMe,m_size,1,0);
   delete[] m_sortMe;
}
//////////////////////////////////////////////////////////////////
//Desctructor
//////////////////////////////////////////////////////////////////
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
KdTree<position_t,metric,dimget,dim,real>::~KdTree(){
   delete[] m_splits;
   delete[] m_data;
}

static unsigned sortDim = 0;

template <class position_t,class dimget>
bool position_lessthan(const position_t *const p1,const position_t *const p2){
   const dimget dg;
   return dg(*p1,sortDim) < dg(*p2,sortDim);
}

//---------------------------------------------------------------------
//FIND BEST SPLITTING DIRECTION - the one with highest variance
//---------------------------------------------------------------------
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
byte KdTree<position_t,metric,dimget,dim,real>::bestSplitDim(position_t **array,unsigned size){
   real E[dim];
   real V[dim];
   dimget getter;

   //CALCULATE EXPECTED VALUE
   for(unsigned j = 0; j < dim; ++j){
      E[j] = V[j] = 0.0;
   }

   for(unsigned i = 0; i < size; ++i){
      for(unsigned j = 0; j < dim; ++j){
	 E[j] += getter(*(array[i]),j);
      }
   }
   real denom = 1.0 / size;
   for(unsigned j = 0; j < dim; ++j){
      E[j] *= denom;
   }

   //CALCULATE VARIANCE
   for(unsigned i = 0; i < size; ++i){
      for(unsigned j = 0; j < dim; ++j){
	 real dif = (E[j] - getter(*(array[i]),j));
	 V[j] += dif * dif;
      }
   }
   unsigned largestDim = 0;
   for(unsigned j = 0; j < dim; ++j){
      V[j] *= denom;
      if(V[j] > V[largestDim]){
	 largestDim = j;
      }
   }
   return largestDim;
}

//---------------------------------------------------------------------
//CREATE TREE
//---------------------------------------------------------------------
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
void KdTree<position_t,metric,dimget,dim,real>::createTree(position_t **array,
							   unsigned size,
							   unsigned splitIndex,
							   unsigned depth){
   const dimget dg;
   //do this stupidly for now :(
      //ideally, we want to sort in the "best" direction
      //we should make this a policy... but not sure how
   sortDim = depth % dim;

      //try getting the best split dimension
      //sortDim = bestSplitDim(array,size);
   /*if(size == 2){
      position_t *one,*two;
      one = (dg(*array[0],sortDim) < dg(*array[1],sortDim)) ? array[0] : array[1];
      two = (one == array[0]) ? array[1] : array[0];
      
      m_splits[splitIndex].data = two;
      m_splits[splitIndex].split = sortDim;
      m_splits[splitIndex << 1].data = one;
      m_splits[splitIndex << 1].split = 0;
   }
   else */
   if(size == 1){
      m_splits[splitIndex].data = array[0];
      m_splits[splitIndex].split = sortDim;
   }
   else if(size == 0){
      //do nothing... split already contains the value
   }
   else{
      

      //calculate to ensure left balanced-ness :)
      int level = log2I(size);//static_cast<unsigned>(log2(static_cast<real>(size)));//number of levels in array
      int numElements = size - ((1 << level) - 1);//# of elements in level
      int LHS;
      int RHS;
      int midIndex;

      int extra = numElements - (1 << (level - 1));//#elements in right subtree row level
                                                        //(if positive!)
      if(extra > 0){
	 RHS = ((1 << (level - 1)) - 1) + extra;
	 LHS = ((1 << level) - 1);
      }
      else{
	 RHS = (1 << (level - 1)) - 1;
	 LHS = RHS + numElements;
      }

      midIndex = LHS;

      //"order" the elements in the sortDim dimension
      std::nth_element(array,array + midIndex,array + size,
		       position_lessthan<position_t,dimget>);
      
      //assign the correct value and sorting dimension to the correct index
      m_splits[splitIndex].data = array[midIndex];
      m_splits[splitIndex].split = sortDim;


      //create the left subtree
      createTree(array,
		 LHS,
		 (splitIndex << 1),
		 depth + 1);
      //create the right subtree
      createTree(array + midIndex + 1,
		 RHS,
		 (splitIndex << 1) + 1,
		 depth + 1);
   }
}


template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
position_t KdTree<position_t,metric,dimget,dim,real>::getNearest(const position_t &ref){
   const dimget dg;
   const metric met;
   position_t closest;
   real distSq = INFINITY;

   unsigned height = log2I(m_size);//static_cast<unsigned>(std::ceil(log2(static_cast<real>(m_size))));
					   
   unsigned *parent = new unsigned[height];
   real *dist1_2 = new real[height];
   unsigned level = 0;
   unsigned index = 1;

   //as long as we haven't eliminated all possibilities
   while(true){
      //move down through the subtrees until leaf is reached
      while(index < ((m_size+1) >> 1)){//(m_splits.size() >> 1) ){
	 byte split = m_splits[index].split;
	 real dist1 = dg(ref,split) - dg(*(m_splits[index].data),split);
	 dist1_2[level] = dist1 * dist1;

	 index = index << 1;

	 if(dist1 > 0.0){
	    ++index;//choose the right child
	 }

	 parent[level] = index;
	 ++level;
      }

      //see if the leaf is the closest so far - if so update
      real dist2 = met(*(m_splits[index].data),ref);
      if(distSq > dist2){
	 distSq = dist2;
	 closest = *m_splits[index].data;
      }

      //move back up the tree until we reach a position we need to check (+ other subtree!)
      unsigned from;
      do{
	 from = index;
	 index = index >> 1;
	 --level;//go to parent
	 if(index == 0){
	    delete[] parent;
	    delete[] dist1_2;
	    return closest;
	 }
      }while(dist1_2[level] >= distSq || from != parent[level]);

      //see if the non-leaf is the closest - if so update
      dist2 = met(*(m_splits[index].data),ref);
      if(distSq > dist2){
	 distSq = dist2;
	 closest =*m_splits[index].data;
      }

      //step into the other subtree
      index = parent[level] ^ 1;
      ++level;
   }
   
}
    
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
std::vector<position_t> KdTree<position_t,metric,dimget,dim,real>::getFixedRadius(const position_t &ref,real radius){
   const dimget dg;
   const metric met;
   std::vector<position_t> closest;
   real distSq = radius * radius;

   unsigned height = log2I(m_size);//static_cast<unsigned>(std::ceil(log2(static_cast<real>(m_size))));
   unsigned *parent = new unsigned[height];
   real *dist1_2 = new real[height];
   unsigned level = 0;
   unsigned index = 1;

   //as long as we haven't eliminated all possibilities
   while(true){
      //move down through the subtrees until leaf is reached
      while(index < ((m_size+1) >> 1)){//(m_splits.size() >> 1) ){
	 byte split = m_splits[index].split;
	 real dist1 = dg(ref,split) - dg(*(m_splits[index].data),split);
	 dist1_2[level] = dist1 * dist1;

	 index = index << 1;

	 if(dist1 > 0.0){
	    ++index;//choose the right child
	 }

	 parent[level] = index;
	 ++level;
      }

      //see if the leaf is within the radius - if so update
      real dist2 = met(*(m_splits[index].data),ref);
      if(distSq > dist2){
	 closest.push_back(*m_splits[index].data);
      }

      //move back up the tree until we reach a position we need to check (+ other subtree!)
      unsigned from;
      do{
	 from = index;
	 index = index >> 1;
	 --level;//go to parent
	 if(index == 0){
	    delete[] parent;
	    delete[] dist1_2;
	    return closest;
	 }
      }while(dist1_2[level] >= distSq || from != parent[level]);

      //see if the non-leaf is the closest - if so update
      dist2 = met(*(m_splits[index].data),ref);
      if(distSq > dist2){
	 closest.push_back(*m_splits[index].data);
      }

      //step into the other subtree
      index = parent[level] ^ 1;
      ++level;
   }
}

//pair compare
template<typename real,class position_t>
struct pair_compare{
   bool operator () (const std::pair<real,position_t*> &one,const std::pair<real,position_t*> &two){
      return one.first < two.first;
   }
};

////////////////////////////////////
//code fully designed by Brian Budge
////////////////////////////////////
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
std::vector<position_t> KdTree<position_t,metric,dimget,dim,real>::getKNearest(const position_t &ref,unsigned k){
   typedef std::pair<real,position_t*> position_pair;

   
   const dimget dg;
   const metric met;
   
   std::pair<real, position_t*> *closest = new position_pair[k];
   std::pair<real, position_t*> *closest_k = closest + k;
   std::pair<real, position_t*> *closest_km1 = closest + k-1;
   real distSq = INFINITY;
   real distTemp = 0;

   unsigned height = log2I(m_size) + 1;
   unsigned *chosen = new unsigned[height];
   real *dist1d_2 = new real[height];
   int level = 0;
   unsigned index = 1;
   unsigned kSoFar = 0;
   pair_compare<real,position_t> the_pair_compare = pair_compare<real,position_t>();
   //----------------------------------------------------------
   //as long as we haven't eliminated all possibilities
   while(true){
      //move down through the subtrees until leaf is reached
      while(index < m_size+1){
	 byte split = m_splits[index].split;
	 real dist1d = dg(ref,split) - dg(*(m_splits[index].data),split);
	 dist1d_2[level] = dist1d * dist1d;

	 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 //Decide if m_splits[index] (the node) should be added
	 real dist2 = met(*(m_splits[index].data),ref);
	 if(kSoFar < (k-1)){
	    closest[kSoFar] = position_pair(dist2,m_splits[index].data);
	    ++kSoFar;
	 }
	 else if(kSoFar == (k-1)){
	    closest[kSoFar] = position_pair(dist2,m_splits[index].data);
	    ++kSoFar;
	    make_heap(closest,closest + kSoFar,the_pair_compare);
	    distSq = closest[0].first;
	 }
	 else if(distSq > dist2){
	    pop_heap(closest,closest_k,the_pair_compare);
	    
	    *closest_km1 = position_pair(dist2,m_splits[index].data);
	    push_heap(closest,closest_k,the_pair_compare);
	    distSq = closest[0].first;
	 }
	 //
	 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 
	 index <<= 1;//move to child

	 if(dist1d > 0.0){//right or left child?
	    ++index;//choose the right child
	 }
	 chosen[level] = index;
	 ++level;
      }
      //
      //----------------------------------------------------------
      
      //----------------------------------------------------------
      //move up levels until we find another subtree to descend
      do{
	 --level;
	 if(level < 0){//finished... return values
	    std::vector<position_t> posVect(kSoFar);
	    position_t *ptp = &posVect[0];
	    position_t *ptpe = &posVect[0] + kSoFar;
	    std::pair<real,position_t*> *i = closest;
	    for(; ptp < ptpe; ++ptp,++i){
	       *ptp = *(i->second);
	    }
	    
	    delete[] chosen;
	    delete[] dist1d_2;
	    delete[] closest;
	    
	    return posVect;
	 }
      }while(dist1d_2[level] > distSq);
      //
      //----------------------------------------------------------

      //step into the other subtree
      index = chosen[level] ^ 1;
   }
}

/*
////////////////////////////////////
//code adapted from Christensen
////////////////////////////////////
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim,
	  typename real>
std::vector<position_t> KdTree<position_t,metric,dimget,dim,real>::getKNearest(const position_t &ref,unsigned k){
   //   typedef typename std::map<real,position_t*,std::less<real> >::iterator M_I;

   const dimget dg;
   const metric met;

   
   std::pair<real, position_t*> *closest = new std::pair<real, position_t*>[k];
   real distSq = INFINITY;
   real distTemp = 0;

   unsigned height = log2I(m_size) + 1;
   unsigned *chosen = new unsigned[height];
   real *dist1d_2 = new real[height];
   unsigned level = 0;
   unsigned index = 1;
   unsigned kSoFar = 0;
   pair_compare<real,position_t> the_pair_compare = pair_compare<real,position_t>();

   //as long as we haven't eliminated all possibilities
   while(true){
      //move down through the subtrees until leaf is reached
      while(index < (m_size+1) >> 1){
	 byte split = m_splits[index].split;
	 real dist1d = dg(ref,split) - dg(*(m_splits[index].data),split);
	 dist1d_2[level] = dist1d * dist1d;

	 index <<=  1;//move to child

	 if(dist1d > 0.0){//right or left child?
	    ++index;//choose the right child
	 }
	 //else choose left child

	 chosen[level] = index;
	 ++level;
      }

      //see if the leaf is the closest so far - if so update
      real dist2 = met(*(m_splits[index].data),ref);
      if(kSoFar < (k-1)){
	 closest[kSoFar] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 ++kSoFar;
	 //push_heap(closest,closest + kSoFar,the_pair_compare);
      }
      else if(kSoFar == (k-1)){
	 closest[kSoFar] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 ++kSoFar;
	 make_heap(closest,closest + kSoFar,the_pair_compare);
	 distSq = closest[0].first;
      }
      else if(distSq >= dist2){
	 pop_heap(closest,closest + k,the_pair_compare);
	   
	 closest[k-1] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 push_heap(closest,closest + k,the_pair_compare);
	 distSq = closest[0].first;
      }
      
      //move back up the tree until we reach a position we need to check (+ other subtree!)

      unsigned from;
      from = index;
      index >>= 1;//go to parent
      --level;//go to parent
      while(dist1d_2[level] >= distSq || from != chosen[level]){
	 from = index;
	 index >>= 1;//go to parent
	 --level;//go to parent
	 if(index == 0){//finished... return values
	    std::vector<position_t> posVect(kSoFar);
	    position_t *ptp = &posVect[0];
	    position_t *ptpe = &posVect[0] + kSoFar;
	    std::pair<real,position_t*> *i = closest;
	    for(; ptp < ptpe; ++ptp,++i){
	       *ptp = *(i->second);
	    }
	    delete[] chosen;
	    delete[] dist1d_2;
	    delete[] closest;
	    
	    return posVect;
	 }
      }


      //see if the non-leaf is the closest - if so update
      dist2 = met(*(m_splits[index].data),ref);
      if(kSoFar < (k-1)){
	 closest[kSoFar] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 ++kSoFar;
	 //push_heap(closest,closest + kSoFar,the_pair_compare);
      }
      else if(kSoFar == (k-1)){
	 closest[kSoFar] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 ++kSoFar;
	 make_heap(closest,closest + kSoFar,the_pair_compare);
	 distSq = closest[0].first;
      }
      else if(distSq >= dist2){
	 pop_heap(closest,closest + k,the_pair_compare);
	   
	 closest[k-1] = std::pair<real,position_t*>(dist2,m_splits[index].data);
	 push_heap(closest,closest + k,the_pair_compare);
	 distSq = closest[0].first;
      }
     
      //step into the other subtree
      index = chosen[level] ^ 1;
      ++level;
   }
}
*/
