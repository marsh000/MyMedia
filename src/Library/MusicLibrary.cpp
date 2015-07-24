#include <include/Library/MusicLibrary.h>
#include <include/Custom Controls/Media Players/MusicPlayer.h>
#include <include/Device/DeviceBase.h>
#include <include/ErrorLog.h>
#include <include/FilePaths.h>
#include <include/Media/Song.h>
#include <include/Helper.h>

#include "fileref.h"
#include "tag.h"

CMusicLibrary::CMusicLibrary(wxWindow* pParent, wxWindowID ID, cmsDatabase* pDatabase, CDevice* pDevice, CMusicPlayer* pPlayer)
: CLibrary(pParent, ID), m_pMusicPlayerCtrl(pPlayer)
{
    wxASSERT_MSG(pDevice, "pDevice must point to a valid Device");

    m_LibraryName = "Music";
    m_pDatabase = pDatabase;
    m_pDevice = pDevice;
    m_MediaType = MEDIATYPE_MUSIC;

    // set library path
    wxString Path = pDevice->GetDeviceBasePath();
    if (pDevice->GetDeviceType() == DEVTYPE_MASTER)
    {
        Path << "My Music\\MyMedia\\Music Library\\";
    }
    else
    {
        Path << "Music\\";
    }
    this->SetLibraryPath(Path);

    // set up columns
    m_pTreelistCtrl->AppendColumn("Artist / Album / Title", 100, wxALIGN_LEFT);
    m_pTreelistCtrl->AppendColumn("Genre",                  150, wxALIGN_LEFT);
    m_pTreelistCtrl->AppendColumn("Time",                    65, wxALIGN_RIGHT);
    m_pTreelistCtrl->AppendColumn("Track",                   65, wxALIGN_RIGHT);
    m_pTreelistCtrl->AppendColumn("Year",                    65, wxALIGN_RIGHT);
    m_pTreelistCtrl->AppendColumn("Type",                    65, wxALIGN_RIGHT);
    m_pTreelistCtrl->AppendColumn("Bit Rate",                80, wxALIGN_RIGHT);
    m_pTreelistCtrl->AppendColumn("Sample Rate",            100, wxALIGN_RIGHT);

    m_pTreelistCtrl->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, &CMusicLibrary::OnTreeListItemChecked, this);
    m_pTreelistCtrl->Bind(wxEVT_COMMAND_TREELIST_ITEM_CONTEXT_MENU, &CMusicLibrary::OnTreeListItemRightClick, this);
}

CMusicLibrary::~CMusicLibrary()
{

}

bool CMusicLibrary::InitDatabase()
{
    if (this->GetDevice() == NULL)
        return false;

    wxString SqlStr = wxEmptyString;
    SqlStr << "CREATE TABLE IF NOT EXISTS main.'" << m_pDevice->GetDeviceName() << "' "
           << "("
           << "col_Artist TEXT, "
           << "col_Album TEXT, "
           << "col_Title TEXT, "
           << "col_Genre TEXT, "
           << "col_Time TEXT, "
           << "col_Track INT, "
           << "col_Year INT, "
           << "col_Type TEXT, "
           << "col_BitRate INT, "
           << "col_SampleRate INT, "
           << "col_Channels INT, "
           << "col_Comment TEXT, "
           << "col_FullPath TEXT"
           << ")";
    if (m_pDatabase->ProcessStatement(SqlStr) )
    {
/*//////////////////////////////////////////////////////////////////////////
        wxArrayString Files;
        size_t NumFiles = 0;
        NumFiles += wxDir::GetAllFiles(this->GetLibraryPath(), &Files, "*.m4a");
        NumFiles += wxDir::GetAllFiles(this->GetLibraryPath(), &Files, "*.m4p");
        NumFiles += wxDir::GetAllFiles(this->GetLibraryPath(), &Files, "*.aac");
        NumFiles += wxDir::GetAllFiles(this->GetLibraryPath(), &Files, "*.mp3");
        for (size_t x = 0; x < NumFiles; x++)
        {
            CSong Song(Files[x] );
            wxString str;
            str << "INSERT INTO main." << m_pDevice->GetDeviceName()
                << " (col_Artist,col_Album,col_Title,col_Genre,"
                << "col_Time,col_Track,col_Year,col_Type,col_BitRate,col_SampleRate,"
                << "col_Channels,col_Comment,col_FullPath) VALUES ("
                << "'" << cms::sqlEscapeQuotes(Song.GetArtist() ) << "',"
                << "'" << cms::sqlEscapeQuotes(Song.GetAlbum() ) << "',"
                << "'" << cms::sqlEscapeQuotes(Song.GetTitle() ) << "',"
                << "'" << cms::sqlEscapeQuotes(Song.GetGenre() ) << "',"
                << "'" << Song.GetTimeString() << "',"
                << "'" << Song.GetTrack() << "',"
                << "'" << Song.GetYear() << "',"
                << "'" << Song.GetType() << "',"
                << "'" << Song.GetBitRate() << "',"
                << "'" << Song.GetSampleRate() << "',"
                << "'" << Song.GetChannels() << "',"
                << "'" << cms::sqlEscapeQuotes(Song.GetComment() ) << "',"
                << "'" << cms::sqlEscapeQuotes(Song.GetFullPath() ) << "')";
            if (!m_pDatabase->ProcessStatement(str) )
            {
                wxString msg;
                msg << x << " of " << NumFiles << " Songs copied";
                cmsLogError(msg);
                return false;
            }
        }
        wxString msg;
        msg << NumFiles << " Songs copied";
        cmsLogError(msg);
*///////////////////////////////////////////////////////////////////////////
        return true;
    }
    return false;
}

bool CMusicLibrary::LoadLibraryFromDatabase()
{
    wxString SqlStr = wxString::Format("SELECT ROWID,* FROM main.'%s'", m_pDevice->GetDeviceName() );
    cmsSQLQueryId* pQueryId = NULL;

    if (m_pDatabase->BeginQuery(SqlStr, &pQueryId) )
    {
        int RetVal = m_pDatabase->GetNextRowFromQuery(pQueryId);
        while (RetVal == SQLITE_ROW)
        {
            CSong Song;
            this->GetSongFromQuery(pQueryId, &Song);
            this->InsertMediaInList(&Song);
            RetVal = m_pDatabase->GetNextRowFromQuery(pQueryId);
        }
        m_pDatabase->EndQuery(&pQueryId);
        if (RetVal != SQLITE_DONE)
        {
            return false;
        }

        // expand all items
        wxTreeListItem Artist = m_pTreelistCtrl->GetFirstChild(m_pTreelistCtrl->GetRootItem() );
        while (Artist.IsOk() )
        {
            m_pTreelistCtrl->Expand(Artist);
            wxTreeListItem Album = m_pTreelistCtrl->GetFirstChild(Artist);
            while (Album.IsOk() )
            {
                m_pTreelistCtrl->Expand(Album);
                Album = m_pTreelistCtrl->GetNextSibling(Album);
            }
            Artist = m_pTreelistCtrl->GetNextSibling(Artist);
        }
        return true;
    }
    SqlStr << "\n\n" << m_pDatabase->GetLastErrorMsg();
    cmsLogError(SqlStr);
    return false;
}

bool CMusicLibrary::InsertMediaInList(CMedia* pMedia)
{
    CSong* pThisSong = static_cast<CSong*>(pMedia);

    wxTreeListItem ArtistItem,
                   AlbumItem,
                   TitleItem,
                   ThisItem,
                   LastArtistItem,
                   LastAlbumItem,
                   LastTitleItem;

    // artist
    ThisItem = m_pTreelistCtrl->GetFirstChild(m_pTreelistCtrl->GetRootItem() );
    if (!ThisItem.IsOk() )
    {
        ArtistItem = m_pTreelistCtrl->AppendItem(m_pTreelistCtrl->GetRootItem(), pThisSong->GetArtist() );
    }
    else
    {
        while (ThisItem.IsOk() )
        {
            int comp = pThisSong->GetArtist().CmpNoCase(m_pTreelistCtrl->GetItemText(ThisItem, 0) );
            if (comp > 0) // keep going
            {
                LastArtistItem = ThisItem;
                ThisItem = m_pTreelistCtrl->GetNextSibling(LastArtistItem);
                if (!ThisItem.IsOk() )
                {
                    ArtistItem = m_pTreelistCtrl->AppendItem(m_pTreelistCtrl->GetRootItem(), pThisSong->GetArtist() );
                    break;
                }
            }
            else if (comp < 0)  // prepend
            {
                if (LastArtistItem.IsOk() )
                    ArtistItem = m_pTreelistCtrl->InsertItem(m_pTreelistCtrl->GetRootItem(), LastArtistItem, pThisSong->GetArtist() );
                else
                    ArtistItem = m_pTreelistCtrl->PrependItem(m_pTreelistCtrl->GetRootItem(), pThisSong->GetArtist() );
                break;
            }
            else if (comp == 0) // append here
            {
                ArtistItem = ThisItem;
                break;
            }
        }
    }

    // album
    ThisItem = m_pTreelistCtrl->GetFirstChild(ArtistItem);
    if (!ThisItem.IsOk() )
    {
        AlbumItem = m_pTreelistCtrl->AppendItem(ArtistItem, pThisSong->GetAlbum() );
    }
    else
    {
        while (ThisItem.IsOk() )
        {
            int comp = pThisSong->GetAlbum().CmpNoCase(m_pTreelistCtrl->GetItemText(ThisItem, 0) );
            if (comp > 0)  // append
            {
                LastAlbumItem = ThisItem;
                ThisItem = m_pTreelistCtrl->GetNextSibling(LastAlbumItem);
                if (!ThisItem.IsOk() )
                {
                    AlbumItem = m_pTreelistCtrl->AppendItem(ArtistItem, pThisSong->GetAlbum() );
                    break;
                }
            }
            else if (comp < 0)  // prepend
            {
                if (LastAlbumItem.IsOk() )
                    AlbumItem = m_pTreelistCtrl->InsertItem(ArtistItem, LastAlbumItem, pThisSong->GetAlbum() );
                else
                    AlbumItem = m_pTreelistCtrl->PrependItem(ArtistItem, pThisSong->GetAlbum() );
                break;
            }
            else if (comp == 0) // append here
            {
                AlbumItem = ThisItem;
                break;
            }
        }
    }

    // track
    ThisItem = m_pTreelistCtrl->GetFirstChild(AlbumItem);
    if (!ThisItem.IsOk() )
    {
        TitleItem = m_pTreelistCtrl->AppendItem(AlbumItem, pThisSong->GetTitle() );
    }
    else
    {
        while (ThisItem.IsOk() )
        {
            wxString temp = m_pTreelistCtrl->GetItemText(ThisItem, TRACK_COLUMN_NUM);
            long CurrentTrack = -1;
            temp.ToLong(&CurrentTrack);
            int ThisTrack = pThisSong->GetTrack();
            if (ThisTrack > CurrentTrack)  // append
            {
                LastTitleItem = ThisItem;
                ThisItem = m_pTreelistCtrl->GetNextSibling(LastTitleItem);
                if (!ThisItem.IsOk() )
                {
                    TitleItem = m_pTreelistCtrl->AppendItem(AlbumItem, pThisSong->GetTitle() );
                    break;
                }
            }
            else if (ThisTrack < CurrentTrack)  // prepend
            {
                if (LastTitleItem.IsOk() )
                    TitleItem = m_pTreelistCtrl->InsertItem(AlbumItem, LastTitleItem, pThisSong->GetTitle() );
                else
                    TitleItem = m_pTreelistCtrl->PrependItem(AlbumItem, pThisSong->GetTitle() );
                break;
            }
            else if (ThisTrack == CurrentTrack) // duplicate item
            {
                return true;
            }
        }
    }

    // song info
    m_pTreelistCtrl->SetItemText(TitleItem, GENRE_COLUMN_NUM, pThisSong->GetGenre() );
    m_pTreelistCtrl->SetItemText(TitleItem, TIME_COLUMN_NUM, pThisSong->GetTimeString() );
    m_pTreelistCtrl->SetItemText(TitleItem, TRACK_COLUMN_NUM, wxString::Format("%d", pThisSong->GetTrack() ) );
    m_pTreelistCtrl->SetItemText(TitleItem, YEAR_COLUMN_NUM, wxString::Format("%d", pThisSong->GetYear() ) );
    m_pTreelistCtrl->SetItemText(TitleItem, TYPE_COLUMN_NUM, pThisSong->GetType() );
    m_pTreelistCtrl->SetItemText(TitleItem, BITRATE_COLUMN_NUM, wxString::Format("%d", pThisSong->GetBitRate() ) );
    m_pTreelistCtrl->SetItemText(TitleItem, SAMPLERATE_COLUMN_NUM, wxString::Format("%d", pThisSong->GetSampleRate() ) );
    CListItemData* pData = new CListItemData(pThisSong->GetID() );
    m_pTreelistCtrl->SetItemData(TitleItem, pData);
    m_ItemCount++;

    return true;
}

bool CMusicLibrary::DeleteMedia(CMedia* pMedia)
{

    return true;
}

bool CMusicLibrary::GetMedia(int Id, CMedia* pMedia)
{
    wxString SqlStr = wxString::Format("SELECT ROWID,* FROM main.%s WHERE ROWID=%d", m_pDevice->GetDeviceName(), Id);
    cmsSQLQueryId* pQueryId = NULL;
    bool RetVal = false;
    if (m_pDatabase->BeginQuery(SqlStr, &pQueryId) )
    {
        if (m_pDatabase->GetNextRowFromQuery(pQueryId) == SQLITE_ROW)
        {
            this->GetSongFromQuery(pQueryId, static_cast<CSong*>(pMedia) );
            RetVal = true;
        }
    }
    m_pDatabase->EndQuery(&pQueryId);
    return RetVal;
}

bool CMusicLibrary::SaveMedia(CMedia* pMedia)
{
    CSong* pSong = static_cast<CSong*>(pMedia);
    long ID = pSong->GetID();

    // save the song to the device
    wxString Path = this->GetLibraryPath();
    bool OK = true;
    OK = OK | m_pDevice->AddFolder(cms::ReplaceIllegalPathCharsWithUnderscore(pSong->GetArtist() ), Path);
    OK = OK | m_pDevice->AddFolder(cms::ReplaceIllegalPathCharsWithUnderscore(pSong->GetAlbum() ), Path);
    Path << cms::ReplaceIllegalPathCharsWithUnderscore(pSong->GetTitle() ) << ".";
    Path << CMedia::GetFileTypeStr(pSong->GetFullPath() );
    if (!OK)
    {
        return false;
    }
    if (!m_pDevice->SaveMedia(pSong, Path) )
    {
        return false;
    }

    // add song to library database
    wxString str;
    if (pSong->GetID() < 0)
    {
        str << "INSERT INTO main." << m_pDevice->GetDeviceName()
            << " (col_Artist,col_Album,col_Title,col_Genre,"
            << "col_Time,col_Track,col_Year,col_Type,col_BitRate,col_SampleRate,"
            << "col_Channels,col_Comment,col_FullPath) VALUES ("
            << "'" << cms::sqlEscapeQuotes(pSong->GetArtist() ) << "',"
            << "'" << cms::sqlEscapeQuotes(pSong->GetAlbum() ) << "',"
            << "'" << cms::sqlEscapeQuotes(pSong->GetTitle() ) << "',"
            << "'" << cms::sqlEscapeQuotes(pSong->GetGenre() ) << "',"
            << "'" << pSong->GetTimeString() << "',"
            << "'" << pSong->GetTrack() << "',"
            << "'" << pSong->GetYear() << "',"
            << "'" << pSong->GetType() << "',"
            << "'" << pSong->GetBitRate() << "',"
            << "'" << pSong->GetSampleRate() << "',"
            << "'" << pSong->GetChannels() << "',"
            << "'" << cms::sqlEscapeQuotes(pSong->GetComment() ) << "',"
            << "'" << cms::sqlEscapeQuotes(pSong->GetFullPath() ) << "')";
    }
    else
    {
        str << "UPDATE main." << m_pDevice->GetDeviceName() << " SET "
            << "col_Artist=" << cms::sqlEscapeQuotes(pSong->GetArtist() ) << ","
            << "col_Album=" << cms::sqlEscapeQuotes(pSong->GetAlbum() ) << ","
            << "col_Title=" << cms::sqlEscapeQuotes(pSong->GetTitle() ) << ","
            << "col_Genre=" << cms::sqlEscapeQuotes(pSong->GetGenre() ) << ","
            << "col_Time=" << pSong->GetTimeString() << ","
            << "col_Track=" << pSong->GetTrack() << ","
            << "col_Year=" << pSong->GetYear() << ","
            << "col_Type=" << pSong->GetType() << ","
            << "col_BitRate=" << pSong->GetBitRate() << ","
            << "col_SampleRate=" << pSong->GetSampleRate() << ","
            << "col_Channels=" << pSong->GetChannels() << ","
            << "col_Comment=" << cms::sqlEscapeQuotes(pSong->GetComment() ) << ","
            << "col_FullPath=" << cms::sqlEscapeQuotes(pSong->GetFullPath() )
            << " WHERE ROWID=" << pSong->GetID();
    }
    if (!m_pDatabase->ProcessStatement(str) )
    {
        return false;
    }
    return true;
}

void CMusicLibrary::GetSongFromQuery(cmsSQLQueryId* pQueryId, CSong* pSong)
{
    pSong->SetID(m_pDatabase->GetItemAsInteger(pQueryId, 0) );
    pSong->SetArtist(m_pDatabase->GetItem(pQueryId, 1) );
    pSong->SetAlbum(m_pDatabase->GetItem(pQueryId, 2) );
    pSong->SetTitle(m_pDatabase->GetItem(pQueryId, 3) );
    pSong->SetGenre(m_pDatabase->GetItem(pQueryId, 4) );
    pSong->SetTimeString(m_pDatabase->GetItem(pQueryId, 5) );
    pSong->SetTrack(m_pDatabase->GetItemAsInteger(pQueryId, 6) );
    pSong->SetYear(m_pDatabase->GetItemAsInteger(pQueryId, 7) );
    pSong->SetType(m_pDatabase->GetItem(pQueryId, 8) );
    pSong->SetBitRate(m_pDatabase->GetItemAsInteger(pQueryId, 9) );
    pSong->SetSampleRate(m_pDatabase->GetItemAsInteger(pQueryId, 10) );
    pSong->SetChannels(m_pDatabase->GetItemAsInteger(pQueryId, 11) );
    pSong->SetComment(m_pDatabase->GetItem(pQueryId, 12) );
    pSong->SetFullPath(m_pDatabase->GetItem(pQueryId, 13) );
}

void CMusicLibrary::OnTreeListItemChecked(wxTreeListEvent& Event)
{
    wxTreeListItem Item = Event.GetItem();
    wxCheckBoxState State = m_pTreelistCtrl->GetCheckedState(Item);
    m_pTreelistCtrl->CheckItemRecursively(Item, State);
    m_pTreelistCtrl->UpdateItemParentStateRecursively(Item);
}

void CMusicLibrary::OnTreeListItemRightClick(wxTreeListEvent& Event)
{
    wxTreeListItem ThisItem = Event.GetItem();
    wxClientData* pClientData = m_pTreelistCtrl->GetItemData(ThisItem);
    if (pClientData == NULL)
    {
        return;
    }
    CListItemData* pData = static_cast<CListItemData*>(pClientData);
    CSong Song;
    this->GetMedia(pData->GetId(), &Song);

    wxMenu PopupMenu;
    PopupMenu.Append(idPOPUP_MENU_PLAY, "Play", "Play this song");
    PopupMenu.Append(idPOPUP_MENU_DELETE, "Delete", "Delete this song");

    int Selection = m_pTreelistCtrl->GetPopupMenuSelectionFromUser(PopupMenu);
    switch (Selection)
    {
    case idPOPUP_MENU_PLAY:
        {
            m_pMusicPlayerCtrl->Load(&Song);
        }
        break;

    case idPOPUP_MENU_DELETE:
        {
            if (wxYES == wxMessageBox("This action will permanently remove the selected item.\nAre you sure you want to do this?",
                                      "Remove Item",
                                      wxYES_NO | wxCENTRE | wxICON_EXCLAMATION) )
            {
                if (this->DeleteMedia(&Song) )
                {
                    wxMessageBox("Item deleted!");
                }
            }
        }
        break;

    case wxID_NONE:
    default:
        return;
    }
}
