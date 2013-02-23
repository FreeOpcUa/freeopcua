/// @author Alexander Rykovanov 2012
/// @email rykovanov.as@gmail.com
/// @brief Com registry implementation.
/// @license GNU GPL/LGPL
///
/// Distributed under the GNU GPL/LGPL License
/// (See accompanying file LICENSE or copy at 
/// http://www.gnu.org/copyleft/gpl.html)
///
/// $Id:  $
/// $Date: $
/// $Revision: $

#include "sqlite_registry.h"

#include <com/com_errors.h>
#include <com/impl/error.h>

#include <map>
#include <sqlite3.h>
#include <stdexcept>
#include <vector>

namespace
{
  typedef std::map<std::string, std::string> Row;
  typedef std::vector<Row> Rows;

  const char* TABLE_NAME = "class_libraries";
  const char* CREATE_DATABASE_STRUCTURE_SQL = "CREATE TABLE IF NOT EXISTS class_libraries (clsid  STRING PRIMARY KEY, library_path STRING)";

  class ErrorMsg
  {
  public:
    ErrorMsg()
      : Message(NULL)
    {
    }
    
    ~ErrorMsg()
    {
      if (Message != NULL)
      {
        sqlite3_free(Message);
      }
    }

    operator char**()
    {
      return &Message;
    }

    operator std::string() const
    {
      return Message ? Message : std::string();
    }

  private:
    char* Message;
  };
 
  struct StatementResult
  {
    Rows SelectedRows;
    std::string ErrorMessage;
    
    StatementResult()
    {
    }
  };

  class SqliteDatabase
  {
  public:
    explicit SqliteDatabase(const std::string& databasePath)
      : DbConnection(OpenDatabase(databasePath))
    {
    }

    explicit SqliteDatabase()
    {
    }

    ~SqliteDatabase()
    {
      CloseDatabase();
    }

   
    Rows Query(const std::string& sql) const
    {
      StatementResult result;
      ErrorMsg errorMsg;
      const int sqliteError = sqlite3_exec(DbConnection, sql.c_str(), Callback, &result, static_cast<char**>(errorMsg));
      if (sqliteError == SQLITE_OK)
      {
        return result.SelectedRows;
      }

      if (sqliteError == SQLITE_ABORT)
      {
        throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, std::string("Performing of query '") + sql + std::string("' was aborted. SQLite error: ") + static_cast<std::string>(errorMsg));
      }

      throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, std::string("query '") + sql + std::string("' failed. SQLite error: ") + static_cast<std::string>(errorMsg));
    }

    void Execute(const std::string& sql)    
    {
      Execute(DbConnection, sql);
    }


  private:
    sqlite3* OpenDatabase(const std::string& path)
    {
      sqlite3* db = NULL;
      const int sqliteError = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
      if (sqliteError == SQLITE_OK)
      {
        Execute(db, CREATE_DATABASE_STRUCTURE_SQL);
        return db;
      }
      throw Com::Impl::Error(Com::COM_UNABLE_OPEN_REGISTRY, std::string("Unable to open database '") + path + std::string("'"));
    }

    void Execute(sqlite3* db, const std::string& sql) const
    {
      ErrorMsg errorMsg;
      const int sqliteError = sqlite3_exec(db, sql.c_str(), NULL, NULL, static_cast<char**>(errorMsg));
      if (sqliteError != SQLITE_OK)
      {
        throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, std::string("query '") + sql + std::string("' failed. SQLite error: ") + static_cast<std::string>(errorMsg));
      }
    }

    void CloseDatabase() throw()
    { 
      int error = sqlite3_close(DbConnection);
    }  

    static int Callback(void* param, int colsCount, char** values, char** colNames)
    {
      StatementResult* result = reinterpret_cast<StatementResult*>(param);
      try
      {
        Row row;
        for (int i = 0; i < colsCount; ++i)
        {
          row.insert(std::make_pair(colNames[i], values[i]));
        }
        result->SelectedRows.push_back(row);
        return 0;
      }
      catch (const std::exception& exc)
      {
        result->ErrorMessage = exc.what();
      }
      return -1;
    }

  private:
    sqlite3* DbConnection;
  };

  
  class SqliteRegistry : public Com::ClassRegistry
  {
  public:
    SqliteRegistry(const std::string& databasePath)
      : Database(databasePath)
    {
    }

    virtual std::string GetLibraryPath(const std::string& clsId) const;
    virtual void RegisterClass(const std::string& clsid, const std::string& libraryPath);
    virtual void UnregisterClass(const std::string& clsid);

  private:
    SqliteDatabase Database;
  };

  std::string SqliteRegistry::GetLibraryPath(const std::string& clsId) const
  {
    if (clsId.empty())
    {
      throw Com::Impl::Error(Com::COM_INVALID_PARAMETER1, "cls id empty.");
    }

    std::string sql = std::string("select * from class_libraries where clsid='") + clsId + std::string("'");
    const Rows& rows = Database.Query(sql);
    if (rows.empty())
    {
      throw Com::Impl::Error(Com::COM_CLASS_NOT_REGISTERED, std::string("Class with id '") + clsId +"' not registered");
    }
    if (rows.size() != 1)
    {
      throw Com::Impl::Error(Com::COM_CLASS_REGISTERED_MULTIPLE_TIMES, std::string("Class with id '") + clsId + std::string(" registered multiple times."));
    }

    const Row& rowFields = *rows.begin();
    const Row::const_iterator& fieldIt = rowFields.find("library_path");
    if (fieldIt == rowFields.end())
    {
      throw Com::Impl::Error(Com::COM_INTERNAL_ERROR, "Unable to find field 'library_path' in the result.");
    }
    return fieldIt->second;
  }

  void SqliteRegistry::RegisterClass(const std::string& clsId, const std::string& libraryPath)
  {
    if (clsId.empty())
    {
      throw Com::Impl::Error(Com::COM_INVALID_PARAMETER1, "Class id is empty.");
    }

    if (libraryPath.empty())
    {
      throw Com::Impl::Error(Com::COM_INVALID_PARAMETER2, "Library path is empty.");
    }

    const Rows& rows = Database.Query(std::string("select * from class_libraries where clsid='") + clsId + std::string("'"));
    if (!rows.empty())
    {
      throw Com::Impl::Error(Com::COM_CLASS_ALREADY_REGISTERED, std::string("Class with id '") + clsId + std::string(" already registered."));
    }

    Database.Execute(std::string("insert into class_libraries values('") + clsId + "', '" + libraryPath + "')");
  }

  void SqliteRegistry::UnregisterClass(const std::string& clsId)
  {
    if (clsId.empty())
    {
      throw Com::Impl::Error(Com::COM_INVALID_PARAMETER1, "Class id is empty.");
    }

    std::string sql;
    sql += "delete from class_libraries where clsid = '" + clsId + "'"; 
    Database.Execute(sql);
  }

} // namespace

std::unique_ptr<Com::ClassRegistry> Com::CreateSqliteRegistry(const std::string& databasePath)
{
  return std::unique_ptr<ClassRegistry>(new SqliteRegistry(databasePath));
}

