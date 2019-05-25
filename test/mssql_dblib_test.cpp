/*

g++ -std=c++14 -I ../../ -I /usr/local/include -I ../../../tdspp -I ../../../json/single_include/nlohmann ../mssql_dblib_test.cpp -o testing -L /usr/lib/x86_64-linux-gnu -L /usr/local/lib -ltdsclient -lsybdb

*/

// Log to stdout
#define _DEBUG(x) 								\
	do { 										\
		std::cout << "  (" << x << ")" << endl;	\
	} while(0)

#include <iostream>
#include <string>
#include <cassert>
#include "../MSSQLDbLibConnection.h"

using namespace active911;
using namespace std;
using namespace com::eztier;

int main(int argc, char **argv) {

	cout << "Creating connections..." << endl;
	std::shared_ptr<MSSQLDbLibConnectionFactory>connection_factory(new MSSQLDbLibConnectionFactory(
    "localhost",
    1433,
    "test",
    "admin",
    "12345678"
  ));
	std::shared_ptr<ConnectionPool<MSSQLDbLibConnection> >pool(new ConnectionPool<MSSQLDbLibConnection>(40, connection_factory));
	ConnectionPoolStats stats=pool->get_stats();
	assert(stats.pool_size == 40);

  cout << "POOL SIZE: " << stats.pool_size << endl;

	{
    bool exception_thrown=false;

    // Iterate 40 times
    for (int i = 0; i < 40; i++) {
      for (int i = 0; i < 100; i++) {
        try {

          std::shared_ptr<MSSQLDbLibConnection> conn=pool->borrow();

          int rc = 0;
          vector<string> fieldNames;
          vector<vector<string>> fieldValues;

          auto db = conn->sql_connection;

          db->sql("select convert(varchar(23), current_timestamp, 126) right_now");
          rc = db->execute();

          if (rc) {
            cerr << "Error occurred." << endl;
            break;
          }

          fieldNames = std::move(db->fieldNames);
          fieldValues = std::move(db->fieldValues);

          int idx = 0, cnt = fieldNames.size() - 1;
          for (auto& fld : fieldNames) {
            cout << fld << (idx < cnt ? "\t" : "");
            idx++;
          }
          cout << endl;
          idx = 0;

          for (auto& row : fieldValues) {
            for (const auto& col: row) {
              cout << col << (idx < cnt ? "\t" : "");
            }
            cout << endl;
          }
          cout << endl;

          pool->unborrow(conn);

        } catch (std::exception& e) {

          cout << "Exception thrown (intentional) iter: " << i << endl;
          exception_thrown=true;
        }
        
      }
      cout << stats.borrowed_size << endl;
    }

  }
}
