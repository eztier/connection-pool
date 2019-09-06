#pragma once

#include <sstream>

#include "ConnectionPool.h"

// Client: https://github.com/eztier/tds-client
#include "tds-client.h"

using namespace active911;

namespace com::eztier {

  class MSSQLDbLibConnection : public Connection {

	public:
		~MSSQLDbLibConnection() {
			if(this->sql_connection) {
				_DEBUG("MSSQL dblib Destruct");

				this->sql_connection.reset(); 	// Release and destruct				
			}
		};

		std::shared_ptr<tds::TDSClient> sql_connection;
		int a;
	};

  class MSSQLDbLibConnectionFactory : public ConnectionFactory {

	public:
		MSSQLDbLibConnectionFactory(std::string server, int port, std::string database, std::string username, std::string password) {
      
			this->server = server;
			this->port = port;
      this->database = database;
      this->username = username;
			this->password = password;
      
		};

		// Any exceptions thrown here should be caught elsewhere
		std::shared_ptr<Connection> create() {
			// Create the connection
			std::shared_ptr<MSSQLDbLibConnection>conn(new MSSQLDbLibConnection());

			tds::TDSClient* cnx;

      try {
				cnx = new tds::TDSClient();
				
				// Connect
				stringstream ss;
				ss << server << ":" << port;
				cnx->connect(ss.str(), username, password);
				cnx->useDatabase(database);

				conn->sql_connection=std::shared_ptr<tds::TDSClient>(cnx);
			} catch (const tds::TDSException& e) {
				std::cerr << e.what() << endl;
				throw;
			} catch (const std::exception& e) {
				std::cerr << e.what() << endl;
				throw;
			}

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
