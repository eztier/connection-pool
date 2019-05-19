/*

g++ -std=c++14 -I ../../ -I /usr/local/include -I ../../../tdspp -I ../../../json/single_include/nlohmann ../mssql_test.cpp -o testing -L /usr/lib/x86_64-linux-gnu -L /usr/local/lib -ltds++ -lct

*/

// Log to stdout
#define _DEBUG(x) 								\
	do { 										\
		std::cout << "  (" << x << ")" << endl;	\
	} while(0)

#include <iostream>
#include <string>
#include <cassert>
#include "../MSSQLConnection.h"

using namespace active911;
using namespace std;
using namespace com::eztier;

int main(int argc, char **argv) {

	cout << "Creating connections..." << endl;
	std::shared_ptr<MSSQLConnectionFactory>connection_factory(new MSSQLConnectionFactory(
    "localhost",
    1433,
    "test",
    "admin",
    "12345678"
  ));
	std::shared_ptr<ConnectionPool<MSSQLConnection> >pool(new ConnectionPool<MSSQLConnection>(40, connection_factory));
	ConnectionPoolStats stats=pool->get_stats();
	assert(stats.pool_size == 40);

  cout << "POOL SIZE: " << stats.pool_size << endl;

	{
    bool exception_thrown=false;

    // Iterate 40 times
    for (int i = 0; i < 40; i++) {
      for (int i = 0; i < 100; i++) {
        try {

          std::shared_ptr<MSSQLConnection> conn=pool->borrow();

          Query *q = conn->sql_connection->sql("select convert(varchar(23), current_timestamp, 126) right_now");
          q->execute();

          while (!q->eof()) {
            for (int i=0; i < q->fieldcount; i++) {
              auto fd = q->fields(i);
              cout << fd->to_str() << endl;
            }
            q->next();
          }

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