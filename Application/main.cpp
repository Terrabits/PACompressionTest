#include "helpers.hpp"

// core
#include "Settings.h"
#include "mainwindow.h"
#include "MiniPage.h"

// Logging
#include "logging.hpp"

// RsaToolbox
#include "About.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QApplication>


// constants
const boost::log::trivial::severity_level level = boost::log::trivial::trace;
const char *main_log_filename = MAIN_LOG_FILENAME_QBA.constData();


int main(int argc, char *argv[])
{
    // init boost log
    init_logging(level, main_log_filename);

    // log app, version
    QString message;
    message = "%1 %2";
    message = message.arg(APP_NAME);
    message = message.arg(APP_VERSION);
    QByteArray bytes;
    bytes = message.toUtf8();
    LOG(info) << bytes.constData();

    LOG(info) << "Creating Qt Application";
    QApplication app(argc, argv);

    if (isAboutMenu(argc, argv)) {
      return 0;
    }

    LOG(info) << "Connecting to VNA";
    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);

    LOG(info) << "Loading application settings";
    Keys keys(KEY_PATH);

    LOG(info) << "Checking VNA connection, make, and model";
    if (isNoConnection(vna) || isUnknownModel(vna)) {
      return(0);
    }

    LOG(info) << "Loading application resources (qrc)";
    Q_INIT_RESOURCE(CoreResources);

    LOG(info) << "Creating mini page view";
    MiniPage miniPage;

    LOG(info) << "Creating MainWindow";
    MainWindow w(vna, keys);
    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint);
    w.setMiniPage(&miniPage);
    w.show();

    LOG(info) << "Starting Qt application event loop";
    return app.exec();
}
