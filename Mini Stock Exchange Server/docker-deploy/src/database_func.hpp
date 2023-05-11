#ifndef __DATABASE_FUNC_HPP__
#define __DATABASE_FUNC_HPP__

#include <iomanip>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "order.hpp"

using namespace std;
using namespace pqxx;
using namespace xercesc;

void sql_exec(connection *C, string sql);

int create_database();
void drop_tables(connection *C);
void create_table_SYMBOL(connection *C);
void create_table_ACCOUNT(connection *C);
void create_table_TRANSACTION(connection *C);
void create_table_EXECUTED(connection *C);

bool SymbolExists_SYMBOL(connection *C, string SymbolName);
bool SymbolExists_SYMBOL_W(work &W, string SymbolName);
int update_SYMBOL_W(work &W, string SymbolName);
int update_SYMBOL(connection *C, string SymbolName);

bool AccountExists_ACCOUNT(connection *C, long int AccountID);
bool AccountExists_ACCOUNT_W(work &W, long int AccountID);
bool SymbolExists_ACCOUNT(connection *C, long int AccountID, string SymbolName);
bool SymbolExists_ACCOUNT_W(work &W, long int AccountID, string SymbolName);
int create_ACCOUNT(connection *C, long int AccountID, double Balance);
int update_ACCOUNT_Position(connection *C, long int AccountID, string SymbolName, double Amount);  // suggested usage: for <create>
int update_ACCOUNT_Position_W(work &W, long int AccountID, string SymbolName, double Amount);
int update_ACCOUNT_Amount(work &W, long int AccountID, string SymbolName, double Amount);  // suggested usage: for <transaction>
int update_ACCOUNT_Balance(work &W, long int AccountID, double Balance);                   // suggested usage: for <transaction>
double get_ACCOUNT_Amount(work &W, long int AccountID, string SymbolName);
double get_ACCOUNT_Balance(work &W, long int AccountID);

long current_time();
bool TransIDExists_TRANSACTION(connection *C, long int TransID);
bool TransIDExists_TRANSACTION_W(work &W, long int TransID);
long int create_TRANSACTION(work &W, long int AccountID, string SymbolName, double Amount, double PriceLimit);
int update_TRANSACTION_Amount(work &W, long int TransID, double Amount);
int update_TRANSACTION_CancelTime(work &W, long int TransID, long int CancelTime);
double get_TRANSACTION_Amount(work &W, long int TransID);  // on transaction: if transID exists and Amount is not 0 and CancelTime is NOTCANCELLED, update Amount
long int get_TRANSACTION_CancelTime(work &W, long int TransID);
string get_TRANSACTION_SymbolName(work &W, long int TransID);
double get_TRANSACTION_PriceLimit(work &W, long int TransID);
long int get_TRANSACTION_AccountID(connection *C, long int TransID);
// long int get_TRANSACTION_OrderTime(connection *C, long int TransID);
vector<ActiveOrder> get_TRANSACTION_BuyingOrders(work &W, string SymbolName);
vector<ActiveOrder> get_TRANSACTION_SellingOrders(work &W, string SymbolName);

int create_EXECUTED(work &W, long int TransID, long int ExecTime, double Amount, double Price);

bool insufficientFunds(work &W, long int AccountID, string SymbolName, double Price);
bool insufficientShares(work &W, long int AccountID, string SymbolName, double Amount);

void matchOrders(work &W, string SymbolName);

int cancel_order(connection *C, long int &TransID, long int &AccountID);
Order get_order(connection *C, long int &TransID);  // first goota check if TransID exists

#endif