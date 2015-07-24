#ifndef MUSICLIBRARY_H_INCLUDED
#define MUSICLIBRARY_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Database.h>
#include <include/Library/LibraryBase.h>
#include <include/Library/LibraryTreeItemData.h>

#define TITLE_COLUMN_NUM 0
#define GENRE_COLUMN_NUM 1
#define TIME_COLUMN_NUM 2
#define TRACK_COLUMN_NUM 3
#define YEAR_COLUMN_NUM 4
#define TYPE_COLUMN_NUM 5
#define BITRATE_COLUMN_NUM 6
#define SAMPLERATE_COLUMN_NUM 7

class CDevice;
class CMusicPlayer;
class CSong;

class CMusicLibrary : public CLibrary
{
public:
    CMusicLibrary(wxWindow* pParent, wxWindowID ID, cmsDatabase* pDatabase, CDevice* pDevice, CMusicPlayer* pPlayer);
    ~CMusicLibrary();
    bool InitDatabase();
    bool LoadLibraryFromDatabase();
    bool InsertMediaInList(CMedia* pMedia);
    bool DeleteMedia(CMedia* pMedia);
    bool GetMedia(int Id, CMedia* pMedia);
    bool SaveMedia(CMedia* pMedia);

protected:
    enum
    {
        idPOPUP_MENU_PLAY,
        idPOPUP_MENU_DELETE,
    };

private:
    void GetSongFromQuery(cmsSQLQueryId* pQueryId, CSong* pSong);
    void OnTreeListItemChecked(wxTreeListEvent& Event);
    void OnTreeListItemRightClick(wxTreeListEvent& Event);

    CMusicPlayer*                 m_pMusicPlayerCtrl;

};

#endif // MUSICLIBRARY_H_INCLUDED
