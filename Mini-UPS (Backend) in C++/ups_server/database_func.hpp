#ifndef __DATABASE_FUNC_HPP__
#define __DATABASE_FUNC_HPP__

#include <iomanip>
#include <iostream>
#include <pqxx/pqxx>
#include <stdexcept>

#include "pkg.hpp"

using namespace std;
using namespace pqxx;

void sql_exec(connection *C, string sql);

int create_database();
void drop_tables(connection *C);
void truncate_tables(connection *C);
void create_table_TRUCK(connection *C);
void create_table_PKG(connection *C);
void create_table_USERR(connection *C);

// func for TRUCK // 0: idle, 1: enrouteWH, 2: waitingpickup, 3: delivering
bool truckExists_TRUCK(connection *C, int &truckID);
bool truckExists_TRUCK_W(work *W, int &truckID);
int insert_TRUCK(connection *C, int &truckID, int &status);
int update_TRUCK_status(connection *C, int &truckID, int &status);
int update_TRUCK_location(connection *C, int &truckID, int &truckX, int &truckY);
int get_TRUCK_status(connection *C, int &truckID, int &status);

// func for PKG
bool pkgExists_PKG(connection *C, long &pkgID);
bool pkgExists_PKG_W(work *W, long &pkgID);
bool userExists_PKG(connection *C, int &userID);
bool userExists_PKG_W(work *W, int &userID);
int insert_PKG(connection *C, long &pkgID, int &whID, int &destX, int &destY, string &item, int &truckID, int &userID);
int update_PKG_truckID(connection *C, long &pkgID, int &truckID);
int update_PKG_dest(connection *C, long &pkgID, int &destX, int &destY);  // does not include pre-check of truck status
int get_PKG_truckID(connection *C, long &pkgID, int &truckID);
int get_PKG_dest(connection *C, long &pkgID, int &destX, int &destY);
int get_PKG_Pkg(connection *C, long &pkgID, Pkg &pkg);
int get_PKG_Pkgs(connection *C, int &userID, vector<Pkg> &pkgs);
vector<long> get_shipIDs(connection *C, int &whID, int &TruckID);  // todo: not good

// func for USERR
bool userExists_USERR(connection *C, int &userID);
bool userExists_USERR_W(work *W, int &userID);
int insert_USERR(connection *C, int &userID);

// func for auth_user
bool userExists_auth_user(connection *C, int &userID);

#endif