#pragma once

#include "CIntObject.h"
#include "SDL_Extensions.h"
#include "../FunctionList.h"

struct SDL_Surface;
struct Rect;
class CAnimImage;
class CLabel;
class CAnimation;
class CDefHandler;

/*
 * CPicture.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

// Window GUI class
class CSimpleWindow : public CIntObject
{
public:
	SDL_Surface * bitmap; //background
	virtual void show(SDL_Surface * to);
	CSimpleWindow():bitmap(NULL){}; //c-tor
	virtual ~CSimpleWindow(); //d-tor
};

// Image class
class CPicture : public CIntObject
{
public: 
	SDL_Surface * bg;
	Rect * srcRect; //if NULL then whole surface will be used
	bool freeSurf; //whether surface will be freed upon CPicture destruction
	bool needRefresh;//Surface needs to be displayed each frame

	operator SDL_Surface*()
	{
		return bg;
	}

	CPicture(const Rect & r, const SDL_Color & color, bool screenFormat = false); //rect filled with given color
	CPicture(const Rect & r, ui32 color, bool screenFormat = false); //rect filled with given color
	CPicture(SDL_Surface * BG, int x = 0, int y=0, bool Free = true); //wrap existing SDL_Surface
	CPicture(const std::string &bmpname, int x=0, int y=0);
	CPicture(SDL_Surface *BG, const Rect &SrcRext, int x = 0, int y = 0, bool free = false); //wrap subrect of given surface
	~CPicture();
	void init();

	void createSimpleRect(const Rect &r, bool screenFormat, ui32 color);
	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to);
	void convertToScreenBPP();
	void colorizeAndConvert(int player);
	void colorize(int player);
};

namespace config{struct ButtonInfo;}

/// Base class for buttons.
class CButtonBase : public CKeyShortcut
{
public:
	enum ButtonState
	{
		NORMAL=0,
		PRESSED=1,
		BLOCKED=2,
		HIGHLIGHTED=3
	};
private:
	int bitmapOffset; // base offset of visible bitmap from animation
	ButtonState state;//current state of button from enum

public:
	bool swappedImages,//fix for some buttons: normal and pressed image are swapped
		keepFrame; // don't change visual representation

	void addTextOverlay(const std::string &Text, EFonts font, SDL_Color color = Colors::Cornsilk);
	void update();//to refresh button after image or text change

	void setOffset(int newOffset);
	void setState(ButtonState newState);
	ButtonState getState();

	//just to make code clearer
	void block(bool on);
	bool isBlocked();
	bool isHighlighted();

	CAnimImage * image; //image for this button
	CLabel * text;//text overlay

	CButtonBase(); //c-tor
	virtual ~CButtonBase(); //d-tor
};

/// Typical Heroes 3 button which can be inactive or active and can 
/// hold further information if you right-click it
class CAdventureMapButton : public CButtonBase
{
	std::vector<std::string> imageNames;//store list of images that can be used by this button
	size_t currentImage;
public:
	std::map<int, std::string> hoverTexts; //text for statusbar
	std::string helpBox; //for right-click help
	CFunctionList<void()> callback;
	bool actOnDown,//runs when mouse is pressed down over it, not when up
		hoverable,//if true, button will be highlighted when hovered
		borderEnabled,
		soundDisabled;
	SDL_Color borderColor;

	void clickRight(tribool down, bool previousState);
	virtual void clickLeft(tribool down, bool previousState);
	void hover (bool on);

	CAdventureMapButton(); //c-tor
	CAdventureMapButton( const std::string &Name, const std::string &HelpBox, const CFunctionList<void()> &Callback, int x, int y, const std::string &defName, int key=0, std::vector<std::string> * add = NULL, bool playerColoredButton = false );//c-tor
	CAdventureMapButton( const std::pair<std::string, std::string> &help, const CFunctionList<void()> &Callback, int x, int y, const std::string &defName, int key=0, std::vector<std::string> * add = NULL, bool playerColoredButton = false );//c-tor
	CAdventureMapButton( const std::string &Name, const std::string &HelpBox, const CFunctionList<void()> &Callback, config::ButtonInfo *info, int key=0);//c-tor

	void init(const CFunctionList<void()> &Callback, const std::map<int,std::string> &Name, const std::string &HelpBox, bool playerColoredButton, const std::string &defName, std::vector<std::string> * add, int x, int y, int key );

	void setIndex(size_t index, bool playerColoredButton=false);
	void setImage(CAnimation* anim, bool playerColoredButton=false);
	void setPlayerColor(int player);
	void showAll(SDL_Surface * to);
};

/// A button which can be selected/deselected
class CHighlightableButton 
	: public CAdventureMapButton
{
public:
	CHighlightableButton(const CFunctionList<void()> &onSelect, const CFunctionList<void()> &onDeselect, const std::map<int,std::string> &Name, const std::string &HelpBox, bool playerColoredButton, const std::string &defName, std::vector<std::string> * add, int x, int y, int key=0);
	CHighlightableButton(const std::pair<std::string, std::string> &help, const CFunctionList<void()> &onSelect, int x, int y, const std::string &defName, int myid, int key=0, std::vector<std::string> * add = NULL, bool playerColoredButton = false );//c-tor
	CHighlightableButton(const std::string &Name, const std::string &HelpBox, const CFunctionList<void()> &onSelect, int x, int y, const std::string &defName, int myid, int key=0, std::vector<std::string> * add = NULL, bool playerColoredButton = false );//c-tor
	bool onlyOn;//button can not be de-selected
	bool selected;//state of highlightable button
	int ID; //for identification
	CFunctionList<void()> callback2; //when de-selecting
	void select(bool on);
	void clickLeft(tribool down, bool previousState);
};

/// A group of buttons where one button can be selected
class CHighlightableButtonsGroup : public CIntObject
{
public:
	CFunctionList2<void(int)> onChange; //called when changing selected button with new button's id
	std::vector<CHighlightableButton*> buttons;
	bool musicLike; //determines the behaviour of this group

	//void addButton(const std::map<int,std::string> &tooltip, const std::string &HelpBox, const std::string &defName, int x, int y, int uid);
	void addButton(CHighlightableButton* bt);//add existing button, it'll be deleted by CHighlightableButtonsGroup destructor
	void addButton(const std::map<int,std::string> &tooltip, const std::string &HelpBox, const std::string &defName, int x, int y, int uid, const CFunctionList<void()> &OnSelect=0, int key=0); //creates new button
	CHighlightableButtonsGroup(const CFunctionList2<void(int)> &OnChange, bool musicLikeButtons = false);
	~CHighlightableButtonsGroup();
	void select(int id, bool mode); //mode==0: id is serial; mode==1: id is unique button id
	void selectionChanged(int to);
	void show(SDL_Surface * to);
	void showAll(SDL_Surface * to);
	void block(ui8 on);
};

/// A typical slider which can be orientated horizontally/vertically.
class CSlider : public CIntObject
{
public:
	CAdventureMapButton *left, *right, *slider; //if vertical then left=up
	int capacity,//how many elements can be active at same time
		amount, //how many elements
		positions, //number of highest position (0 if there is only one)
		value; //first active element
	bool horizontal;
	bool wheelScrolling;
	bool keyScrolling;

	boost::function<void(int)> moved;

	void redrawSlider(); 
	void sliderClicked();
	void moveLeft();
	void moveRight();
	void moveTo(int to);
	void block(bool on);
	void setAmount(int to);

	void keyPressed(const SDL_KeyboardEvent & key);
	void wheelScrolled(bool down, bool in);
	void clickLeft(tribool down, bool previousState);
	void mouseMoved (const SDL_MouseMotionEvent & sEvent);
	void showAll(SDL_Surface * to);

	CSlider(int x, int y, int totalw, boost::function<void(int)> Moved, int Capacity, int Amount, 
		int Value=0, bool Horizontal=true, int style = 0); //style 0 - brown, 1 - blue
	~CSlider();
	void moveToMax();
};

/// Used as base for Tabs and List classes
class CObjectList : public CIntObject
{
public:
	typedef boost::function<CIntObject* (size_t)> CreateFunc;
	typedef boost::function<void(CIntObject *)> DestroyFunc;

private:
	CreateFunc createObject;
	DestroyFunc destroyObject;

protected:
	//Internal methods for safe creation of items (Children capturing and activation/deactivation if needed)
	void deleteItem(CIntObject* item);
	CIntObject* createItem(size_t index);

	CObjectList(CreateFunc create, DestroyFunc destroy = DestroyFunc());//Protected constructor
};

/// Window element with multiple tabs
class CTabbedInt : public CObjectList
{
private:
	CIntObject * activeTab;
	size_t activeID;

public:
	//CreateFunc, DestroyFunc - see CObjectList
	//Pos - position of object, all tabs will be moved to this position
	//ActiveID - ID of initially active tab
	CTabbedInt(CreateFunc create, DestroyFunc destroy = DestroyFunc(), Point position=Point(), size_t ActiveID=0);

	void setActive(size_t which);
	//recreate active tab
	void reset();

	//return currently active item
	CIntObject * getItem();
};

/// List of IntObjects with optional slider
class CListBox : public CObjectList
{
private:
	std::list< CIntObject* > items;
	size_t first;
	size_t totalSize;

	Point itemOffset;
	CSlider * slider;

	void updatePositions();
public:
	//CreateFunc, DestroyFunc - see CObjectList
	//Pos - position of first item
	//ItemOffset - distance between items in the list
	//VisibleSize - maximal number of displayable at once items
	//TotalSize 
	//Slider - slider style, bit field: 1 = present(disabled), 2=horisontal(vertical), 4=blue(brown)
	//SliderPos - position of slider, if present
	CListBox(CreateFunc create, DestroyFunc destroy, Point Pos, Point ItemOffset, size_t VisibleSize,
		size_t TotalSize, size_t InitialPos=0, int Slider=0, Rect SliderPos=Rect() );

	//recreate all visible items
	void reset();

	//return currently active items
	std::list< CIntObject * > getItems();

	//scroll list
	void moveToPos(size_t which);
	void moveToNext();
	void moveToPrev();
};

/// Status bar which is shown at the bottom of the in-game screens
class CStatusBar
	: public CIntObject, public IStatusBar
{
public:
	SDL_Surface * bg; //background
	int middlex, middley; //middle of statusbar
	std::string current; //text currently printed

	CStatusBar(int x, int y, std::string name="ADROLLVR.bmp", int maxw=-1); //c-tor
	~CStatusBar(); //d-tor
	void print(const std::string & text); //prints text and refreshes statusbar
	void clear();//clears statusbar and refreshes
	void show(SDL_Surface * to); //shows statusbar (with current text)
	std::string getCurrent(); //getter for current
};

/// Label which shows text
class CLabel
	: public virtual CIntObject
{
public:
	EAlignment alignment;
	EFonts font;
	SDL_Color color;
	std::string text;
	CPicture *bg;
	bool autoRedraw;  //whether control will redraw itself on setTxt
	Point textOffset; //text will be blitted at pos + textOffset with appropriate alignment
	bool ignoreLeadingWhitespace; 

	virtual void setTxt(const std::string &Txt);
	void showAll(SDL_Surface * to); //shows statusbar (with current text)
	CLabel(int x=0, int y=0, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = Colors::Cornsilk, const std::string &Text =  "");
};

/// a multi-line label that tries to fit text with given available width and height; if not possible, it creates a slider for scrolling text
class CTextBox
	: public CLabel
{
public:
	int maxW; //longest line of text in px
	int maxH; //total height needed to print all lines

	int sliderStyle;
	bool redrawParentOnScrolling;

	std::vector<std::string> lines;
	std::vector<CAnimImage* > effects;
	CSlider *slider;

	//CTextBox( std::string Text, const Point &Pos, int w, int h, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = Colors::Cornsilk);
	CTextBox(std::string Text, const Rect &rect, int SliderStyle, EFonts Font = FONT_SMALL, EAlignment Align = TOPLEFT, const SDL_Color &Color = Colors::Cornsilk);
	void showAll(SDL_Surface * to); //shows statusbar (with current text)
	void setTxt(const std::string &Txt);
	void setBounds(int limitW, int limitH);
	void recalculateLines(const std::string &Txt);

	void sliderMoved(int to);
};

/// Status bar which is shown at the bottom of the in-game screens
class CGStatusBar
	: public CLabel, public IStatusBar
{
	void init();
public:
	IStatusBar *oldStatusBar;

	//statusbar interface overloads
	void print(const std::string & Text); //prints text and refreshes statusbar
	void clear();//clears statusbar and refreshes
	std::string getCurrent(); //returns currently displayed text
	void show(SDL_Surface * to); //shows statusbar (with current text)

	CGStatusBar(int x, int y, EFonts Font = FONT_SMALL, EAlignment Align = CENTER, const SDL_Color &Color = Colors::Cornsilk, const std::string &Text =  "");
	CGStatusBar(CPicture *BG, EFonts Font = FONT_SMALL, EAlignment Align = CENTER, const SDL_Color &Color = Colors::Cornsilk); //given CPicture will be captured by created sbar and it's pos will be used as pos for sbar
	CGStatusBar(int x, int y, std::string name, int maxw=-1); 

	~CGStatusBar();
	void calcOffset();
};

/// UIElement which can get input focus
class CFocusable 
	: public virtual CIntObject
{
public:
	bool focus; //only one focusable control can have focus at one moment

	void giveFocus(); //captures focus
	void moveFocus(); //moves focus to next active control (may be used for tab switching)

	static std::list<CFocusable*> focusables; //all existing objs
	static CFocusable *inputWithFocus; //who has focus now
	CFocusable();
	~CFocusable();
};

/// Text input box where players can enter text
class CTextInput
	: public CLabel, public CFocusable
{
public:
	CFunctionList<void(const std::string &)> cb;

	void setText(const std::string &nText, bool callCb = false);

	CTextInput(const Rect &Pos, const Point &bgOffset, const std::string &bgName, const CFunctionList<void(const std::string &)> &CB);
	CTextInput(const Rect &Pos, SDL_Surface *srf = NULL);
	~CTextInput();
	void showAll(SDL_Surface * to);
	void clickLeft(tribool down, bool previousState);
	void keyPressed(const SDL_KeyboardEvent & key);
};

/// Listbox UI Element
class CList : public CIntObject
{
public:
	SDL_Surface * bg; //background bitmap
	CDefHandler *arrup, *arrdo; //button arrows for scrolling list
	SDL_Surface *empty, *selection;
	SDL_Rect arrupp, arrdop; //positions of arrows
	int posw, posh; //position width/height
	int selected, //id of selected position, <0 if none
		from;
	const int SIZE; //size of list
	tribool pressed; //true=up; false=down; indeterminate=none

	CList(int Size = 5); //c-tor
	void clickLeft(tribool down, bool previousState);
	void activate();
	void deactivate();
	virtual void mouseMoved (const SDL_MouseMotionEvent & sEvent)=0; //call-in
	virtual void genList()=0;
	virtual void select(int which)=0;
	virtual void draw(SDL_Surface * to)=0;
	virtual int size() = 0; //how many elements do we have
	void fixPos(); //scrolls list, so the selection will be visible
};

/// Shows a text by moving the mouse cursor over the object
class CHoverableArea: public virtual CIntObject
{
public:
	std::string hoverText;

	virtual void hover (bool on);

	CHoverableArea();
	virtual ~CHoverableArea();
};

/// Can interact on left and right mouse clicks, plus it shows a text when by hovering over it
class LRClickableAreaWText: public CHoverableArea
{
public:
	std::string text;

	LRClickableAreaWText();
	LRClickableAreaWText(const Rect &Pos, const std::string &HoverText = "", const std::string &ClickText = "");
	virtual ~LRClickableAreaWText();
	void init();

	virtual void clickLeft(tribool down, bool previousState);
	virtual void clickRight(tribool down, bool previousState);
};