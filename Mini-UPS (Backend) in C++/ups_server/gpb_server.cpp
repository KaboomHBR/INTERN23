#include "gpb_server.hpp"

#define MAX_SEQ_NUM 50000

using namespace std;

// mutex mtx;
long worldID = -1;
// vector<Truck> truckList;
// map<long, bool> seqNumAcked_amazon;  // map from seqNum to whether it's acked, we send the same seqNum from amazon to world
// map<long, bool> seqNumAcked_world;   // map from seqNum to whether it's acked by world
bool temp_IWANNARUN = true;

void gpb_server() {
    // const char * web_port = "12951";
    const char* world_Host = "vcm-32254.vm.duke.edu";
    const char* world_Port = "12345";
    int world_socket = -1;
    const char* amazon_Port = "34567";
    int amazon_socket = -1;

    // Step 0: now try pass the seqNumAckeds as a parameter to the thread
    mutex mtx;
    map<long, bool> seqNumAcked_amazon;  // map from seqNum to whether it's acked, we send the same seqNum from amazon to world
    map<long, bool> seqNumAcked_world;   // map from seqNum to whether it's acked by world
    vector<Truck> truckList;
    long local_seqNum = 10000;
    // Step 1: connect to world and amazon
    try {
        connect_world(world_socket, world_Host, world_Port, truckList);
        connect_amazon(amazon_socket, amazon_Port);  // todo: uncomment this line later
    } catch (const exception& e) {
        cout << "Error in connecting to world or amazon" << endl;
        cerr << e.what() << endl;
        close(world_socket);
        close(amazon_socket);  // todo: uncomment this line later
        return;
    }
    // Step 2: create a thread whenever there's a new AUcommands coming from amazon
    try {
        thread t_amazon(keep_listen_amazon, amazon_socket, world_socket, ref(seqNumAcked_amazon), ref(seqNumAcked_world), ref(mtx), ref(truckList));
        thread t_world(keep_listen_world, amazon_socket, world_socket, ref(seqNumAcked_amazon), ref(seqNumAcked_world), ref(mtx), ref(truckList));
        // thread t_querying(keep_querying, world_socket, ref(truckList));
        thread t_querying(keep_querying, world_socket, ref(truckList), ref(seqNumAcked_world), ref(local_seqNum));
        t_amazon.join();
        t_world.join();
        t_querying.join();
    } catch (const exception& e) {
        cout << "Error in creating thread to handle AUcommand" << endl;
        cerr << e.what() << endl;
        close(world_socket);
        close(amazon_socket);  // todo: uncomment this line later
        return;
    }
    sleep(5);
    close(world_socket);   // todo: unnecessary later
    close(amazon_socket);  // todo: uncomment this line later
}

void connect_world(int& world_socket, const char* world_Host, const char* world_Port, vector<Truck>& truckList) {
    // Step 1: create a socket
    world_socket = create_client_socket(world_Host, world_Port);
    if (world_socket < 0) {
        throw runtime_error("Error in creating socket");
    }
    // Step 2: send UConnect to world
    UConnect connect_msg;
    int num_Trucks = 65;
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int truckStatus = 0;  // default: idle
    for (int i = 0; i < num_Trucks; i++) {
        UInitTruck* init_truck = connect_msg.add_trucks();
        init_truck->set_id(i);
        init_truck->set_x(1 + i * 2);
        init_truck->set_y(1 + i * 2);
        truckList.push_back(Truck(i, 1 + i * 2, 1 + i * 2, truckStatus));
        // insert into TRUCK table
        insert_TRUCK(C, i, truckStatus);
    }
    // connect_msg.set_worldid(worldID);  // todo: comment out this line for a new world
    connect_msg.set_isamazon(false);
    unique_ptr<Ostream> out(new Ostream(world_socket));
    if (!sendMesgTo<UConnect>(connect_msg, out.get())) {
        throw runtime_error("Error in sending UConnect");
    }
    // Step 3: receive UConnected from world
    UConnected connected_msg;
    unique_ptr<Istream> in(new Istream(world_socket));
    if (!recvMesgFrom<UConnected>(connected_msg, in.get())) {
        throw runtime_error("Error in receiving UConnected");
    }
    if (connected_msg.result() != "connected!") {
        throw runtime_error("Error in initializing world");
    }
    worldID = connected_msg.worldid();
    delete C;
    cout << "Connected to world with worldID: " << worldID << endl;
}

void connect_amazon(int& amazon_socket, const char* amazon_Port) {
    // Step 1: create a socket as a server and wait for amazon to connect
    amazon_socket = create_server_socket(amazon_Port);
    cout << "socket created " << amazon_socket << endl;
    // Step 2: send UAinitWorld to amazon
    UAinitWorld init_msg;
    init_msg.set_worldid(worldID);
    unique_ptr<Ostream> out(new Ostream(amazon_socket));
    if (!sendMesgTo<UAinitWorld>(init_msg, out.get())) {
        cout << "Error in sending UAinitWorld" << endl;
    }
    // Step 3: receive AUconnectedWorld from amazon with a while loop
    AUconnectedWorld connected_msg;
    unique_ptr<Istream> in(new Istream(amazon_socket));
    while (true) {
        recvMesgFrom<AUconnectedWorld>(connected_msg, in.get());
        if (connected_msg.success()) {
            break;
        }
    }
    cout << "Connected to amazon" << endl;
}

void keep_listen_amazon(int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList) {
    AUcommands au_commands;
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    try {
        do {
            if (recvMesgFrom<AUcommands>(au_commands, in_amazon.get())) {
                cout << "---Received AUcommands from amazon---" << endl;
                cout << au_commands.DebugString() << endl;
                if (au_commands.ByteSize() == 0) {
                    continue;
                }
                thread t(handle_AUrequests, au_commands, amazon_socket, world_socket, ref(seqNumAcked_amazon), ref(seqNumAcked_world), ref(mtx), ref(truckList));
                t.detach();
                au_commands.Clear();
            }
        } while (temp_IWANNARUN);
    } catch (void*) {
        cout << "amazon wrong" << endl;
    }
}

void keep_listen_world(int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList) {
    UResponses u_responses;
    unique_ptr<Istream> in_world(new Istream(world_socket));
    try {
        do {
            if (recvMesgFrom<UResponses>(u_responses, in_world.get())) {
                cout << "---Received UResponses from world---" << endl;
                cout << u_responses.DebugString() << endl;
                if (u_responses.ByteSize() == 0) {
                    cout << "w infact empty" << endl;
                    continue;
                }
                thread t(handle_WUrequests, u_responses, amazon_socket, world_socket, ref(seqNumAcked_amazon), ref(seqNumAcked_world), ref(mtx), ref(truckList));
                t.detach();
                // clear up the u_responses
                u_responses.Clear();
            }
        } while (temp_IWANNARUN);
    } catch (void*) {
        cout << "world wrong" << endl;
    }
}

// void keep_querying(int world_socket, vector<Truck>& truckList) {
void keep_querying(int world_socket, vector<Truck>& truckList, map<long, bool>& seqNumAcked_world, long& seqNum) {
    // keep sending UCommands with all the UQueries to world
    UCommands u_commands;
    // long local_seqNum = 10000;
    unique_ptr<Ostream> out(new Ostream(world_socket));
    while (temp_IWANNARUN) {
        for (auto& truck : truckList) {
            if (truck.status != 0) {
                UQuery* query = u_commands.add_queries();
                query->set_truckid(truck.id);
                query->set_seqnum(seqNum);
                seqNumAcked_world[seqNum] = false;
                seqNum++;
            }
        }
        if (!sendMesgTo<UCommands>(u_commands, out.get())) {
            cout << "Error in sending UCommands for querying" << endl;
        }
        sleep(20);
    }
}

void handle_AUrequests(AUcommands au_commands, int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList) {
    // First send an Acked Only UAcommands to amazon
    // Then check each of the seqnum from AUcommand, see if they are in SeqNumAcked_world:
    // if not, this is a first seen AUcommand:
    //      add it to SeqNumAcked_world, set to false, and proceed to handle it by spawn bunch of threads to send UCommands to world,
    //      and resend them if SeqNumAcked_world is still false after a certain amount of time; so each thread corrosponds to one seqNum
    // if yes and it's not acked, no need to do anything cuz the threads above are still running on the same issue;
    // if yes and it's acked, don't do anything.
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    myAUcommands my_au_commands(au_commands, seqNumAcked_amazon, mtx);
    // Step 1: send an Acked Only UAcommands to amazon // todo: not robust, what if this is lost?
    if (my_au_commands.seqNums.size() != 0) {
        UAcommands ua_commands;
        cout << "acks -- ";
        for (int i = 0; i < my_au_commands.seqNums.size(); i++) {
            ua_commands.add_acks(i);
            ua_commands.set_acks(i, my_au_commands.seqNums[i]);
            cout << " " << my_au_commands.seqNums[i];
        }
        cout << endl;
        if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
            throw runtime_error("handle_AUrequests: Error in sending UAcommands");
        }
        cout << "---Sent UAcommands (acks only) to amazon---" << endl;
        cout << ua_commands.DebugString() << endl;
    }
    // Step 2: check each of the seqnum from AUcommand, see if they are in SeqNumAcked_world:
    lock_guard<mutex> lock(mtx);
    for (auto a : my_au_commands.AUreqPickups) {
        if (!seqNumAcked_world.count(a.seqnum() + 1)) {
            seqNumAcked_world[a.seqnum() + 1] = false;
            thread t(handle_AUreqPickup, a, world_socket, ref(seqNumAcked_world), ref(truckList));
            t.detach();
        }
    }
    for (auto a : my_au_commands.AUreqDeliveries) {
        if (!seqNumAcked_world.count(a.seqnum() + 1)) {
            seqNumAcked_world[a.seqnum() + 1] = false;
            thread t(handle_AUreqDelivery, a, world_socket, ref(seqNumAcked_world));
            t.detach();
        }
    }
    for (auto a : my_au_commands.AUchangeDestns) {
        if (!seqNumAcked_amazon.count(a.seqnum() + 1)) {
            seqNumAcked_amazon[a.seqnum() + 1] = false;
            thread t(handle_AUchangeDestn, a, amazon_socket, ref(seqNumAcked_amazon));
            t.detach();
        }
    }
    for (auto a : my_au_commands.AUbindUPSs) {
        if (!seqNumAcked_amazon.count(a.seqnum() + 1)) {
            seqNumAcked_amazon[a.seqnum() + 1] = false;
            thread t(handle_AUbindUPS, a, amazon_socket, ref(seqNumAcked_amazon));
            t.detach();
        }
    }
    // for (auto a : my_au_commands.errs) {
    //     if (!seqNumAcked_world.count(a.seqnum())) {
    //         seqNumAcked_world[a.seqnum()] = false;  // todo: not sure
    //         thread t(handle_AUErr, ref(a), ref(world_socket));
    //         t.detach();
    //     }
    // }
    if (my_au_commands.disconnect) {
        // send disconnect to world
        handle_disconnect_world(world_socket);
    }
}

void handle_WUrequests(UResponses u_responses, int amazon_socket, int world_socket, map<long, bool>& seqNumAcked_amazon, map<long, bool>& seqNumAcked_world, mutex& mtx, vector<Truck>& truckList) {
    unique_ptr<Ostream> out_world(new Ostream(world_socket));
    myUResponses my_u_responses(u_responses, seqNumAcked_world, mtx);
    // Step 1: send an Acked Only UCommands to world // todo: not robust, what if this is lost?
    if (my_u_responses.seqNums.size() != 0) {
        UCommands u_commands;
        for (int i = 0; i < my_u_responses.seqNums.size(); i++) {
            u_commands.add_acks(i);
            u_commands.set_acks(i, my_u_responses.seqNums[i]);
        }
        if (!sendMesgTo<UCommands>(u_commands, out_world.get())) {
            throw runtime_error("handle_WUrequests: Error in sending UCommands");
        }
        cout << "Sent UCommands (acks only) to world" << endl;
    }
    // Step 2: check each of the seqnum from UResponses, see if they are in SeqNumAcked_amazon:
    lock_guard<mutex> lock(mtx);
    for (auto a : my_u_responses.UFinisheds) {
        if (!seqNumAcked_amazon.count(a.seqnum() + 1)) {
            seqNumAcked_amazon[a.seqnum() + 1] = false;
            thread t(handle_UFinished, a, amazon_socket, ref(seqNumAcked_amazon), ref(truckList));
            t.detach();
        }
    }
    for (auto a : my_u_responses.UDeliveryMades) {
        if (!seqNumAcked_amazon.count(a.seqnum() + 1)) {
            seqNumAcked_amazon[a.seqnum() + 1] = false;
            thread t(handle_UDeliveryMade, a, amazon_socket, ref(seqNumAcked_amazon));
            t.detach();
        }
    }
    for (auto a : my_u_responses.UTrucks) {
        // if (!seqNumAcked_amazon.count(a.seqnum() + 1)) {
        //     seqNumAcked_amazon[a.seqnum() + 1] = false;
        thread t(handle_UTruck, a, amazon_socket, ref(seqNumAcked_amazon), ref(mtx), ref(truckList));
        t.detach();
        // }
    }
    for (auto a : my_u_responses.UErrs) {
        if (!seqNumAcked_amazon.count(a.seqnum())) {
            seqNumAcked_amazon[a.seqnum()] = false;
            thread t(handle_UErr, a, ref(seqNumAcked_world), ref(mtx));
            t.detach();
        }
    }
    if (my_u_responses.finished) {
        // send disconnect to amazon
        handle_disconnect_amazon(amazon_socket);
        // sleep for 3s and shut down
        sleep(3);
        temp_IWANNARUN = false;
    }
}
/*
 *
 *
 *
 *
 *
 *
 *
 */

void handle_AUreqPickup(AUreqPickup au_req_pickup, int world_socket, map<long, bool>& seqNumAcked_world, vector<Truck>& truckList) {
    unique_ptr<Ostream> out_world(new Ostream(world_socket));
    // Step 1: parse the au_req_pickup and update the PKG table in database
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int whID = au_req_pickup.whid();
    int destX = au_req_pickup.destinationx();
    int destY = au_req_pickup.destinationy();
    long pkgID = au_req_pickup.shipid();
    int upsID = -1;
    if (au_req_pickup.has_upsid()) {
        upsID = au_req_pickup.upsid();
    }
    int truckID = assign_truck(whID, truckList);
    if (truckID == -1) {
        cout << "handle_AUreqPickup: Error in assigning truck" << endl;
    }
    cout << "handle_AUreqPickup: truckID: " << truckID << endl;
    string item = to_string(au_req_pickup.products_size()) + "--";
    for (int i = 0; i < au_req_pickup.products_size(); i++) {
        item += to_string(au_req_pickup.products(i).count());
        item += "_";
        item += au_req_pickup.products(i).description() + "__";
    }
    cout << "item: " << item << endl;
    // now update the database
    if (insert_PKG(C, pkgID, whID, destX, destY, item, truckID, upsID) != 0) {  // todo: more status code here
        cout << "handle_AUreqPickup: Error in inserting PKG: pkgID exists" << endl;
    }
    // Step 2: construct a UCommand with UGoPickup and the same seqnum
    UCommands u_commands;
    UGoPickup* u_go_pickup = u_commands.add_pickups();
    u_go_pickup->set_truckid(truckID);
    u_go_pickup->set_whid(whID);
    u_go_pickup->set_seqnum(au_req_pickup.seqnum() + 1);
    // Step 3: send the UCommand to world by a while loop until it is acked by seqNumAcked_world
    // use a for loop to print out all the seqNumAcked_world
    cout << "seqNumAcked_world: " << endl;
    for (auto a : seqNumAcked_world) {
        cout << a.first << " " << a.second;
    }
    cout << "---" << endl;
    while (!seqNumAcked_world[au_req_pickup.seqnum() + 1]) {
        if (!sendMesgTo<UCommands>(u_commands, out_world.get())) {
            throw runtime_error("handle_AUreqPickup: Error in sending UCommands");
        }
        cout << "---Sent UCommands to world---" << endl;
        cout << u_commands.DebugString() << endl;
        sleep(2);
    }
    delete C;
}

int assign_truck(int& whID, vector<Truck>& truckList) {
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
    int truckStatus = 1;
    int startID = whID * 5;
    // if (truckList[startID].status != 1) {
    //     truckList[startID].status = 1;
    //     truckList[startID].whid = whID;
    //     // update status in TRUCK table
    //     if (update_TRUCK_status(C, startID, truckStatus) != 0) {
    //         delete C;
    //         throw runtime_error("assign_truck: Error in updating TRUCK status");
    //     }
    //     return startID;
    // }
    for (int i = startID; i < truckList.size(); i++) {
        if (truckList[i].status != 1) {
            // if (truckList[i].status == 0) {
            truckList[i].status = 1;
            truckList[i].whid = whID;
            // update status in TRUCK table
            if (update_TRUCK_status(C, i, truckStatus) != 0) {
                delete C;
                throw runtime_error("assign_truck: Error in updating TRUCK status");
            }
            delete C;
            return i;
        }
    }
    delete C;
    return -1;
}

void handle_AUreqDelivery(AUreqDelivery au_req_delivery, int world_socket, map<long, bool>& seqNumAcked_world) {
    unique_ptr<Istream> in_world(new Istream(world_socket));
    unique_ptr<Ostream> out_world(new Ostream(world_socket));
    // Step 1: parse the au_req_delivery and update the PKG table in database
    long pkgID = au_req_delivery.shipid();
    // now update the database
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int curr_truckStatus = -3;
    int new_truckStatus = 3;
    int truckID = -1;
    int destX = -1;
    int destY = -1;
    if (get_PKG_dest(C, pkgID, destX, destY) != 0) {
        throw runtime_error("handle_AUreqDelivery: Error in getting PKG destX");
    }
    if (get_PKG_truckID(C, pkgID, truckID) != 0) {
        throw runtime_error("handle_AUreqDelivery: Error in getting PKG truckID");
    }
    if (get_TRUCK_status(C, truckID, curr_truckStatus) != 0) {
        throw runtime_error("handle_AUreqDelivery: Error in getting TRUCK status");
    } else {
        cout << "handle_AUreqDelivery: curr_truckStatus: " << curr_truckStatus << endl;
    }
    // if (curr_truckStatus == 2) {
    //     if (update_TRUCK_status(C, truckID, new_truckStatus) != 0) {
    //         throw runtime_error("handle_AUreqDelivery: Error in updating TRUCK status");
    //     }
    // }
    // Step 2: construct a UCommand with UGoDeliver and the same seqnum
    UCommands u_commands;
    UGoDeliver* u_go_deliver = u_commands.add_deliveries();
    u_go_deliver->set_truckid(truckID);
    UDeliveryLocation* u_delivery_location = u_go_deliver->add_packages();
    u_delivery_location->set_packageid(pkgID);
    u_delivery_location->set_x(destX);
    u_delivery_location->set_y(destY);
    u_go_deliver->set_seqnum(au_req_delivery.seqnum() + 1);
    // Step 3: send the UCommand to world by a while loop until it is acked by seqNumAcked_world
    cout << "--handle_AUreqDelivery--" << endl;
    while (!seqNumAcked_world[au_req_delivery.seqnum() + 1]) {
        if (!sendMesgTo<UCommands>(u_commands, out_world.get())) {
            throw runtime_error("handle_AUreqDelivery: Error in sending UCommands");
        }
        sleep(2);
    }
    cout << "---Sent UCommands to world---" << endl;
    cout << u_commands.DebugString() << endl;
    delete C;
}

// void handle_AUquery(AUquery au_query, int world_socket, map<long, bool>& seqNumAcked_world) {
//     unique_ptr<Istream> in_world(new Istream(world_socket));
//     unique_ptr<Ostream> out_world(new Ostream(world_socket));
//     // Step 1: parse the au_query
//     int shipID = au_query.shipid();
//     // get the truckID
//     connection* C;
//     try {
//         C = new connection("dbname=myups user=postgres password=passw0rd");
//         if (!(C->is_open())) {
//             throw std::runtime_error("Can't open database");
//         }
//     } catch (const std::exception& e) {
//         cerr << e.what() << std::endl;
//         return;
//     }
//     int truckID = -1;
//     if (get_PKG_truckID(C, shipID, truckID) != 0) {
//         throw runtime_error("handle_AUquery: Error in getting PKG truckID");
//     }
//     // Step 2: construct a UCommand with UQuery and the same seqnum
//     UCommands u_commands;
//     UQuery* u_query = u_commands.add_queries();
//     u_query->set_truckid(truckID);
//     u_query->set_seqnum(au_query.seqnum() + 1);
//     // Step 3: send the UCommand to world by a while loop until it is acked by seqNumAcked_world
//     while (!seqNumAcked_world[au_query.seqnum() + 1]) {
//         if (!sendMesgTo<UCommands>(u_commands, out_world.get())) {
//             throw runtime_error("handle_AUquery: Error in sending UCommands");
//         }
//         sleep(1);
//     }
//     cout << "---Sent UCommands to world---" << endl;
//     cout << u_commands.DebugString() << endl;
//     delete C;
// }

void handle_AUchangeDestn(AUchangeDestn au_change_destn, int amazon_socket, map<long, bool>& seqNumAcked_amazon) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // Step 1: parse the au_change_destn
    long shipID = au_change_destn.shipid();
    int newDestX = au_change_destn.destinationx();
    int newDestY = au_change_destn.destinationy();
    // Step 2: check if change is valid
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int truckID = -3;
    bool success = false;
    if (get_PKG_truckID(C, shipID, truckID) != 0) {
        throw runtime_error("handle_AUchangeDestn: Error in getting PKG truckID");
    }
    if (truckID < 0) {
        cout << "cannot change destination of a delivered or uncreated package" << endl;
    } else {
        // valid change:
        success = true;
        // update the PKG table in database
        if (update_PKG_dest(C, shipID, newDestX, newDestY) != 0) {
            throw runtime_error("handle_AUchangeDestn: Error in updating PKG dest");
        }
    }
    // Step 3: construct a UAcommands with UAchangeResp and the same seqnum
    UAcommands ua_commands;
    UAchangeResp* ua_change_resp = ua_commands.add_changeresp();
    ua_change_resp->set_success(success);
    ua_change_resp->set_acks(au_change_destn.seqnum() + 1);  // todo: really? acks???
    // Step 4: send the UAcommands to amazon by a while loop until it is acked by seqNumAcked_amazon
    while (!seqNumAcked_amazon[au_change_destn.seqnum() + 1]) {
        if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
            throw runtime_error("handle_AUchangeDestn: Error in sending UAcommands");
        }
        sleep(1);
    }
    cout << "---Sent UAcommands to amazon---" << endl;
    cout << ua_commands.DebugString() << endl;
    delete C;
}

void handle_AUbindUPS(AUbindUPS au_bind_ups, int amazon_socket, map<long, bool>& seqNumAcked_amazon) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // Step 1: parse the au_bind_ups
    int ownerID = au_bind_ups.ownerid();
    int upsID = au_bind_ups.upsid();
    // Step 2: check if the upsID is valid
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    bool valid = false;
    if ((userExists_auth_user(C, upsID) || (userExists_USERR(C, upsID)))) {
        valid = true;
    } else {
        valid = false;
    }
    // Step 3: construct a UAcommands with UAbindUPSResponse and the same seqnum
    UAcommands ua_commands;
    UAbindUPSResponse* ua_bind_resp = ua_commands.add_bindupsresponse();
    ua_bind_resp->set_status(valid);
    ua_bind_resp->set_ownerid(ownerID);
    ua_bind_resp->set_upsid(upsID);
    ua_bind_resp->set_seqnum(au_bind_ups.seqnum() + 1);
    // Step 4: send the UAcommands to amazon by a while loop until it is acked by seqNumAcked_amazon
    while (!seqNumAcked_amazon[au_bind_ups.seqnum() + 1]) {
        if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
            throw runtime_error("handle_AUbindUPS: Error in sending UAcommands");
        }
        sleep(1);
    }
    cout << "---Sent UAcommands to amazon---" << endl;
    cout << ua_commands.DebugString() << endl;
    delete C;
}

// void handle_AUErr(Err err, int world_socket, map<long, bool>& seqNumAcked_amazon) {
//     // Step 1: parse the err
//     string err_msg = err.err();
//     cout << "Error from amazon: " << err_msg << endl;
//     // Step 2: set the corrosponding seqNumAcked_amazon to false
//     try {
//         lock_guard<mutex> lock(mtx);
//         seqNumAcked_amazon[err.seqnum()] = false;  // todo: not useful
//     } catch (const std::out_of_range& oor) {
//         cout << "handle_AUErr: seqNumAcked_amazon out of range" << endl;
//     }
// }

void handle_disconnect_world(int world_socket) {
    // send a Ucommands with disconnect to world
    UCommands u_commands;
    u_commands.set_disconnect(true);
    unique_ptr<Istream> in_world(new Istream(world_socket));
    unique_ptr<Ostream> out_world(new Ostream(world_socket));
    if (!sendMesgTo<UCommands>(u_commands, out_world.get())) {
        throw runtime_error("handle_disconnect_world: Error in sending UCommands");
    }
}

/*
 *
 *
 *
 *
 *
 *
 *
 */

void handle_UFinished(UFinished u_finished, int amazon_socket, map<long, bool>& seqNumAcked_amazon, vector<Truck>& truckList) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // Step 1: parse the u_finished
    int truckID = u_finished.truckid();
    int x = u_finished.x();
    int y = u_finished.y();
    string status = u_finished.status();
    if (status == "ARRIVE WAREHOUSE") {
        // reach the warehouse
        cout << "handle_UFinished: reach the warehouse" << endl;
        long int seqNum = u_finished.seqnum();
        int whID = assign_wh(truckID, truckList);
        if (whID < 0) {
            cout << "truckID: " << truckID << endl;
            cout << "status" << truckList[truckID].status << endl;
            throw runtime_error("handle_UFinished: Error in assigning warehouse");
        }
        connection* C;
        try {
            C = new connection("dbname=myups user=postgres password=passw0rd");
            if (!(C->is_open())) {
                throw std::runtime_error("Can't open database");
            }
        } catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            return;
        }
        vector<long> shipIDs = get_shipIDs(C, whID, truckID);  // todo: not good
        // Step 3: construct a UACommand with a bunch of UAtruckArrived
        UAcommands ua_commands;
        for (int i = 0; i < shipIDs.size(); i++) {
            UAtruckArrived* ua_truck_arrived = ua_commands.add_truckarr();
            ua_truck_arrived->set_whid(whID);
            ua_truck_arrived->set_shipid(shipIDs[i]);
            ua_truck_arrived->set_truckid(truckID);
            ua_truck_arrived->set_seqnum(seqNum + 1);
        }
        // Step 4: send the UACommand to amazon by a while loop until it is acked by seqNumAcked_amazon
        while (!seqNumAcked_amazon[seqNum + 1]) {
            // use a for loop to print out the seqNumAcked_amazon
            cout << "seqNumAcked_amazon: ";
            for (auto it = seqNumAcked_amazon.begin(); it != seqNumAcked_amazon.end(); it++) {
                cout << it->first << "-" << it->second << "__";
            }
            cout << endl;
            cout << "---Sent UAcommands to amazon---" << endl;
            cout << ua_commands.DebugString() << endl;
            if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
                throw runtime_error("handle_UFinished: Error in sending UAcommands");
            }
            sleep(2);
        }
        delete C;
    } else {
        // finished all of its deliveries
        cout << "handle_UFinished: finished all of its deliveries" << endl;
        int truckStatus = 0;
        truckList[truckID].status = truckStatus;
        // update status in TRUCK table
        connection* C;
        try {
            C = new connection("dbname=myups user=postgres password=passw0rd");
            if (!(C->is_open())) {
                throw std::runtime_error("Can't open database");
            }
        } catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            return;
        }
        if (update_TRUCK_status(C, truckID, truckStatus) != 0) {
            throw runtime_error("handle_UFinished: Error in updating TRUCK status");
        }
        delete C;
    }
}

int assign_wh(int& truckID, vector<Truck>& truckList) {
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return -1;
    }
    int status = 2;  // waiting for pickup
    int whID = truckList[truckID].whid;
    if (update_TRUCK_status(C, truckID, status) != 0) {
        delete C;
        throw runtime_error("assign_wh: Error in updating TRUCK status");
    }
    truckList[truckID].status = status;
    delete C;
    return whID;
    // if ((truckList[truckID].status == 1) || (truckList[truckID].status == 2)) {
    //     truckList[truckID].status = status;
    //     // update status in TRUCK table
    //     if (update_TRUCK_status(C, truckID, status) != 0) {
    //         delete C;
    //         throw runtime_error("assign_wh: Error in updating TRUCK status");
    //     }
    //     delete C;
    //     return truckID;
    // }
    // for (int i = truckID; i < truckList.size(); i++) {
    //     if ((truckList[i].status == 1) || (truckList[i].status == 2)) {
    //         truckList[i].status = status;
    //         // update status in TRUCK table
    //         if (update_TRUCK_status(C, i, status) != 0) {
    //             delete C;
    //             throw runtime_error("assign_wh: Error in updating TRUCK status");
    //         }
    //         delete C;
    //         return i;
    //     }
    // }
    // delete C;
    // return -1;
}

void handle_UDeliveryMade(UDeliveryMade u_delivery_made, int amazon_socket, map<long, bool>& seqNumAcked_amazon) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // Step 1: parse the u_delivery_made
    int truckID = u_delivery_made.truckid();
    long shipID = u_delivery_made.packageid();
    long int seqNum = u_delivery_made.seqnum();
    // Step 2: update the database
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int status = -2;  // delivered
    if (update_PKG_truckID(C, shipID, status) != 0) {
        delete C;
        throw runtime_error("handle_UDeliveryMade: Error in updating PKG status");
    }
    // Step 3: construct a UACommand with UAdelivered
    UAcommands ua_commands;
    UAdelivered* ua_delivered = ua_commands.add_delivered();
    ua_delivered->set_seqnum(seqNum + 1);
    ua_delivered->set_shipid(shipID);
    // Step 4: send the UACommand to amazon by a while loop until it is acked by seqNumAcked_amazon
    while (!seqNumAcked_amazon[seqNum + 1]) {
        if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
            throw runtime_error("handle_UDeliveryMade: Error in sending UAcommands");
        }
        sleep(1);
    }
    cout << "---Sent UAcommands to amazon---" << endl;
    cout << ua_commands.DebugString() << endl;
    delete C;
}

void handle_UTruck(UTruck u_truck, int amazon_socket, map<long, bool>& seqNumAcked_amazon, mutex& mtx, vector<Truck>& truckList) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // Step 1: parse the u_truck
    int truckID = u_truck.truckid();
    string status = u_truck.status();
    int x = u_truck.x();
    int y = u_truck.y();
    long int seqNum = u_truck.seqnum();
    // Step 2: update the database
    connection* C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
    } catch (const std::exception& e) {
        cerr << e.what() << std::endl;
        return;
    }
    int truckStatus = assign_status(status);
    if (truckStatus == -3) {
        delete C;
        cout << "the status is not valid--" << status << endl;
        throw runtime_error("handle_UTruck: Error in assigning truck status");
    }
    if (update_TRUCK_status(C, truckID, truckStatus) != 0) {
        delete C;
        throw runtime_error("handle_UTruck: Error in updating TRUCK status");
    }
    if (update_TRUCK_location(C, truckID, x, y) != 0) {
        delete C;
        throw runtime_error("handle_UTruck: Error in updating TRUCK location");
    }
    // update TruckList
    lock_guard<mutex> lock(mtx);
    truckList[truckID].status = truckStatus;
    // // Step 3: construct a UACommand with UAstatus
    // UAcommands ua_commands;
    // UAstatus* ua_status = ua_commands.add_status();
    // ua_status->set_status(status);
    // ua_status->set_seqnum(seqNum + 1);
    // // Step 4: send the UACommand to amazon by a while loop until it is acked by seqNumAcked_amazon
    // while (!seqNumAcked_amazon[seqNum + 1]) {
    //     if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
    //         delete C;
    //         throw runtime_error("handle_UTruck: Error in sending UAcommands");
    //     }
    //     sleep(1);
    // }
    // cout << "---Sent UAcommands to amazon---" << endl;
    // cout << ua_commands.DebugString() << endl;
    delete C;
}

int assign_status(string& status) {
    // IDLE, TRAV, ARRIVE, LOADING, DELIVERING
    if (status.find("IDLE") != string::npos) {
        return 0;
    }
    if (status.find("TRAV") != string::npos) {
        return 1;
    }
    if ((status.find("ARRIV") != string::npos) || (status.find("LOAD") != string::npos)) {
        return 2;
    }
    if (status.find("DELIVERING") != string::npos) {
        return 3;
    } else {
        return -3;
    }
}

void handle_UErr(UErr err, map<long, bool>& seqNumAcked_world, mutex& mtx) {
    // Step 1: parse the err
    string err_msg = err.err();
    cout << "Error from world: " << err_msg << endl;
    // Step 2: set the corrosponding seqNumAcked_world to false
    try {
        lock_guard<mutex> lock(mtx);
        seqNumAcked_world[err.seqnum()] = false;  // todo: not useful
    } catch (const std::out_of_range& oor) {
        cout << "handle_UErr: seqNumAcked_world out of range" << endl;
    }
}

void handle_disconnect_amazon(int amazon_socket) {
    unique_ptr<Istream> in_amazon(new Istream(amazon_socket));
    unique_ptr<Ostream> out_amazon(new Ostream(amazon_socket));
    // send a UAcommands with disconnect to amazon
    UAcommands ua_commands;
    ua_commands.set_disconnect(true);
    if (!sendMesgTo<UAcommands>(ua_commands, out_amazon.get())) {
        throw runtime_error("handle_disconnect_amazon: Error in sending UAcommands");
    }
}
