#include "handle_xml.hpp"

int handle_request(connection *C, string xml, string &response) {
    XMLDocument doc;
    doc.Parse(xml.c_str());
    XMLElement *xmlRoot = doc.RootElement();
    if (xmlRoot == NULL) {
        response = "Error: XML is empty";  // todo: log it
        return 1;
    }
    string type = xmlRoot->Name();
    if (type == "create") {
        handle_create(C, xmlRoot, response);
        return 0;
    } else if (type == "transactions") {
        handle_transaction(C, xmlRoot, response);
        return 0;
    } else {
        // response = "Error: XML is not a valid request";  // todo: log it
        response = "ERROR: " + xml;
        return 1;
    }
}

void handle_create(connection *C, XMLElement *xmlRoot, string &response) {
    // handle <create> tag
    // store string of XML in response
    long int myAccountID = 0;
    double myBalance = 0;
    string mySymbolName;
    double myAmount = 0;
    XMLDocument doc;
    XMLElement *root = doc.NewElement("results");
    XMLElement *child = xmlRoot->FirstChildElement();
    if (child == NULL) {
        // error message if no <account> or <symbol> tags
        error_NothingInTag(doc, root);
    }
    while (child) {
        string name = child->Name();
        if (name == "account") {
            myAccountID = atoi(child->Attribute("id"));
            myBalance = atof(child->Attribute("balance"));
            int error = create_ACCOUNT(C, myAccountID, myBalance);
            if (error) {
                error_AccountCreate(doc, root, myAccountID);
            } else {
                success_AccountCreate(doc, root, myAccountID);
            }
        } else if (name == "symbol") {
            mySymbolName = child->Attribute("sym");
            XMLElement *symbolchild = child->FirstChildElement();
            if (symbolchild == NULL) {
                error_SymbolEmpty(doc, root, mySymbolName);
            }
            while (symbolchild) {
                string symbolchlidname = symbolchild->Name();
                if (symbolchlidname == "account") {
                    myAccountID = atoi(symbolchild->Attribute("id"));
                    myAmount = atof(symbolchild->GetText());
                    int error = update_ACCOUNT_Position(C, myAccountID, mySymbolName, myAmount);
                    if (error) {
                        error_SymbolCreate(doc, root, myAccountID, mySymbolName);
                    } else {
                        success_SymbolCreate(doc, root, myAccountID, mySymbolName);
                    }
                } else {
                    // todo: double check
                    error_InvalidTag(doc, root);
                }
                symbolchild = symbolchild->NextSiblingElement();
            }
        } else {
            error_InvalidTag(doc, root);
        }
        child = child->NextSiblingElement();
    }

    doc.InsertEndChild(root);
    XMLPrinter printer;
    doc.Print(&printer);
    response = printer.CStr();
}

void handle_transaction(connection *C, XMLElement *xmlRoot, string &response) {
    // handle <transaction> tag
    // store string of XML in response
    long int myAccountID = atoi(xmlRoot->Attribute("id"));
    string mySymbolName;
    double myAmount = 0;
    double myActualAmount = 0;
    double myActualBalance = 0;
    double myPriceLimit = 0;
    long int myTransID = 0;
    XMLDocument doc;
    XMLElement *root = doc.NewElement("results");
    if (!AccountExists_ACCOUNT(C, myAccountID)) {
        cout << "Account does not exist" << endl;
        error_AccountDoesNotExist(doc, root, myAccountID);
        doc.InsertEndChild(root);
        XMLPrinter printer;
        doc.Print(&printer);
        response = printer.CStr();
        return;
    }
    XMLElement *child = xmlRoot->FirstChildElement();
    while (child) {
        string name = child->Name();
        if (name == "order") {
            cout << "Order---------" << endl;
            work W(*C);
            mySymbolName = child->Attribute("sym");
            myAmount = atof(child->Attribute("amount"));
            myPriceLimit = atof(child->Attribute("limit"));
            if (myAmount == 0) {
                error_AmountIsZero(doc, root, mySymbolName, myAmount, myPriceLimit);
                child = child->NextSiblingElement();
                continue;
            } else if (myAmount > 0) {
                cout << "Buy order" << endl;
                int error = handle_transaction_buyorder(W, doc, root, myAccountID, mySymbolName, myAmount, myPriceLimit, myTransID);
                if (error) {
                    // error_InsufficientFunds(doc, root, mySymbolName, myAmount, myPriceLimit);
                    child = child->NextSiblingElement();
                    continue;
                }
            } else {
                double positiveAmount = -myAmount;
                int error = handle_transaction_sellorder(W, doc, root, myAccountID, mySymbolName, positiveAmount, myPriceLimit, myTransID);
                if (error) {
                    // error_InsufficientShares(doc, root, mySymbolName, myAmount, myPriceLimit);
                    child = child->NextSiblingElement();
                    continue;
                }
            }
            matchOrders(W, mySymbolName);
            success_TransactionCreate(doc, root, mySymbolName, myAmount, myPriceLimit, myTransID);
            W.commit();
        } else if (name == "query") {
            myTransID = atoi(child->Attribute("id"));
            Order myOrder = get_order(C, myTransID);
            if (myOrder.TransID == -1) {
                error_TransactionDoesNotExist(doc, root, myTransID);
                child = child->NextSiblingElement();
                continue;
            }
            if (myOrder.AccountID != myAccountID) {
                error_AccountDoesNotOwnTransaction(doc, root, myTransID);
                child = child->NextSiblingElement();
                continue;
            }
            success_TransactionQuery(doc, root, myOrder);
        } else if (name == "cancel") {
            cout << "-------------cancel--------------" << endl;
            myTransID = atoi(child->Attribute("id"));
            // first check if myTransID exists
            if (!TransIDExists_TRANSACTION(C, myTransID)) {
                cout << "Transaction does not exist---" << endl;
                error_TransactionDoesNotExist(doc, root, myTransID);
                child = child->NextSiblingElement();
                continue;
            }
            // then check if myTransID belongs to myAccountID
            long int newAccountID = get_TRANSACTION_AccountID(C, myTransID);
            if (newAccountID != myAccountID) {
                error_AccountDoesNotOwnTransaction(doc, root, myTransID);
                child = child->NextSiblingElement();
                continue;
            }
            int error = cancel_order(C, myTransID, myAccountID);
            if (error) {
                cout << "Transaction already cancelled or finished" << endl;
                error_TransactionAlreadyCancelledFinished(doc, root, myTransID);
                child = child->NextSiblingElement();
                continue;
            } else {
                cout << "Transaction cancelled" << endl;
            }
            Order myOrder = get_order(C, myTransID);
            cout << "transID: " << myOrder.TransID << endl;
            success_TransactionCancel(doc, root, myOrder);
            cout << "-------------cancel success--------------" << endl;
        } else {
            error_InvalidTag(doc, root);
        }
        child = child->NextSiblingElement();
    }
    doc.InsertEndChild(root);
    XMLPrinter printer;
    doc.Print(&printer);
    response = printer.CStr();
}

int handle_transaction_buyorder(work &W, XMLDocument &doc, XMLElement *xmlRoot, long int &AccountID, string &SymbolName, double &Amount, double &PriceLimit, long int &myTransID) {
    double expecting_spending = Amount * PriceLimit;
    if (insufficientFunds(W, AccountID, SymbolName, expecting_spending)) {
        error_InsufficientFunds(doc, xmlRoot, SymbolName, Amount, PriceLimit);
        return 1;
    }
    myTransID = create_TRANSACTION(W, AccountID, SymbolName, Amount, PriceLimit);
    if (myTransID == -1) {
        error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, Amount, PriceLimit);
        return 1;
    }
    double myActualBalance = get_ACCOUNT_Balance(W, AccountID);
    int error = update_ACCOUNT_Balance(W, AccountID, myActualBalance - Amount * PriceLimit);
    if (error) {
        error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, Amount, PriceLimit);
        return 1;
    }
    // double myActualAmount = get_ACCOUNT_Amount(W, AccountID, SymbolName);
    // error = update_ACCOUNT_Position_W(W, AccountID, SymbolName, myActualAmount + Amount);
    // if (error) {
    //     error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, Amount, PriceLimit);
    //     return 1;
    // }
    cout << "Buy order success" << endl;
    return 0;
}

int handle_transaction_sellorder(work &W, XMLDocument &doc, XMLElement *xmlRoot, long int &AccountID, string &SymbolName, double &Amount, double &PriceLimit, long int &myTransID) {
    if (insufficientShares(W, AccountID, SymbolName, Amount)) {
        error_InsufficientShares(doc, xmlRoot, SymbolName, Amount, PriceLimit);
        return 1;
    }
    double inputAmount = -Amount;
    myTransID = create_TRANSACTION(W, AccountID, SymbolName, inputAmount, PriceLimit);
    if (myTransID == -1) {
        error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, inputAmount, PriceLimit);
        return 1;
    }
    double myActualAmount = get_ACCOUNT_Amount(W, AccountID, SymbolName);
    int error = update_ACCOUNT_Amount(W, AccountID, SymbolName, myActualAmount - Amount);
    if (error) {
        error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, inputAmount, PriceLimit);
        return 1;
    }
    // double myActualBalance = get_ACCOUNT_Balance(W, AccountID);
    // error = update_ACCOUNT_Balance(W, AccountID, myActualBalance + Amount * PriceLimit);
    // if (error) {
    //     error_InvalidTransactionCreate(doc, xmlRoot, SymbolName, inputAmount, PriceLimit);
    //     return 1;
    // }
    return 0;
}

void success_AccountCreate(XMLDocument &doc, XMLElement *root, long int &AccountID) {
    XMLElement *child = doc.NewElement("created");
    child->SetAttribute("id", to_string(AccountID).c_str());
    root->InsertEndChild(child);
}

void success_SymbolCreate(XMLDocument &doc, XMLElement *root, long int &AccountID, string SymbolName) {
    XMLElement *child = doc.NewElement("created");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("id", to_string(AccountID).c_str());
    root->InsertEndChild(child);
}

void error_AccountCreate(XMLDocument &doc, XMLElement *root, long int &AccountID) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("id", to_string(AccountID).c_str());
    XMLText *msg = doc.NewText("Account already exists");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_SymbolCreate(XMLDocument &doc, XMLElement *root, long int &AccountID, string SymbolName) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("id", to_string(AccountID).c_str());
    XMLText *msg = doc.NewText("Account does not exist");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_SymbolEmpty(XMLDocument &doc, XMLElement *root, string SymbolName) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    XMLText *msg = doc.NewText("No account specified");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_InvalidTag(XMLDocument &doc, XMLElement *root) {
    XMLElement *child = doc.NewElement("error");
    XMLText *msg = doc.NewText("Invalid tag");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_NothingInTag(XMLDocument &doc, XMLElement *root) {
    XMLElement *child = doc.NewElement("error");
    XMLText *msg = doc.NewText("Nothing in tag");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_AccountDoesNotExist(XMLDocument &doc, XMLElement *root, long int &AccountID) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("id", to_string(AccountID).c_str());
    XMLText *msg = doc.NewText("Account does not exist");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_TransactionDoesNotExist(XMLDocument &doc, XMLElement *root, long int &TransID) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("id", to_string(TransID).c_str());
    XMLText *msg = doc.NewText("TransactionID does not exist");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_AccountDoesNotOwnTransaction(XMLDocument &doc, XMLElement *root, long int &TransID) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("id", to_string(TransID).c_str());
    XMLText *msg = doc.NewText("Account does not own transaction, access denied");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_TransactionAlreadyCancelledFinished(XMLDocument &doc, XMLElement *root, long int &TransID) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("id", to_string(TransID).c_str());
    XMLText *msg = doc.NewText("Transaction already cancelled or finished");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_AmountIsZero(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("amount", to_string(Amount).c_str());
    child->SetAttribute("limit", to_string(PriceLimit).c_str());
    XMLText *msg = doc.NewText("Input amount is zero");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_InsufficientFunds(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("amount", to_string(Amount).c_str());
    child->SetAttribute("limit", to_string(PriceLimit).c_str());
    XMLText *msg = doc.NewText("Insufficient funds");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_InsufficientShares(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("amount", to_string(Amount).c_str());
    child->SetAttribute("limit", to_string(PriceLimit).c_str());
    XMLText *msg = doc.NewText("Insufficient shares");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void error_InvalidTransactionCreate(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit) {
    XMLElement *child = doc.NewElement("error");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("amount", to_string(Amount).c_str());
    child->SetAttribute("limit", to_string(PriceLimit).c_str());
    XMLText *msg = doc.NewText("Invalid transaction create");
    child->InsertEndChild(msg);
    root->InsertEndChild(child);
}

void success_TransactionCreate(XMLDocument &doc, XMLElement *root, string SymbolName, double &Amount, double &PriceLimit, long int &TransID) {
    XMLElement *child = doc.NewElement("opened");
    child->SetAttribute("sym", SymbolName.c_str());
    child->SetAttribute("amount", to_string(Amount).c_str());
    child->SetAttribute("limit", to_string(PriceLimit).c_str());
    child->SetAttribute("id", to_string(TransID).c_str());
    root->InsertEndChild(child);
}

void success_TransactionQuery(XMLDocument &doc, XMLElement *root, Order &myOrder) {
    XMLElement *child = doc.NewElement("status");
    child->SetAttribute("id", to_string(myOrder.TransID).c_str());
    if (myOrder.Amount == 0) {
        success_PrintExecuted(doc, child, myOrder);
    } else if (myOrder.CancelTime == -1) {
        success_PrintOpened(doc, child, myOrder);
        success_PrintExecuted(doc, child, myOrder);
    } else {
        success_PrintCancelled(doc, child, myOrder);
        success_PrintExecuted(doc, child, myOrder);
    }
    root->InsertEndChild(child);
}

void success_TransactionCancel(XMLDocument &doc, XMLElement *root, Order &myOrder) {
    XMLElement *child = doc.NewElement("canceled");
    child->SetAttribute("id", to_string(myOrder.TransID).c_str());
    success_PrintCancelled(doc, child, myOrder);
    success_PrintExecuted(doc, child, myOrder);
    root->InsertEndChild(child);
}

void success_PrintExecuted(XMLDocument &doc, XMLElement *child, Order &myOrder) {
    // just print the executed stuff
    // onto multiple new lines of the XML
    for (vector<ExecutedOrder>::iterator it = myOrder.ExecutedOrders.begin(); it < myOrder.ExecutedOrders.end(); it++) {
        XMLElement *child2 = doc.NewElement("executed");
        double newAmount = it->Amount;
        if (newAmount < 0) {
            newAmount = -newAmount;
        }  // todo: notice this is required by the tasksheet that the showing amount is always positive from the example
        child2->SetAttribute("shares", to_string(newAmount).c_str());
        child2->SetAttribute("price", to_string(it->Price).c_str());
        child2->SetAttribute("time", to_string(it->ExecTime).c_str());
        child->InsertEndChild(child2);
    }
}

void success_PrintOpened(XMLDocument &doc, XMLElement *child, Order &myOrder) {
    // just print one line: <open shares=.../>
    XMLElement *child2 = doc.NewElement("open");
    child2->SetAttribute("shares", to_string(myOrder.Amount).c_str());
    child->InsertEndChild(child2);
}

void success_PrintCancelled(XMLDocument &doc, XMLElement *child, Order &myOrder) {
    // just print one line <canceled shares=... time=.../>
    XMLElement *child2 = doc.NewElement("canceled");
    child2->SetAttribute("shares", to_string(myOrder.Amount).c_str());
    child2->SetAttribute("time", to_string(myOrder.CancelTime).c_str());
    child->InsertEndChild(child2);
}