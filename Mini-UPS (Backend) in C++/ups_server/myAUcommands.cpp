#include "myAUcommands.hpp"

using namespace std;

myAUcommands::myAUcommands(const AUcommands& r, map<long, bool>& seqNumAcked_amazon, mutex& mtx) {
    // cout << "myAUcommands constructor-";
    for (int i = 0; i < r.pickup_size(); i++) {
        AUreqPickups.push_back(move(r.pickup(i)));
        seqNums.push_back(r.pickup(i).seqnum());
    }
    for (int i = 0; i < r.delivery_size(); i++) {
        AUreqDeliveries.push_back(move(r.delivery(i)));
        seqNums.push_back(r.delivery(i).seqnum());
    }
    for (int i = 0; i < r.changedest_size(); i++) {
        AUchangeDestns.push_back(move(r.changedest(i)));
        seqNums.push_back(r.changedest(i).seqnum());
    }
    for (int i = 0; i < r.bindups_size(); i++) {
        AUbindUPSs.push_back(move(r.bindups(i)));
        seqNums.push_back(r.bindups(i).seqnum());
    }
    for (int i = 0; i < r.err_size(); i++) {
        errs.push_back(move(r.err(i)));
        seqNums.push_back(r.err(i).seqnum());
    }
    if (r.disconnect()) {
        disconnect = true;
    }
    for (int i = 0; i < r.acks_size(); i++) {
        // set seqNumAcked_amazon to true
        lock_guard<mutex> lock(mtx);
        seqNumAcked_amazon[r.acks(i)] = true;
    }
    // cout << "seqNumAcked_amazon: ";
    // for (auto a : seqNumAcked_amazon) {
    //     cout << a.first << "-" << a.second << "__";
    // }
    // cout << endl;
}
