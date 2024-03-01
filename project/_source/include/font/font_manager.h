#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <string>
#include <map>
#include <irrlicht.h>

namespace irr
{
namespace gui
{
    class IGUIFont;
};
};
class CGUITTFace;

class FontManager
{
public:
    FontManager();
    ~FontManager();
    irr::gui::IGUIFont* GetTtFont(irr::video::IVideoDriver* driver, irr::io::IFileSystem *fs, const char* filename_, unsigned int size_, bool antiAlias_=true, bool transparency_=true);

protected:
    std::string MakeFontIdentifier(const char* filename_, unsigned int size_, bool antiAlias_, bool transparency_);

private:
    typedef std::map<std::string, CGUITTFace*> FaceMap;
    FaceMap mFaceMap;

    typedef std::map<std::string, irr::gui::IGUIFont*> FontMap;
    FontMap mFontMap;
};

#endif // FONT_MANAGER_H
