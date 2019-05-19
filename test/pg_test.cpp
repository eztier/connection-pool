/*

g++ -std=c++14 -I ../../ -I /usr/local/include -I /usr/include/postgresql -I ../../../libpqmxx/include -I ../../../json/single_include/nlohmann ../pg_test.cpp -o testing -L /usr/lib/x86_64-linux-gnu -L /usr/local/lib -llibpqmxx -lpq

*/

// Log to stdout
#define _DEBUG(x) 								\
	do { 										\
		std::cout << "  (" << x << ")" << endl;	\
	} while(0)

#include <iostream>
#include <string>
#include <cassert>
#include "../PostgreSQLConnection.h"

using namespace active911;
using namespace std;
using namespace com::eztier;

int main(int argc, char **argv) {

	cout << "Creating connections..." << endl;
	std::shared_ptr<PostgreSQLConnectionFactory>connection_factory(new PostgreSQLConnectionFactory(
    "localhost",
    5432,
    "eventstore",
    "streamer",
    "streamer"
  ));
	std::shared_ptr<ConnectionPool<PostgreSQLConnection> >pool(new ConnectionPool<PostgreSQLConnection>(20, connection_factory));
	ConnectionPoolStats stats=pool->get_stats();
	assert(stats.pool_size == 20);

  cout << "POOL SIZE: " << stats.pool_size << endl;

	{
    bool exception_thrown=false;

    for (int i = 0; i < 10; i++) {
      try {

        std::shared_ptr<PostgreSQLConnection> conn=pool->borrow();

        auto &now = conn->sql_connection->execute("SELECT to_char(current_timestamp, 'YYYY-MM-DD HH:MI:SS.MS')");
        std::cout << now.as<std::string>(0) << std::endl;

        pool->unborrow(conn);

      } catch (std::exception& e) {

        cout << "Exception thrown (intentional) iter: " << i << endl;
        exception_thrown=true;
      }
      
    }
cout << stats.borrowed_size << endl;

  }
}
