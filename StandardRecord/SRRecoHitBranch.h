#ifndef CAFSRRECOHITBRANCH_H
#define CAFSRRECOHITBRANCH_H

#include "garsoft/StandardRecord/SRTPCRecoHit.h"
#include "garsoft/StandardRecord/SRRecoHit.h"

#include <vector>

namespace caf
{
  class SRRecoHitBranch
  {
  public:
    SRRecoHitBranch();

    unsigned int ntpc;
    std::vector<SRTPCRecoHit> tpc;

    unsigned int necal;
    float ecaltotE;
    std::vector<SRRecoHit> ecal;

    unsigned int nmuid;
    float muidtotE;
    std::vector<SRRecoHit> muid;
  };
}

#endif
