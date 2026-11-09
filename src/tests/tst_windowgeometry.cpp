#include <QtTest>
#include "windowgeometry.h"

class TestWindowGeometry : public QObject
{
    Q_OBJECT

private slots:
    void restore_data();
    void restore();
    void primaryWithOffset();
    void noScreens();
};

void TestWindowGeometry::restore_data()
{
    QTest::addColumn<QRect>("saved");
    QTest::addColumn<QList<QRect>>("screens");
    QTest::addColumn<QRect>("expected");

    const QRect primary(0, 0, 1920, 1040);
    const QList<QRect> single{primary};
    const QRect centered(560, 220, 800, 600);
    QTest::newRow("primary") << QRect(100, 100, 800, 600) << single << QRect(100, 100, 800, 600);
    QTest::newRow("removed-right") << QRect(2100, 100, 800, 600) << single << centered;
    QTest::newRow("removed-left") << QRect(-1800, 100, 800, 600) << single << centered;
    QTest::newRow("removed-above") << QRect(100, -1000, 800, 600) << single << centered;
    QTest::newRow("title-above-screen") << QRect(100, -100, 800, 600) << single << centered;
    QTest::newRow("taskbar-only") << QRect(100, 1045, 800, 600) << single << centered;
    QTest::newRow("tiny-visible-sliver") << QRect(1900, 100, 800, 600) << single << centered;
    QTest::newRow("partly-visible") << QRect(-200, 100, 800, 600) << single << QRect(-200, 100, 800, 600);
    QTest::newRow("oversized-removed") << QRect(2100, 100, 2560, 1440) << single << primary;
    QTest::newRow("negative-secondary") << QRect(-1800, 100, 800, 600)
        << QList<QRect>{primary, QRect(-1920, 0, 1920, 1080)} << QRect(-1800, 100, 800, 600);
    QTest::newRow("spanning-screens") << QRect(1700, 100, 800, 600)
        << QList<QRect>{primary, QRect(1920, 0, 1920, 1080)} << QRect(1700, 100, 800, 600);
    QTest::newRow("gap-between-screens") << QRect(2100, 100, 800, 600)
        << QList<QRect>{primary, QRect(3000, 0, 1920, 1080)} << centered;
}

void TestWindowGeometry::restore()
{
    QFETCH(QRect, saved);
    QFETCH(QList<QRect>, screens);
    QFETCH(QRect, expected);
    QCOMPARE(visibleWindowGeometry(saved, screens, screens.first()), expected);
}

void TestWindowGeometry::primaryWithOffset()
{
    const QRect primary(-1920, 40, 1920, 1040);
    QCOMPARE(visibleWindowGeometry(QRect(2100, 100, 800, 600), {primary}, primary),
             QRect(-1360, 260, 800, 600));
}

void TestWindowGeometry::noScreens()
{
    const QRect saved(100, 100, 800, 600);
    QCOMPARE(visibleWindowGeometry(saved, {}, QRect()), saved);
}

QTEST_GUILESS_MAIN(TestWindowGeometry)
#include "tst_windowgeometry.moc"
