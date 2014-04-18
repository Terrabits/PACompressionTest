

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
#include "Key.h"
using namespace RsaToolbox;


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Get key instance
    Key key(APP_FOLDER);
    
    // Create, display window
    MainWindow w(&key);
    w.show();
    return app.exec();
}

