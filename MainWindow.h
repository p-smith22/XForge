#pragma once

#include "XFoil_Interface.h"
#include "Plane.h"

#include <QMainWindow>
#include <QtCharts>
#include <QThread>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QLabel>


// Runs XFOIL on a background thread so the GUI doesn't freeze.
class XFoilWorker : public QObject {
    Q_OBJECT
public:
    XFoilWorker(XFoil_Interface* xf) : m_xf(xf) {}
public slots:
    void run() { m_xf->runXFOIL(); emit finished(); }
signals:
    void finished();
public:
    XFoil_Interface* m_xf;
};


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    // Functions
    void setupUI();
    void plot(QChartView* view, QLineSeries* series,
              const std::vector<double>& x, const std::vector<double>& y);

    // Slots
    void onSimulate();
    void onSimulationFinished();

    // The simulation object
    Plane m_plane;

    // 2D inputs
    QLineEdit       *m_naca;
    QDoubleSpinBox  *m_aStart, *m_aEnd, *m_aStep;
    QDoubleSpinBox  *m_re, *m_mach;
    QSpinBox        *m_ncrit;
    QCheckBox       *m_visc;

    // 3D inputs
    QDoubleSpinBox  *m_chord, *m_span, *m_eOswald, *m_rho, *m_vinf;

    // Run button
    QPushButton     *m_runBtn;

    // Charts
    QChartView      *m_clView, *m_cdView, *m_cmView, *m_polarView;
    QChartView      *m_ld2dView, *m_ld3dView, *m_cl3dView;
    QLineSeries     *m_clSer,  *m_cdSer,  *m_cmSer,  *m_polarSer;
    QLineSeries     *m_ld2dSer, *m_ld3dSer, *m_cl3dSer;

    // Numerical data
    QLabel  *m_lblClMax, *m_lblCdMin, *m_lblLd2, *m_lblStall, *m_lblCm;
    QLabel  *m_lblAR, *m_lblA3D, *m_lblLd3, *m_lblLdOpt;

    // Log
    QTextEdit       *m_log;

    // Threading
    QThread         *m_thread;
    XFoilWorker     *m_worker;
};