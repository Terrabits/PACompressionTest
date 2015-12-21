#ifndef SETTINGS_H
#define SETTINGS_H


// RsaToolbox
#include <Definitions.h>
#include <General.h>
#include <GenericBus.h>


// Application settings
const QString APP_NAME = "R&S PA Compression Test";
const QString APP_VERSION = "1.5.1";
const QString MANUFACTURER_FOLDER = "Rohde-Schwarz";
const QString APP_FOLDER = "PA Compression Test";
const QString APP_DESCRIPTION = "Measure the compression point of a Power Amplifier";
const QString CONTACT_INFO = "<html><head/><body><p>Nick Lalic<br/>VNA Software Developer<br/>Cupertino, CA USA<br/>+1 424 200 2846<br/>nick.lalic@rsa.rohde-schwarz.com<br/><a href=\"http://vna.rs-us.net\"><span style=\"text-decoration: underline; color:#0000ff;\">http://vna.rs-us.net</span></a></p></body></html>";

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
const RsaToolbox::ConnectionType CONNECTION_TYPE = RsaToolbox::ConnectionType::TCPIP_CONNECTION;
#ifdef DEBUG_MODE
//const QString INSTRUMENT_ADDRESS = "127.0.0.1";
const QString INSTRUMENT_ADDRESS = "192.168.1.111";
#else
const QString INSTRUMENT_ADDRESS = "127.0.0.1";
#endif
const unsigned int TIMEOUT_MS = 1000;

// Keys
const QString START_FREQUENCY_KEY        = "START_FREQUENCY";
const QString STOP_FREQUENCY_KEY         = "STOP_FREQUENCY";
const QString FREQUENCY_POINTS_KEY       = "FREQUENCY_POINTS";
const QString IF_BW_KEY                  = "IF_BW";

const QString START_POWER_KEY            = "START_POWER";
const QString STOP_POWER_KEY             = "STOP_POWER";
const QString POWER_POINTS_KEY           = "POWER_POINTS";
const QString COMPRESSION_LEVEL_KEY      = "COMPRESSION_LEVEL";
const QString IS_GAIN_EXPANSION_KEY      = "IS_GAIN_EXPANSION";
const QString IS_STOP_AT_COMPRESSION_KEY = "IS_STOP_AT_COMPRESSION";
const QString POST_CONDITION_KEY         = "POST_CONDITION";

const QString CHANNEL_KEY                = "CHANNEL";
const QString OUTPUT_PORT_KEY            = "OUTPUT_PORT";
const QString INPUT_PORT_KEY             = "INPUT_PORT";
const QString SWEEP_TYPE_KEY             = "SWEEP_TYPE";

const QString TRACES_KEY                 = "TRACES";

const QString EXPORT_PATH_KEY            = "EXPORT_PATH";
#endif

