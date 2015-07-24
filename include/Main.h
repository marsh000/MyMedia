/***************************************************************
 * Name:      Main.h
 * Purpose:   Defines Application Frame
 * Author:    Raymond Marsh (raymond.marsh@circlemsoftware.com)
 * Created:   2013-06-30
 * Copyright: 2013 - 2015 Raymond Marsh, All Rights Reserved
 * License:   GPL 3.0
 **************************************************************/

/**
    This file is distributed under the terms of the GNU General Public License. To
view a copy of the license see the file 'copying.txt'. This file is part of the
MyMedia Suite. MyMedia is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.
    MyMedia is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with MyMedia.
If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef MAIN_H
#define MAIN_H

#include <include/wxMSWHeaders.h>
#include <include/DeviceRegistry.h>

static const char cmsPROGRAM_NAME[] = "MyMedia";

class cmsDatabase;
class cmsTreebookCtrl;
class cmsTreebookPage;
class CDevice;
class CLibrary;
class CMusicLibrary;
class CPictureLibrary;
class CMusicPlayer;

class CMyMediaFrame : public wxFrame
{
public:
	CMyMediaFrame();
	virtual ~CMyMediaFrame();

    void SetStatusBarMusicCount(int x);
    bool DoDeviceArrived(CDevice* pDevice);
    bool CreateLibraryPages(CDevice* pDevice, wxVector<CLibrary*>& vPages);
    bool DoDeviceRemoved(CDevice* pDevice);
///    void DoImportMedia(std::vector<CSong>& MediaList, CLibraryBase* pDestLib, bool Remove = false);
    void ShowMessage(const wxString& Msg, int LineNumber = -1, const wxString& FileName = wxEmptyString);
///    CSettings* GetSettings();

    WXLRESULT MSWWindowProc(WXUINT Msg, WXWPARAM wParam, WXLPARAM lParam);
    bool RegisterForDeviceNotifications();

protected:
    enum
    {
        idMENU_SETTINGS,
        idMENU_EXIT,
        idMENU_IMPORT_ITUNES,
        idMENU_SEARCH_MUSIC,
        idMENU_ABOUT,
        idMENU_HELP,
        idPOPUP_MENU_PLAY,
        idPOPUP_MENU_EXPORT,
        idPOPUP_MENU_IMPORT,
        idPOPUP_MENU_DELETE
    };

private:
	void OnCloseApp(wxCloseEvent& event);
    void OnMenuFileSettings(wxCommandEvent& Event);
    void OnMenuFileExit(wxCommandEvent& Event);
    void OnMenuMusicImportITunes(wxCommandEvent& Event);
    void OnMenuMusicSearch(wxCommandEvent& Event);
    void OnMenuHelpAbout(wxCommandEvent& Event);
    void OnMenuHelpHelp(wxCommandEvent& Event);
    void OnTreeItemChanged(wxTreeEvent& Event);
///    void OnListItemDoubleClick(wxTreeListEvent& Event);
///    void OnListItemRightClick(wxTreeListEvent& Event);
///    void OnListItemChecked(wxTreeListEvent& Event);

    cmsDatabase*                  m_pMusicDatabase;
    cmsDatabase*                  m_pPictureDatabase;
    wxAuiManager                  m_AuiManager;
    wxTreeCtrl*                   m_pLibraryTreeCtrl;
//    wxAuiToolBar*                 m_pToolBar;
///    CMessagePanel*                m_pMessagePanel;
///    CSettings*                    m_pSettings;
    CDeviceRegistry               m_DeviceRegistry;
    wxTreeItemId                  m_MasterLibraryTreeItemId;
    CLibrary*                     m_pCurrentLibrary;
    CMusicLibrary*                m_pMusicLibrary;
    CPictureLibrary*              m_pPictureLibrary;
    CMusicPlayer*                 m_pMusicPlayerCtrl;
    HDEVNOTIFY                    m_hDevNotify;
};

#endif
