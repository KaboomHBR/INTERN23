#include "client.hpp"

using namespace std;
// ./client vcm-30963.vm.duke.edu 12345
// ./client vcm-32254.vm.duke.edu 12345

std::string generate_id(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    // return std::to_string(((rand()% (100000)) + 0));
    return std::to_string(((rand() % (10)) + 10));
}
// balance
double generate_balance(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand() % (10000)) + 10);
}
// symbol
char generate_sym(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    // int rand_num = rand()%26;
    int rand_num = rand() % 10;
    return ('A' + rand_num);
}
// amount
double generate_amount(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand() % (500)) + 100);
}
//<transactions>
// price
double generate_price(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    return ((rand() % (100)) + 10);
}
std::string generate_trans_id(unsigned seed) {
    // unsigned seed;
    // seed = time(NULL);
    srand(seed);
    // return std::to_string(((rand()% (100000)) + 0));
    return std::to_string(((rand() % (11)) + 9));
}
// order:0 / query:1 / cancel: 2 [0,3)
int generate_mode(unsigned seed) {
    srand(seed);
    return ((rand() % 30) + 10);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "./client <machine_name> <port_num>" << std::endl;
        exit(1);
    }
    const char *hostname = argv[1];
    const char *port = argv[2];
    int socket_fd = create_client(hostname, port);
    // generate client requests in XML
    // todo: generate <create> XML
    // todo: generate <transaction> XML
    string xml;
    vector<string> xmls;

    // int times = 500;
    // unsigned seed = time(NULL);
    // // randomly generate requests
    // for (int i = 0; i < times; i++) {
    //     seed += i;

    //     string id = generate_id(seed);
    //     cout << "account_id: " << id << endl;

    //     double balance = generate_amount(seed);
    //     cout << "balance: " << balance << endl;
    //     stringstream ssBalance;
    //     ssBalance << balance;
    //     string balance_str = ssBalance.str();

    //     char symbol = generate_sym(seed);
    //     cout << "symbol: " << symbol << endl;
    //     string symbol_str = to_string(symbol);

    //     double amount = generate_amount(seed);
    //     cout << "amount: " << amount << endl;
    //     stringstream ssAmount;
    //     ssAmount << amount;
    //     string amount_str = ssAmount.str();

    //     double price = generate_price(seed);
    //     cout << "price: " << price << endl;
    //     string price_str = to_string(price);

    //     string trans_id = generate_trans_id(seed);
    //     cout << "trans_id: " << trans_id << endl;

    //     int mode = generate_mode(seed);
    //     cout << "mode: " << mode << endl;

    //     if (i % 3 == 1) {
    //         createXML_account(xml, id, balance_str);
    //         xmls.push_back(xml);
    //     } else if (i % 3 == 2) {
    //         createXML_symbol(xml, id, symbol_str, amount_str);
    //         xmls.push_back(xml);
    //     } else {
    //         createXML(xml, id, balance_str, symbol_str, amount_str);
    //         xmls.push_back(xml);
    //     }

    // xml is:
    // <?xml version="1.0" encoding="UTF-8"?>
    // <create>
    //   <account id="11" balance="1000"/>
    //   <symbol sym="SPY">
    //      <account id="11">100000</account>
    //   </symbol>
    // </create>
    xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    xml += "\n<create>";
    xml += "\n\t<account id=\"11\" balance=\"1000\"/>";
    xml += "\n\t<symbol sym=\"SPY\">";
    xml += "\n\t\t<account id=\"11\">100000</account>";
    xml += "\n\t</symbol>";
    xml += "\n</create>";

    // // ============================== <create> XML ==============================
    // createXML_account(xml, "1", "100");
    // xmls.push_back(xml);
    // createXML_symbol(xml, "1", "Apple", "10");
    // xmls.push_back(xml);
    // createXML_account(xml, "2", "100");
    // xmls.push_back(xml);
    // // xmls.push_back(xml);
    // // // ============================== <create> XML: errors ==============================
    // // createXML_empty(xml);
    // // xmls.push_back(xml);
    // // createXML_stupid(xml);
    // // xmls.push_back(xml);
    // // ============================== <transaction> XML ==============================
    // transactionsXML_sell(xml, "1", "Apple", "-2", "5");  // transID 1
    // xmls.push_back(xml);
    // transactionsXML_sell(xml, "1", "Apple", "-2", "6");  // transID 2
    // xmls.push_back(xml);
    // transactionsXML_sell(xml, "1", "Apple", "-2", "5");  // transID 3
    // xmls.push_back(xml);
    // transactionsXML_buy(xml, "2", "Apple", "3", "6");  // transID 4
    // xmls.push_back(xml);
    // transactionsXML_buy(xml, "2", "Apple", "2", "5");  // transID 5
    // xmls.push_back(xml);
    // transactionsXML_cancel(xml, "2", "5");
    // xmls.push_back(xml);
    // transactionsXML_query(xml, "1", "1");  // transID 3
    // xmls.push_back(xml);
    // transactionsXML_query(xml, "1", "2");
    // xmls.push_back(xml);
    // transactionsXML_query(xml, "1", "3");
    // xmls.push_back(xml);
    // transactionsXML_query(xml, "2", "4");
    // xmls.push_back(xml);
    // transactionsXML_query(xml, "2", "5");
    // xmls.push_back(xml);
    // // ============================== END ==============================
    char server_response[65536] = {0};
    int length = 0;
    std::string response;
    for (auto &xml : xmls) {
        cout << "client generated request:=========" << endl
             << xml << endl;
        // send requests to server
        send(socket_fd, xml.c_str(), xml.length(), 0);
        // receive response from server
        memset(server_response, 0, sizeof(server_response));
        length = recv(socket_fd, server_response, sizeof(server_response), 0);
        cout << "client received response length: " << length << endl;
        response.assign(server_response, length);
        cout << "client received response:---------" << endl
             << response << endl;
    }

    close(socket_fd);
    return 0;
}
/*
 *
 *
 *
 *
 *
 *
 */

void createXML_account(string &xml, string account_id, string balance) {
    // length of the xml
    //<?xml version="1.0" encoding="UTF-8"?>
    //<create>
    //   <account id="account_id" balance="balance"/>
    //</create>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n\t<account id=\"" + account_id + "\" balance=\"" + balance + "\"/>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void createXML_symbol(string &xml, string account_id, string symbol, string amount) {
    //<create>
    //  <symbol sym="symbol">
    //    <account id="account_id">amount</account>
    //  </symbol>
    //</create>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n\t<symbol sym=\"" + symbol + "\">\n\t\t<account id=\"" + account_id + "\">" + amount + "</account>\n\t</symbol>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void createXML(string &xml, string account_id, string balance, string symbol, string amount) {
    //<create>
    //  <account id="account_id" balance="balance"/>
    //  <sym="symbol">
    //    <account id="account_id">amount</account>
    //    <account id="1236">amount</account>
    //  </symbol>
    //</create>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n\t<account id=\"" + account_id + "\" balance=\"" + balance + "\"/>\n";
    xml_temp += "\t<symbol sym=\"" + symbol + "\">\n";
    xml_temp += "\t\t<account id=\"" + account_id + "\">" + amount + "</account>\n";
    xml_temp += "\t\t<account id=\"1236\">" + amount + "</account>\n";
    xml_temp += "\t\t<account id=\"" + account_id + "\">" + amount + "</account>\n";
    xml_temp += "\t</symbol>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void createXML_empty(string &xml) {
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void createXML_symbol_empty(string &xml) {
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n\t<symbol sym=\"AAPL\">\n\t</symbol>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void createXML_stupid(string &xml) {
    // helloworld
    //<create>
    //  <stupid sym="symbol">
    //  </stupid>
    //</create>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<create>\n\t<stupid sym=\"AAPL\">\n\t</stupid>\n</create>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void transactionsXML_buy(string &xml, string account_id, string symbol, string amount, string limit) {
    //<transactions id="account_id">
    //  <order sym="symbol" amount="amount" limit="limit"/>
    //</transactions>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<transactions id=\"" + account_id + "\">\n";
    xml_temp += "\t<order sym=\"" + symbol + "\" amount=\"" + amount + "\" limit=\"" + limit + "\"/>\n";
    xml_temp += "</transactions>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void transactionsXML_sell(string &xml, string account_id, string symbol, string amount, string limit) {
    //<transactions id="account_id">
    //  <order sym="symbol" amount="amount" limit="limit"/>
    //</transactions>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<transactions id=\"" + account_id + "\">\n";
    xml_temp += "\t<order sym=\"" + symbol + "\" amount=\"" + amount + "\" limit=\"" + limit + "\"/>\n";
    xml_temp += "</transactions>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}

void transactionsXML_query(string &xml, string account_id, string trans_id) {
    //<transactions id="account_id">
    //  <query id="trans_id">
    //</transactions>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<transactions id=\"" + account_id + "\">\n";
    xml_temp += "\t<query id=\"" + trans_id + "\"/>\n";
    xml_temp += "</transactions>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}
void transactionsXML_cancel(string &xml, string account_id, string trans_id) {
    //<transactions id="account_id">
    //  <cancel id="trans_id">
    //</transactions>
    string xml_temp;
    xml_temp += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml_temp += "<transactions id=\"" + account_id + "\">\n";
    xml_temp += "\t<cancel id=\"" + trans_id + "\"/>\n";
    xml_temp += "</transactions>";
    xml = to_string(xml_temp.length()) + "\n" + xml_temp;
}