#include "timingstats.h"
#include <iostream>

using namespace std;

//#ifndef __USE_XOPEN2K
//void clock_getcpuclockid(unsigned pid,clockid_t *id){
//   static clockid_t ids = 1;
//   *id = ids++;
//}
//#endif

ostream &operator << (ostream &out,const timespec &t){
  double nsec = 1e9 * t.tv_sec + t.tv_nsec;
  if(nsec > 3600e9){
    nsec /= 3600e9;
    out << nsec << " hours";
  }
  else if(nsec > 60e9){
    nsec /= 60e9;
    out << nsec << " minutes";
  }
  else if(nsec > 1e9){
    nsec /= 1e9;
    out << nsec << " seconds";
  }
  else if(nsec > 1e6){
    nsec /= 1e6;
    out << nsec << " milliseconds";
  }
  else if(nsec > 1e3){
    nsec /= 1e3;
    out << nsec << " microseconds";
  }
  else{
    out << nsec << " nanoseconds";
  }
  return out;
}

clockid_t TimingStats::m_clockID = 0;
timespec TimingStats::m_localTotal = {0,0};
timespec TimingStats::m_localTime = {0,0};
timespec TimingStats::m_localCurrent = {0,0};
timespec TimingStats::m_localLast = {0,0};


void TimingStats::initTimingStats(){
  clock_getcpuclockid(0/*this pid*/,
		      &m_clockID);
}

void TimingStats::resetLocal(){
  m_localTotal.tv_sec = 0;
  m_localTotal.tv_nsec = 0;
}

void TimingStats::startLocal(){
  clock_gettime(m_clockID,&m_localLast);
}

void TimingStats::stopLocal(){
  clock_gettime(m_clockID,&m_localCurrent);
  m_localTime = m_localCurrent - m_localLast;
  m_localTotal = m_localTotal + m_localTime;
}

void TimingStats::printLocal(ostream &out){
  out << "Last local time:" << endl;
  out << "\t" << m_localTime << endl;
}

void TimingStats::printStats(ostream &out){
  out << "Total running time: " << m_localTotal << endl;
}
