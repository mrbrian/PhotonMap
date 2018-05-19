#ifndef TIMINGSTATS_H
#define TIMINGSTATS_H

//use the old school time header to get the clock_gettime() etc... functions
#include <time.h>
#include <iostream>
const unsigned one_billion = 1000 * 1000 * 1000;

#ifndef __USE_XOPEN2K
/*struct timespec{
   long int tv_sec;
   long int tv_nsec;
   };*/
typedef __clockid_t clockid_t;

void clock_getcpuclockid(unsigned pid,clockid_t *id);
/*
inline void clock_gettime(clockid_t id,timespec *ts){
   clock_t t = clock();
   ts->tv_sec = t / CLOCKS_PER_SEC;
   ts->tv_nsec = 1000 * (t % CLOCKS_PER_SEC);
}
*/
#endif

inline timespec operator - (const timespec &t1,const timespec &t2){
  timespec t;
  t.tv_sec = t1.tv_sec - t2.tv_sec;
  if(t1.tv_nsec < t2.tv_nsec){
    t.tv_sec--;
    t.tv_nsec = one_billion + t1.tv_nsec - t2.tv_nsec;
  }
  else{
    t.tv_nsec = t1.tv_nsec - t2.tv_nsec;
  }
  return t;
}


inline timespec operator + (const timespec &t1,const timespec &t2){
  timespec t;
  unsigned nsec = t1.tv_nsec + t2.tv_nsec;
  if(nsec > one_billion){
    t.tv_sec = t1.tv_sec + t2.tv_sec + 1;
    t.tv_nsec = nsec % one_billion;
  }
  else{
    t.tv_sec = t1.tv_sec + t2.tv_sec;
    t.tv_nsec = nsec;
  }
  return t;
}

inline bool operator < (const timespec &t1,const timespec &t2){
  if(t1.tv_sec < t2.tv_sec){
    return true;
  }
  else if(t1.tv_sec > t2.tv_sec){
    return false;
  }
  else{
    return t1.tv_nsec < t2.tv_nsec;
  }
}

inline timespec operator / (const timespec &t,unsigned n){
  timespec tout;
  tout.tv_sec = t.tv_sec / n;
  if(t.tv_sec < 1){
    tout.tv_nsec = (one_billion * t.tv_sec + t.tv_nsec) / n;
  }
  else{
    tout.tv_nsec = t.tv_nsec / n;
  }
  return tout;
}

class TimingStats{
  static clockid_t m_clockID;
  
  static timespec m_localTotal;
  static timespec m_localTime;
  static timespec m_localCurrent;
  static timespec m_localLast;

public:
  static void initTimingStats();

  static void resetLocal();
  static void startLocal();
  static void stopLocal();

  static void printLocal(std::ostream &out = std::cout);
  
  static void printStats(std::ostream &out = std::cout);
};
  

#endif 
