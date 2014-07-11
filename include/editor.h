#include <windows.h>

#define _UNICODE

const char * className = "mainWindow";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void createEditor(HWND hwnd);
void resizeEditor(HWND hwnd);
void openFile(HWND hwnd);
void saveFile(HWND hwnd);
BOOL loadText(HWND hEdit, LPCTSTR fileName);
BOOL writeText(HWND hEdit, LPCTSTR fileName);
BOOL isKeyDown(int vk);