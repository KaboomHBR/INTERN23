#include "myUResponses.hpp"

using namespace std;

myUResponses::myUResponses(const UResponses& r, map<long, bool>& seqNumAcked_world, mutex& mtx) {
    // cout << "myUResponses constructor-";
    for (int i = 0; i < r.completions_size(); i++) {
        UFinisheds.push_back(move(r.completions(i)));
        seqNums.push_back(r.completions(i).seqnum());
    }
    for (int i = 0; i < r.delivered_size(); i++) {
        UDeliveryMades.push_back(move(r.delivered(i)));
        seqNums.push_back(r.delivered(i).seqnum());
    }
    if (r.finished()) {
        finished = true;
    }
    for (int i = 0; i < r.truckstatus_size(); i++) {
        UTrucks.push_back(move(r.truckstatus(i)));
        seqNums.push_back(r.truckstatus(i).seqnum());
    }
    for (int i = 0; i < r.error_size(); i++) {
        UErrs.push_back(move(r.error(i)));
        seqNums.push_back(r.error(i).seqnum());
    }
    for (int i = 0; i < r.acks_size(); i++) {
        lock_guard<mutex> lock(mtx);
        seqNumAcked_world[r.acks(i)] = true;
    }
    // use a for loop to print out all the seqNumAcked_world
    // cout << "seqNumAcked_world: ";
    // for (auto a : seqNumAcked_world) {
    //     cout << a.first << "-" << a.second << "__";
    // }
    // cout << endl;
}