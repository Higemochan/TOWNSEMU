#ifndef INPROCESS_FILEDIALOG_IS_INCLUDED
#define INPROCESS_FILEDIALOG_IS_INCLUDED

#include <string>
#include <vector>
#include "fssimplewindow.h"

class InProcessFileDialog
{
public:
	bool active=false;
	int driveType=-1;  // 0=CD, 1=FD0, 2=FD1

	struct Entry
	{
		std::string name;
		bool isDir;
	};

	std::string currentDir;
	std::string selectedPath;  // Set when user confirms selection
	std::vector<Entry> entries;
	int selectedIndex=0;
	int scrollOffset=0;

	// Font
	GLuint fontTexId=0;
	bool fontInitialized=false;

	static const int CHAR_W=8;
	static const int CHAR_H=16;
	static const int DIALOG_MARGIN=32;
	static const int ITEM_HEIGHT=18;
	static const int HEADER_HEIGHT=40;
	static const int FOOTER_HEIGHT=24;

	void InitFont(void);
	void Open(int driveType);
	void Close(void);

	bool HandleMouseClick(int evt,int mx,int my,int winWid,int winHei);
	bool HandleKey(int keyCode);

	void Render(int winWid,int winHei);
	std::string GetSelectedPath(void) const;

private:
	void RefreshFileList(void);
	void NavigateInto(int index);
	void NavigateUp(void);
	std::string GetBaseDir(void) const;
	int MaxVisibleItems(int winHei) const;
	int DialogX0(int winWid) const;
	int DialogY0(int winHei) const;
	int DialogX1(int winWid) const;
	int DialogY1(int winHei) const;
	int ListY0(int winHei) const;
	int ListY1(int winHei) const;

	void DrawRect(int x0,int y0,int x1,int y1,float r,float g,float b,float a) const;
	void DrawChar(int x,int y,char ch,float r,float g,float b) const;
	void DrawString(int x,int y,const char *str,float r,float g,float b) const;
	void DrawStringClipped(int x,int y,const char *str,int maxW,float r,float g,float b) const;

	bool MatchesExtension(const std::string &name) const;
	void EnsureSelectionVisible(void);
	void ConfirmSelection(void);
};

#endif
