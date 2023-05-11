#include "database_func.hpp"
#include "gpb_server.hpp"
#include "pkg.hpp"
// todo: not sure if this is needed
// #include "./proto_files/ups_amazon.pb.h"
// #include "./proto_files/world_ups.pb.h"

using namespace std;

int main() {
    int status = create_database();
    if (status == 0) {
        cout << "from main -- Database created successfully" << endl;
    } else {
        cout << "Database creation failed" << endl;
    }
    gpb_server();
    return 0;
}