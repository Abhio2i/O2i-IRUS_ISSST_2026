/* =============================================================================
 * FILE:         main.cpp
 * MODULE:       Application Entry Point
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 * STANDARD:     RTCA DO-178C / ED-12C, DAL B
 * COVERAGE:     Branch / Decision Coverage required (100% true/false paths)
 *
 * DESCRIPTION:  Application entry point – initialises QApplication, registers
 *               custom meta-types, shows splash screen, applies a dark theme
 *               stylesheet, installs a custom message handler for debug/log
 *               output, initialises the TDF (Tool Data Folder) structure,
 *               creates and shows the main window, and enters the Qt event loop.
 *
 * REQUIREMENTS: REQ-MAIN-030  Application initialisation
 *               REQ-MAIN-031  Register custom meta-types for Qt signals/slots
 *               REQ-MAIN-032  Apply global dark theme stylesheet
 *               REQ-MAIN-033  Install custom message handler for logging
 *               REQ-MAIN-034  Initialise TDF folder structure at startup
 *               REQ-MAIN-035  Show main window and start event loop
 *
 * AUTHOR:       Arti Rajpoot
 * REVIEWED BY:  [Reviewer Name], [Review Date] — SPR-MAIN-002
 *
 * COPYRIGHT:    Oxygen 2 Innovation (O2I). All rights reserved.
 *               Restricted circulation — defence simulation use only.
 * =============================================================================
 */
#include "GUI/mainwindow.h"
#include "GUI/splashwindow.h"
#include "core/Config/scenarioconfig.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QWindow>
#include <QElapsedTimer>
#include <QThread>
#include "core/Debug/console.h"
#include <string>
#include "Setup.h"
#include "GUI/Theme/theme.h"

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qRegisterMetaType<std::string>("std::string");
    QApplication a(argc, argv);
    qInstallMessageHandler(customMessageHandler);

    const int minSplashMs = 3000;

    QElapsedTimer splashTimer;
    splashTimer.start();

    // ---- Splash ----
    QPixmap pix(":/icons/images/map.png");
    SplashWindow splash(pix,
                        "ISSST",
                        QString("V_%1").arg(ScenarioConfig::software_version),
                        QString::fromUtf8("© Oxygen 2 Innovation (O2I). All rights reserved."));
    splash.show();
    splash.setProgress(5, "Starting application...");

    splash.setProgress(20, "Initializing TDF structure...");
    TDFManager::instance()->initializeTDFStructure();

    splash.setProgress(45, "Applying theme...");
    qApp->setStyle(QStyleFactory::create("Fusion"));
    ThemeManager::instance().installAutoTheme();
    ThemeManager::instance().applyPalette();

    splash.setProgress(70, "Loading main window...");
    MainWindow *w = new MainWindow;

    const int startPct = 70;
    while (splashTimer.elapsed() < minSplashMs) {
        int pct = startPct + (100 - startPct) * int(splashTimer.elapsed()) / minSplashMs;
        splash.setProgress(qMin(pct, 99), "Almost ready...");
        QThread::msleep(15);
    }
    splash.setProgress(100, "Ready");

    w->show();
    splash.close();
    a.processEvents();

    w->setWindowState((w->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    w->raise();
    w->activateWindow();
    QTimer::singleShot(0, w, [w]() {
        w->raise();
        w->activateWindow();
        if (w->windowHandle())
            w->windowHandle()->requestActivate();
    });

    return a.exec();
}
