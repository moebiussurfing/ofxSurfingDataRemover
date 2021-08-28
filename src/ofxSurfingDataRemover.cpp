#include "ofxSurfingDataRemover.h"

#pragma mark - OF

//--------------------------------------------------------------
ofxSurfingDataRemover::ofxSurfingDataRemover()
{
	// path for settings
	setPathGlobal("ofxSurfingDataRemover/");
	path_Params_Control = "params_Control.xml";
	DISABLE_Callbacks = true;
	setActive(true);//add key and mouse listeners

	ofAddListener(ofEvents().update, this, &ofxSurfingDataRemover::update);
	ofAddListener(ofEvents().draw, this, &ofxSurfingDataRemover::draw, OF_EVENT_ORDER_AFTER_APP);

	setup();
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::setup()
{
	//log mode
	ofSetLogLevel("ofxSurfingDataRemover", OF_LOG_NOTICE);
	
	//default
	path_Params_AppSettings = path_GLOBAL+"params_AppSettings.xml";

	//--

	// window
	screenW = ofGetWidth();
	screenH = ofGetHeight();

	//--

	//// params control (addon variables)
	//Addon_Active.set("ADDON ACTIVE", true);
	//Addon_Float.set("ADDON float", 0, 0, 1);

	// group
	params_Addon.setName("ADDON");
	//params_Addon.add(Addon_Active);
	//params_Addon.add(Addon_Float);

	params_Addon.add(filesList);

	// callback
	ofAddListener(params_Addon.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params_Addon);

	//-

	// addon control(internal)

	// params

	MODE_Active.set("ACTIVE", true);
	ENABLE_keys.set("KEYS", true);
	SHOW_Help.set("HELP", false);
	//MODE_App.set("APP MODE", 0, 0, NUM_MODES_APP - 1);
	//MODE_App_Name.set("", "");
	//MODE_App_Name.setSerializable(false);
	ENABLE_Debug.set("DEBUG", true);
	ENABLE_AutoSave.set("AUTO SAVE", true);
	SHOW_Gui.set("GUI", false);
	SHOW_Gui.setSerializable(false);
	Gui_Position.set("GUI POSITION",
		glm::vec2(screenW * 0.5, screenH * 0.5),
		glm::vec2(0, 0),
		glm::vec2(screenW, screenH)
	);

	// params control (internal)
	params_Control.setName("INTERNAL");
	//params_Control.add(MODE_App);
	//params_Control.add(MODE_App_Name);
	params_Control.add(MODE_Active);
	params_Control.add(ENABLE_keys);
	params_Control.add(SHOW_Help);
	params_Control.add(ENABLE_Debug);
	params_Control.add(ENABLE_AutoSave);
	params_Control.add(SHOW_Gui);
	params_Control.add(Gui_Position);
	params_Control.add(presetNamePath);

	//-

	// params App settings
	params_AppSettings.setName("AppSettings");
	//params_AppSettings.add(bInitialize);
	params_AppSettings.add(bClear);
	params_AppSettings.add(bRun);
	params_AppSettings.add(filesList);
	params_AppSettings.add(foldersList);

	bSave.setSerializable(false);
	bClear.setSerializable(false);
	bRun.setSerializable(false);

	//-

	// callback
	ofAddListener(params_Control.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params_Control);

	//-

	// all params
	params.setName("ALL PARAMS");
	params.add(params_AppSettings);
	params.add(params_Addon);
	params.add(params_Control);

	// callback
	ofAddListener(params.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params);

	//--

	// gui

	// theme
	
	string _path = "assets/fonts/";
	_path += "telegrama_render.otf";
	float _size = 6;
	//_path += "overpass-mono-bold.otf";
	//float _size = 7;

	ofFile file(_path);
	if (file.exists())
	{
		ofxGuiSetFont(_path, _size);
	}
	else
	{
		ofLogError(__FUNCTION__) << "ofxGui Theme '" << _path << "' Not found!";
	}

	ofxGuiSetDefaultHeight(30);
	ofxGuiSetBorderColor(32);
	ofxGuiSetFillColor(ofColor(48));
	ofxGuiSetTextColor(ofColor::white);
	ofxGuiSetHeaderColor(ofColor(24));
	ofxGuiSetBackgroundColor(ofColor::black);

	// setup gui
	gui_Control.setup("ofxSurfingDataRemover");
	gui_Control.add(params);//add control (internal) and addon params
	gui_Control.setPosition(ofGetWidth() - 210, 20);

	// collapse groups
	auto &g0 = gui_Control.getGroup("ALL PARAMS");//1st level
	auto &g2 = g0.getGroup("ADDON");//2nd level
	auto &g3 = g0.getGroup("INTERNAL");//2nd level
	auto &g31 = g3.getGroup("GUI POSITION");//3nd level
	//g0.minimize();
	g2.minimize();
	g3.minimize();
	g31.minimize();

	//-

	fontBox.loadFont(_path, 9, true, true, true);

	//--

	// button
	font.loadFont(_path, 20, true, true, true);
	//font.loadFont(OF_TTF_SANS, 20, true, true, true);

	int x, y, w, h, pad;
	pad = 10;
	h = 70;
	w = ofGetWidth() - 2 * pad;
	y = ofGetHeight() - h - pad;
	x = pad;
	float _round = 5;

	butRemove.setup("RUN CLEANER!", x, y, w, h);
	butRemove.setFont(&font);
	butRemove.setEnabled(true);
	butRemove.setPressedColor(255);
	butRemove.setActiveColor(255);
	butRemove.setHoverColor(128);
	butRemove.setStringColor(255);
	butRemove.setBackgroundColor(0);
	butRemove.setAutoMouse(true);
	butRemove.setStringColor(255);
	butRemove.setCornerRounded(_round);

	y -= h + 1 * pad;
	butClear.setup("CLEAR LIST", x, y, w, h);
	butClear.setFont(&font);
	butClear.setEnabled(true);
	butClear.setPressedColor(255);
	butClear.setActiveColor(255);
	butClear.setHoverColor(128);
	butClear.setStringColor(255);
	butClear.setBackgroundColor(0);
	butClear.setAutoMouse(true);
	butClear.setStringColor(255);
	butClear.setCornerRounded(_round);

	y -= h + 1 * pad;
	butSave.setup("SAVE LIST", x, y, w, h);
	butSave.setFont(&font);
	butSave.setEnabled(true);
	butSave.setPressedColor(255);
	butSave.setActiveColor(255);
	butSave.setHoverColor(128);
	butSave.setStringColor(255);
	butSave.setBackgroundColor(0);
	butSave.setAutoMouse(true);
	butSave.setStringColor(255);
	butSave.setCornerRounded(_round);

	y -= h + 1 * pad;
	butLoad.setup("LOAD LIST", x, y, w, h);
	butLoad.setFont(&font);
	butLoad.setEnabled(true);
	butLoad.setPressedColor(255);
	butLoad.setActiveColor(255);
	butLoad.setHoverColor(128);
	butLoad.setStringColor(255);
	butLoad.setBackgroundColor(0);
	butLoad.setAutoMouse(true);
	butLoad.setStringColor(255);
	butLoad.setCornerRounded(_round);

	ofAddListener(butRemove.clickEvent, this, &ofxSurfingDataRemover::onButRemove);
	ofAddListener(butClear.clickEvent, this, &ofxSurfingDataRemover::onButClear);
	ofAddListener(butSave.clickEvent, this, &ofxSurfingDataRemover::onButSave);
	ofAddListener(butLoad.clickEvent, this, &ofxSurfingDataRemover::onButLoad);

	//--

	// startup
	startup();

	//--
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::startup()
{
	ofLogNotice(__FUNCTION__);

	DISABLE_Callbacks = false;

	//-

	// settings
	ofxSurfingHelpers::loadGroup(params_Control, path_GLOBAL + path_Params_Control);

	ofxSurfingHelpers::loadGroup(params_AppSettings, path_Params_AppSettings);
	//ofxSurfingHelpers::loadGroup(params_AppSettings, path_GLOBAL + path_Params_AppSettings);

	MODE_Active = true;

	// set gui position after window setup/resizing
	windowResized(screenW, screenH);

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::update(ofEventArgs & args) {

	// autosave
	if (ENABLE_AutoSave && ofGetElapsedTimeMillis() - timerLast_Autosave > timeToAutosave)
	{
		DISABLE_Callbacks = true;
		// get gui position before save
		Gui_Position = glm::vec2(gui_Control.getPosition());
		ofxSurfingHelpers::saveGroup(params_Control, path_GLOBAL + path_Params_Control);
		DISABLE_Callbacks = false;

		timerLast_Autosave = ofGetElapsedTimeMillis();
		ofLogVerbose(__FUNCTION__) << "AutoSaved DONE";
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::draw(ofEventArgs & args)
{
	if (SHOW_Gui)
	{
		// edit mode
		if (MODE_App == 1)
		{
			//controlPoints.draw();
		}

		gui_Control.draw();
	}

	drawInfo();


	butClear.draw();
	butRemove.draw();
	butSave.draw();
	butLoad.draw();

	//ofPushStyle();
	//ofColor()
	//ofFill();
	//ofDrawRectangle(rectButton);
	//bool b = false;
	//if (ofGetMousePressed()) {
	//	b = ofxMyUtil::Event::RectButton(rectButton, ofGetMouseX(), ofGetMouseY());
	//}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::exit()
{
	// get gui position before save
	Gui_Position = glm::vec2(gui_Control.getPosition());

	ofxSurfingHelpers::saveGroup(params_Control, path_GLOBAL + path_Params_Control);

	ofxSurfingHelpers::saveGroup(params_AppSettings, path_Params_AppSettings);
	//ofxSurfingHelpers::saveGroup(params_AppSettings, path_GLOBAL + path_Params_AppSettings);
}

//--------------------------------------------------------------
ofxSurfingDataRemover::~ofxSurfingDataRemover()
{
	setActive(false); // remove keys and mouse listeners

	// remove params callbacks listeners
	ofRemoveListener(params.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params);
	ofRemoveListener(params_Control.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params_Control);
	ofRemoveListener(params_Addon.parameterChangedE(), this, &ofxSurfingDataRemover::Changed_params_Addon);

	ofRemoveListener(ofEvents().update, this, &ofxSurfingDataRemover::update);
	ofRemoveListener(ofEvents().draw, this, &ofxSurfingDataRemover::draw, OF_EVENT_ORDER_AFTER_APP);

	exit();
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::setLogLevel(ofLogLevel level)
{
	ofSetLogLevel(__FUNCTION__, level);
}


#pragma mark - OF LISTENERS

//--------------------------------------------------------------
void ofxSurfingDataRemover::windowResized(int w, int h)
{
	screenW = w;
	screenH = h;

	// user gui deopending on window dimensions
	//gui_Control.setPosition(screenW * 0.5 - 200, screenH - 200);

	//-

	// button
	int x, y, ww, hh, pad;
	pad = 10;
	hh = 70;
	ww = ofGetWidth() - 2 * pad;
	y = ofGetHeight() - hh - pad;
	x = pad;

	//butRemove.setPosition(x, y);
	//butRemove.setSize(w, h);

	butRemove.setup("RUN CLEANER!", x, y, ww, hh);

	y -= hh + 1 * pad;
	butClear.setup("CLEAR LIST", x, y, ww, hh);

	y -= hh + 1 * pad;
	butSave.setup("SAVE LIST", x, y, ww, hh);

	y -= hh + 1 * pad;
	butLoad.setup("LOAD LIST", x, y, ww, hh);
}

// keys
//--------------------------------------------------------------
void ofxSurfingDataRemover::keyPressed(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << (char)key << " [" << key << "]";

	// modifiers
	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);

	bool debug = false;
	if (debug)
	{
		ofLogNotice(__FUNCTION__) << "mod_COMMAND: " << (mod_COMMAND ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_CONTROL: " << (mod_CONTROL ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_ALT: " << (mod_ALT ? "ON" : "OFF");
		ofLogNotice(__FUNCTION__) << "mod_SHIFT: " << (mod_SHIFT ? "ON" : "OFF");
	}

	//-

	//disabler for all keys. (independent from MODE_Active)
	if (ENABLE_keys)
	{
		if (0) {}

		////custom
		//if (key == ' ')
		//{
		//}
		//else if (key == ' ')
		//{
		//}

		////custom with modifiers
		//if (key == OF_KEY_UP && mod_ALT)
		//{
		//	ofLogNotice(__FUNCTION__) << "";
		//}
		//else if (key == OF_KEY_UP)
		//{
		//	ofLogNotice(__FUNCTION__) << "";
		//}

		//general
		//if (key == key_MODE_App)
		//{
		//	int i = MODE_App;
		//	i++;
		//	MODE_App = i % NUM_MODES_APP;
		//}

		else if (key == 'g')
		{
			SHOW_Gui = !SHOW_Gui;
		}
		else if (key == 'h')
		{
			SHOW_Help = !SHOW_Help;
		}
		else if (key == 'd')
		{
			ENABLE_Debug = !ENABLE_Debug;
		}
	}

	//--

	// key enabler
	if (key == 'k')
	{
		ENABLE_keys = !ENABLE_keys;
		ofLogNotice(__FUNCTION__) << "KEYS: " << (ENABLE_keys ? "ON" : "OFF");

		if (!ENABLE_keys)
		{
			ofLogNotice(__FUNCTION__) << "ALL KEYS DISABLED. PRESS 'k' TO ENABLE GAIN!";
		}
		else
		{
			ofLogNotice(__FUNCTION__) << "KEYS ENABLED BACK";
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::keyReleased(ofKeyEventArgs &eventArgs)
{
	const int &key = eventArgs.key;
	ofLogNotice(__FUNCTION__) << (char)key << " [" << key << "]";

	bool mod_COMMAND = eventArgs.hasModifier(OF_KEY_COMMAND);
	bool mod_CONTROL = eventArgs.hasModifier(OF_KEY_CONTROL);
	bool mod_ALT = eventArgs.hasModifier(OF_KEY_ALT);
	bool mod_SHIFT = eventArgs.hasModifier(OF_KEY_SHIFT);
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::addKeysListeners()
{
	ofAddListener(ofEvents().keyPressed, this, &ofxSurfingDataRemover::keyPressed);
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::removeKeysListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingDataRemover::keyPressed);
}

//mouse
//--------------------------------------------------------------
void ofxSurfingDataRemover::mouseDragged(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mouseDragged " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::mousePressed(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mousePressed " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::mouseReleased(ofMouseEventArgs &eventArgs)
{
	const int &x = eventArgs.x;
	const int &y = eventArgs.y;
	const int &button = eventArgs.button;
	//ofLogNotice(__FUNCTION__) << "mouseReleased " << x << ", " << y << ", " << button;
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::addMouseListeners()
{
	ofAddListener(ofEvents().mouseDragged, this, &ofxSurfingDataRemover::mouseDragged);
	ofAddListener(ofEvents().mousePressed, this, &ofxSurfingDataRemover::mousePressed);
	ofAddListener(ofEvents().mouseReleased, this, &ofxSurfingDataRemover::mouseReleased);
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::removeMouseListeners()
{
	ofRemoveListener(ofEvents().keyPressed, this, &ofxSurfingDataRemover::keyPressed);
}


#pragma mark - API

//--------------------------------------------------------------
void ofxSurfingDataRemover::setActive(bool b)
{
	//* disables all keys and mouse interaction listeners from the addon

	MODE_Active = b;
	if (!b)
	{
		removeKeysListeners();
		removeMouseListeners();
	}
	else
	{
		addKeysListeners();
		addMouseListeners();
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::setGuiVisible(bool b)
{
	SHOW_Gui = b;
}


#pragma mark - CALLBACKS

// all params
//--------------------------------------------------------------
void ofxSurfingDataRemover::Changed_params(ofAbstractParameter &e)
{
	string name = e.getName();
	ofLogNotice(__FUNCTION__) << name << " : " << e;

	if (0) {}

	// filter params

	else if (name == bClear.getName() && bClear.get())
	{
		doClear();
	}
	else if (name == bRun.getName() && bRun.get())
	{
		doRun();
	}
	else if (name == bSave.getName() && bSave.get())
	{
		doSave();
	}

	//else if (name == bInitialize.getName() && bInitialize.get())
	//{
	//	bInitialize = false;
	//	filesList = "";
	//}
}

// addon engine params
//--------------------------------------------------------------
void ofxSurfingDataRemover::Changed_params_Addon(ofAbstractParameter &e)
{
	if (DISABLE_Callbacks) return;

	string name = e.getName();

	// exclude debugs
	if (name != "exclude"
		&& name != "exclude")
	{
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}

	//// params
	//if (name == "")
	//{
	//}
}

//addon control (internal) params
//--------------------------------------------------------------
void ofxSurfingDataRemover::Changed_params_Control(ofAbstractParameter &e)
{
	if (DISABLE_Callbacks) return;

	string name = e.getName();

	// exclude debugs
	if (name != "exclude"
		&& name != "exclude")
	{
		ofLogNotice(__FUNCTION__) << name << " : " << e;
	}

	if (0) {}

	////control params
	//else if (name == "")
	//{
	//}
	//else if (name == "APP MODE")
	//{
	//	switch (MODE_App)
	//	{
	//	case 0:
	//		MODE_App_Name = "RUN";
	//		//setActive(false);
	//		break;
	//	case 1:
	//		MODE_App_Name = "EDIT";
	//		//setActive(true);
	//		break;
	//	default:
	//		MODE_App_Name = "UNKNOWN";
	//		break;
	//	}
	//}
	//else if (name == "ACTIVE")
	//{
	//	setActive(MODE_Active);
	//}
	//else if (name == "GUI")
	//{
	//}
	//else if (name == "HELP")
	//{
	//}
	//else if (name == "APP MODE")
	//{
	//}

	else if (name == presetNamePath.getName())
	{
		path_Params_AppSettings = presetNamePath;
	}

	else if (name == "GUI POSITION")
	{
		gui_Control.setPosition(Gui_Position.get().x, Gui_Position.get().y);
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::setKey_MODE_App(int k)
{
	key_MODE_App = k;
}

#pragma mark - FILE SETTINGS

//--------------------------------------------------------------
void ofxSurfingDataRemover::setPathGlobal(string s)//must call before setup. disabled by default
{
	path_GLOBAL = s;

	ofxSurfingHelpers::CheckFolder(path_GLOBAL);
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::dragEvent(ofDragInfo info) {
	if (info.files.size() > 0) {
		dragPt = info.position;

		////clear info
		//if (filesList.get() == "") {
		//	msg = "";
		//}

		//draggedImages.assign(info.files.size(), ofImage());
		for (unsigned int k = 0; k < info.files.size(); k++)
		{
			string name = info.files[k];

			auto ss = ofSplitString(name, ".");
			bool bIsAFolder = (ss.size() == 1);

			ofLogNotice(__FUNCTION__) << k << " : " << name;

			if (!bIsAFolder) {
				filesList += name;
				filesList += "\n";
			}
			else {
				foldersList += name;
				foldersList += "\n";
			}
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::drawInfo() {
	int x, y, w, ww, h, pad;
	x = 20;
	y = 25;
	pad = 10;

	string ss = "LIST";
	ofxSurfingHelpers::drawTextBoxed(fontBox, ss, x, y);

	y += 90;
	ss = "FILES" + ofToString((filesList.get() != "") ? "\n\n" : " NONE") + filesList.get();
	ss += "\n";
	ss += "FOLDERS" + ofToString((foldersList.get() != "") ? "\n\n" : " NONE") + foldersList.get();
	ofxSurfingHelpers::drawTextBoxed(fontBox, ss, x, y);

	y = 25;
	ss = "Drag files or folders to queue paths...";
	ww = ofxSurfingHelpers::getWidthBBtextBoxed(fontBox, ss) + pad;
	x = ofGetWidth() - ww;
	ofxSurfingHelpers::drawTextBoxed(fontBox, ss, x, y);

	ss = msg;
	w = 270;
	ww = ofxSurfingHelpers::getWidthBBtextBoxed(fontBox, ss) + pad;
	x = ofGetWidth() - ww;
	h = 70;
	y = ofGetHeight() - 2 * h - 4 * pad - 50;

	if (ss != "") {
		ofxSurfingHelpers::drawTextBoxed(fontBox, ss, x, y);
		//ofDrawBitmapStringHighlight(ss, x, y);
	}
}

//--------------------------------------------------------------
void ofxSurfingDataRemover::doRemoveDataFiles() {

	bool bRelativeToData = false;

	ofLogNotice(__FUNCTION__) << "Removing files...";
	auto ssfiles = ofSplitString(filesList, "\n");
	for (int i = 0; i < ssfiles.size(); i++)
	{
		string filepath = ssfiles[i];
		if (filepath == "") continue;//skip

		ofLogNotice(__FUNCTION__) << "#" << i << " : " << filepath;
		ofFile::removeFile(filepath, bRelativeToData);
	}

	ofLogNotice(__FUNCTION__) << "Removing folders...";
	auto ssfolders = ofSplitString(foldersList, "\n");
	for (int i = 0; i < ssfolders.size(); i++)
	{
		string folderpath = ssfolders[i];
		if (folderpath == "") continue;//skip

		ofLogNotice(__FUNCTION__) << "#" << i << " : " << folderpath;
		ofDirectory::removeDirectory(folderpath, true, bRelativeToData);
	}

	msg = "Removed files and folders. Done!";
	//filesList = "";

	//// remove all the settings folder
	//const filesystem::path path = path_Global;
	//ofDirectory::removeDirectory(path, true, true);

	//// remove ini file
	//const filesystem::path file = ofToDataPath("../imgui.ini");
	//ofFile::removeFile(file, true);
}
