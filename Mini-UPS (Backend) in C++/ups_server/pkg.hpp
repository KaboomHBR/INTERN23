#ifndef __PKG_HPP__
#define __PKG_HPP__

#include <string>

// class Pkg includes all the attributes of a package, which is stored in PKG table
class Pkg {
   public:
    int pkgID;
    int whID;
    int destX;
    int destY;
    std::string item;
    int truckID;
    int userID;
    Pkg() {
        pkgID = -1;
        whID = -1;
        destX = -1;
        destY = -1;
        item = "";
        truckID = -1;
        userID = -1;
    }
    Pkg(int _pkgID, int _whID, int _destX, int _destY, std::string _item, int _truckID, int _userID) {
        pkgID = _pkgID;
        whID = _whID;
        destX = _destX;
        destY = _destY;
        item = _item;
        truckID = _truckID;
        userID = _userID;
    }
    Pkg(const Pkg &p) {
        pkgID = p.pkgID;
        whID = p.whID;
        destX = p.destX;
        destY = p.destY;
        item = p.item;
        truckID = p.truckID;
        userID = p.userID;
    }
    Pkg &operator=(const Pkg &p) {
        pkgID = p.pkgID;
        whID = p.whID;
        destX = p.destX;
        destY = p.destY;
        item = p.item;
        truckID = p.truckID;
        userID = p.userID;
        return *this;
    }
};

#endif