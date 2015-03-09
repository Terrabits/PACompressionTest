

// PaCompressionTest
#include "Settings.h"
#include "LandingPage.h"

// RsaToolbox
#include "Log.h"
#include "Vna.h"
#include "Keys.h"
#include "Wizard.h"
#include "WizardPage.h"
using namespace RsaToolbox;

// Qt
#include <QString>
#include <QApplication>
#include <QMessageBox>


bool isNoConnection(Vna &vna);
bool isUnknownModel(Vna &vna);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Log log(LOG_FILENAME, APP_NAME, APP_VERSION);
    log.printHeader();

//    Vna vna(CONNECTION_TYPE, INSTRUMENT_ADDRESS);
//    vna.useLog(&log);
//    vna.printInfo();

    Keys keys(KEY_PATH);

//    if (isNoConnection(vna) || isUnknownModel(vna))
//        return 0;

    LandingPage *landingPage = new LandingPage();
    landingPage->setName("Landing Page");

    Wizard wizard;
    wizard.addPage(landingPage);
    wizard.show();

    return a.exec();
}


bool isNoConnection(Vna &vna) {
    if (vna.isDisconnected()) {
        QString error_message
                = "Instrument not found. Please run this application on a Rohde & Schwarz VNA.";
        QMessageBox::critical(NULL,
                              "R&S PA Compression Test",
                              error_message);
        vna.print(error_message);
        return true;
    }
    else
        return false;
}
bool isUnknownModel(Vna &vna) {
    if (vna.properties().isUnknownModel()) {
        QString error_message(QString("VNA not recognized.\n")
                              + "Please use R&S PA Compression Test with a Rohde & Schwarz instrument");
        QMessageBox::critical(NULL,
                              "R&S PA Compression Test",
                              error_message);
        vna.print(error_message);
        return true;
    }
    else
        return false;
}

