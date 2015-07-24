#ifndef FILEPATHS_H_INCLUDED
#define FILEPATHS_H_INCLUDED

#include <include/wxMSWHeaders.h>

namespace cmsPaths
{
    static wxString GetImageDirPath()
    {
        return wxGetCwd() + "\\media\\images\\";
    }

    static wxString GetSoundDirPath()
    {
        return wxGetCwd() + "\\media\\sounds\\";
    }

    static wxString GetErrorLogDirPath()
    {
        return wxGetCwd() + "\\log\\";
    }

    static wxString GetErrorLogFilePath()
    {
        return cmsPaths::GetErrorLogDirPath() + "ErrorLog.log";
    }

    static wxString GetMasterLibraryDirPath()
    {
        // GetDocumentsDir() = "C:\Documents and Settings\username\My Documents"
        return wxStandardPaths::Get().GetDocumentsDir() + "\\";
    }

    static wxString GetDatabaseDirPath()
    {
        // GetUserConfigDir() = "C:\Documents and Settings\username\Application Data"
        return wxStandardPaths::Get().GetUserConfigDir() + "\\cms\\MyMedia\\";
    }

    static wxString GetSettingsFilePath()
    {
        return cmsPaths::GetDatabaseDirPath() + "Settings.db";
    }

    static wxString GetRegistryFilePath()
    {
        return cmsPaths::GetDatabaseDirPath() + "Registry.db";
    }

    static void Init()
    {
        if (!wxFileName::DirExists(cmsPaths::GetErrorLogDirPath() ) )
        {
            wxFileName::Mkdir(cmsPaths::GetErrorLogDirPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        if (!wxFileName::DirExists(cmsPaths::GetMasterLibraryDirPath() ) )
        {
            wxFileName::Mkdir(cmsPaths::GetMasterLibraryDirPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        if (!wxFileName::DirExists(cmsPaths::GetDatabaseDirPath() ) )
        {
            wxFileName::Mkdir(cmsPaths::GetDatabaseDirPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
    }
};

#endif // FILEPATHS_H_INCLUDED
