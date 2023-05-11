#ifndef __ORDER_HPP__
#define __ORDER_HPP__

#include <iostream>
#include <string>
#include <vector>
/*
 * Primarily used for giving responses
 * for requests just use the TransID, no need for encapsulation
 */

using namespace std;

class ExecutedOrder {
    // an Order might be split into multiple ExecutedOrders
   public:
    long int TransID;
    long int ExecTime;
    double Amount;  // the actual amount of shares traded
    double Price;   // the actual price of each share
    ExecutedOrder() {
        TransID = 0;
        ExecTime = 0;
        Amount = 0;
        Price = 0;
    }
    ExecutedOrder(long int _TransID, long int _ExecTime, double _Amount, double _Price) {
        TransID = _TransID;
        ExecTime = _ExecTime;
        Amount = _Amount;
        Price = _Price;
    }
};

class Order {
    // this is the equivalence of an instance of TRANSACTION in the database
   public:
    long int TransID;
    long int AccountID;
    string SymbolName;
    double Amount;
    double PriceLimit;
    long int CancelTime;
    vector<ExecutedOrder> ExecutedOrders;
    Order() {
        TransID = 0;
        AccountID = 0;
        SymbolName;
        Amount = 0;
        PriceLimit = 0;
        CancelTime = 0;
        ExecutedOrders;
    }
    Order(long int _TransID, long int _AccountID, string _SymbolName, double _Amount, double _PriceLimit, long int _CancelTime, vector<ExecutedOrder> _ExecutedOrders) {
        TransID = _TransID;
        AccountID = _AccountID;
        SymbolName = _SymbolName;
        Amount = _Amount;
        PriceLimit = _PriceLimit;
        CancelTime = _CancelTime;
        ExecutedOrders = _ExecutedOrders;
    }
};

class ActiveOrder {
   public:
    long int TransID;    // 0
    long int OrderTime;  // 1
    long int AccountID;  // 2
    double Amount;       // 4
    double PriceLimit;   // 5
    ActiveOrder() {
        TransID = 0;
        OrderTime = 0;
        AccountID = 0;
        Amount = 0;
        PriceLimit = 0;
    }
    ActiveOrder(long int _TransID, long int _OrderTime, long int _AccountID, double _Amount, double _PriceLimit) {
        TransID = _TransID;
        OrderTime = _OrderTime;
        AccountID = _AccountID;
        Amount = _Amount;
        PriceLimit = _PriceLimit;
    }
    ActiveOrder& operator=(const ActiveOrder& other) {
        TransID = other.TransID;
        OrderTime = other.OrderTime;
        AccountID = other.AccountID;
        Amount = other.Amount;
        PriceLimit = other.PriceLimit;
        return *this;
    }
    bool operator==(const ActiveOrder& other) {
        return (TransID == other.TransID && OrderTime == other.OrderTime && AccountID == other.AccountID && Amount == other.Amount && PriceLimit == other.PriceLimit);
    }
};

#endif