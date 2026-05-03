#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>
#include <QtCharts>
#include <QProgressBar>
#include <QTextEdit>
#include <QSplitter>
#include <QGroupBox>
#include <QComboBox>
#include <QStatusBar>
#include <QThread>
#include "XFoil_Interface.h"
#include "Plane.h"


// Worker thread so GUI doesn't freeze during XFOIL run.
// Still typed against the base class — Plane IS-A XFoil_Interface,
// so this works unchanged when we hand it &m_plane.
class XFoilWorker : public QObject {
    Q_OBJECT
public:
    explicit XFoilWorker(XFoil_Interface* xfoil, QObject* parent = nullptr)
        : QObject(parent), m_xfoil(xfoil) {}

public slots:
    void run() {
        m_xfoil->runXFOIL();
        emit finished();
    }

signals:
    void finished();

private:
    XFoil_Interface* m_xfoil;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onSimulate();
    void onSimulationFinished();
    void onAirfoilTypeChanged(int index);

private:
    // ---- UI setup helpers ----
    void setupUI();
    QWidget* buildInputPanel();
    QWidget* buildOutputPanel();
    void buildCharts();

    // ---- Plot update helpers (2D) ----
    void updateCharts();
    void updateCLvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CL);
    void updateCDvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CD);
    void updateCMvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CM);
    void updateCLvsCDPolar(const std::vector<double>& CL,
                           const std::vector<double>& CD);
    void updateLDvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CL,
                         const std::vector<double>& CD);

    // ---- Plot update helpers (3D, from Plane) ----
    void updateLD3D(const std::vector<double>& alpha,
                    const std::vector<double>& CL,
                    const std::vector<double>& CD);
    // Self-contained: takes XFOIL CL data + reads geometry from spinboxes,
    // does NOT depend on Plane::polar (avoids the shadowing bug).
    void updateSchrenk(const std::vector<double>& CL);
    void updateLiftingLine(const std::vector<double>& alpha);

    // Updates the numerical results panel
    void updateMetrics(const std::vector<double>& alpha,
                       const std::vector<double>& CL,
                       const std::vector<double>& CD,
                       const std::vector<double>& CM);

    void setChartTheme(QChart* chart);
    void setSimulating(bool simulating);

    // ---- Core object ----
    // Plane inherits from XFoil_Interface, so all the existing
    // setAirfoil/setAlpha/etc. setters are inherited.
    Plane m_plane;

    // ---- Input widgets (2D / XFOIL) ----
    QComboBox* m_airfoilType;
    QLineEdit* m_nacaCode;
    QDoubleSpinBox* m_alphaStart;
    QDoubleSpinBox* m_alphaEnd;
    QDoubleSpinBox* m_alphaStep;
    QDoubleSpinBox* m_reynolds;
    QDoubleSpinBox* m_mach;
    QSpinBox*       m_nCrit;
    QDoubleSpinBox* m_xtrTop;
    QDoubleSpinBox* m_xtrBot;
    QCheckBox*      m_viscous;
    QLineEdit*      m_outputFile;
    QPushButton*    m_simButton;
    QProgressBar*   m_progressBar;

    // ---- Input widgets (3D wing geometry) ----
    QDoubleSpinBox* m_chord;
    QDoubleSpinBox* m_wingspan;
    QDoubleSpinBox* m_epsilon;     // Oswald efficiency factor
    QDoubleSpinBox* m_rho;         // air density (kg/m^3)
    QDoubleSpinBox* m_vinf;        // freestream velocity (m/s)
    QSpinBox*       m_lltN;        // # of points for lifting-line solve

    // ---- Output widgets ----
    QTabWidget*     m_chartTabs;
    QChartView*     m_clAlphaView;
    QChartView*     m_cdAlphaView;
    QChartView*     m_cmAlphaView;
    QChartView*     m_polarView;
    QChartView*     m_ldView;
    QChartView*     m_schrenkView;     // NEW
    QChartView*     m_lltView;         // NEW
    QTextEdit*      m_logOutput;

    // ---- Chart series ----
    QLineSeries*    m_clSeries;
    QLineSeries*    m_cdSeries;
    QLineSeries*    m_cmSeries;
    QLineSeries*    m_polarSeries;
    QLineSeries*    m_ldSeries;        // 2D L/D
    QLineSeries*    m_ld3DSeries;      // NEW: 3D L/D overlaid on same chart
    QLineSeries*    m_schrenkSeries;   // NEW
    QLineSeries*    m_lltSeries;       // NEW

    // ---- Numerical results panel ----
    QLabel* m_metricCLmax;
    QLabel* m_metricCDmin;
    QLabel* m_metricCMrange;
    QLabel* m_metricLDmax2D;
    QLabel* m_metricStallAlpha;
    QLabel* m_metricAR;
    QLabel* m_metricA3D;
    QLabel* m_metricLDmax3D;
    QLabel* m_metricLDopt;

    // ---- Thread management ----
    QThread*        m_workerThread;
    XFoilWorker*    m_worker;
};
