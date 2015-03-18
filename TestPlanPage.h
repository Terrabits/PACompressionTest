#ifndef TESTPLANPAGE_H
#define TESTPLANPAGE_H


// Project
#include "TestPlan.h"

// RsaToolbox
#include "Vna.h"
#include "WizardPage.h"


namespace Ui {
class TestPlanPage;
}

class TestPlanPage : public RsaToolbox::WizardPage
{
    Q_OBJECT

public:
    explicit TestPlanPage(QWidget *parent = 0);
    ~TestPlanPage();

    virtual bool isReadyForNext();

public slots:

private slots:
    void on_testButton_clicked();

private:
    Ui::TestPlanPage *ui;

    TestPlan _plan;
    RsaToolbox::Vna _vna;
};

#endif // TESTPLANPAGE_H
