#include <include/Media/Song.h>
#include <include/Helper.h>

CSong::CSong()
: CMedia()
{

}

CSong::CSong(const wxString& Filename)
{
    this->GetMetaDataFromFile(Filename);
}

CSong::~CSong()
{

}

wxString CSong::GetExtPathFromMetadata()
{
    wxString Path;
    Path << cms::ReplaceIllegalPathCharsWithUnderscore(m_Artist) << "\\"
         << cms::ReplaceIllegalPathCharsWithUnderscore(m_Album)  << "\\";
    return Path;
}

wxString CSong::GetFullFilenameFromMetadata()
{
    wxString Name;
    Name << cms::ReplaceIllegalPathCharsWithUnderscore(m_Title) << "."
         << cms::ReplaceIllegalPathCharsWithUnderscore(m_Type);
    return Name;
}

bool CSong::GetMetaDataFromFile(const wxString& Filename)
{
    this->SetFullPath(Filename);
    m_Type = CMedia::GetFileTypeStr(Filename);
    TagLib::File* pFile = this->GetTagLibFileObject(&Filename, NULL, CMedia::GetFileType(Filename) );
    if (pFile == NULL)
    {
        return false;
    }
    this->GetTagLibTags(pFile);
    return true;
}

bool CSong::GetMetaDataFromStream(const wxString& Filename, const wxString& ObjectID, IStream* pStream, unsigned long TransferSize)
{
    // transfer the data in the IStream to a memory buffer
    TagLib::ByteVector ByteVector;
    char* Buffer = new char[TransferSize];
    HRESULT hr = S_OK;

    while (hr == S_OK)
    {
        unsigned long NumRead = 0;
        hr = pStream->Read( (void*)Buffer, TransferSize, &NumRead);
        if (NumRead > 0)
        {
            TagLib::ByteVector TempByteVector(Buffer, NumRead);
            ByteVector.append(TempByteVector);
        }
        if (NumRead < TransferSize)
            break;
    }
    if (hr != S_FALSE && hr != S_OK)
    {
        return false;
    }

    // convert the memory buffer to a TagLib::ByteVectorStream
    TagLib::ByteVectorStream FileDataStream(ByteVector);

    // get the filename from the Object ID


    // set the file type and set m_FullPath to the Object ID
    this->SetFullPath(Filename);
    m_Type = CMedia::GetFileTypeStr(Filename);

    // get the tags
    TagLib::File* pFile = this->GetTagLibFileObject(NULL, &FileDataStream, CMedia::GetFileType(Filename) );
    if (pFile == NULL)
        return false;

    this->GetTagLibTags(pFile);
    delete[] Buffer;
    return true;
}

TagLib::File* CSong::GetTagLibFileObject(const wxString* pFilename, TagLib::ByteVectorStream* pStream, cmsFileType Type)
{
    if (Type == FILETYPE_MP3)
    {
        if (pFilename)
            return new TagLib::MPEG::File( (TagLib::FileName)pFilename->fn_str() );
        else
        {
            TagLib::ID3v2::FrameFactory* pFrameFactory = TagLib::ID3v2::FrameFactory::instance();
            return new TagLib::MPEG::File(pStream, pFrameFactory);
        }
    }
    if (Type == FILETYPE_M4A)
    {
        if (pFilename)
            return new TagLib::MP4::File( (TagLib::FileName)pFilename->fn_str() );
        else
            return new TagLib::MP4::File(pStream);
    }
    return NULL;
}

void CSong::GetTagLibTags(TagLib::File* pFile)
{
    TagLib::FileRef Media(pFile);
    if (!Media.isNull() && Media.tag() )
    {
        TagLib::Tag *pTags = Media.tag();
        m_Artist = pTags->artist().toCString();
        m_Album = pTags->album().toCString();
        m_Title = pTags->title().toCString();
        m_Genre = pTags->genre().toCString();
        m_Track = pTags->track();
        m_Year = pTags->year();
        m_Comment = pTags->comment().toCString();
    }
    if (!Media.isNull() && Media.audioProperties() )
    {
        TagLib::AudioProperties *pProperties = Media.audioProperties();
        m_Time = CTime(pProperties->length() );
        m_BitRate = pProperties->bitrate();
        m_SampleRate = pProperties->sampleRate();
        m_Channels = pProperties->channels();
    }
}

