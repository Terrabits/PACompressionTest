

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"
#include "MiniPage.h"

// RsaToolbox
#include "About.h"
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QApplication>
#include <QMessageBox>
#include <QDebug>


bool isAboutMenu(int argc, char *argv[]);
bool isConnected(Vna &vna);
bool isKnownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (isAboutMenu(argc, argv)) {
        return 0;
    }

    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
    vna.startLog(LOG_FILENAME, APP_NAME, APP_VERSION);
    if (!isConnected(vna) || !isKnownModel(vna)) {
        return 1;
    }

    Keys keys(KEY_PATH);
    Q_INIT_RESOURCE(CoreResources);

    MiniPage miniPage;
    MainWindow w(vna, keys);
    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint);
    w.setMiniPage(&miniPage);
    w.show();
    return app.exec();
}

bool isAboutMenu(int argc, char *argv[]) {
    if (argc != 2)
        return false;

    QString arg(argv[1]);
    arg = arg.trimmed().toUpper();
    if (arg == "-ABOUT" || arg == "--ABOUT") {
        Q_INIT_RESOURCE(AboutResources);
        About about;
        about.setAppName(APP_NAME);
        about.setVersion(APP_VERSION);
        about.setDescription(APP_DESCRIPTION);
        about.setContactInfo(CONTACT_INFO);
        about.exec();
        return true;
    }

    return false;
}
bool isConnected(Vna &vna) {
    if (vna.isOpen() && vna.isResponding()) {
        return true;
    }

    QString msg = "Instrument not found.\n";
    msg += "Please run this application on the instrument.";
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return false;
}
bool isKnownModel(Vna &vna) {
    if (vna.properties().isKnownModel()) {
        return true;
    }

    QString msg = "Instrument not recognized.\n";
    msg += "Please use %1 with a Rohde & Schwarz VNA";
    msg = msg.arg(APP_NAME);
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return false;
}
