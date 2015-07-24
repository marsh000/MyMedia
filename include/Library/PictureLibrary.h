#ifndef PICTURELIBRARY_H_INCLUDED
#define PICTURELIBRARY_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Library/LibraryBase.h>

class CDevice;

class CPictureLibrary : public CLibrary
{
public:
    CPictureLibrary(wxWindow* pParent, wxWindowID ID, cmsDatabase* pDatabase, CDevice* pDevice);
    ~CPictureLibrary();
    bool InitDatabase();
    bool LoadLibraryFromDatabase();
    bool InsertMediaInList(CMedia* pMedia);
    bool DeleteMedia(CMedia* pMedia);
    bool GetMedia(int Id, CMedia* pMedia);
    bool SaveMedia(CMedia* pMedia);

private:

};

#endif // PICTURELIBRARY_H_INCLUDED
