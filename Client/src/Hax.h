#pragma once


#ifdef _DEBUG
#define Error(msg) MessageBox(NULL, msg, NULL, MB_OK);
#else
#define Error(msg)
#endif