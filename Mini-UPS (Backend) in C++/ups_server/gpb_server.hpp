#ifndef __GPB_SERVER_HPP__
#define __GPB_SERVER_HPP__

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
typedef google::protobuf::io::FileOutputStream Ostream;
typedef google::protobuf::io::FileInputStream Istream;

#include <mutex>
#include <thread>

#include "./proto_files/ups_amazon.pb.h"
#include "./proto_files/world_ups.pb.h"
#include "database_func.hpp"
#include "myAUcommands.hpp"
#include "myUResponses.hpp"
#include "pkg.hpp"
#include "socket.hpp"
#include "truck.hpp"

using namespace std;
using namespace pqxx;

/* Global Variables */
// extern mutex mtx;
extern long worldID;
// extern vector<Truck> truckList;
// extern map<long, bool> seqNumAcked_amazon;  // map from seqNum to whether it's acked, we send the same seqNum from amazon to world
// extern map<long, bool> seqNumAcked_world;   // map from seqNum to whether it's acked by world
extern bool temp_IWANNARUN;  // a temp variable to check if we need to stop the loop

template <typename T>
bool sendMesgTo(const T& message,
                google::protobuf::io::FileOutputStream* out) {
    {
        google::protobuf::io::CodedOutputStream output(out);
        const int size = message.ByteSize();
        output.WriteVarint32(size);
        uint8_t*
            buffer = output.GetDirectBufferForNBytesAndAdvance(size);
        if (buffer != NULL) {
            message.SerializeWithCachedSizesToArray(buffer);
        } else {
            message.SerializeWithCachedSizes(&output);
            if (output.HadError()) {
                return false;
            }
        }
    }
    out->Flush();
    return true;
}

template <typename T>
bool recvMesgFrom(T& message,
                  google::protobuf::io::FileInputStream* in) {
    google::protobuf::io::CodedInputStream input(in);
    uint32_t size;
    if (!input.ReadVarint32(&size)) {
        return false;
    }
    google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);
    if (!message.MergeFromCodedStream(&input)) {
        return false;
    }
    if (!input.ConsumedEntireMessage()) {
        return false;
    }
    input.PopLimit(limit);
    return true;
}

void gpb_server();
void connect_world(int& world_socket, const char* world_Host, const char* world_Port, vector<Truck>& truckList);
void connect_amazon(int& amazon_socket, const char* amazon_Port);

void keep_listen_amazon(int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList);
void keep_listen_world(int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList);
// void keep_querying(int world_socket, vector<Truck>& truckList);
void keep_querying(int world_socket, vector<Truck>& truckList, map<long, bool>& seqNumAcked_world, long& seqNum);

void handle_AUrequests(AUcommands au_commands, int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList);
void handle_WUrequests(UResponses u_responses, int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList);

// for AUrequests
void handle_AUreqPickup(AUreqPickup au_req_pickup, int world_socket, map<long, bool>& seqNumAcked_world, vector<Truck>& truckList);
int assign_truck(int& whID, vector<Truck>& truckList);
void handle_AUreqDelivery(AUreqDelivery au_req_delivery, int world_socket, map<long, bool>& seqNumAcked_world);
void handle_AUchangeDestn(AUchangeDestn au_change_destn, int amazon_socket, map<long, bool>& seqNumAcked_amazon);
void handle_AUbindUPS(AUbindUPS au_bind_ups, int amazon_socket, map<long, bool>& seqNumAcked_amazon);
// void handle_AUErr(Err err, int world_socket, map<long, bool>& seqNumAcked_world);
void handle_disconnect_world(int world_socket);

// for WUrequests, i.e. UResponses
void handle_UFinished(UFinished u_finished, int amazon_socket, map<long, bool>& seqNumAcked_amazon, vector<Truck>& truckList);
int assign_wh(int& truckID, vector<Truck>& truckList);
void handle_UDeliveryMade(UDeliveryMade u_delivery_made, int amazon_socket, map<long, bool>& seqNumAcked_amazon);
void handle_UTruck(UTruck u_truck, int amazon_socket, map<long, bool>& seqNumAcked_amazon, mutex& mtx, vector<Truck>& truckList);
int assign_status(string& status);
void handle_UErr(UErr err, map<long, bool>& seqNumAcked_amazon, mutex& mtx);
void handle_disconnect_amazon(int amazon_socket);

#endif