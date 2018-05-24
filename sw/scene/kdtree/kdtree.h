//applying doxygen style commenting...
/** @file kdtree.h
 *  \brief Contains a kd-tree data structure to allow fast searches.
 *   
 *  kd-tree template written by Brian Budge 11/09/2003 \<p>
 *  search code adapted from Per Christensen, SIGGRAPH coursenotes 2002
 *
 *  To Do:\<p>
 *  2) Make build iterative\<p>
 *  3) Allow different build policies such as alternating dim, high variance dim,
 *  greatest difference dim\<p>
 *  5) Allow epsilon error?  see ANN\<p>
 *  6) Decide design decisions to make this easily applicable to anyone's
 *  kd-tree needs\<p>
 *  8) Allow incremental build, or just static build?\<p>
 */
#ifndef KDTREE_H
#define KDTREE_H

#include <algorithm>
#include <vector>
#include <utility>
#include <iostream>
#include <functional>
#include <cmath>

#ifndef INFINITY
#define INFINITY 1.0 / 0.0
#endif

#ifndef MISCMATH_H
/** fast log base 2 template for real values
 *  should go in some other file later
 */
template<typename real>
real log2(real val){
   const real m_lg_2 = 1.0 / std::log(2.0);
   return std::log(val) * m_lg_2;
}

/** \fn log2I
 *  \brief fast log base 2 for unsigned values
 */
inline unsigned log2I(unsigned val){
   unsigned i = 0;
   val >>= 1;
   for(; val > 0; ++i,val >>= 1);
   return i;
}
#endif

/** \typedef byte
 *  \brief Call unsigned char byte instead
 */
typedef unsigned char byte;

/** \class KdTree
 *  \brief A very fast/adaptable kd-tree template implementation
 */
template <class position_t,
	  class metric,
	  class dimget,
	  unsigned dim = 3,
	  typename real = float>
class KdTree{
private:
   /** \struct KdSplit
    *  kd-tree split structure used to keep
    *  track of splitting plane information
    */
   struct KdSplit{
      /// \var pointer to one position_t
      position_t *data;
      /// \var 
      byte split;
   };

private:
   /// \var the actual data we are storing 
   position_t *m_data; 

   /// \var the splits 
   KdSplit *m_splits; 
   
   /// \var intermediate data
   position_t **m_sortMe;
   
   /// \var the size of the data
   unsigned m_size;

private:
   /// \fn helper function to build the tree
   void createTree(position_t **array,unsigned size,
		   unsigned splitIndex,unsigned depth);

   byte bestSplitDim(position_t **array,unsigned size);
   
public:
   /// \fn constructor
   KdTree(const std::vector<position_t> &data);

   /// \fn returns the nearest position_t to ref according to the specified metric
   position_t getNearest(const position_t &ref);

   /// \fn returns k nearest position_t's to ref according to the specified metric
   std::vector<position_t> getKNearest(const position_t &ref,unsigned k);

   /// \fn returns all position_t's within radius (according to metric) of ref
   std::vector<position_t> getFixedRadius(const position_t &ref, real radius);

   /// \fn destructor
   ~KdTree(); 
};

#include "kdtree.hxx"

#endif//KDTREE_H
