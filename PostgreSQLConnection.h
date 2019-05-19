#pragma once

#include <sstream>

#include "ConnectionPool.h"

// Client: https://github.com/once-ler/libpqmxx
#include "postgres-connection.h"
#include "postgres-exceptions.h"

using namespace active911;

namespace com::eztier {

  class PostgreSQLConnection : public Connection {

	public:
		~PostgreSQLConnection() {
			if(this->sql_connection) {
				_DEBUG("PostgreSQL Destruct");

				this->sql_connection->close();
				this->sql_connection.reset(); 	// Release and destruct				
			}
		};

		std::shared_ptr<db::postgres::Connection> sql_connection;
		int a;
	};

  class PostgreSQLConnectionFactory : public ConnectionFactory {

	public:
		PostgreSQLConnectionFactory(std::string server, int port, std::string database, std::string username, std::string password) {

			this->server = server;
			this->port = port;
      this->database = database;
      this->username = username;
			this->password = password;
      
		};

		// Any exceptions thrown here should be caught elsewhere
		std::shared_ptr<Connection> create() {
			// Create the connection
			std::shared_ptr<PostgreSQLConnection>conn(new PostgreSQLConnection());

      db::postgres::Settings settings{true};
      auto cnx = new db::postgres::Connection(settings);
      
      // Connect
      // postgresql://[user[:password]@][netloc][:port][/dbname][?param1=value1&...]
      stringstream ss;
      ss << "postgresql://" << username << ":" << password << "@" << server << ":" << port << "/" << database;
			cnx->connect(ss.str().c_str());

      conn->sql_connection=std::shared_ptr<db::postgres::Connection>(cnx);

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
