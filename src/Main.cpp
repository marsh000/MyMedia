#include <include/Main.h>
#include <include/Database.h>
#include <include/dnd.h>
#include <include/ErrorLog.h>
#include <include/FilePaths.h>
#include <include/version.h>
#include <include/Custom Controls/Media Players/MusicPlayer.h>
#include <include/Device/VolumeDevice.h>
#include <include/Library/LibraryTreeItemData.h>
#include <include/Library/MusicLibrary.h>
#include <include/Library/PictureLibrary.h>

CMyMediaFrame::CMyMediaFrame()
: wxFrame(NULL, wxID_ANY, wxEmptyString),
  m_hDevNotify(NULL), m_pMusicLibrary(NULL), m_pPictureLibrary(NULL), m_pCurrentLibrary(NULL)
{
//#ifdef DEBUG
//    cmsStartDebugConsole();
//#endif // DEBUG
#ifdef _DEBUG
    this->SetTitle(wxString::Format("%s v %i.%i.%i Revision %i (DEBUG Version)",
                                    cmsPROGRAM_NAME, AutoVersion::MAJOR, AutoVersion::MINOR, AutoVersion::BUILD, AutoVersion::REVISION) );
#else
    this->SetTitle(wxString::Format("%s v %i.%i",
                                    cmsPROGRAM_NAME, AutoVersion::MAJOR, AutoVersion::MINOR) );
#endif

    /* tell wxAuiManager to manage this frame */
    m_AuiManager.SetManagedWindow(this);

	/* set up directory structure */
	cmsPaths::Init();

    /* open media databases */
    bool OK = true;
    wxString ErrMsg;

    m_pMusicDatabase = new cmsDatabase();
    if (!m_pMusicDatabase->OpenDatabase(cmsPaths::GetDatabaseDirPath() + "Music.db") )
    {
        ErrMsg << "Could not open Music database.\nSQL Message: "
               << m_pMusicDatabase->GetLastErrorMsg() << "\n";
        OK = false;
    }

    m_pPictureDatabase = new cmsDatabase();
    if (!m_pPictureDatabase->OpenDatabase(cmsPaths::GetDatabaseDirPath() + "Pictures.db") )
    {
        ErrMsg << "Could not open Picture database.\nSQL Message: "
               << m_pMusicDatabase->GetLastErrorMsg() << "\n";
        OK = false;
    }

    if (OK == false)
    {
        cmsLogError("Could not open one or more media databases!");
        this->Close();
    }

    /* initialize device registry */
    m_DeviceRegistry.Init();

/**
    /* initialize settings
    m_pSettings = new CSettings(this);
    m_pSettings->LoadGlobalSettings();
**/

    /* create menu bar */
    // file menu
    wxMenu* pFileMenu = new wxMenu();
    pFileMenu->Append(idMENU_SETTINGS, "Settings", "Change user settings");
    pFileMenu->AppendSeparator();
    pFileMenu->Append(idMENU_EXIT, "Exit", "Exit this Application");

    // music menu
    wxMenu* pMusicMenu = new wxMenu();
    pMusicMenu->Append(idMENU_SEARCH_MUSIC, "Search", "Search for Music to add to your Library");
    pMusicMenu->AppendSeparator();
    pMusicMenu->Append(idMENU_IMPORT_ITUNES, "Import iTunes library", "Import your iTunes library to MyMusic");

    // help menu
    wxMenu* pHelpMenu = new wxMenu();
    pHelpMenu->Append(idMENU_ABOUT, "About", "About this Application");
    pHelpMenu->Append(idMENU_HELP, "Help", "Help using this Application");

    // create the menu bar
    wxMenuBar* pMenuBar = new wxMenuBar();
    pMenuBar->Append(pFileMenu, "&File");
    pMenuBar->Append(pMusicMenu, "&Music");
    pMenuBar->Append(pHelpMenu, "&Help");

    this->SetMenuBar(pMenuBar);

    /* create the tool bar */
//    m_pToolBar = new wxAuiToolBar(this, wxID_ANY);

    // add buttons
//    m_pToolBar->AddControl(new wxButton(m_pToolBar, idToolbarBtnAddToLib, "Add To User Library...") );
//    m_pToolBar->AddControl(new wxButton(m_pToolBar, idToolbarBtnRemoveFromLib, "Remove From Library...") );
//    m_pToolBar->AddControl(new wxButton(m_pToolBar, idToolbarBtnAddToPlaylist, "Add to Playlist") );

//    m_pToolBar->Realize();

    // add to aui manager
//    m_AuiManager.AddPane(m_pToolBar,
//                         wxAuiPaneInfo().Name("ToolBar")
//                                        .CaptionVisible(false)
//                                        .CloseButton(false)
//                                        .Top() );

    /* set up library wxTree ctrl */
    m_pLibraryTreeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_HIDE_ROOT);
    m_pLibraryTreeCtrl->AddRoot("Root");
    m_MasterLibraryTreeItemId = m_pLibraryTreeCtrl->AppendItem(m_pLibraryTreeCtrl->GetRootItem(), "Library");
    m_pLibraryTreeCtrl->ExpandAll();
    m_pLibraryTreeCtrl->SetDropTarget(new CFileDropTarget(m_pLibraryTreeCtrl) );

    // add wxTree ctrl to the aui manager
    m_AuiManager.AddPane(m_pLibraryTreeCtrl,
                         wxAuiPaneInfo().Name("LibraryTree")
                                        .Caption("Libraries")
                                        .CloseButton(false)
                                        .Left()
                                        .MinSize(250, 100) );

    /* set up debug window */
#ifdef _DEBUG
    cmsInitDebugWindow;
    m_AuiManager.AddPane(cmsDebugWindow,
                         wxAuiPaneInfo().Name("DebugWindow")
                                        .Caption("Debug Messages")
                                        .CloseButton(false)
                                        .Left()
                                        .MinSize(250, 100) );
#endif

    /* set up music player ctrl */
    m_pMusicPlayerCtrl = new CMusicPlayer(this, wxID_ANY);
    if (m_pMusicPlayerCtrl->InitializeDisplay() )
    {
        m_AuiManager.AddPane(m_pMusicPlayerCtrl,
                             wxAuiPaneInfo().Name("MusicPlayer")
                                            .Caption("Audio Controls")
                                            .CloseButton(false)
                                            .Left()
                                            .BestSize(250, 175)
                                            .Fixed() );
    }
    else
    {

    }

    /* create master volume device */
    CVolumeDevice* pMasterDevice = new CVolumeDevice('C', true);
    this->DoDeviceArrived(pMasterDevice);

    /* check for connected devices */
    // volume devices
    wxString VolumeDeviceList;
    int NumVolumeDevices = m_DeviceRegistry.EnumerateVolumeDevices(VolumeDeviceList);
    cmsDebugMessage(wxString::Format("Found %d Volume Devices", NumVolumeDevices) );

    for (int Index = 0; Index <  NumVolumeDevices; Index++)
    {
        CVolumeDevice* pDevice = new CVolumeDevice(VolumeDeviceList[Index] );
        this->DoDeviceArrived(pDevice);
    }
/**
    // pnp devices
    wxArrayString PnPParentDeviceIDs;
    int NumParentDevices = m_pDeviceManager->EnumeratePnPDevices(PnPParentDeviceIDs);

    cmsDbgMsg(wxString::Format("Found %d PnP Parent Devices", NumParentDevices) );

    for (int x = 0; x <  NumParentDevices; x++)
    {
        CDevicePnPParent* pPnPParentDevice = new CDevicePnPParent(PnPParentDeviceIDs[x] );
        pPnPParentDevice->SetDeviceName(m_pDeviceManager->GetFriendlyNameFromPnPDevice(PnPParentDeviceIDs[x].wchar_str() ) );
        pPnPParentDevice->SetManufacturer(m_pDeviceManager->GetManufacturerFromPnPDevice(PnPParentDeviceIDs[x].wchar_str() ) );
        pPnPParentDevice->SetDescription(m_pDeviceManager->GetDescriptionFromPnPDevice(PnPParentDeviceIDs[x].wchar_str() ) );
        if (pPnPParentDevice->GetDeviceName() == wxEmptyString)
        {
            pPnPParentDevice->SetDeviceName(pPnPParentDevice->GetDescription() );
        }
        this->DoDeviceArrived(pPnPParentDevice);

        wxArrayString PnPChildDeviceIDs;
        int NumChildDevices = pPnPParentDevice->EnumerateChildDevices(PnPChildDeviceIDs);
        cmsDbgMsg(wxString::Format("Found %d PnP Child Devices", NumChildDevices) );
        for (int y = 0; y <  NumChildDevices; y++)
        {
            CDevicePnPChild* pPnPChildDevice = new CDevicePnPChild(PnPChildDeviceIDs[y], pPnPParentDevice);
            this->DoDeviceArrived(pPnPChildDevice);
            pPnPParentDevice->AddChild(pPnPChildDevice);
        }
    }
**/
/**
    /* set status bar
    this->CreateStatusBar(2);
    CLibraryBase* pLib = m_pMediaTreeCtrl->GetMasterMusicLibrary();
    if (pLib != NULL)
        this->SetStatusBarMusicCount(pLib->GetItemCount() );
**/
    /* commit changes to the AUI */
    m_AuiManager.Update();

    /* bind event handlers */
    this->Bind(wxEVT_CLOSE_WINDOW, &CMyMediaFrame::OnCloseApp, this);
    this->Bind(wxEVT_TREE_SEL_CHANGED, &CMyMediaFrame::OnTreeItemChanged, this);
///    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMyMediaFrame::OnMenuFileExit, this, idMENU_FILE_EXIT);
///    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMyMediaFrame::OnMenuMusicSearch, this, idMENU_MUSIC_SEARCH);
///    this->Bind(wxEVT_COMMAND_MENU_SELECTED, &CMyMediaFrame::OnMenuMusicImportITunes, this, idMENU_MUSIC_IMPORT_ITUNES);
///    this->Bind(wxEVT_COMMAND_TREELIST_ITEM_ACTIVATED, &CMyMediaFrame::OnListItemDoubleClick, this);
///    this->Bind(wxEVT_COMMAND_TREELIST_ITEM_CONTEXT_MENU, &CMyMediaFrame::OnListItemRightClick, this);
///    this->Bind(wxEVT_COMMAND_TREELIST_ITEM_CHECKED, &CMyMediaFrame::OnListItemChecked, this);

    this->RegisterForDeviceNotifications();
}

CMyMediaFrame::~CMyMediaFrame()
{
    m_AuiManager.UnInit();
}

void CMyMediaFrame::SetStatusBarMusicCount(int x)
{
    wxString Str = wxString::Format("%d songs in Music Library", x);
    this->SetStatusText(Str, 1);
}

bool CMyMediaFrame::DoDeviceArrived(CDevice* pDevice)
{
    // check if device is registered (pnp child devices do not get registered!)
/* NOTE (Developer 1#2#): Not registering pnp children may cause issues if user removes
an sd card from a phone or camera and inserts it into an sd card reader */
    bool IsRegistered = false;
    if (pDevice->GetDeviceType() != DEVTYPE_PNP_CHILD)
    {
        IsRegistered = m_DeviceRegistry.IsRegistered(pDevice);
        if (!IsRegistered)
        {
            // register the device
            if (!m_DeviceRegistry.RegisterDevice(pDevice) )
            {
                cmsLogError("Failed to register device");
            }
        }
    }

    // add device to list of connected devices
    m_DeviceRegistry.AddDevice(&pDevice);

    // add the device to the Treebook page
    wxTreeItemId DeviceId;
    if (pDevice->GetDeviceType() == DEVTYPE_MASTER)
    {
        DeviceId = m_MasterLibraryTreeItemId;
    }
    else
    {
        DeviceId = m_pLibraryTreeCtrl->AppendItem(m_pLibraryTreeCtrl->GetRootItem(), pDevice->GetDeviceName() );
    }
    CLibraryTreeItemData* pData = new CLibraryTreeItemData(pDevice, NULL);
    m_pLibraryTreeCtrl->SetItemData(DeviceId, pData);

    // create the library pages
    wxVector<CLibrary*> vPages;
    if (this->CreateLibraryPages(pDevice, vPages) )
    {
        // create library sqlite tables if needed and populate the library pages
        wxTreeItemId LibId;
        wxVector<CLibrary*>::iterator iter;
        for (iter = vPages.begin(); iter != vPages.end(); ++iter)
        {
            // create library sqlite tables if needed
            CLibrary* pLib = static_cast<CLibrary*>(*iter);
            if (!IsRegistered)
            {
                pLib->InitDatabase();
            }

            // populate the library pages
            pLib->LoadLibraryFromDatabase();

            // add library pages to library tree ctrl and aui manager
            bool ShowIt = (pLib->GetMediaType() == MEDIATYPE_MUSIC &&
                           pDevice->GetDeviceType() == DEVTYPE_MASTER) ? true : false;
            wxString Caption = wxString::Format("%s::%s", pDevice->GetDeviceName(), pLib->GetLibraryName() );
            m_AuiManager.AddPane(pLib,
                                 wxAuiPaneInfo().Name(Caption)
                                                .Caption(Caption)
                                                .CaptionVisible(true)
                                                .CloseButton(false)
                                                .Center()
                                                .Show(ShowIt) );
//                                                .MinSize(250, 175) );
            CLibraryTreeItemData* pData = new CLibraryTreeItemData(pDevice, pLib);
            LibId = m_pLibraryTreeCtrl->AppendItem(DeviceId, pLib->GetLibraryName(), -1, -1, pData);
            if (ShowIt)
            {
                m_pLibraryTreeCtrl->SelectItem(LibId);
            }
        }
    }
    m_pLibraryTreeCtrl->ExpandAll();
    return true;
}

bool CMyMediaFrame::CreateLibraryPages(CDevice* pDevice, wxVector<CLibrary*>& vPages)
{
    CMusicLibrary* pMusicLib = new CMusicLibrary(this, wxID_ANY, m_pMusicDatabase, pDevice, m_pMusicPlayerCtrl);
    vPages.push_back(pMusicLib);

    CPictureLibrary* pPicLib = new CPictureLibrary(this, wxID_ANY, m_pPictureDatabase, pDevice);
    vPages.push_back(pPicLib);

    return true;
}

bool CMyMediaFrame::DoDeviceRemoved(CDevice* pDevice)
{
    // hide library page if current
    if (m_pCurrentLibrary != NULL)
    {
        if (m_pCurrentLibrary->GetDevice() == pDevice)
        {
            m_AuiManager.GetPane(m_pCurrentLibrary).Hide();
            m_pCurrentLibrary = NULL;
        }
    }

    // get library pages associated with this device
    wxTreeItemIdValue DeviceCookie, LibraryCookie;
    wxTreeItemId DeviceItemId = m_pLibraryTreeCtrl->GetFirstChild(m_pLibraryTreeCtrl->GetRootItem(), DeviceCookie);
    while (DeviceItemId.IsOk() )
    {
        CLibraryTreeItemData* pDeviceData = static_cast<CLibraryTreeItemData*>(m_pLibraryTreeCtrl->GetItemData(DeviceItemId) );
        if (pDeviceData->GetDevice() == pDevice)
        {
            // remove the library pages from aui manager and tree ctrl and delete them
            wxTreeItemId LibraryItemId = m_pLibraryTreeCtrl->GetFirstChild(DeviceItemId, LibraryCookie);
            while (LibraryItemId.IsOk() )
            {
                wxTreeItemId NextLibItem = m_pLibraryTreeCtrl->GetNextSibling(LibraryItemId);
                CLibraryTreeItemData* pLibData = static_cast<CLibraryTreeItemData*>(m_pLibraryTreeCtrl->GetItemData(LibraryItemId) );
                CLibrary* pLib = pLibData->GetLibrary();
                m_AuiManager.DetachPane(pLib);
                delete pLib;
                pLib = NULL;
                pLibData->SetLibrary(NULL);
                m_pLibraryTreeCtrl->Delete(LibraryItemId);
                LibraryItemId = NextLibItem;
            }
            // delete the device and remove it from the tree ctrl
            pDevice->CloseDevice();
            delete pDevice;
            pDevice = NULL;
            m_pLibraryTreeCtrl->Delete(DeviceItemId);
            return true;
        }
        DeviceItemId = m_pLibraryTreeCtrl->GetNextSibling(DeviceItemId);
    }
    return false;
}

/**
void CMyMediaFrame::DoImportMedia(std::vector<CSong>& MediaList, CLibraryBase* pDestLib, bool Remove)
{
    this->ShowMessage("Working...");
    m_pMessagePanel->Refresh();
    m_pMessagePanel->Update();

    int NumCopied = 0, NumNotCopied = 0, NumError = 0;
    wxString Str = wxEmptyString;
    std::vector<CSong>::iterator i;
    for (i = MediaList.begin(); i != MediaList.end(); ++i)
    {
        int RetCode = pDestLib->GetDevice()->AddMedia(&(*i) );
        switch (RetCode)
        {
        case cmsSUCCESS:
            pDestLib->AddMediaToLibrary(*i);
            NumCopied++;
            Str << "Imported ";
            Str << i->GetArtist() << " '" << i->GetTitle() << "' (" << i->GetAlbum() << ")";
            Str << " to " << pDestLib->GetLibraryName();
            this->ShowMessage(Str);
            m_pMessagePanel->Refresh();
            m_pMessagePanel->Update();
            break;

        case cmsFILE_EXISTS:
            NumNotCopied++;
            Str << i->GetArtist() << " '" << i->GetTitle() << "' (" << i->GetAlbum() << ")";
            Str << " not imported because it already exists on " << pDestLib->GetLibraryName();
            this->ShowMessage(Str);
            m_pMessagePanel->Refresh();
            m_pMessagePanel->Update();
            break;

        case cmsCREATE_FOLDER_FAIL:
            NumError++;
            Str << "Unable to create file directory for ";
            Str << i->GetArtist() << " '" << i->GetTitle() << "' (" << i->GetAlbum() << ")";
            this->ShowMessage(Str);
            break;

        case cmsFAILED:
            NumError++;
            Str << "Failed to import ";
            Str << i->GetArtist() << " '" << i->GetTitle() << "' (" << i->GetAlbum() << ")";
            this->ShowMessage(Str);
            break;

        default:
            this->ShowMessage("Invalid case value", __LINE__, __FILE__);

        }
    }
    if (NumCopied == 0 && NumNotCopied == 0 && NumError == 0)
    {
        this->ShowMessage("No music in source library to import");
    }
    else
    {
        if (NumCopied > 0)
        {
            this->SetStatusBarMusicCount(m_pMediaTreeCtrl->GetMasterMusicLibrary()->GetItemCount() );
            Str = wxString::Format("%d songs imported", NumCopied);
            this->ShowMessage(Str);
        }
        if (NumNotCopied > 0)
        {
            Str = wxString::Format("%d duplicate songs not imported", NumNotCopied);
            this->ShowMessage(Str);
        }
        if (NumError > 0)
        {
            Str = wxString::Format("%d songs failed to import", NumError);
            this->ShowMessage(Str);
        }
    }
    m_pMessagePanel->AddBlankLine();
}
**/

void CMyMediaFrame::ShowMessage(const wxString& Msg, int LineNumber, const wxString& FileName)
{
/**
    if (m_pMessagePanel)
    {
        m_pMessagePanel->AddMessage(Msg, MsgType, LineNumber, FileName);
    }
#ifdef _DEBUG
    else
    {
        wxString str;
        str << Msg << "\nLine: " << LineNumber << ", File: " << FileName;
        wxMessageBox(str);
    }
#endif
**/
}

/**
CSettings* CMyMediaFrame::GetSettings()
{
    return m_pSettings;
}
**/
WXLRESULT CMyMediaFrame::MSWWindowProc(WXUINT Msg, WXWPARAM wParam, WXLPARAM lParam)
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
{
    PDEV_BROADCAST_HDR pDevice = (PDEV_BROADCAST_HDR)lParam;
    wxString Str;
    if (Msg == WM_DEVICECHANGE)
    {
        switch (wParam)
        {
        case DBT_DEVICEARRIVAL:
            switch (pDevice->dbch_devicetype)
            {
            case DBT_DEVTYP_DEVICEINTERFACE:
//                {
//                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] DBT_DEVTYP_DEVICEINTERFACE");
//                    PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = (PDEV_BROADCAST_DEVICEINTERFACE)pDevice;
//                    cmsDebugMessage(pDevInterface->dbcc_name);
//                    std::vector<CDevice> DeviceList;
//                    int NumDevices = m_pDeviceManager->EnumeratePnPDevices(DeviceList);
//                    for (std::vector<CDevice>::iterator i = DeviceList.begin(); i != DeviceList.end(); ++i)
//                    {
//                        this->DoDeviceArrived(*i);
//                    }
//                }
                break;

            case DBT_DEVTYP_HANDLE:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] DBT_DEVTYP_HANDLE");
                }
                break;

            case DBT_DEVTYP_OEM:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] DBT_DEVTYP_OEM");
                }
                break;

            case DBT_DEVTYP_PORT:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] DBT_DEVTYP_PORT");
                }
                break;

            case DBT_DEVTYP_VOLUME:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] DBT_DEVTYP_VOLUME");
                    PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME)pDevice;
                    if (pDevVolume->dbcv_flags & DBTF_MEDIA)
                    {
                        // change affects media in a drive
                        char i;
                        unsigned long unitmask = pDevVolume->dbcv_unitmask;
                        for (i = 0; i < 26; ++i)
                        {
                            if (unitmask & 0x1)
                                break;
                            unitmask = unitmask >> 1;
                        }
                        i = i + 'A';
                        Str << "Drive " << i << ":\\ New media has arrived";
                        cmsDebugMessage(Str);
                    }
                    else
                    {
                        // change affects physical drive
                        char Index;
                        unsigned long UnitMask = pDevVolume->dbcv_unitmask;
                        for (Index = 0; Index < 26; ++Index)
                        {
                            if (UnitMask & 0x1)
                            {
                                char Drive = Index + 'A';
                                CVolumeDevice* pVolumeDevice = new CVolumeDevice(Drive);
                                if (pVolumeDevice->IsOK() )
                                {
                                    this->DoDeviceArrived(pVolumeDevice);
                                    Str << "Drive " << Drive << ":\\ has arrived";
                                    cmsDebugMessage(Str);
                                }
                                else
                                {
                                    delete pVolumeDevice;
                                }
                            }
                            UnitMask = UnitMask >> 1;
                        }
                    }
                }
                break;

            default:
                {
                    Str = "[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEARRIVAL, [LPARAM] Unknown (";
                    Str << lParam << ")";
                    cmsDebugMessage(Str);
                }
                break;
            }
            break;

        case DBT_DEVICEREMOVECOMPLETE:
            switch (pDevice->dbch_devicetype)
            {
            case DBT_DEVTYP_DEVICEINTERFACE:
//                {
//                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] DBT_DEVTYP_DEVICEINTERFACE");
//                    PDEV_BROADCAST_DEVICEINTERFACE pDevInterface = (PDEV_BROADCAST_DEVICEINTERFACE)pDevice;
//                    cmsDebugMessage(pDevInterface->dbcc_name);
//                    std::vector<CDevice> DeviceList;
//                    int NumDevices = m_pDeviceManager->EnumeratePnPDevices(DeviceList, true);
//                    for (std::vector<CDevice>::iterator i = DeviceList.begin(); i != DeviceList.end(); ++i)
//                    {
//                        this->DoDeviceRemoved(&(*i) );
//                    }
//                }
                break;

            case DBT_DEVTYP_HANDLE:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] DBT_DEVTYP_HANDLE");
                }
                break;

            case DBT_DEVTYP_OEM:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] DBT_DEVTYP_OEM");
                }
                break;

            case DBT_DEVTYP_PORT:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] DBT_DEVTYP_PORT");
                }
                break;

            case DBT_DEVTYP_VOLUME:
                {
                    cmsDebugMessage("[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] DBT_DEVTYP_VOLUME");
                    PDEV_BROADCAST_VOLUME pDevVolume = (PDEV_BROADCAST_VOLUME)pDevice;
                    if (pDevVolume->dbcv_flags & DBTF_MEDIA)
                    {
                        // change affects media in a drive
                        char Index;
                        unsigned long UnitMask = pDevVolume->dbcv_unitmask;
                        for (Index = 0; Index < 26; ++Index)
                        {
                            if (UnitMask & 0x1)
                                break;
                            UnitMask = UnitMask >> 1;
                        }
                        Index = Index + 'A';
                        Str << "Drive " << Index << ":\\ Media has been removed";
                        cmsDebugMessage(Str);
                    }
                    else
                    {
                        // change affects physical drive
                        char Index;
                        unsigned long UnitMask = pDevVolume->dbcv_unitmask;
                        for (Index = 0; Index < 26; ++Index)
                        {
                            if (UnitMask & 0x1)
                            {
                                char d = Index + 'A';
                                wxString VolumeStr = wxEmptyString;
                                VolumeStr << d << ":\\";
                                CDevice* pDevice;
                                if (m_DeviceRegistry.GetDeviceFromVolumeStr(VolumeStr, &pDevice) )
                                {
                                    this->DoDeviceRemoved(pDevice);
                                    Str << "Drive " << d << ":\\ has been removed";
                                    cmsDebugMessage(Str);
                                }
                                else
                                {
                                    cmsDebugMessage("m_pDeviceManager::GetDeviceFromVolume returned False");
                                }
                            }
                            UnitMask = UnitMask >> 1;
                        }
                    }
                }
                break;

            default:
                {
                    Str = "[MSG] WM_DEVICECHANGE, [WPARAM] DBT_DEVICEREMOVECOMPLETE, [LPARAM] Unknown (";
                    Str << lParam << ")";
                    cmsDebugMessage(Str);
                }
                break;
            }
            break;
        }
    }
    return wxFrame::MSWWindowProc(Msg, wParam, lParam);
}

bool CMyMediaFrame::RegisterForDeviceNotifications()
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size       = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid  = GUID_DEVINTERFACE_WPD;
    m_hDevNotify = RegisterDeviceNotification( (HANDLE)this->GetHandle(),
                                              &NotificationFilter,
                                              DEVICE_NOTIFY_WINDOW_HANDLE);
    if (!m_hDevNotify)
    {
       cmsLogError("RegisterDeviceNotification Failed");
       return false;
    }
    return true;
}

void CMyMediaFrame::OnCloseApp(wxCloseEvent& event)
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
{
    // unregister device notifications
    if (m_hDevNotify)
    {
        if (!UnregisterDeviceNotification(m_hDevNotify) )
        {
            cmsLogError("UnregisterDeviceNotification Failed");
        }
    }

    // delete devices
    m_DeviceRegistry.Clear();

///    // save settings and delete
///    if (m_pSettings)
///    {
///        m_pSettings->SaveSettings();
///        delete m_pSettings;
///        m_pSettings = NULL;
///    }

    // close media databases
    if (m_pPictureDatabase != NULL)
    {
        m_pPictureDatabase->CloseDatabase();
        delete m_pPictureDatabase;
        m_pPictureDatabase = NULL;
    }
    if (m_pMusicDatabase != NULL)
    {
        m_pMusicDatabase->CloseDatabase();
        delete m_pMusicDatabase;
        m_pMusicDatabase = NULL;
    }

    // exit the main app
    this->Destroy();
}

void CMyMediaFrame::OnMenuFileSettings(wxCommandEvent& Event)
{

}

void CMyMediaFrame::OnMenuFileExit(wxCommandEvent& Event)
{
    this->Close();
}

void CMyMediaFrame::OnMenuMusicImportITunes(wxCommandEvent& Event)
{
/**
    wxString Str;
    Str << "This option will import your iTunes library to MyMedia.\n"
        << "Only music in the stock iTunes directory will be imported.\n"
        << "If you have a custom directory location set up for your iTunes\n"
        << "library then you will have to specify that location under the\n"
        << "<Search> option in the Music menu.\n\n"
        << "Do you wish to continue?";
    if (wxNO == wxMessageBox(Str, "Import iTunes Library", wxYES_NO | wxICON_QUESTION, this) )
    {
        return;
    }

    Str = wxStandardPaths::Get().GetDocumentsDir() + "\\My Music\\iTunes\\iTunes Media\\Music\\";
    wxDir FileSearcher(Str);
    if (!FileSearcher.IsOpened() )
    {
        this->ShowMessage("iTunes directory does not exist.");
        return;
    }

    bool ClearITunesDir = false;
    Str = "Do you want to clear your iTunes Library?\n";
    Str << "Click <Yes> to remove music from your iTunes library.\n"
        << "Click <No> to just copy the music.";
    if (wxYES == wxMessageBox(Str, "Import iTunes Library", wxYES_NO | wxICON_QUESTION, this) )
    {
        ClearITunesDir = true;
    }

    wxArrayString FileList;
    cmsDirTraverser Traverser(FileList, true);
    FileSearcher.Traverse(Traverser, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

    std::vector<CSong> SongList;
    for (size_t index = 0; index < FileList.Count(); index++)
    {
        CSong Song(FileList[index] );
        SongList.push_back(Song);
    }

    this->DoImportMedia(SongList, m_pMediaTreeCtrl->GetMasterMusicLibrary(), ClearITunesDir);
**/
}

void CMyMediaFrame::OnMenuMusicSearch(wxCommandEvent& Event)
{
/**
    wxArrayString FileList;
    CMediaSearchDlg Dlg(this, FileList);
    int Ret = Dlg.ShowModal();
    if (Ret != wxID_OK)
    {
        return;
    }

    std::vector<CSong> SongList;
    for (size_t index = 0; index < FileList.Count(); index++)
    {
        CSong Song(FileList[index] );
        SongList.push_back(Song);
    }

    this->DoImportMedia(SongList, m_pMediaTreeCtrl->GetMasterMusicLibrary(), false);
**/
}

void CMyMediaFrame::OnMenuHelpAbout(wxCommandEvent& Event)
{

}

void CMyMediaFrame::OnMenuHelpHelp(wxCommandEvent& Event)
{

}

void CMyMediaFrame::OnTreeItemChanged(wxTreeEvent& Event)
{

    // hide current page
    wxTreeItemId TreeItemToHide = Event.GetOldItem();
    if (TreeItemToHide.IsOk() )
    {
        CLibraryTreeItemData* pData = static_cast<CLibraryTreeItemData*>(m_pLibraryTreeCtrl->GetItemData(TreeItemToHide) );
        CLibrary* pLibToHide = pData->GetLibrary();
        wxAuiPaneInfo& PaneToHide = m_AuiManager.GetPane(pLibToHide);
        if (PaneToHide.IsOk() )
        {
            PaneToHide.Hide();
        }
    }

    // show the new page
    wxTreeItemId TreeItemToShow = Event.GetItem();
    if (TreeItemToShow.IsOk() )
    {
        CLibraryTreeItemData* pData = static_cast<CLibraryTreeItemData*>(m_pLibraryTreeCtrl->GetItemData(TreeItemToShow) );
        CLibrary* pLibToShow = pData->GetLibrary();
        wxAuiPaneInfo& PaneToShow = m_AuiManager.GetPane(pLibToShow);
        if (PaneToShow.IsOk() )
        {
            PaneToShow.Show();
        }
    }

    // update the aui manager
    m_AuiManager.Update();

}

/**
void CMyMediaFrame::OnListItemDoubleClick(wxTreeListEvent& Event)
{
    CMusicLibrary* pLib = (CMusicLibrary*)m_pMediaTreeCtrl->GetCurrentPage();
    CSong Song;
    if (pLib->GetMediaFromLibrary(Event.GetItem(), &Song) )
    {
        m_pMediaPlayerCtrl->Load(&Song);
    }
}
**/
/**
void CMyMediaFrame::OnListItemRightClick(wxTreeListEvent& Event)
{
    CMusicLibrary* pSourceLib = (CMusicLibrary*)m_pMediaTreeCtrl->GetCurrentPage();
    wxTreeListItem ClickedItem = Event.GetItem();
    if (pSourceLib->GetItemData(ClickedItem) == NULL)
    {
        return;
    }
    CSong Song;

    wxMenu PopupMenu;
    PopupMenu.Append(idPOPUP_MENU_PLAY, "Play", "Play this song");
    if (pSourceLib->GetCheckedState(ClickedItem) != wxCHK_UNCHECKED)
    {
        if (m_pDeviceManager->GetNumberConnectedDevices() > 0)
        {
            if (pSourceLib->GetDevice()->IsMaster() )
            {
                PopupMenu.Append(idPOPUP_MENU_EXPORT, "Export to Device", "Export checked item(s) to a connected device");
            }
            else
            {
                PopupMenu.Append(idPOPUP_MENU_IMPORT, "Import from Device", "Import checked item(s) to Master Library");
            }
        }
    }
    PopupMenu.Append(idPOPUP_MENU_DELETE, "Delete", "Delete this item");

    int Selection = m_pMediaTreeCtrl->GetPopupMenuSelectionFromUser(PopupMenu);
    switch (Selection)
    {
    case idPOPUP_MENU_PLAY:
        {
            if (pSourceLib->GetMediaFromLibrary(Event.GetItem(), &Song) )
            {
                m_pMediaPlayerCtrl->Load(&Song);
            }
            else
            {
                wxString str = "Could not retrieve item";
                this->ShowMessage(str);
            }
        }
        break;

    case idPOPUP_MENU_EXPORT:
        {
            wxArrayString DeviceList;
            m_pDeviceManager->GetConnectedDeviceNames(DeviceList);
            if (DeviceList.Count() == 0)
                return;
            wxString DeviceDesc = wxGetSingleChoice("Please select the device to export to",
                                                    "Export Media",
                                                    DeviceList,
                                                    0,
                                                    this);
            if (DeviceDesc == wxEmptyString)
            {
                return;
            }

            std::vector<wxTreeListItem> ItemList;
            pSourceLib->GetCheckedItemsRecursively(pSourceLib->GetRootItem(), ItemList);
            if (ItemList.size() > 0)
            {
                CMusicLibrary* pDestLib = (CMusicLibrary*)m_pMediaTreeCtrl->GetPageFromText(DeviceDesc);
                if (pDestLib == NULL)
                {
                    this->ShowMessage("Dest lib returned NULL", __LINE__, __FILE__);
                    return;

                }
                std::vector<CSong> SongList;
                std::vector<wxTreeListItem>::iterator i;
                for (i = ItemList.begin(); i != ItemList.end(); ++i)
                {
                    SongList.push_back(*( (CSong*)pSourceLib->GetItemData(*i) ) );
                }
                this->DoImportMedia(SongList, pDestLib);
            }
        }
        break;

    case idPOPUP_MENU_IMPORT:
        {
            std::vector<wxTreeListItem> ItemList;
            pSourceLib->GetCheckedItemsRecursively(pSourceLib->GetRootItem(), ItemList);
            if (ItemList.size() > 0)
            {
                if (m_pMediaTreeCtrl->GetMasterMusicLibrary() == NULL)
                {
                    this->ShowMessage("Master Music Library not initialized?!", __LINE__, __FILE__);
                    return;
                }
                std::vector<CSong> SongList;
                std::vector<wxTreeListItem>::iterator i;
                for (i = ItemList.begin(); i != ItemList.end(); ++i)
                {
                    SongList.push_back(*( (CSong*)pSourceLib->GetItemData(*i) ) );
                }
                this->DoImportMedia(SongList, m_pMediaTreeCtrl->GetMasterMusicLibrary());
            }
        }
        break;

    case idPOPUP_MENU_DELETE:
        {
            if (wxYES == wxMessageBox("This action will permanently remove the selected item.\nAre you sure you want to do this?",
                                      "Remove Item",
                                      wxYES_NO | wxCENTRE | wxICON_EXCLAMATION) )
            {
                if (pSourceLib->RemoveMediaFromLibrary(Event.GetItem() ) )
                {
                    CLibraryBase* pLib = m_pMediaTreeCtrl->GetMasterMusicLibrary();
                    if (pLib != NULL)
                    {
                        this->SetStatusBarMusicCount(pLib->GetItemCount() );
                    }
                    this->ShowMessage("Item deleted!");
                }
            }
        }
        break;

    case wxID_NONE:
    default:
        return;
    }
}
**/
/**
void CMyMediaFrame::OnListItemChecked(wxTreeListEvent& Event)
{
    CMusicLibrary* pLib = (CMusicLibrary*)m_pMediaTreeCtrl->GetCurrentPage();
    wxTreeListItem Item = Event.GetItem();
    wxCheckBoxState State = pLib->GetCheckedState(Item);
    pLib->CheckItemRecursively(Item, State);
    pLib->UpdateItemParentStateRecursively(Item);
}
**/
