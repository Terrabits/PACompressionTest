

// RsaPaCompressionTest
#include "Settings.h"
#include "mainwindow.h"

// RsaToolbox
#include "About.h"
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
using namespace RsaToolbox;

// Qt
#include <QApplication>
#include <QMessageBox>


bool isAboutMenu(int argc, char *argv[]);
bool isNoConnection(Vna &vna);
bool isUnknownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (isAboutMenu(argc, argv))
            return 0;

    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();

    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
    vna.useLog(&log);
    vna.printInfo();

    Keys keys(KEY_PATH);

    if (isNoConnection(vna) || isUnknownModel(vna))
            return(0);

    MainWindow w(vna, keys);
    w.setWindowFlags(w.windowFlags() | Qt::MSWindowsFixedSizeDialogHint | Qt::WindowStaysOnTopHint);
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
bool isNoConnection(Vna &vna) {
    if (vna.isConnected() && !vna.idString().isEmpty())
        return false;

    QString msg = "Instrument not found.\n";
    msg += "Please run this application on the instrument.";
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return true;
}
bool isUnknownModel(Vna &vna) {
    if (vna.properties().isKnownModel())
        return false;

    QString msg = "Instrument not recognized.\n";
    msg += "Please use %1 with a Rohde & Schwarz VNA";
    msg = msg.arg(APP_NAME);
    QMessageBox::critical(NULL,
                          APP_NAME,
                          msg);
    vna.print(msg);
    return true;
}
