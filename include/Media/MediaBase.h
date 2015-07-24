#ifndef MEDIABASE_H_INCLUDED
#define MEDIABASE_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <tbytevectorstream.h>

enum cmsMediaType
{
    MEDIATYPE_UNKNOWN  = 0x0000,
    MEDIATYPE_MUSIC    = 0x0001,
    MEDIATYPE_PICTURE  = 0x0002,
    MEDIATYPE_VIDEO    = 0x0004
};

enum cmsFileType
{
    FILETYPE_UNKNOWN   = 0x0000,
    FILETYPE_MP3       = 0x0001,
    FILETYPE_M4A       = 0x0002
};

class CMedia
{
public:
    CMedia() : m_ID(-1) { }
    virtual ~CMedia() { }

    long GetID() const                           { return m_ID; }
    wxString GetFullPath() const                 { return m_FullPath; }

    void SetID(long ID)                          { m_ID = ID; }
    void SetFullPath(const wxString& Path)       { m_FullPath = Path; }

    static cmsFileType GetFileType(const wxString& Path);
    static wxString GetFileTypeStr(const wxString& Path);

    // pure virtual methods
    virtual cmsMediaType GetMediaType() const = 0;
    virtual wxString GetExtPathFromMetadata() = 0;
    virtual wxString GetFullFilenameFromMetadata() = 0;
    virtual bool GetMetaDataFromFile(const wxString&) = 0;
    virtual bool GetMetaDataFromStream(const wxString&, const wxString&, IStream*, unsigned long) = 0;

private:
    long                     m_ID;
    wxString                 m_FullPath;
};

#endif // MEDIABASE_H_INCLUDED
