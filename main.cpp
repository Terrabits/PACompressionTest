

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"

// Qt
#include <QApplication>
#include <QMessageBox>
#include <QLocale>

// Rsa
#include "Definitions.h"
#include "General.h"
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get key instance
    Keys keys(KEY_PATH);

    // Log
    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();
    
    // Create, display window
    MainWindow w(&keys, &log);
    w.show();
    return app.exec();
}

