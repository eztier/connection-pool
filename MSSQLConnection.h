#pragma once

#include <sstream>

#include "ConnectionPool.h"

// Client: https://github.com/eztier/tdspp
#include "tdspp.hh"

using namespace active911;

namespace com::eztier {

  class MSSQLConnection : public Connection {

	public:
		~MSSQLConnection() {
			if(this->sql_connection) {
				_DEBUG("MSSQL Destruct");

				this->sql_connection.reset(); 	// Release and destruct				
			}
		};

		std::shared_ptr<TDSPP> sql_connection;
		int a;
	};

  class MSSQLConnectionFactory : public ConnectionFactory {

	public:
		MSSQLConnectionFactory(std::string server, int port, std::string database, std::string username, std::string password) {
      
			this->server = server;
			this->port = port;
      this->database = database;
      this->username = username;
			this->password = password;
      
		};

		// Any exceptions thrown here should be caught elsewhere
		std::shared_ptr<Connection> create() {
			// Create the connection
			std::shared_ptr<MSSQLConnection>conn(new MSSQLConnection());

      auto cnx = new TDSPP();
      
      // Connect
      stringstream ss;
      ss << server << ":" << port;
      cnx->connect(ss.str(), username, password);
      ss.str("");
      ss.clear();
      ss << "use " << database;
      cnx->execute(ss.str());

      conn->sql_connection=std::shared_ptr<TDSPP>(cnx);

			// return boost::static_pointer_cast<Connection>(conn);
      return std::static_pointer_cast<Connection>(conn);
		};

	private:
		string server;
		string username;
		string password;
    int port;
    string database;
	};

}
