#include "QuaZipTest.h"
#include <JlCompress.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

QuaZipTest::QuaZipTest(QObject *parent) : TestClass(parent)
{

}

void QuaZipTest::createZip() {
    QDir src(SOURCE_DIR);
    QDir fixtures = src;
    fixtures.cd("fixtures");

    const QString results = fixtures.filePath("results.zip");
    const QString resultsDir = fixtures.filePath("results");
    const QString file1   = fixtures.filePath("results/file1.txt");
    const QString file2   = fixtures.filePath("results/file2.txt");

    if (QFileInfo(results).exists()) {
        QFile(results).remove();
    }
    QVERIFY(!QFileInfo(results).exists());
    QVERIFY(QFileInfo(file1).exists());
    QVERIFY(QFileInfo(file2).exists());

    QVERIFY(JlCompress::compressDir(results, resultsDir));
    QVERIFY(QFileInfo(results).exists());
    QFile(results).remove();
}
