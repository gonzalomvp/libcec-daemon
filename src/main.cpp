/**
 * libcec-daemon
 * A simple daemon to connect libcec to uinput. That is, using your TV to control your PC!
 * by Andrew Brampton
 *
 * TODO
 *
 */
#include "main.h"

#define VERSION     "libcec-daemon v0.9"

#define CEC_NAME    "linux PC"
#define UINPUT_NAME "libcec-daemon"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <cstdint>
#include <cstddef>
#include <csignal>
#include <vector>
#include <unistd.h>

#include <boost/program_options.hpp>
#include "accumulator.hpp"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

#include <boost/filesystem.hpp>
#include <SDL2/SDL.h>
#include "pugixml.hpp"

using namespace CEC;
using namespace log4cplus;

using std::cout;
using std::cerr;
using std::endl;
using std::min;
using std::string;
using std::vector;

std::map<string, int> key2cec;
std::map<int, int> cec2sdl;
std::map<int, int> sdl2input;
namespace fs = boost::filesystem;
static Logger logger = Logger::getInstance("main");

const vector<__u16> Main::uinputCecMap = Main::setupUinputMap();

Main & Main::instance() {
	// Singleton pattern so we can use main from a sighandle
	static Main main;
	return main;
}

Main::Main() : cec(getCecName(), this), uinput(UINPUT_NAME, uinputCecMap), makeActive(true), running(true) {
	LOG4CPLUS_TRACE_STR(logger, "Main::Main()");

	signal (SIGINT,  &Main::signalHandler);
	signal (SIGTERM, &Main::signalHandler);
}

Main::~Main() {
	LOG4CPLUS_TRACE_STR(logger, "Main::~Main()");
	stop();
}

void Main::loop() {
	LOG4CPLUS_TRACE_STR(logger, "Main::loop()");

	cec.open();

	if (makeActive) {
		cec.makeActive();
	}

	while (running) {
		LOG4CPLUS_TRACE_STR(logger, "Loop");
		sleep(1);
	}
	cec.close();
}

void Main::stop() {
	LOG4CPLUS_TRACE_STR(logger, "Main::stop()");
	running = false;
}

void Main::listDevices() {
	LOG4CPLUS_TRACE_STR(logger, "Main::listDevices()");
	cec.listDevices(cout);
}

void Main::signalHandler(int sigNum) {
	LOG4CPLUS_DEBUG_STR(logger, "Main::signalHandler()");

	Main::instance().stop();
}

char *Main::getCecName() {
	LOG4CPLUS_TRACE_STR(logger, "Main::getCecName()");
	if (gethostname(cec_name,HOST_NAME_MAX) < 0 ) {
		LOG4CPLUS_TRACE_STR(logger, "Main::getCecName()");
		strncpy(cec_name, CEC_NAME, sizeof(HOST_NAME_MAX));
	}

	return cec_name;
}

const std::vector<__u16> & Main::setupUinputMap() {
	static std::vector<__u16> uinputCecMap;

	if (uinputCecMap.empty()) {
		uinputCecMap.resize(CEC_USER_CONTROL_CODE_MAX + 1, 0);
		uinputCecMap[CEC_USER_CONTROL_CODE_SELECT                      ] = KEY_ENTER;
		uinputCecMap[CEC_USER_CONTROL_CODE_UP                          ] = KEY_UP;
		uinputCecMap[CEC_USER_CONTROL_CODE_DOWN                        ] = KEY_DOWN;
		uinputCecMap[CEC_USER_CONTROL_CODE_LEFT                        ] = KEY_LEFT;
		uinputCecMap[CEC_USER_CONTROL_CODE_RIGHT                       ] = KEY_RIGHT;
		uinputCecMap[CEC_USER_CONTROL_CODE_RIGHT_UP                    ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_RIGHT_DOWN                  ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_LEFT_UP                     ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_LEFT_DOWN                   ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_ROOT_MENU                   ] = KEY_TAB;
		uinputCecMap[CEC_USER_CONTROL_CODE_SETUP_MENU                  ] = KEY_C;
		uinputCecMap[CEC_USER_CONTROL_CODE_CONTENTS_MENU               ] = KEY_C;
		uinputCecMap[CEC_USER_CONTROL_CODE_FAVORITE_MENU               ] = KEY_FAVORITES;
		uinputCecMap[CEC_USER_CONTROL_CODE_EXIT                        ] = KEY_ESC;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER0                     ] = KEY_0;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER1                     ] = KEY_1;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER2                     ] = KEY_2;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER3                     ] = KEY_3;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER4                     ] = KEY_4;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER5                     ] = KEY_5;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER6                     ] = KEY_6;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER7                     ] = KEY_7;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER8                     ] = KEY_8;
		uinputCecMap[CEC_USER_CONTROL_CODE_NUMBER9                     ] = KEY_9;
		uinputCecMap[CEC_USER_CONTROL_CODE_DOT                         ] = KEY_DOT;
		uinputCecMap[CEC_USER_CONTROL_CODE_ENTER                       ] = KEY_ENTER;
		uinputCecMap[CEC_USER_CONTROL_CODE_CLEAR                       ] = KEY_EXIT;
		uinputCecMap[CEC_USER_CONTROL_CODE_NEXT_FAVORITE               ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_CHANNEL_UP                  ] = KEY_CHANNELUP;
		uinputCecMap[CEC_USER_CONTROL_CODE_CHANNEL_DOWN                ] = KEY_CHANNELDOWN;
		uinputCecMap[CEC_USER_CONTROL_CODE_PREVIOUS_CHANNEL            ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_SOUND_SELECT                ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_INPUT_SELECT                ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION         ] = KEY_I;
		uinputCecMap[CEC_USER_CONTROL_CODE_HELP                        ] = KEY_HELP;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAGE_UP                     ] = KEY_PAGEUP;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAGE_DOWN                   ] = KEY_PAGEDOWN;
		uinputCecMap[CEC_USER_CONTROL_CODE_POWER                       ] = KEY_POWER;
		uinputCecMap[CEC_USER_CONTROL_CODE_VOLUME_UP                   ] = KEY_VOLUMEUP;
		uinputCecMap[CEC_USER_CONTROL_CODE_VOLUME_DOWN                 ] = KEY_VOLUMEDOWN;
		uinputCecMap[CEC_USER_CONTROL_CODE_MUTE                        ] = KEY_MUTE;
		uinputCecMap[CEC_USER_CONTROL_CODE_PLAY                        ] = KEY_P;
		uinputCecMap[CEC_USER_CONTROL_CODE_STOP                        ] = KEY_X;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAUSE                       ] = KEY_SPACE;
		uinputCecMap[CEC_USER_CONTROL_CODE_RECORD                      ] = KEY_RECORD;
		uinputCecMap[CEC_USER_CONTROL_CODE_REWIND                      ] = KEY_R;
		uinputCecMap[CEC_USER_CONTROL_CODE_FAST_FORWARD                ] = KEY_F;
		uinputCecMap[CEC_USER_CONTROL_CODE_EJECT                       ] = KEY_EJECTCD;
		uinputCecMap[CEC_USER_CONTROL_CODE_FORWARD                     ] = KEY_DOT;
		uinputCecMap[CEC_USER_CONTROL_CODE_BACKWARD                    ] = KEY_COMMA;
		uinputCecMap[CEC_USER_CONTROL_CODE_STOP_RECORD                 ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAUSE_RECORD                ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_ANGLE                       ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_SUB_PICTURE                 ] = KEY_L;
		uinputCecMap[CEC_USER_CONTROL_CODE_VIDEO_ON_DEMAND             ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_ELECTRONIC_PROGRAM_GUIDE    ] = KEY_E;
		uinputCecMap[CEC_USER_CONTROL_CODE_TIMER_PROGRAMMING           ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_INITIAL_CONFIGURATION       ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_PLAY_FUNCTION               ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAUSE_PLAY_FUNCTION         ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_RECORD_FUNCTION             ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_PAUSE_RECORD_FUNCTION       ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_STOP_FUNCTION               ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_MUTE_FUNCTION               ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_RESTORE_VOLUME_FUNCTION     ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_TUNE_FUNCTION               ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_SELECT_MEDIA_FUNCTION       ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_SELECT_AV_INPUT_FUNCTION    ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_SELECT_AUDIO_INPUT_FUNCTION ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_POWER_TOGGLE_FUNCTION       ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_POWER_OFF_FUNCTION          ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_POWER_ON_FUNCTION           ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_F1_BLUE                     ] = KEY_F4;
		uinputCecMap[CEC_USER_CONTROL_CODE_F2_RED                      ] = KEY_F1;
		uinputCecMap[CEC_USER_CONTROL_CODE_F3_GREEN                    ] = KEY_F2;
		uinputCecMap[CEC_USER_CONTROL_CODE_F4_YELLOW                   ] = KEY_F3;
		uinputCecMap[CEC_USER_CONTROL_CODE_F5                          ] = 0;
		uinputCecMap[CEC_USER_CONTROL_CODE_DATA                        ] = KEY_V;
		uinputCecMap[CEC_USER_CONTROL_CODE_AN_RETURN                   ] = KEY_ESC;
		uinputCecMap[CEC_USER_CONTROL_CODE_AN_CHANNELS_LIST            ] = KEY_LIST;
	}

	return uinputCecMap;
}

int Main::onCecLogMessage(const cec_log_message &message) {
	LOG4CPLUS_DEBUG(logger, "Main::onCecLogMessage(" << message << ")");
	return 1;
}

int Main::onCecKeyPress(const cec_keypress &key) {
	LOG4CPLUS_DEBUG(logger, "Main::onCecKeyPress(" << key << ")");

	int uinputKey = 0;

	// Check bounds and find uinput code for this cec keypress
	if (key.keycode >= 0 && key.keycode <= CEC_USER_CONTROL_CODE_MAX)
		uinputKey = cec2sdl[key.keycode];

	if (uinputKey != 0) {
		LOG4CPLUS_DEBUG(logger, "sent " << uinputKey);

		#if LIBCEC_VERSION_CURRENT < 0x2005
		uinput.send_event(EV_KEY, uinputKey, key.duration == 0 ? EV_KEY_PRESSED : EV_KEY_RELEASED);
		#else
		uinput.send_event(EV_KEY, uinputKey, key.duration == 0 ? EV_KEY_REPEAT : EV_KEY_RELEASED);
		#endif
		uinput.sync();
		
		if (uinputKey == KEY_F4) {
			LOG4CPLUS_DEBUG(logger, "sent " << uinputKey);

			#if LIBCEC_VERSION_CURRENT < 0x2005
			uinput.send_event(EV_KEY, cec2sdl[key2cec["start"]], key.duration == 0 ? EV_KEY_PRESSED : EV_KEY_RELEASED);
			uinput.send_event(EV_KEY, cec2sdl[key2cec["select"]], key.duration == 0 ? EV_KEY_PRESSED : EV_KEY_RELEASED);
			#else
			uinput.send_event(EV_KEY, cec2sdl[key2cec["start"]], key.duration == 0 ? EV_KEY_REPEAT : EV_KEY_RELEASED);
			uinput.send_event(EV_KEY, cec2sdl[key2cec["select"]], key.duration == 0 ? EV_KEY_REPEAT : EV_KEY_RELEASED);
			#endif
			uinput.sync();
		}
	}

	return 1;
}

int Main::onCecCommand(const cec_command & command) {
	LOG4CPLUS_DEBUG(logger, "Main::onCecCommand(" << command << ")");
	return 1;
}


int Main::onCecConfigurationChanged(const libcec_configuration & configuration) {
	LOG4CPLUS_DEBUG(logger, "Main::onCecConfigurationChanged(" << configuration << ")");
	return 1;
}

int main (int argc, char *argv[]) {

    BasicConfigurator config;
    config.configure();

    int loglevel = 0;

	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help,h",    "show help message")
	    ("version,V", "show version (and exit)")
	    ("daemon,d",  "daemon mode, run in background")
	    ("list,l",    "list available CEC adapters and devices")
	    ("verbose,v", accumulator<int>(&loglevel)->implicit_value(1), "verbose output (use -vv for more)")
	    ("quiet,q",   "quiet output (print almost nothing)")
	    ("donotactivate,a", "do not activate device on startup")
	    ("usb", po::value<string>(), "USB adapter path (as shown by --list)")
		("config,c", po::value<string>(), "emulationstation input file")
	;

	po::positional_options_description p;
	p.add("usb", 1);
	p.add("config", 1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "Usage: " << argv[0] << " [options] [usb]" << endl << endl;
	    cout << desc << endl;
	    return 0;
	}

	if (vm.count("version")) {
		cout << VERSION << endl;
		const char *str2 = "transmission-daemon";
		bool running = Main::isRunning(str2);
		if (running){
			cout << "is Running()\n";
		} else {
			cout << "is NOT Running()\n";
		}
		return 0;
	}

	if(vm.count("quiet")) {
		loglevel = -1;
	} else {
		loglevel = min(loglevel, 2);
	}

	Logger root = Logger::getRoot();
	switch (loglevel) {
		case 2:  root.setLogLevel(TRACE_LOG_LEVEL); break;
		case 1:  root.setLogLevel(DEBUG_LOG_LEVEL); break;
		default: root.setLogLevel(INFO_LOG_LEVEL); break;
		case -1: root.setLogLevel(FATAL_LOG_LEVEL); break;
	}

	try {
		// Create the main
		Main & main = Main::instance();

		if (vm.count("donotactivate")) {
			main.setMakeActive(false);
		}

		if (vm.count("list")) {
			main.listDevices();
			return 0;
		}

		if (vm.count("usb")) {
			cout << vm["usb"].as< string >() << endl;
		}

		if (vm.count("daemon")) {
			daemon(0, 0);
		}
		
		string configFile = "";
		if (vm.count("config")) {
			configFile = vm["config"].as< string >();
		}
		main.loadCECFromXML(configFile);
		
			
		// for (auto&& it : uinputCecMap)
		// std::cout << it << ' ';
			
		vector<int> v;
		// // for(auto it: myMap){
		//main.loadCECFromXML();
		for(auto&& it : cec2sdl){
		  v.push_back(it.first);
		  cout << it.first << "-->" << it.second <<"\n";
		// for(map<int,int>::iterator it = myMap.begin(); it != myMap.end(); ++it) {
		  // v.push_back(it->first);
		  // cout << it->first << "-->" << it->second <<"\n";
		}
		
		
		main.loop();

	} catch (std::exception & e) {
		cerr << e.what() << endl;
		return -1;
	}

	return 0;
}
bool Main::loadCECFromXML(string configFile)
{
	sdl2input[SDLK_UNKNOWN] = KEY_UNKNOWN;
	sdl2input[SDLK_BACKSPACE] = KEY_BACKSPACE;
	sdl2input[SDLK_TAB] = KEY_TAB;
	sdl2input[SDLK_CLEAR] = KEY_CLEAR;
	sdl2input[SDLK_RETURN] = KEY_ENTER;
	sdl2input[SDLK_PAUSE] = KEY_PAUSE;
	sdl2input[SDLK_ESCAPE] = KEY_ESC;
	sdl2input[SDLK_SPACE] = KEY_SPACE;
	sdl2input[SDLK_DOLLAR] = KEY_DOLLAR;
	sdl2input[SDLK_LEFTPAREN] = KEY_KPLEFTPAREN;
	sdl2input[SDLK_RIGHTPAREN] = KEY_KPRIGHTPAREN;
	sdl2input[SDLK_ASTERISK] = KEY_KPASTERISK;
	sdl2input[SDLK_PLUS] = KEY_KPPLUS;
	sdl2input[SDLK_COMMA] = KEY_COMMA;
	sdl2input[SDLK_MINUS] = KEY_MINUS;
	sdl2input[SDLK_PERIOD] = KEY_DOT;
	sdl2input[SDLK_SLASH] = KEY_SLASH;
	sdl2input[SDLK_0] = KEY_0;
	sdl2input[SDLK_1] = KEY_1;
	sdl2input[SDLK_2] = KEY_2;
	sdl2input[SDLK_3] = KEY_3;
	sdl2input[SDLK_4] = KEY_4;
	sdl2input[SDLK_5] = KEY_5;
	sdl2input[SDLK_6] = KEY_6;
	sdl2input[SDLK_7] = KEY_7;
	sdl2input[SDLK_8] = KEY_8;
	sdl2input[SDLK_9] = KEY_9;
	sdl2input[SDLK_SEMICOLON] = KEY_SEMICOLON;
	sdl2input[SDLK_EQUALS] = KEY_EQUAL;
	sdl2input[SDLK_QUESTION] = KEY_QUESTION;
	sdl2input[SDLK_LEFTBRACKET] = KEY_LEFTBRACE;
	sdl2input[SDLK_BACKSLASH] = KEY_BACKSLASH;
	sdl2input[SDLK_RIGHTBRACKET] = KEY_RIGHTBRACE;
	sdl2input[SDLK_a] = KEY_A;
	sdl2input[SDLK_b] = KEY_B;
	sdl2input[SDLK_c] = KEY_C;
	sdl2input[SDLK_d] = KEY_D;
	sdl2input[SDLK_e] = KEY_E;
	sdl2input[SDLK_f] = KEY_F;
	sdl2input[SDLK_g] = KEY_G;
	sdl2input[SDLK_h] = KEY_H;
	sdl2input[SDLK_i] = KEY_I;
	sdl2input[SDLK_j] = KEY_J;
	sdl2input[SDLK_k] = KEY_K;
	sdl2input[SDLK_l] = KEY_L;
	sdl2input[SDLK_m] = KEY_M;
	sdl2input[SDLK_n] = KEY_N;
	sdl2input[SDLK_o] = KEY_O;
	sdl2input[SDLK_p] = KEY_P;
	sdl2input[SDLK_q] = KEY_Q;
	sdl2input[SDLK_r] = KEY_R;
	sdl2input[SDLK_s] = KEY_S;
	sdl2input[SDLK_t] = KEY_T;
	sdl2input[SDLK_u] = KEY_U;
	sdl2input[SDLK_v] = KEY_V;
	sdl2input[SDLK_w] = KEY_W;
	sdl2input[SDLK_x] = KEY_X;
	sdl2input[SDLK_y] = KEY_Y;
	sdl2input[SDLK_z] = KEY_Z;
	sdl2input[SDLK_DELETE] = KEY_DELETE;
	sdl2input[SDLK_KP_PERIOD] = KEY_KPDOT;
	sdl2input[SDLK_KP_DIVIDE] = KEY_KPSLASH;
	sdl2input[SDLK_KP_MULTIPLY] = KEY_KPASTERISK;
	sdl2input[SDLK_KP_MINUS] = KEY_KPMINUS;
	sdl2input[SDLK_KP_PLUS] = KEY_KPPLUS;
	sdl2input[SDLK_KP_ENTER] = KEY_KPENTER;
	sdl2input[SDLK_KP_EQUALS] = KEY_KPEQUAL;
	sdl2input[SDLK_UP] = KEY_UP;
	sdl2input[SDLK_DOWN] = KEY_DOWN;
	sdl2input[SDLK_RIGHT] = KEY_RIGHT;
	sdl2input[SDLK_LEFT] = KEY_LEFT;
	sdl2input[SDLK_INSERT] = KEY_INSERT;
	sdl2input[SDLK_HOME] = KEY_HOME;
	sdl2input[SDLK_END] = KEY_END;
	sdl2input[SDLK_PAGEUP] = KEY_PAGEUP;
	sdl2input[SDLK_PAGEDOWN] = KEY_PAGEDOWN;
	sdl2input[SDLK_F1] = KEY_F1;
	sdl2input[SDLK_F2] = KEY_F2;
	sdl2input[SDLK_F3] = KEY_F3;
	sdl2input[SDLK_F4] = KEY_F4;
	sdl2input[SDLK_F5] = KEY_F5;
	sdl2input[SDLK_F6] = KEY_F6;
	sdl2input[SDLK_F7] = KEY_F7;
	sdl2input[SDLK_F8] = KEY_F8;
	sdl2input[SDLK_F9] = KEY_F9;
	sdl2input[SDLK_F10] = KEY_F10;
	sdl2input[SDLK_F11] = KEY_F11;
	sdl2input[SDLK_F12] = KEY_F12;
	sdl2input[SDLK_F13] = KEY_F13;
	sdl2input[SDLK_F14] = KEY_F14;
	sdl2input[SDLK_F15] = KEY_F15;
	sdl2input[SDLK_CAPSLOCK] = KEY_CAPSLOCK;
	sdl2input[SDLK_RSHIFT] = KEY_RIGHTSHIFT;
	sdl2input[SDLK_LSHIFT] = KEY_LEFTSHIFT;
	sdl2input[SDLK_RCTRL] = KEY_RIGHTCTRL;
	sdl2input[SDLK_LCTRL] = KEY_LEFTCTRL;
	sdl2input[SDLK_RALT] = KEY_RIGHTALT;
	sdl2input[SDLK_LALT] = KEY_LEFTALT;
	sdl2input[SDLK_MODE] = KEY_MODE;
	sdl2input[SDLK_HELP] = KEY_HELP;
	sdl2input[SDLK_MENU] = KEY_MENU;
	sdl2input[SDLK_POWER] = KEY_POWER;
	sdl2input[SDLK_UNDO] = KEY_UNDO;

	//Populate some defaults
	key2cec["a"] = CEC_USER_CONTROL_CODE_SELECT;
	key2cec["b"] = CEC_USER_CONTROL_CODE_EXIT;
	key2cec["down"] = CEC_USER_CONTROL_CODE_DOWN;
	key2cec["left"] = CEC_USER_CONTROL_CODE_LEFT;
	key2cec["right"] = CEC_USER_CONTROL_CODE_RIGHT;
	key2cec["up"] = CEC_USER_CONTROL_CODE_UP;
	key2cec["pageup"] = CEC_USER_CONTROL_CODE_F2_RED;
	key2cec["pagedown"] = CEC_USER_CONTROL_CODE_F3_GREEN;
	key2cec["start"] = CEC_USER_CONTROL_CODE_SETUP_MENU;
	key2cec["select"] = CEC_USER_CONTROL_CODE_F4_YELLOW;
	key2cec["exit"] = CEC_USER_CONTROL_CODE_CLEAR;
	key2cec["x"] = -1;
	key2cec["y"] = -1;
	
	cec2sdl[key2cec["a"]]=sdl2input[SDLK_RETURN];
	cec2sdl[key2cec["b"]]=sdl2input[SDLK_ESCAPE];
	cec2sdl[key2cec["down"]]=sdl2input[SDLK_DOWN];
	cec2sdl[key2cec["left"]]=sdl2input[SDLK_LEFT];
	cec2sdl[key2cec["right"]]=sdl2input[SDLK_RIGHT];
	cec2sdl[key2cec["up"]]=sdl2input[SDLK_UP];
	cec2sdl[key2cec["pageup"]]=sdl2input[SDLK_RIGHTBRACKET];
	cec2sdl[key2cec["pagedown"]]=sdl2input[SDLK_LEFTBRACKET];
	cec2sdl[key2cec["start"]]=sdl2input[SDLK_F1];
	cec2sdl[key2cec["select"]]=sdl2input[SDLK_F2];
	cec2sdl[key2cec["exit"]] = sdl2input[SDLK_F4];

	cout << "path:" << configFile << "\n";
	if(configFile=="")
		configFile = "/home/pi/.emulationstation/es_input.cfg";
	
	cout << "path:" << configFile << "\n";
	if(!fs::exists(configFile))
		return false;
	
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file(configFile.c_str());

	if(!res)
	{
		LOG4CPLUS_TRACE_STR(logger, "Main::listDevices()");
		return false;
	}

	pugi::xml_node root = doc.child("inputList");
	if(!root)
		return false;

	pugi::xml_node node = root.find_child_by_attribute("inputConfig", "deviceName", "cec");
	if(node)
	{
		for(pugi::xml_node input = node.child("input"); input; input = input.next_sibling("input"))
		{
			string name = input.attribute("name").as_string();
			int id = input.attribute("id").as_int();
			key2cec[name] = id;
		}
	}

	node = root.find_child_by_attribute("inputConfig", "deviceName", "Keyboard");
	if(node)
	{
		for(pugi::xml_node input = node.child("input"); input; input = input.next_sibling("input"))
		{
			string name = input.attribute("name").as_string();
			int id = input.attribute("id").as_int();
			cec2sdl[key2cec[name]] = sdl2input[id];
		}
		cec2sdl[key2cec["exit"]] = sdl2input[SDLK_F4];
	}
	cout << "loadfromXML()\n";
	return true;
}

 bool Main::isRunning(const char* name)  
 {  
   cout << "isRunning()\n";
   char command[32];  
   sprintf(command, "pgrep %s > /dev/null", name);  
   return 0 == system(command);  
 }  
