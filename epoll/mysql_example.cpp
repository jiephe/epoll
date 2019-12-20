#include <stdlib.h>
#include <iostream>

#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <vector>

using namespace std;

int main(void)
{
	try 
	{
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "wgd", "#5D&G&fr543eB");
		
		/* Connect to the MySQL test database */
		con->setSchema("mm");
		
		stmt = con->createStatement();
		std::string room_key("test1");
		std::string query_string("select * from room_info where room_key='");
		query_string.append(room_key);
		query_string.append("'");
		res = stmt->executeQuery(query_string.c_str());
		while (res->next()) 
		{
			cout << "\t... MySQL replies: ";
			/* Access column data by alias or column name */
			
			std::string sValue = res->getString("big_value");
			cout << sValue << endl;
			
			cout << "\t... MySQL says it again: ";
			/* Access column data by numeric offset, 1 is the first column */
			cout << res->getString(1) << endl;
		}		
		delete res;
		delete stmt;
		
		sql::PreparedStatement* pstmt = con->prepareStatement("INSERT INTO room_info(room_key, json_value, big_value) VALUES (?, ?, ?)");
		pstmt->setString(1, "test1");
		pstmt->setString(2, "test2");
		std::string str;
		std::vector<char> vec(40960, 'a');
		str.append(&vec[0], vec.size());
		pstmt->setString(3, str.c_str());
		pstmt->executeUpdate();
		delete pstmt;		
		delete con;
	} 
	catch(sql::SQLException &e)
	{
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}

	return 0;
}