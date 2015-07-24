#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include <include/wxMSWHeaders.h>

#include <sqlite3.h>

typedef sqlite3_stmt cmsSQLQueryId;

class cmsDatabase
{
public:
    cmsDatabase();
    ~cmsDatabase();

    static bool IsValidSQLiteDatabaseFile(const wxString& FullPath);
    bool OpenDatabase(const wxString& FullPath, int Flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    bool IsOk();
    bool CloseDatabase();
    bool ReleaseAllPendingStatements();

    sqlite3* GetDatabaseObject();
    int GetLastErrorCode();
    wxString GetLastErrorMsg();
    wxString GetDatabaseNickname();
    wxString GetDatabaseName();
    wxString GetDatabaseFullPath();
    bool GetTableNames(wxArrayString& TableNames);
    bool GetColumnNames(wxArrayString& ColumnNames, const wxString& TableName, const wxString ColumnToDelete = wxEmptyString);

    bool IsTransactionStarted();
    bool CanRollbackTransaction();
    bool BeginTransaction();
    bool EndTransaction();
    bool RollbackTransaction();

    bool ProcessStatement(const wxString& Statement);
    bool BeginQuery(const wxString& Query, cmsSQLQueryId** ppQueryId);
    int GetNextRowFromQuery(cmsSQLQueryId* pQueryId);
    void EndQuery(cmsSQLQueryId** ppQueryId);
    int GetNumColumnsFromQuery(cmsSQLQueryId* pQueryId);
    wxString GetItem(cmsSQLQueryId* pQueryId, int Column);
    int GetItemAsInteger(cmsSQLQueryId* pQueryId, int Column);
    double GetItemAsReal(cmsSQLQueryId* pQueryId, int Column);
    bool GetItemAsBool(cmsSQLQueryId* pQueryId, int Column);

    wxString GetCreateTableStatement(const wxString& TableName);
    bool RemoveColumnFromStatement(wxString& CreateTableStr, const wxString& ColumnName);

private:
    sqlite3*                      m_pSQLite;
    wxFileName                    m_Filename;
    wxString                      m_DatabaseName;
    wxVector<cmsSQLQueryId*>      m_QueryIdList;
    bool                          m_IsInTransaction;
    bool                          m_CanRollback;
};

#endif // DATABASE_H_INCLUDED
