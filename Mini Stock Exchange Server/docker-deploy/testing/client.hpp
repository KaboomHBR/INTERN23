#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "socket.hpp"

#define PORT "12345"
using namespace std;

#include <algorithm>
#include <vector>
#include <assert.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>

#include <ctime>
// ./client vcm-30963.vm.duke.edu 12345
// ./client vcm-32254.vm.duke.edu 12345

// <create> XML
void createXML_account(string &xml, string account_id, string balance);
void createXML_symbol(string &xml, string account_id, string symbol, string amount);
void createXML(string &xml, string account_id, string balance, string symbol, string amount);

// <create> XML: errors
void createXML_empty(string &xml);
void createXML_symbol_empty(string &xml);
void createXML_stupid(string &xml);

// <transactions> XML
void transactionsXML_buy(string &xml, string account_id, string symbol, string amount, string limit);
void transactionsXML_sell(string &xml, string account_id, string symbol, string amount, string limit);
void transactionsXML_query(string &xml, string account_id, string trans_id);
void transactionsXML_cancel(string &xml, string account_id, string trans_id);

#endif