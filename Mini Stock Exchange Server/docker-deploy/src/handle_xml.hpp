#ifndef __HANDLE_XML_HPP__
#define __HANDLE_XML_HPP__

#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#include "database_func.hpp"
#include "order.hpp"
#include "tinyxml2.hpp"

using namespace tinyxml2;
using namespace pqxx;
using namespace std;

int handle_request(connection *C, string xml, string &response);
void handle_create(connection *C, XMLElement *xmlRoot, string &response);
void handle_transaction(connection *C, XMLElement *xmlRoot, string &response);
int handle_transaction_buyorder(work &W, XMLDocument &doc, XMLElement *xmlRoot, long int &AccountID, string &SymbolName, double &Amount, double &PriceLimit, long int &myTransID);
int handle_transaction_sellorder(work &W, XMLDocument &doc, XMLElement *xmlRoot, long int &AccountID, string &SymbolName, double &Amount, double &PriceLimit, long int &myTransID);
// for <create>
void success_AccountCreate(XMLDocument &doc, XMLElement *root, long int &AccountID);
void success_SymbolCreate(XMLDocument &doc, XMLElement *root, long int &AccountID, string SymbolName);
void error_AccountCreate(XMLDocument &doc, XMLElement *root, long int &AccountID);
void error_SymbolCreate(XMLDocument &doc, XMLElement *root, long int &AccountID, string SymbolName);
void error_SymbolEmpty(XMLDocument &doc, XMLElement *root, string SymbolName);
void error_InvalidTag(XMLDocument &doc, XMLElement *root);
void error_NothingInTag(XMLDocument &doc, XMLElement *root);
// for <transaction>
void error_AccountDoesNotExist(XMLDocument &doc, XMLElement *root, long int &AccountID);
void error_TransactionDoesNotExist(XMLDocument &doc, XMLElement *root, long int &TransID);
void error_AccountDoesNotOwnTransaction(XMLDocument &doc, XMLElement *root, long int &TransID);
void error_TransactionAlreadyCancelledFinished(XMLDocument &doc, XMLElement *root, long int &TransID);
void error_AmountIsZero(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit);
void error_InsufficientFunds(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit);
void error_InsufficientShares(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit);
void error_InvalidTransactionCreate(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit);
void success_TransactionCreate(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit, long int &TransID);
void success_TransactionQuery(XMLDocument &doc, XMLElement *root, Order &myOrder);
void success_TransactionCancel(XMLDocument &doc, XMLElement *root, Order &myOrder);
void success_PrintExecuted(XMLDocument &doc, XMLElement *child, Order &myOrder);
void success_PrintOpened(XMLDocument &doc, XMLElement *child, Order &myOrder);
void success_PrintCancelled(XMLDocument &doc, XMLElement *child, Order &myOrder);
#endif