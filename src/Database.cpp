#include <include/Database.h>

cmsDatabase::cmsDatabase()
: m_pSQLite(NULL), m_IsInTransaction(false), m_CanRollback(false)
{
}

cmsDatabase::~cmsDatabase()
{
    this->CloseDatabase();
}

bool cmsDatabase::IsValidSQLiteDatabaseFile(const wxString& FullPath)
{
    if (FullPath.IsEmpty() )
    {
        return false;
    }
    wxFile File;
    if (File.Open(FullPath) )
    {
        if (File.Length() == 0)
            return true;

        const char* pBuffer = new const char[16];
        size_t NumRead = File.Read( (void*)pBuffer, 16);
        if (NumRead == 16)
        {
            if (strcmp(pBuffer, "SQLite format 3") == 0)
            {
                delete[] pBuffer;
                return true;
            }
        }
        delete[] pBuffer;
    }
    return false;
}

bool cmsDatabase::OpenDatabase(const wxString& FullPath, int Flags)
{
    wxASSERT(m_pSQLite == NULL);
    wxASSERT(!FullPath.IsEmpty() );

    if (SQLITE_OK == sqlite3_open_v2(FullPath, &m_pSQLite, Flags, NULL) )
    {
        m_DatabaseName = "main";
        m_Filename.Assign(FullPath);
        return true;
    }
    this->CloseDatabase();
    m_pSQLite = NULL;
    return false;
}

bool cmsDatabase::IsOk()
{
    return m_pSQLite != NULL;
}

bool cmsDatabase::CloseDatabase()
{
    if (m_pSQLite != NULL)
    {
        if (this->ReleaseAllPendingStatements() )
        {
            if (SQLITE_OK == sqlite3_close_v2(m_pSQLite) )
            {
                m_pSQLite = NULL;
                m_DatabaseName = wxEmptyString;
                m_Filename.Clear();
                return true;
            }
        }
    }
    return false;
}

bool cmsDatabase::ReleaseAllPendingStatements()
{
    if (!m_QueryIdList.empty() )
    {
        wxVector<cmsSQLQueryId*>::iterator iter;
        for (iter = m_QueryIdList.begin(); iter != m_QueryIdList.end(); ++iter)
        {
            this->EndQuery(&(*iter) );
        }
    }
    return true;
}

sqlite3* cmsDatabase::GetDatabaseObject()
{
    return m_pSQLite;
}

int cmsDatabase::GetLastErrorCode()
{
    if (m_pSQLite == NULL)
    {
        return -1;
    }
    return sqlite3_errcode(m_pSQLite);
}

wxString cmsDatabase::GetLastErrorMsg()
{
    if (m_pSQLite == NULL)
    {
        return "No valid database open";
    }
    wxString SqliteErrorMsg = wxEmptyString;
    const int ErrNo = sqlite3_errcode(m_pSQLite);
    if (ErrNo == SQLITE_OK || ErrNo == SQLITE_ROW || ErrNo == SQLITE_DONE)
    {
        SqliteErrorMsg << "Command Successful.\n";
    }
    else
    {
        SqliteErrorMsg << "SQLite Error #" << ErrNo
                       << " (" << sqlite3_errstr(ErrNo)
                       << ") : \n" << sqlite3_errmsg(m_pSQLite) << "\n";
    }
    return SqliteErrorMsg;
}

wxString cmsDatabase::GetDatabaseNickname()
{
    return wxString::Format("%s_%s", m_Filename.GetName(), m_Filename.GetExt() );
}

wxString cmsDatabase::GetDatabaseName()
{
    return m_DatabaseName;
}

wxString cmsDatabase::GetDatabaseFullPath()
{
    return m_Filename.GetFullPath();
}

bool cmsDatabase::GetTableNames(wxArrayString& TableNames)
{
    if (m_pSQLite != NULL)
    {
        TableNames.Empty();
        wxString Query = wxString::Format("SELECT name FROM '%s'.sqlite_master WHERE type='table' ORDER BY 1", m_DatabaseName);
        sqlite3_stmt* pQueryId = NULL;
        if (this->BeginQuery(Query, &pQueryId) )
        {
            while (SQLITE_ROW == this->GetNextRowFromQuery(pQueryId) )
            {
                TableNames.Add(this->GetItem(pQueryId, 0) );
            }
            this->EndQuery(&pQueryId);
            return true;
        }
    }
    return false;
}

bool cmsDatabase::GetColumnNames(wxArrayString& ColumnNames, const wxString& TableName, const wxString ColumnToDelete)
{
    wxString Query = wxString::Format("PRAGMA table_info('%s')", TableName);
    cmsSQLQueryId* pQueryId = NULL;
    if (this->BeginQuery(Query, &pQueryId) )
    {
        while (SQLITE_ROW == this->GetNextRowFromQuery(pQueryId) )
        {
            wxString Name = this->GetItem(pQueryId, 1);
            if (Name != ColumnToDelete)
                ColumnNames.Add(Name);
        }
        this->EndQuery(&pQueryId);
        return true;
    }
    return false;
}

bool cmsDatabase::IsTransactionStarted()
{
    return m_IsInTransaction;
}

bool cmsDatabase::CanRollbackTransaction()
{
    return m_CanRollback;
}

bool cmsDatabase::BeginTransaction()
{
    wxString Statement = "BEGIN TRANSACTION";
    bool RetVal = this->ProcessStatement(Statement);
    if (RetVal)
    {
        m_IsInTransaction = true;
    }
    return RetVal;
}

bool cmsDatabase::EndTransaction()
{
    wxString Statement = "COMMIT TRANSACTION";
    m_IsInTransaction = false;
    m_CanRollback = false;
    return this->ProcessStatement(Statement);
}

bool cmsDatabase::RollbackTransaction()
{
    wxString Statement = "ROLLBACK TRANSACTION";
    m_IsInTransaction = false;
    m_CanRollback = false;
    return this->ProcessStatement(Statement);
}

bool cmsDatabase::ProcessStatement(const wxString& Statement)
{
    bool RetVal = false;
    if (!Statement.IsEmpty() )
    {
        cmsSQLQueryId* pQueryId = NULL;
        const char* pTail = NULL;
        if (SQLITE_OK == sqlite3_prepare_v2(m_pSQLite, Statement, Statement.Length(), &pQueryId, &pTail) )
        {
            wxString NextStatement = pTail;

            int RetCode = sqlite3_step(pQueryId);
            if (RetCode == SQLITE_OK || RetCode == SQLITE_ROW || RetCode == SQLITE_DONE)
            {
                m_CanRollback = true;
                RetVal = true;
                if (!NextStatement.IsEmpty() )
                {
                    RetVal = this->ProcessStatement(NextStatement);
                }
            }
            sqlite3_finalize(pQueryId);
        }
    }
    else
    {
        RetVal = true;
    }
    return RetVal;
}

bool cmsDatabase::BeginQuery(const wxString& Query, cmsSQLQueryId** ppQueryId)
{
    wxASSERT(*ppQueryId == NULL);

    if (SQLITE_OK == sqlite3_prepare_v2(m_pSQLite, Query, Query.Length(), ppQueryId, NULL) )
    {
        m_QueryIdList.push_back(*ppQueryId);
        return true;
    }
    return false;
}

int cmsDatabase::GetNextRowFromQuery(cmsSQLQueryId* pQueryId)
{
    wxASSERT(pQueryId != NULL);
    return sqlite3_step(pQueryId);
}

void cmsDatabase::EndQuery(cmsSQLQueryId** ppQueryId)
{
    wxVector<cmsSQLQueryId*>::iterator rit;
    for (rit = m_QueryIdList.begin(); rit != m_QueryIdList.end(); ++rit)
    {
        if (*rit == *ppQueryId)
        {
            m_QueryIdList.erase(rit);
            break;
        }
    }
    sqlite3_finalize(*ppQueryId);
    *ppQueryId = NULL;
}

int cmsDatabase::GetNumColumnsFromQuery(cmsSQLQueryId* pQueryId)
{
    wxASSERT(pQueryId != NULL);
    return sqlite3_column_count(pQueryId);
}

wxString cmsDatabase::GetItem(cmsSQLQueryId* pQueryId, int Column)
{
    wxASSERT(pQueryId != NULL);
    return sqlite3_column_text(pQueryId, Column);
}

int cmsDatabase::GetItemAsInteger(cmsSQLQueryId* pQueryId, int Column)
{
    wxASSERT(pQueryId != NULL);
    return sqlite3_column_int(pQueryId, Column);
}

double cmsDatabase::GetItemAsReal(cmsSQLQueryId* pQueryId, int Column)
{
    wxASSERT(pQueryId != NULL);
    return sqlite3_column_double(pQueryId, Column);
}

bool cmsDatabase::GetItemAsBool(cmsSQLQueryId* pQueryId, int Column)
{
    wxASSERT(pQueryId != NULL);
    int val = this->GetItemAsInteger(pQueryId, Column);
    if (val == 0)
        return false;
    else
        return true;
}

wxString cmsDatabase::GetCreateTableStatement(const wxString& TableName)
{
    cmsSQLQueryId* pQueryId = NULL;
    wxString CreateTableStatement = wxEmptyString;
    wxString Query = wxString::Format("select sql from '%s'.sqlite_master where type='table' and name='%s'", m_DatabaseName, TableName);
    if (this->BeginQuery(Query, &pQueryId) )
    {
        if (this->GetNextRowFromQuery(pQueryId) == SQLITE_ROW)
        {
            CreateTableStatement = this->GetItem(pQueryId, 0);
        }
    }
    this->EndQuery(&pQueryId);
    return CreateTableStatement;
}

bool cmsDatabase::RemoveColumnFromStatement(wxString& CreateTableStr, const wxString& ColumnNameToRemove)
{
    wxString NewCreateTableStr;

    // get the start and end positions of the column definitions
    int StartPos = CreateTableStr.Find("(") + 1;
    int EndPos = CreateTableStr.Find(")") - 1;
    int Length = CreateTableStr.Length();
    if (StartPos == wxNOT_FOUND || EndPos == wxNOT_FOUND)
        return false;

    // begin building the new create table statement
    NewCreateTableStr << CreateTableStr.Left(StartPos);

    // Extract all the column definitions
    wxString ColDefs = CreateTableStr.Mid(StartPos, (EndPos - StartPos) + 1);

    // parse the definitions
    wxStringTokenizer Tokenizer(ColDefs, ",", wxTOKEN_RET_DELIMS);
    while (Tokenizer.HasMoreTokens() )
    {
        // get the next definition
        wxString Token = Tokenizer.GetNextToken();
        // extract the column name
        wxStringTokenizer NameTokenizer(Token, " ,", wxTOKEN_STRTOK);
        wxString NameToken = NameTokenizer.GetNextToken();
        if (NameToken != ColumnNameToRemove)
        {
            // need to keep this one
            NewCreateTableStr << Token;
        }
    }

    // make sure there is not trailing comma or spaces
    if (NewCreateTableStr.Right(1) == ",")
    {
        NewCreateTableStr.RemoveLast();
    }
    NewCreateTableStr.Trim();

    // get the rest of the string
    NewCreateTableStr << CreateTableStr.Right(Length - (EndPos + 1) );

    CreateTableStr = NewCreateTableStr;
    return true;
}
