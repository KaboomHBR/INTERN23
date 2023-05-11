#ifndef __TRUCK_HPP__
#define __TRUCK_HPP__

class Truck {
   public:
    int id;
    int x;
    int y;
    int status;  // 0: idle, 1: enrouteWH, 2: waitingpickup, 3: delivering
    int whid = -1;
    Truck() {
        id = -1;
        x = -1;
        y = -1;
        status = 0;
    }
    Truck(int _id, int _x, int _y, int _status) {
        id = _id;
        x = _x;
        y = _y;
        status = _status;
    }
    Truck(const Truck &t) {
        id = t.id;
        x = t.x;
        y = t.y;
        status = t.status;
    }
    Truck &operator=(const Truck &t) {
        id = t.id;
        x = t.x;
        y = t.y;
        status = t.status;
        return *this;
    }
};

#endif