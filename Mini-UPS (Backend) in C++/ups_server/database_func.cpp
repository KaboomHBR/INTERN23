#include "database_func.hpp"

#define CREATED -1    // used for truckID at PKG when truck is not assigned, also for userID when user is not registered
#define DELIVERED -2  // used for status at PKG when pkg is delivered

void sql_exec(connection *C, string sql) {
    // execute a sql statement
    work W(*C);
    W.exec(sql);
    W.commit();
}

int create_database() {
    connection *C;
    try {
        C = new connection("dbname=myups user=postgres password=passw0rd");
        if (!(C->is_open())) {
            throw std::runtime_error("Can't open database");
        }
        // drop_tables(C);
        truncate_tables(C);
        // create_table_TRUCK(C);
        // create_table_PKG(C);
        // create_table_USERR(C);
        // Act as an UPS server:
        // do stuff, implement with threads in main.cpp
        // but here let's insert a userr with userID = 568, just for demo
        int userID = 568;
        insert_USERR(C, userID);
        userID = 569;
        insert_USERR(C, userID);
        C->disconnect();
        delete C;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

void drop_tables(connection *C) {
    sql_exec(C, "DROP TABLE IF EXISTS PKG CASCADE;");
    sql_exec(C, "DROP TABLE IF EXISTS TRUCK CASCADE;");
    sql_exec(C, "DROP TABLE IF EXISTS USERR CASCADE;");
}

void truncate_tables(connection *C) {
    sql_exec(C, "TRUNCATE TABLE PKG CASCADE;");
    sql_exec(C, "TRUNCATE TABLE TRUCK CASCADE;");
    sql_exec(C, "TRUNCATE TABLE USERR CASCADE;");
}

void create_table_TRUCK(connection *C) {
    // attributes: truckID status truckX truckY
    sql_exec(C,
             "CREATE TABLE TRUCK("
             "truckID INT PRIMARY KEY,"
             "status INT NOT NULL,"
             "truckX INT,"
             "truckY INT);");
}

// void create_table_TRUCK(connection *C) {
//     // attributes: truckID status
//     sql_exec(C,
//              "CREATE TABLE TRUCK("
//              "truckID INT PRIMARY KEY,"
//              "status INT NOT NULL);");
// }

void create_table_PKG(connection *C) {
    // attributes: pkgID whID destX destY item truckID userID
    sql_exec(C,
             "CREATE TABLE PKG("
             "pkgID BIGINT PRIMARY KEY,"
             "whID INT NOT NULL,"
             "destX INT NOT NULL,"
             "destY INT NOT NULL,"
             "item VARCHAR(256) NOT NULL,"
             "truckID INT NOT NULL,"
             "userID INT NOT NULL,"
             "evaluation VARCHAR(256));");
    //  "FOREIGN KEY (truckID) REFERENCES TRUCK(truckID));");
}

void create_table_USERR(connection *C) {
    // attributes: userID
    sql_exec(C,
             "CREATE TABLE USERR("
             "userID INT PRIMARY KEY);");
}

/*
 *func for TRUCK
 */

bool truckExists_TRUCK(connection *C, int &truckID) {
    // check if truckID exists in TRUCK
    nontransaction N(*C);
    result R(N.exec("SELECT truckID FROM TRUCK WHERE truckID = " + to_string(truckID) + ";"));
    return R.size() == 1;
}

bool truckExists_TRUCK_W(work *W, int &truckID) {
    // check if truckID exists in TRUCK
    result R(W->exec("SELECT truckID FROM TRUCK WHERE truckID = " + to_string(truckID) + ";"));
    return R.size() == 1;
}

int insert_TRUCK(connection *C, int &truckID, int &status) {
    if (truckExists_TRUCK(C, truckID)) {
        return 1;
    } else if ((status < 0) || (status > 3)) {
        return 2;  // invalid status
    }
    work W(*C);
    try {
        W.exec("INSERT INTO TRUCK (truckID, status) VALUES (" + to_string(truckID) + ", " + to_string(status) + ");");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int update_TRUCK_status(connection *C, int &truckID, int &status) {
    if (!truckExists_TRUCK(C, truckID)) {
        return 1;
    } else if ((status < 0) || (status > 3)) {
        return 2;  // invalid status
    }
    work W(*C);
    try {
        W.exec("UPDATE TRUCK SET status = " + to_string(status) + " WHERE truckID = " + to_string(truckID) + ";");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int update_TRUCK_location(connection *C, int &truckID, int &truckX, int &truckY) {
    if (!truckExists_TRUCK(C, truckID)) {
        return 1;
    }
    work W(*C);
    try {
        W.exec("UPDATE TRUCK SET truckX = " + to_string(truckX) + ", truckY = " + to_string(truckY) + " WHERE truckID = " + to_string(truckID) + ";");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int get_TRUCK_status(connection *C, int &truckID, int &status) {
    if (!truckExists_TRUCK(C, truckID)) {
        return 1;
    }
    nontransaction N(*C);
    result R(N.exec("SELECT status FROM TRUCK WHERE truckID = " + to_string(truckID) + ";"));
    status = R[0][0].as<int>();
    return 0;
}

/*
 *func for PKG
 */

bool pkgExists_PKG(connection *C, long &pkgID) {
    // check if pkgID exists in PKG
    nontransaction N(*C);
    result R(N.exec("SELECT pkgID FROM PKG WHERE pkgID = " + to_string(pkgID) + ";"));
    return R.size() == 1;
}

bool pkgExists_PKG_W(work *W, long &pkgID) {
    // check if pkgID exists in PKG
    result R(W->exec("SELECT pkgID FROM PKG WHERE pkgID = " + to_string(pkgID) + ";"));
    return R.size() == 1;
}

bool userExists_PKG(connection *C, int &userID) {
    // check if userID exists in PKG
    nontransaction N(*C);
    result R(N.exec("SELECT userID FROM PKG WHERE userID = " + to_string(userID) + ";"));
    return R.size() == 1;
}

bool userExists_PKG_W(work *W, int &userID) {
    // check if userID exists in PKG
    result R(W->exec("SELECT userID FROM PKG WHERE userID = " + to_string(userID) + ";"));
    return R.size() == 1;
}

int insert_PKG(connection *C, long &pkgID, int &whID, int &destX, int &destY, string &item, int &truckID, int &userID) {
    if (pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    work W(*C);
    try {
        W.exec("INSERT INTO PKG (pkgID, whID, destX, destY, item, truckID, userID) VALUES (" + to_string(pkgID) + ", " + to_string(whID) + ", " + to_string(destX) + ", " + to_string(destY) + ", " + W.quote(item) + ", " + to_string(truckID) + ", " + to_string(userID) + ");");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int update_PKG_truckID(connection *C, long &pkgID, int &truckID) {
    if (!pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    work W(*C);
    try {
        W.exec("UPDATE PKG SET truckID = " + to_string(truckID) + " WHERE pkgID = " + to_string(pkgID) + ";");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int update_PKG_dest(connection *C, long &pkgID, int &destX, int &destY) {
    if (!pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    work W(*C);
    try {
        W.exec("UPDATE PKG SET destX = " + to_string(destX) + ", destY = " + to_string(destY) + " WHERE pkgID = " + to_string(pkgID) + ";");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

int get_PKG_truckID(connection *C, long &pkgID, int &truckID) {
    if (!pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    nontransaction N(*C);
    result R(N.exec("SELECT truckID FROM PKG WHERE pkgID = " + to_string(pkgID) + ";"));
    truckID = R[0][0].as<int>();
    return 0;
}
int get_PKG_dest(connection *C, long &pkgID, int &destX, int &destY) {
    if (!pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    nontransaction N(*C);
    result R(N.exec("SELECT destX, destY FROM PKG WHERE pkgID = " + to_string(pkgID) + ";"));
    destX = R[0][0].as<int>();
    destY = R[0][1].as<int>();
    return 0;
}

int get_PKG_Pkg(connection *C, long &pkgID, Pkg &pkg) {
    if (!pkgExists_PKG(C, pkgID)) {
        return 1;
    }
    nontransaction N(*C);
    result R(N.exec("SELECT * FROM PKG WHERE pkgID = " + to_string(pkgID) + ";"));
    pkg.pkgID = pkgID;
    pkg.whID = R[0][1].as<int>();
    pkg.destX = R[0][2].as<int>();
    pkg.destY = R[0][3].as<int>();
    pkg.item = R[0][4].as<string>();
    pkg.truckID = R[0][5].as<int>();
    pkg.userID = R[0][6].as<int>();
    return 0;
}

int get_PKG_Pkgs(connection *C, int &userID, vector<Pkg> &pkgs) {
    if (!userExists_PKG(C, userID)) {
        return 1;
    }
    Pkg pkg;
    nontransaction N(*C);
    result R(N.exec("SELECT * FROM PKG WHERE userID = " + to_string(userID) + ";"));
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        pkg.pkgID = c[0].as<int>();
        pkg.whID = c[1].as<int>();
        pkg.destX = c[2].as<int>();
        pkg.destY = c[3].as<int>();
        pkg.item = c[4].as<string>();
        pkg.truckID = c[5].as<int>();
        pkg.userID = c[6].as<int>();
        pkgs.push_back(pkg);
    }
    return 0;
}

vector<long> get_shipIDs(connection *C, int &whID, int &TruckID) {
    vector<long> shipIDs;
    nontransaction N(*C);
    result R(N.exec("SELECT pkgID FROM PKG WHERE whID = " + to_string(whID) + " AND truckID = " + to_string(TruckID) + ";"));
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        shipIDs.push_back(c[0].as<long>());
    }
    return shipIDs;
}

/*
 *func for USERR
 */

bool userExists_USERR(connection *C, int &userID) {
    // check if userID exists in USER
    nontransaction N(*C);
    result R(N.exec("SELECT userID FROM USERR WHERE userID = " + to_string(userID) + ";"));
    return R.size() == 1;
}

bool userExists_USERR_W(work *W, int &userID) {
    // check if userID exists in USER
    result R(W->exec("SELECT userID FROM USERR WHERE userID = " + to_string(userID) + ";"));
    return R.size() == 1;
}

int insert_USERR(connection *C, int &userID) {
    if (userExists_USERR(C, userID)) {
        return 1;
    }
    work W(*C);
    try {
        W.exec("INSERT INTO USERR (userID) VALUES (" + to_string(userID) + ");");
        W.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        W.abort();
        return 3;
    }
    return 0;
}

bool userExists_auth_user(connection *C, int &userID) {
    // read from the django genereated auth_user table
    // check if userID exists in auth_user as username
    nontransaction N(*C);
    result R(N.exec("SELECT username FROM auth_user WHERE username = \'" + to_string(userID) + "\';"));
    return R.size() == 1;
}