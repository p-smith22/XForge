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


    // Worker thread so GUI doesn't freeze during XFOIL run
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

    // ---- Plot update helpers ----
    void updateCharts();
    void updateCLvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CL);
    void updateCDvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CD);
    void updateCLvsCDPolar(const std::vector<double>& CL,
                           const std::vector<double>& CD);
    void updateLDvsAlpha(const std::vector<double>& alpha,
                         const std::vector<double>& CL,
                         const std::vector<double>& CD);
    void setChartTheme(QChart* chart);
    void setSimulating(bool simulating);

    // ---- Core object ----
    XFoil_Interface m_xfoil;

    // ---- Input widgets ----
    QComboBox*      m_airfoilType;      // NACA 4, NACA 5, DAT file
    QLineEdit*      m_nacaCode;         // e.g. "0012"
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

    // ---- Output widgets ----
    QTabWidget*     m_chartTabs;
    QChartView*     m_clAlphaView;
    QChartView*     m_cdAlphaView;
    QChartView*     m_polarView;
    QChartView*     m_ldView;
    QTextEdit*      m_logOutput;

    // ---- Chart series ----
    QLineSeries*    m_clSeries;
    QLineSeries*    m_cdSeries;
    QLineSeries*    m_polarSeries;
    QLineSeries*    m_ldSeries;

    // ---- Thread management ----
    QThread*        m_workerThread;
    XFoilWorker*    m_worker;
};