# Super Fusion
Build order optimizer for StarCraft II

## How to compile

Install Visual Studio Community 2022 from https://visualstudio.microsoft.com/downloads/

Download wxWidgets Windows binaries from https://www.wxwidgets.org/downloads/

- Header files
- Development files
- Release DLLs

Download WinSparkle from https://github.com/vslavik/winsparkle/releases

## How to update version number for new release

* Update CHANGELOG.md
* Update version MDIParent.cpp (for UI)
* Update versions in SCFusiom.rc
    * FILEVERSION
    * PRODUCTVERSION
    * FileVersion
    * ProductVersion
* Update Installer
    * OutputFilename
    * ProductVersion (this will generate new ProductCode and PackageCode)
* Publish updated appcast.xml and release notes
