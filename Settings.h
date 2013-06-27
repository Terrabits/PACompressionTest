#ifndef SETTINGS_H
#define SETTINGS_H


// Rsa
#include "Definitions.h"
#include "General.h"


// Application settings
const char APP_NAME[] = "RSA PA Compression Test";
const char APP_VERSION[] = "0.9.4"; //http://semver.org/
const char APP_FOLDER[] = "RSA PA Compression Test";

#ifdef LOCAL_LOG
const char LOG_PATH[] = SOURCE_DIR;
const char LOG_FILENAME[] = "DEBUG_LOGFILE.txt";
#else
const QString LOG_PATH = RsaToolbox::GetAppDataPath(APP_FOLDER);
const char LOG_FILENAME[] = "RSA PA Compression Test Log.txt";
#endif

// Connection settings
const unsigned int TIMEOUT_MS = 1000;

// Keys
const char CONNECTION_ADDRESS[] = "CONNECTION_ADDRESS";
const char CONNECTION_TYPE[] = "CONNECTION_TYPE";
const char START_FREQ[] = "START_FREQ";
const char START_FREQ_UNITS[] = "START_FREQ_UNITS";
const char STOP_FREQ[] = "STOP_FREQ";
const char STOP_FREQ_UNITS[] = "STOP_FREQ_PREFIX";
const char FREQ_POINTS[] = "FREQ_POINTS";
const char IF_BW[] = "IF_BW";
const char IF_BW_UNITS[] = "IF_BW_PREFIX";
const char START_POWER[] = "START_POWER";
const char STOP_POWER[] = "STOP_POWER";
const char POWER_POINTS[] = "POWER_POINTS";
const char RECEIVE_ATTENUATION[] = "RECEIVE_ATTENUATION";
const char SOURCE_ATTENUATION[] = "SOURCE_ATTENUATION";
const char INPUT_PORT[] = "INPUT_PORT";
const char OUTPUT_PORT[] = "OUTPUT_PORT";
const char COMPRESSION_POINT[] = "COMPRESSION_POINT";
const char POST_CONDITION[] = "POST_CONDITION";
const char OPEN_PATH[] = "OPEN_PATH";
const char SAVE_PATH[] = "SAVE_PATH";
const char EXPORT_PATH[] = "EXPORT_PATH";
const char PRINT_PATH[] = "PRINT_PATH";


#endif

