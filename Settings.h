#ifndef SETTINGS_H
#define SETTINGS_H


// Rsa
#include "Definitions.h"
#include "General.h"


// Application settings
const QString APP_NAME = "R&S PA Compression Test";
const QString APP_VERSION = "1.2";
const QString MANUFACTURER_FOLDER = "Rohde-Schwarz";
const QString APP_FOLDER = "PA Compression Test";

#ifdef DEBUG_MODE
const QDir dataDir(SOURCE_DIR);
const QString LOG_FILENAME = dataDir.filePath("DEBUG_LOGFILE.txt");
const QString KEY_PATH = dataDir.filePath("Keys");
#else
const QDir dataDir(RsaToolbox::GetAppDataPath(MANUFACTURER_FOLDER, APP_FOLDER));
const QString LOG_FILENAME = dataDir.filePath("R&S PA Compression Test Log.txt");
const QString KEY_PATH = dataDir.filePath("Settings");
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
const char SWEEP_MODE[] = "SWEEP_MODE";
const char OPEN_PATH[] = "OPEN_PATH";
const char SAVE_PATH[] = "SAVE_PATH";
const char EXPORT_PATH[] = "EXPORT_PATH";
const char PRINT_PATH[] = "PRINT_PATH";


#endif

