#ifndef SONG_H_INCLUDED
#define SONG_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Media/MediaBase.h>
#include <include/Time.h>

#include <fileref.h>
#include <tag.h>
#include <tbytevectorstream.h>
#include <mpeg\\mpegfile.h>
#include <mpeg\\id3v2\\id3v2framefactory.h>
#include <mp4\\mp4file.h>

class CSong : public CMedia
{
public:
    CSong();
    CSong(const wxString& Filename);
    ~CSong();

    wxString GetArtist() const              { return m_Artist; }
    wxString GetAlbum() const               { return m_Album; }
    wxString GetTitle() const               { return m_Title; }
    wxString GetGenre() const               { return m_Genre; }
    CTime GetTime() const                   { return m_Time; }
    wxString GetTimeString()                { return m_Time.GetTimeStr(); }
    int GetTrack() const                    { return m_Track; }
    int GetYear() const                     { return m_Year; }
    wxString GetType() const                { return m_Type; }
    int GetBitRate() const                  { return m_BitRate; }
    int GetSampleRate() const               { return m_SampleRate; }
    int GetChannels() const                 { return m_Channels; }
    wxString GetComment() const             { return m_Comment; }
    wxString GetPnPObjectID() const         { return m_PnPObjectID; }

    void SetArtist(const wxString& x)       { m_Artist = x; }
    void SetAlbum(const wxString& x)        { m_Album = x; }
    void SetTitle(const wxString& x)        { m_Title = x; }
    void SetGenre(const wxString& x)        { m_Genre = x; }
    void SetTime(const CTime& x)            { m_Time = x; }
    void SetTimeString(const wxString& x)   { m_Time.SetTimeStr(x); }
    void SetTrack(const int x)              { m_Track = x; }
    void SetYear(const int x)               { m_Year = x; }
    void SetType(const wxString& x)         { m_Type = x; }
    void SetBitRate(const int x)            { m_BitRate = x; }
    void SetSampleRate(const int x)         { m_SampleRate = x; }
    void SetChannels(const int x)           { m_Channels = x; }
    void SetComment(const wxString& x)      { m_Comment = x; }
    void SetPnPObjectID(const wxString& x)  { m_PnPObjectID = x; }

    // inherited virtual methods
    cmsMediaType GetMediaType() const       { return MEDIATYPE_MUSIC; };
    wxString GetExtPathFromMetadata();
    wxString GetFullFilenameFromMetadata();
    bool GetMetaDataFromFile(const wxString& Filename);
    bool GetMetaDataFromStream(const wxString& Filename, const wxString& ObjectID, IStream* pStream, unsigned long TransferSize);

private:
    TagLib::File* GetTagLibFileObject(const wxString* pFilename,
                                      TagLib::ByteVectorStream* pStream,
                                      cmsFileType Type);
    void GetTagLibTags(TagLib::File* pFile);

    wxString            m_Artist;
    wxString            m_Album;
    wxString            m_Title;
    wxString            m_Genre;
    CTime               m_Time;
    int                 m_Track;
    int                 m_Year;
    wxString            m_Type;
    int                 m_BitRate;
    int                 m_SampleRate;
    int                 m_Channels;
    wxString            m_Comment;
    wxString            m_PnPObjectID;
};

#endif // SONG_H_INCLUDED
