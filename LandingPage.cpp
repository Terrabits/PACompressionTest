#include "LandingPage.h"
#include "ui_LandingPage.h"


// Project
#include "Settings.h"

// RsaToolbox
// using namespace RsaToolbox;

// Qt
#include <QMessageBox>
#include <QFileDialog>


LandingPage::LandingPage(QWidget *parent) :
    WizardPage(parent),
    ui(new Ui::LandingPage)
{
    ui->setupUi(this);
}

LandingPage::~LandingPage()
{
    delete ui;
}

void LandingPage::initialize() {
    while (wizard()->numberOfPages() > 1) {
        pageAt(1)->resetContents();
        wizard()->removePage(1);
    }
    // _data.reset();
    _url.clear();
    setNextIndex(-1);
    clearHistory();
    buttons()->hide();
    breadCrumbs()->hide();
}
bool LandingPage::isReadyForNext() {
    return !_url.isEmpty();
}
int LandingPage::next() {
    if (_url == "Plot")
        initPlot();
    else if (_url == "Edit")
        initOpen();
    else
        initNew();

    buttons()->show();
    breadCrumbs()->show();
    return nextIndex();
}

bool LandingPage::open() {
    QString result;
    result = QFileDialog::getOpenFileName(this,
                                          "Open Test Plan...",
                                          _lastPath,
                                          "PA Compression Test Plan (*.ctp)");
    if (result.isEmpty())
        return false;

    _lastPath.setFromFilePath(result);
    if (/*!_data.open(result)*/ false) {
        QMessageBox::critical(this,
                              APP_NAME,
                              "Could not open test plan:\n"
                              + result);
        return false;
    }

    return true;
}

void LandingPage::initNew() {
    // _data.reset();

    // Page 1
    //    setNextIndex(1);
    //    QScopedPointer<StandardsPage> standardsPage(new StandardsPage());
    //    standardsPage->setData(&_data);
    //    standardsPage->setName("Standards");
    //    standardsPage->setNextIndex(2);
    //    wizard()->addPage(standardsPage.take());

    // Page 2
    //    QScopedPointer<TestsPage> testsPage(new TestsPage());
    //    testsPage->setData(&_data);
    //    testsPage->setName("Tests");
    //    testsPage->setNextIndex(0);
    //    wizard()->addPage(testsPage.take());

    // For now...
    setNextIndex(0);
}
void LandingPage::initOpen() {
//    if (!open()) {
//        setNextIndex(0);
//        return;
//    }

    // Page 1
    //    setNextIndex(1);
    //    QScopedPointer<StandardsPage> standardsPage(new StandardsPage());
    //    standardsPage->setData(&_data);
    //    standardsPage->setName("Standards");
    //    standardsPage->setNextIndex(2);
    //    wizard()->addPage(standardsPage.take());

    // Page 2
    //    QScopedPointer<TestsPage> testsPage(new TestsPage());
    //    testsPage->setData(&_data);
    //    testsPage->setName("Tests");
    //    testsPage->setNextIndex(0);
    //    wizard()->addPage(testsPage.take());
}
void LandingPage::initPlot() {
//    if (!open()) {
//        setNextIndex(0);
//        return;
//    }

    // Page 1
//    setNextIndex(1);
//    QScopedPointer<VnaPage> vnaPage(new VnaPage());
//    vnaPage->setData(&_data);
//    vnaPage->setName("Connect");
//    vnaPage->setNextIndex(2);
//    wizard()->addPage(vnaPage.take());

    // Page 2
//    QScopedPointer<ChannelPage> channelPage(new ChannelPage());
//    channelPage->setData(&_data);
//    channelPage->setName("Setup");
//    channelPage->setNextIndex(3);
//    wizard()->addPage(channelPage.take());

    // Page 3
//    QScopedPointer<TestPlanPage> testPlanPage(new TestPlanPage());
//    testPlanPage->setData(&_data);
//    testPlanPage->setName("Run");
//    testPlanPage->setNextIndex(0);
//    wizard()->addPage(testPlanPage.take());

    // For now...
    setNextIndex(0);
}

void LandingPage::linkActivated(const QString &url) {
    _url = url;
    wizard()->next();
}
