#ifndef __MYURESPONSES_HPP__
#define __MYURESPONSES_HPP__

#include "gpb_server.hpp"

/*
*message UResponses{
*  repeated UFinished completions = 1;
*  repeated UDeliveryMade delivered = 2;
*  optional bool finished = 3;
*  repeated int64 acks = 4;
*  repeated UTruck truckstatus = 5;
*  repeated UErr error = 6;
}
*/

class myUResponses {
   public:
    vector<UFinished> UFinisheds;
    vector<UDeliveryMade> UDeliveryMades;
    bool finished = false;
    vector<UTruck> UTrucks;
    vector<long> seqNums;
    vector<UErr> UErrs;
    // constructor
    myUResponses(const UResponses& r, map<long, bool>& seqNumAcked_world, mutex& mtx);
    ~myUResponses() {}
};

#endif