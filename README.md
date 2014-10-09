3DGraphics
==========

Repository for VGP 240 - 3D Graphics and Applications.




Build Instructions
------------------

###### Requires Visual Studio 2012

1. Open the Visual Studio 2012 solution file in "project/CMGfx.sln"
2. Build the solution within Visual Studio
3. Run the application within the editor (F5), or launch the executable in "bin/<build configuration>/CMGfx.exe"




Directory Structure
-------------------

* bin - Intermediate directory
  * Configuration(s) - Output files for selected build configuration
* build - Output directory
  * Configuration - Output files for selected build configuration
* project - Visual Studio 2012 solution file
  * CMGfx - Visual Studio 2012 project files
* scripts - Shader scripts
* setup - Microsoft .rc files and MFC
  * res - Resource files
* source - Project source files (C++)




MICROSOFT FOUNDATION CLASS LIBRARY : CMGfx Project Overview
-----------------------------------------------------------

The application wizard has created this CMGfx application for 
you.  This application not only demonstrates the basics of using the Microsoft 
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your CMGfx application.

CMGfx.vcproj
    This is the main project file for VC++ projects generated using an application wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    application wizard.

CMGfx.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CCMGfxApp application class.

CMGfx.cpp
    This is the main application source file that contains the application
    class CCMGfxApp.

CMGfx.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\CMGfx.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file CMGfx.rc.

res\CMGfx.rc2
    This file contains resources that are not edited by Microsoft 
    Visual C++. You should place all resources not editable by
    the resource editor in this file.

CMGfx.reg
    This is an example .reg file that shows you the kind of registration
    settings the framework will set for you.  You can use this as a .reg
    file to go along with your application or just delete it and rely
    on the default RegisterShellFileTypes registration.


/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CMDIFrameWnd and controls all MDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in CMGfx.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

For the child frame window:

ChildFrm.h, ChildFrm.cpp
    These files define and implement the CChildFrame class, which
    supports the child windows in an MDI application.

/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

CMGfxDoc.h, CMGfxDoc.cpp - the document
    These files contain your CCMGfxDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CCMGfxDoc::Serialize).
    The Document will have the following strings:
        File extension:      txt
        File type ID:        CMGfx.Document
        Main frame caption:  CMGfx
        Doc type name:       CMGfx
        Filter name:         CMGfx Files (*.txt)
        File new short name: CMGfx
        File type long name: CMGfx.Document

CMGfxView.h, CMGfxView.cpp - the view of the document
    These files contain your CCMGfxView class.
    CCMGfxView objects are used to view CCMGfxDoc objects.

res\CMGfxDoc.ico
    This is an icon file, which is used as the icon for MDI child windows
    for the CCMGfxDoc class.  This icon is included by the main
    resource file CMGfx.rc.


/////////////////////////////////////////////////////////////////////////////

The application wizard has also created classes specific to OLE

CntrItem.h, CntrItem.cpp
    These files contain your CCMGfxCntrItem class. This class is used to 
    manipulate OLE objects.  OLE objects are usually displayed by your
    CCMGfxView class and serialized as part of your CCMGfxDoc class.



/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

Printing and Print Preview support
    The application wizard has generated code to handle the print, print setup, and print preview
    commands by calling member functions in the CView class from the MFC library.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named CMGfx.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

CMGfx.manifest
	Application manifest files are used by Windows XP to describe an applications 
	dependency on specific versions of Side-by-Side assemblies. The loader uses this 
	information to load the appropriate assembly from the assembly cache or private 
	from the application. The Application manifest  maybe included for redistribution 
	as an external .manifest file that is installed in the same folder as the application 
	executable or it may be included in the executable in the form of a resource. 
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need 
to redistribute the MFC DLLs. If your application is in a language 
other than the operating system's locale, you will also have to 
redistribute the corresponding localized resources MFC80XXX.DLL. 
For more information on both of these topics, please see the section on 
redistributing Visual C++ applications in MSDN documentation. 

/////////////////////////////////////////////////////////////////////////////