#include "database_func.hpp"

#define NOSYMBOL "NOSYMBOL10672"
#define NOTCANCELLED -1

void sql_exec(connection *C, string sql) {
    // execute a sql statement
    work W(*C);
    W.exec(sql);
    W.commit();
}

void drop_tables(connection *C) {
    // Drop table SYMBOL
    sql_exec(C, "DROP TABLE IF EXISTS SYMBOL CASCADE;");
    // Drop table ACCOUNT
    sql_exec(C, "DROP TABLE IF EXISTS ACCOUNT CASCADE;");
    // Drop table TRANSACTION
    sql_exec(C, "DROP TABLE IF EXISTS TRANSACTION CASCADE;");
    // Drop table EXECUTED
    sql_exec(C, "DROP TABLE IF EXISTS EXECUTED CASCADE;");
}

void create_table_SYMBOL(connection *C) {
    // Create table SYMBOL
    sql_exec(C,
             "CREATE TABLE SYMBOL("
             "SymbolName VARCHAR(256) PRIMARY KEY);");
    // insert NOSYMBOL into SYMBOL
    sql_exec(C, "INSERT INTO SYMBOL VALUES ('" + string(NOSYMBOL) + "');");
}

void create_table_ACCOUNT(connection *C) {
    // Create table ACCOUNT
    sql_exec(C,
             "CREATE TABLE ACCOUNT("
             "AccountID BIGINT NOT NULL,"
             "SymbolName VARCHAR(256) NOT NULL,"
             "Amount DOUBLE PRECISION,"
             "Balance DOUBLE PRECISION,"
             "PRIMARY KEY (AccountID, SymbolName),"
             "FOREIGN KEY (SymbolName) REFERENCES SYMBOL(SymbolName) ON DELETE CASCADE ON UPDATE CASCADE);");
}

void create_table_TRANSACTION(connection *C) {
    // Create table TRANSACTION
    sql_exec(C,
             "CREATE TABLE TRANSACTION("
             "TransID BIGSERIAL PRIMARY KEY,"
             "OrderTime BIGINT NOT NULL,"  // added 04/06
             "AccountID BIGINT NOT NULL,"
             "SymbolName VARCHAR(256) NOT NULL,"
             "Amount DOUBLE PRECISION NOT NULL,"
             "PriceLimit DOUBLE PRECISION NOT NULL,"
             "CancelTime BIGINT NOT NULL,"
             //  "FOREIGN KEY (AccountID) REFERENCES ACCOUNT(AccountID) ON DELETE CASCADE ON UPDATE CASCADE,"
             "FOREIGN KEY (SymbolName) REFERENCES SYMBOL(SymbolName) ON DELETE CASCADE ON UPDATE CASCADE);");
}

void create_table_EXECUTED(connection *C) {
    // Create table EXECUTED
    sql_exec(C,
             "CREATE TABLE EXECUTED("
             "TransID BIGINT,"
             "ExecTime BIGINT,"
             "Amount DOUBLE PRECISION NOT NULL,"
             "Price DOUBLE PRECISION NOT NULL,"
             // add one more attribute: ExecID
             "ExecID BIGSERIAL PRIMARY KEY,"
             //  "PRIMARY KEY (TransID, ExecTime),"
             "FOREIGN KEY (TransID) REFERENCES TRANSACTION(TransID) ON DELETE CASCADE ON UPDATE CASCADE);");
}

int create_database() {
    // Allocate & initialize a Postgres connection object
    connection *C;

    try {
        // Establish a connection to the database
        // Parameters: database name, user name, user password
        C = new connection("dbname=STOCK user=postgres password=passw0rd");
        if (C->is_open()) {
            // cout << "Opened database successfully: " << C->dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }

        // create SYMBOL, ACCOUNT, TRANSACTION, and EXECUTED tables in the STOCK database
        // Step 1: you should drop table that already exists
        drop_tables(C);
        // Step 2: you should create table
        // create_table_SYMBOL(C);
        create_table_ACCOUNT(C);
        create_table_TRANSACTION(C);
        create_table_EXECUTED(C);
        // Step 3: act as a exchange matching server
        // do stuff

        // Close database connection
        C->disconnect();

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

bool SymbolExists_SYMBOL(connection *C, string SymbolName) {
    // Check if the symbol exists in table SYMBOL
    work W(*C);
    string sql = "SELECT * FROM SYMBOL WHERE SymbolName = " + W.quote(SymbolName) + ";";
    result R(W.exec(sql));
    W.commit();
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

bool SymbolExists_SYMBOL_W(work &W, string SymbolName) {
    string sql = "SELECT * FROM SYMBOL WHERE SymbolName = " + W.quote(SymbolName) + ";";
    result R(W.exec(sql));
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

int update_SYMBOL(connection *C, string SymbolName) {
    if (!SymbolExists_SYMBOL(C, SymbolName)) {
        work W(*C);
        string sql = "INSERT INTO SYMBOL (SymbolName) VALUES (" + W.quote(SymbolName) + ");";
        W.exec(sql);
        W.commit();
    }
    return 0;
}

int update_SYMBOL_W(work &W, string SymbolName) {
    if (!SymbolExists_SYMBOL_W(W, SymbolName)) {
        string sql = "INSERT INTO SYMBOL (SymbolName) VALUES (" + W.quote(SymbolName) + ");";
        W.exec(sql);
    }
    return 0;
}

bool AccountExists_ACCOUNT(connection *C, long int AccountID) {
    // Check if the account exists in table ACCOUNT
    // work W(*C);
    nontransaction N(*C);
    string sql = "SELECT * FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + ";";
    // result R(W.exec(sql));
    // W.commit();
    result R(N.exec(sql));
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

bool AccountExists_ACCOUNT_W(work &W, long int AccountID) {
    string sql = "SELECT * FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + ";";
    result R(W.exec(sql));
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

bool SymbolExists_ACCOUNT(connection *C, long int AccountID, string SymbolName) {
    // Check if the symbol exists in table ACCOUNT
    work W(*C);
    string sql = "SELECT * FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
    result R(W.exec(sql));
    W.commit();
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

bool SymbolExists_ACCOUNT_W(work &W, long int AccountID, string SymbolName) {
    string sql = "SELECT * FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
    result R(W.exec(sql));
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

int create_ACCOUNT(connection *C, long int AccountID, double Balance) {
    work W(*C);
    // if account does not exist, create a new account
    if (AccountExists_ACCOUNT_W(W, AccountID)) {
        return 1;
    }
    string sql = "INSERT INTO ACCOUNT (AccountID, SymbolName, Balance) VALUES (" + to_string(AccountID) + ", " + W.quote(NOSYMBOL) + ", " + to_string(Balance) + ");";
    W.exec(sql);
    W.commit();
    return 0;
}

int update_ACCOUNT_Position(connection *C, long int AccountID, string SymbolName, double Amount) {
    // first check AccountID exists
    //   if not, return 1
    // then check if SymbolName exists
    //   if yes, new amount = old amount + Amount
    //   if no, new amount = Amount -- it actually "create_"
    work W(*C);
    string sql;
    if (!AccountExists_ACCOUNT_W(W, AccountID)) {
        return 1;
    } else if (SymbolExists_ACCOUNT_W(W, AccountID, SymbolName)) {
        sql = "UPDATE ACCOUNT SET Amount = Amount + " + to_string(Amount) + " WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
    } else {
        int nothing = update_SYMBOL_W(W, SymbolName);
        sql = "INSERT INTO ACCOUNT (AccountID, SymbolName, Amount) VALUES (" + to_string(AccountID) + ", " + W.quote(SymbolName) + ", " + to_string(Amount) + ");";
    }
    W.exec(sql);
    W.commit();
    return 0;
}

int update_ACCOUNT_Position_W(work &W, long int AccountID, string SymbolName, double Amount) {
    string sql;
    if (!AccountExists_ACCOUNT_W(W, AccountID)) {
        return 1;
    } else if (SymbolExists_ACCOUNT_W(W, AccountID, SymbolName)) {
        sql = "UPDATE ACCOUNT SET Amount = Amount + " + to_string(Amount) + " WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
    } else {
        int nothing = update_SYMBOL_W(W, SymbolName);
        sql = "INSERT INTO ACCOUNT (AccountID, SymbolName, Amount) VALUES (" + to_string(AccountID) + ", " + W.quote(SymbolName) + ", " + to_string(Amount) + ");";
    }
    W.exec(sql);
    return 0;
}

int update_ACCOUNT_Amount(work &W, long int AccountID, string SymbolName, double Amount) {
    if (!AccountExists_ACCOUNT_W(W, AccountID) || !SymbolExists_ACCOUNT_W(W, AccountID, SymbolName)) {
        return 1;
    } else {
        // work W(*C);
        string sql;
        sql = "UPDATE ACCOUNT SET Amount = " + to_string(Amount) + " WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
        W.exec(sql);
        return 0;
    }
}

int update_ACCOUNT_Balance(work &W, long int AccountID, double Balance) {
    if (!AccountExists_ACCOUNT_W(W, AccountID)) {
        return 1;
    } else {
        // work W(*C);
        string sql;
        sql = "UPDATE ACCOUNT SET Balance = " + to_string(Balance) + " WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(NOSYMBOL) + ";";
        W.exec(sql);
        // W.commit();
        return 0;
    }
}

double get_ACCOUNT_Amount(work &W, long int AccountID, string SymbolName) {
    if (!AccountExists_ACCOUNT_W(W, AccountID) || !SymbolExists_ACCOUNT_W(W, AccountID, SymbolName)) {
        return -1;
    } else {
        // work W(*C);
        string sql;
        sql = "SELECT Amount FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(SymbolName) + ";";
        result R(W.exec(sql));
        // W.commit();
        return R[0][0].as<double>();
    }
}

double get_ACCOUNT_Balance(work &W, long int AccountID) {
    if (!AccountExists_ACCOUNT_W(W, AccountID)) {
        return -1;
    } else {
        // work W(*C);
        string sql;
        sql = "SELECT Balance FROM ACCOUNT WHERE AccountID = " + to_string(AccountID) + " AND SymbolName = " + W.quote(NOSYMBOL) + ";";
        result R(W.exec(sql));
        // W.commit();
        return R[0][0].as<double>();
    }
}

bool TransIDExists_TRANSACTION(connection *C, long int TransID) {
    // Check if the transID exists in table TRANSACTION
    // work W(*C);
    nontransaction N(*C);
    string sql = "SELECT * FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    // result R(W.exec(sql));
    result R(N.exec(sql));
    // W.commit();
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

bool TransIDExists_TRANSACTION_W(work &W, long int TransID) {
    // Check if the transID exists in table TRANSACTION
    string sql = "SELECT * FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(W.exec(sql));
    if (R.size() == 0) {
        return false;
    } else {
        return true;
    }
}

long int create_TRANSACTION(work &W, long int AccountID, string SymbolName, double Amount, double PriceLimit) {
    // first update SYMBOL table
    int nothing = update_SYMBOL_W(W, SymbolName);
    // create a new transaction
    // and return the transID
    long currentTime = current_time();
    string sql = "INSERT INTO TRANSACTION (OrderTime, AccountID, SymbolName, Amount, PriceLimit, CancelTime) VALUES (" + to_string(currentTime) + ", " + to_string(AccountID) + ", " + W.quote(SymbolName) + ", " + to_string(Amount) + ", " + to_string(PriceLimit) + ", " + to_string(NOTCANCELLED) + ");";
    W.exec(sql);
    sql = "SELECT max(TransID) FROM TRANSACTION;";  // todo: they said max(TransID) is the most reliable way???
    result R(W.exec(sql));
    long int TransID = R[0][0].as<long int>();
    return TransID;
}

int update_TRANSACTION_Amount(work &W, long int TransID, double Amount) {
    // if transID exists, update Amount
    if (!TransIDExists_TRANSACTION_W(W, TransID)) {
        return 1;
    }
    // work W(*C);
    string sql = "UPDATE TRANSACTION SET Amount = " + to_string(Amount) + " WHERE TransID = " + to_string(TransID) + ";";
    W.exec(sql);
    // W.commit();
    return 0;
}

int update_TRANSACTION_CancelTime(work &W, long int TransID, long int CancelTime) {
    // if transID exists, update CancelTime
    if (!TransIDExists_TRANSACTION_W(W, TransID)) {
        return 1;
    }
    // work W(*C);
    string sql = "UPDATE TRANSACTION SET CancelTime = " + to_string(CancelTime) + " WHERE TransID = " + to_string(TransID) + ";";
    W.exec(sql);
    // W.commit();
    return 0;
}

double get_TRANSACTION_Amount(work &W, long int TransID) {
    // if transID exists, return Amount
    if (!TransIDExists_TRANSACTION_W(W, TransID)) {
        return -1;
    }
    // work W(*C);
    string sql = "SELECT Amount FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(W.exec(sql));
    // W.commit();
    return R[0][0].as<double>();
}

long int get_TRANSACTION_CancelTime(work &W, long int TransID) {
    // if transID exists, return CancelTime
    if (!TransIDExists_TRANSACTION_W(W, TransID)) {
        return -1;
    }
    // work W(*C);
    string sql = "SELECT CancelTime FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(W.exec(sql));
    // W.commit();
    return R[0][0].as<long int>();
}

string get_TRANSACTION_SymbolName(work &W, long int TransID) {
    // work W(*C);
    string sql = "SELECT SymbolName FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(W.exec(sql));
    // W.commit();
    return R[0][0].as<string>();
}

double get_TRANSACTION_PriceLimit(work &W, long int TransID) {
    // work W(*C);
    string sql = "SELECT PriceLimit FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(W.exec(sql));
    // W.commit();
    return R[0][0].as<double>();
}

long int get_TRANSACTION_AccountID(connection *C, long int TransID) {
    nontransaction N(*C);
    string sql = "SELECT AccountID FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    result R(N.exec(sql));
    return R[0][0].as<long int>();
}

// long int get_TRANSACTION_OrderTime(connection *C, long int TransID) {
//     work W(*C);
//     string sql = "SELECT OrderTime FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
//     result R(W.exec(sql));
//     W.commit();
//     return R[0][0].as<long int>();
// }

vector<ActiveOrder> get_TRANSACTION_BuyingOrders(work &W, string SymbolName) {
    // return all active buying orders for a given SymbolName
    // work W(*C);
    string sql = "SELECT * FROM TRANSACTION WHERE SymbolName = " + W.quote(SymbolName) + " AND Amount > 0 AND CancelTime = " + to_string(NOTCANCELLED) + " ORDER BY PriceLimit DESC;";
    result R(W.exec(sql));
    // W.commit();
    vector<ActiveOrder> ActiveOrders;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        ActiveOrder AO = ActiveOrder(c[0].as<long int>(), c[1].as<long int>(), c[2].as<long int>(), c[4].as<double>(), c[5].as<double>());
        ActiveOrders.push_back(AO);
    }  // todo: double check
    return ActiveOrders;
}

vector<ActiveOrder> get_TRANSACTION_SellingOrders(work &W, string SymbolName) {
    // return all active selling orders for a given SymbolName
    // work W(*C);
    string sql = "SELECT * FROM TRANSACTION WHERE SymbolName = " + W.quote(SymbolName) + " AND Amount < 0 AND CancelTime = " + to_string(NOTCANCELLED) + " ORDER BY PriceLimit ASC;";
    result R(W.exec(sql));
    // W.commit();
    vector<ActiveOrder> ActiveOrders;
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        ActiveOrder AO = ActiveOrder(c[0].as<long int>(), c[1].as<long int>(), c[2].as<long int>(), c[4].as<double>(), c[5].as<double>());
        ActiveOrders.push_back(AO);
    }
    return ActiveOrders;
}

int create_EXECUTED(work &W, long int TransID, long int ExecTime, double Amount, double Price) {
    // if transID exists, create a new instance in EXECUTED
    if (!TransIDExists_TRANSACTION_W(W, TransID)) {
        return 1;
    }
    // work W(*C);
    string sql = "INSERT INTO EXECUTED (TransID, ExecTime, Amount, Price) VALUES (" + to_string(TransID) + ", " + to_string(ExecTime) + ", " + to_string(Amount) + ", " + to_string(Price) + ");";
    W.exec(sql);
    // W.commit();
    return 0;
}

bool insufficientFunds(work &W, long int AccountID, string SymbolName, double Price) {
    // check if there is enough money in ACCOUNT for buyer
    double balance = get_ACCOUNT_Balance(W, AccountID);
    if (balance < Price) {
        return true;
    }
    return false;
}

bool insufficientShares(work &W, long int AccountID, string SymbolName, double Amount) {
    // check if there is enough shares in ACCOUNT for seller
    double shares = get_ACCOUNT_Amount(W, AccountID, SymbolName);
    if (shares < Amount) {
        return true;
    }
    return false;
}

long current_time() {
    time_t now = time(nullptr);
    stringstream ss;
    ss << now;
    long time;
    ss >> time;
    return time;
}

void matchOrders(work &W, string SymbolName) {
    cout << "Matching orders for " << SymbolName << endl;
    // match orders in table TRANSACTION for a specific SymbolName
    //  generate two new vector<ActiveOrder> for buying and selling orders
    vector<ActiveOrder> BuyingOrders = get_TRANSACTION_BuyingOrders(W, SymbolName);
    vector<ActiveOrder> SellingOrders = get_TRANSACTION_SellingOrders(W, SymbolName);

    // print BuyingOrders and SellingOrders
    cout << "init BuyingOrders: " << endl;
    for (int i = 0; i < BuyingOrders.size(); i++) {
        cout << BuyingOrders[i].TransID << " " << BuyingOrders[i].OrderTime << " " << BuyingOrders[i].AccountID << " " << BuyingOrders[i].Amount << " " << BuyingOrders[i].PriceLimit << endl;
    }
    cout << "init SellingOrders: " << endl;
    for (int i = 0; i < SellingOrders.size(); i++) {
        cout << SellingOrders[i].TransID << " " << SellingOrders[i].OrderTime << " " << SellingOrders[i].AccountID << " " << SellingOrders[i].Amount << " " << SellingOrders[i].PriceLimit << endl;
    }
    // if there is a match, create a new instance in EXECUTED
    //  a match is when the PriceLimit of buying order is greater than or equal to the PriceLimit of selling order
    while (BuyingOrders.size() > 0 && SellingOrders.size() > 0) {
        if (BuyingOrders[0].PriceLimit >= SellingOrders[0].PriceLimit) {
            // create a new instance in EXECUTED
            long int ExecTime = current_time();
            double sellingAmount = -SellingOrders[0].Amount;
            double Amount = min(BuyingOrders[0].Amount, sellingAmount);
            double Price = 0;
            // the price is the older order's PriceLimit, so compare the OrderTime
            if (BuyingOrders[0].OrderTime <= SellingOrders[0].OrderTime) {
                Price = BuyingOrders[0].PriceLimit;
            } else {
                Price = SellingOrders[0].PriceLimit;
            }
            create_EXECUTED(W, BuyingOrders[0].TransID, ExecTime, Amount, Price);
            create_EXECUTED(W, SellingOrders[0].TransID, ExecTime, -Amount, Price);
            // update amount in TRANSACTION
            update_TRANSACTION_Amount(W, BuyingOrders[0].TransID, BuyingOrders[0].Amount - Amount);
            update_TRANSACTION_Amount(W, SellingOrders[0].TransID, SellingOrders[0].Amount + Amount);
            // update amount and balance in ACCOUNT
            update_ACCOUNT_Position_W(W, BuyingOrders[0].AccountID, SymbolName, Amount);
            double currentBalance = get_ACCOUNT_Balance(W, SellingOrders[0].AccountID);
            update_ACCOUNT_Balance(W, SellingOrders[0].AccountID, Amount * Price + currentBalance);
            // remove the order from the vector if it is fully executed
            if (BuyingOrders[0].Amount == Amount) {
                BuyingOrders.erase(BuyingOrders.begin());
                // update the amount in SellingOrders[0]
                SellingOrders[0].Amount += Amount;
                if (SellingOrders[0].Amount == 0) {
                    SellingOrders.erase(SellingOrders.begin());
                }
            } else if (SellingOrders[0].Amount == -Amount) {
                SellingOrders.erase(SellingOrders.begin());
                // update the amount in BuyingOrders[0]
                BuyingOrders[0].Amount -= Amount;
                if (BuyingOrders[0].Amount == 0) {
                    BuyingOrders.erase(BuyingOrders.begin());
                }
            }
            // aaa
            if (BuyingOrders.size() == 0 || SellingOrders.size() == 0) {
                break;
            }
        } else {
            break;
        }
    }
}

Order get_order(connection *C, long int &TransID) {
    // return an Order object for a given TransID
    // first check if TransID exists in TRANSACTION
    Order myOrder;
    if (!TransIDExists_TRANSACTION(C, TransID)) {
        myOrder.TransID = -1;
        return myOrder;
    }
    // get TransID, AccountID, SymbolName, Amount, PriceLimit, CancelTime from TRANSACTION
    // work W(*C);
    nontransaction N(*C);
    string sql = "SELECT * FROM TRANSACTION WHERE TransID = " + to_string(TransID) + ";";
    // result R(W.exec(sql));
    result R(N.exec(sql));
    myOrder.TransID = R[0][0].as<long int>();
    myOrder.AccountID = R[0][2].as<long int>();
    myOrder.SymbolName = R[0][3].as<string>();
    myOrder.Amount = R[0][4].as<double>();
    myOrder.PriceLimit = R[0][5].as<double>();
    myOrder.CancelTime = R[0][6].as<long int>();
    // get executed orders from EXECUTED
    sql = "SELECT * FROM EXECUTED WHERE TransID = " + to_string(TransID) + ";";
    // R = W.exec(sql);
    R = N.exec(sql);
    // W.commit();
    for (result::const_iterator c = R.begin(); c != R.end(); ++c) {
        ExecutedOrder EO(c[0].as<long int>(), c[1].as<long int>(), c[2].as<double>(), c[3].as<double>());  // todo: double check
        myOrder.ExecutedOrders.push_back(EO);
    }
    return myOrder;
}

int cancel_order(connection *C, long int &TransID, long int &AccountID) {
    // first check if Amount in TRANSACTION is 0 or CancelTime is not NOTCANCELLED
    // if so return 1
    work W(*C);
    double currentTransAmount = get_TRANSACTION_Amount(W, TransID);
    string currentSymbolName = get_TRANSACTION_SymbolName(W, TransID);
    long int currentCancelTime = get_TRANSACTION_CancelTime(W, TransID);
    if (currentTransAmount == 0 || currentCancelTime != NOTCANCELLED) {
        return 1;
    }
    // update CancelTime in TRANSACTION
    update_TRANSACTION_CancelTime(W, TransID, current_time());
    // if currentAmount is smaller than zero this is a sell order: increase number of Amount in ACCOUNT
    // else this is a buy order: increase balance in ACCOUNT
    if (currentTransAmount < 0) {
        double currentAccAmount = get_ACCOUNT_Amount(W, AccountID, currentSymbolName);
        double newAmount = currentAccAmount - currentTransAmount;
        update_ACCOUNT_Amount(W, AccountID, currentSymbolName, newAmount);
    } else {
        double currentAccBalance = get_ACCOUNT_Balance(W, AccountID);
        double currentPriceLimit = get_TRANSACTION_PriceLimit(W, TransID);
        double newBalance = currentAccBalance + currentTransAmount * currentPriceLimit;
        update_ACCOUNT_Balance(W, AccountID, newBalance);
    }
    W.commit();
    return 0;
}