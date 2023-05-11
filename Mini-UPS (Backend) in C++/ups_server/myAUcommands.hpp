#ifndef __MYAUCOMMAND_HPP__
#define __MYAUCOMMAND_HPP__

#include "gpb_server.hpp"

/*
*message AUcommands{
*	repeated AUreqPickup pickup = 1;
*	repeated AUreqDelivery delivery =  2;
*	repeated AUquery query = 3;
*	repeated AUchangeDestn changeDest = 4;
*	repeated AUbindUPS bindUPS= 5;
*	repeated Err err = 6;
*	optional bool disconnect = 7;
*	repeated int64 acks = 8;
}
*/

class myAUcommands {
   public:
    vector<AUreqPickup> AUreqPickups;
    vector<AUreqDelivery> AUreqDeliveries;
    vector<AUchangeDestn> AUchangeDestns;
    vector<AUbindUPS> AUbindUPSs;
    vector<long> seqNums;
    vector<Err> errs;
    bool disconnect = false;
    // constructor
    myAUcommands(const AUcommands& r, map<long, bool>& seqNumAcked_amazon, mutex& mtx);
    ~myAUcommands() {}
};

#endif