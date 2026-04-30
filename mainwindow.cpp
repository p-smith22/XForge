#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QFont>
#include <QFrame>
#include <QPalette>
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QtCharts>
#include <cmath>

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
{
    setWindowTitle("XForge — XFOIL GUI");
    setMinimumSize(1200, 780);
    resize(1400, 860);

    // Dark-mode palette
    QPalette pal;
    pal.setColor(QPalette::Window,          QColor("#0f1117"));
    pal.setColor(QPalette::WindowText,      QColor("#e8eaf0"));
    pal.setColor(QPalette::Base,            QColor("#1a1d27"));
    pal.setColor(QPalette::AlternateBase,   QColor("#222533"));
    pal.setColor(QPalette::Text,            QColor("#e8eaf0"));
    pal.setColor(QPalette::Button,          QColor("#1e2130"));
    pal.setColor(QPalette::ButtonText,      QColor("#e8eaf0"));
    pal.setColor(QPalette::Highlight,       QColor("#3a7bd5"));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::Disabled, QPalette::Text,       QColor("#505468"));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#505468"));
    setPalette(pal);
    QApplication::setPalette(pal);

    setStyleSheet(R"(
        QMainWindow, QWidget { background: #0f1117; color: #e8eaf0; font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; }
        QGroupBox {
            border: 1px solid #2a2e40;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 6px;
            font-weight: 600;
            color: #8892b0;
            font-size: 10px;
            letter-spacing: 1px;
            text-transform: uppercase;
        }
        QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; left: 10px; padding: 0 4px; }
        QLineEdit, QDoubleSpinBox, QSpinBox, QComboBox {
            background: #1a1d27;
            border: 1px solid #2a2e40;
            border-radius: 4px;
            padding: 4px 8px;
            color: #e8eaf0;
            selection-background-color: #3a7bd5;
        }
        QLineEdit:focus, QDoubleSpinBox:focus, QSpinBox:focus, QComboBox:focus {
            border: 1px solid #3a7bd5;
        }
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button,
        QSpinBox::up-button, QSpinBox::down-button {
            background: #2a2e40; border: none; width: 16px;
        }
        QDoubleSpinBox::up-arrow, QSpinBox::up-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-bottom: 5px solid #8892b0; }
        QDoubleSpinBox::down-arrow, QSpinBox::down-arrow { image: none; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #8892b0; }
        QComboBox::drop-down { border: none; background: #2a2e40; width: 24px; }
        QComboBox QAbstractItemView { background: #1a1d27; border: 1px solid #2a2e40; selection-background-color: #3a7bd5; }
        QPushButton {
            background: #1e2130;
            border: 1px solid #3a7bd5;
            border-radius: 4px;
            padding: 8px 20px;
            color: #64b5f6;
            font-weight: 600;
            letter-spacing: 0.5px;
        }
        QPushButton:hover { background: #263050; border-color: #5b9ee8; color: #90caf9; }
        QPushButton:pressed { background: #1a2540; }
        QPushButton:disabled { background: #181b26; border-color: #2a2e40; color: #404560; }
        QPushButton#simBtn {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1e5fa8, stop:1 #163d70);
            border: 1px solid #3a7bd5;
            color: #e3f2fd;
            font-size: 13px;
            padding: 10px 24px;
        }
        QPushButton#simBtn:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2470c0, stop:1 #1a4a88); }
        QPushButton#simBtn:disabled { background: #181b26; border-color: #2a2e40; color: #404560; }
        QCheckBox { spacing: 8px; }
        QCheckBox::indicator { width: 14px; height: 14px; border: 1px solid #2a2e40; border-radius: 3px; background: #1a1d27; }
        QCheckBox::indicator:checked { background: #3a7bd5; border-color: #3a7bd5; image: none; }
        QTabWidget::pane { border: 1px solid #2a2e40; background: #0f1117; }
        QTabBar::tab { background: #1a1d27; border: 1px solid #2a2e40; padding: 6px 18px; margin-right: 2px; color: #8892b0; }
        QTabBar::tab:selected { background: #0f1117; border-bottom: 2px solid #3a7bd5; color: #e8eaf0; }
        QTabBar::tab:hover { color: #c0c8e0; background: #1e2130; }
        QProgressBar { border: 1px solid #2a2e40; border-radius: 3px; background: #1a1d27; text-align: center; color: #8892b0; height: 6px; }
        QProgressBar::chunk { background: #3a7bd5; border-radius: 3px; }
        QTextEdit { background: #0a0c12; border: 1px solid #1a1d27; color: #64b5f6; font-family: 'Consolas', monospace; font-size: 11px; }
        QScrollBar:vertical { background: #0f1117; width: 8px; }
        QScrollBar::handle:vertical { background: #2a2e40; border-radius: 4px; min-height: 20px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
        QSplitter::handle { background: #2a2e40; width: 1px; }
        QStatusBar { background: #0a0c12; border-top: 1px solid #1a1d27; color: #505468; font-size: 11px; }
        QLabel#sectionTitle { color: #3a7bd5; font-size: 11px; font-weight: 700; letter-spacing: 1px; }
        QFrame#divider { background: #2a2e40; max-height: 1px; }
    )");

    setupUI();
    statusBar()->showMessage("Ready  —  Configure parameters and press Simulate");
}

MainWindow::~MainWindow() {
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

// ---------------------------------------------------------------------------
// UI Setup
// ---------------------------------------------------------------------------
void MainWindow::setupUI() {
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(buildInputPanel());
    splitter->addWidget(buildOutputPanel());
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({320, 1080});
    setCentralWidget(splitter);
    buildCharts();
}

QWidget* MainWindow::buildInputPanel() {
    QWidget* panel = new QWidget;
    panel->setFixedWidth(300);
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    // ---- Header ----
    QLabel* title = new QLabel("XFORGE");
    title->setStyleSheet("font-size:22px; font-weight:700; color:#3a7bd5; letter-spacing:4px;");
    QLabel* subtitle = new QLabel("XFOIL Aerodynamic Analysis");
    subtitle->setStyleSheet("font-size:10px; color:#505468; letter-spacing:1px;");
    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    QFrame* div0 = new QFrame; div0->setObjectName("divider"); div0->setFrameShape(QFrame::HLine);
    mainLayout->addWidget(div0);

    // ---- Airfoil Group ----
    QGroupBox* airfoilBox = new QGroupBox("AIRFOIL");
    QFormLayout* afLayout = new QFormLayout(airfoilBox);
    afLayout->setSpacing(8);

    m_airfoilType = new QComboBox;
    m_airfoilType->addItems({"NACA 4-Series", "NACA 5-Series", "DAT File (manual)"});
    afLayout->addRow("Type:", m_airfoilType);

    m_nacaCode = new QLineEdit("0012");
    m_nacaCode->setPlaceholderText("e.g. 0012 / 23012");
    m_nacaCode->setToolTip("4-digit or 5-digit NACA code, or full NACA designation");
    afLayout->addRow("Code:", m_nacaCode);

    mainLayout->addWidget(airfoilBox);
    connect(m_airfoilType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onAirfoilTypeChanged);

    // ---- Alpha Sweep Group ----
    QGroupBox* alphaBox = new QGroupBox("ANGLE OF ATTACK SWEEP");
    QFormLayout* alphaLayout = new QFormLayout(alphaBox);
    alphaLayout->setSpacing(8);

    m_alphaStart = new QDoubleSpinBox; m_alphaStart->setRange(-30, 0);  m_alphaStart->setValue(-8);  m_alphaStart->setSuffix(" °"); m_alphaStart->setSingleStep(0.5);
    m_alphaEnd   = new QDoubleSpinBox; m_alphaEnd->setRange(0, 30);     m_alphaEnd->setValue(12);    m_alphaEnd->setSuffix(" °");   m_alphaEnd->setSingleStep(0.5);
    m_alphaStep  = new QDoubleSpinBox; m_alphaStep->setRange(0.05, 5);  m_alphaStep->setValue(1.0);  m_alphaStep->setSuffix(" °");  m_alphaStep->setSingleStep(0.05);

    alphaLayout->addRow("Start:", m_alphaStart);
    alphaLayout->addRow("End:",   m_alphaEnd);
    alphaLayout->addRow("Step:",  m_alphaStep);
    mainLayout->addWidget(alphaBox);

    // ---- Flow Conditions Group ----
    QGroupBox* flowBox = new QGroupBox("FLOW CONDITIONS");
    QFormLayout* flowLayout = new QFormLayout(flowBox);
    flowLayout->setSpacing(8);

    m_viscous = new QCheckBox("Viscous (OPER VISC)");
    m_viscous->setChecked(true);
    flowLayout->addRow(m_viscous);

    m_reynolds = new QDoubleSpinBox;
    m_reynolds->setRange(1e3, 1e8);
    m_reynolds->setValue(1e6);
    m_reynolds->setDecimals(0);
    m_reynolds->setSingleStep(1e5);
    m_reynolds->setEnabled(true);
    m_reynolds->setToolTip("Reynolds number (requires viscous mode)");
    flowLayout->addRow("Re:", m_reynolds);

    m_mach = new QDoubleSpinBox;
    m_mach->setRange(0.0, 0.8);
    m_mach->setValue(0.0);
    m_mach->setDecimals(3);
    m_mach->setSingleStep(0.01);
    flowLayout->addRow("Mach:", m_mach);

    connect(m_viscous, &QCheckBox::toggled, m_reynolds, &QDoubleSpinBox::setEnabled);
    mainLayout->addWidget(flowBox);

    // ---- Transition Group ----
    QGroupBox* transBox = new QGroupBox("TRANSITION (Ncrit / Xtr)");
    QFormLayout* transLayout = new QFormLayout(transBox);
    transLayout->setSpacing(8);

    m_nCrit = new QSpinBox;
    m_nCrit->setRange(1, 20);
    m_nCrit->setValue(9);
    m_nCrit->setToolTip("Free-stream turbulence factor (9 = clean tunnel)");
    transLayout->addRow("Ncrit:", m_nCrit);

    m_xtrTop = new QDoubleSpinBox; m_xtrTop->setRange(0, 1); m_xtrTop->setValue(1.0); m_xtrTop->setDecimals(2); m_xtrTop->setSingleStep(0.05);
    m_xtrBot = new QDoubleSpinBox; m_xtrBot->setRange(0, 1); m_xtrBot->setValue(1.0); m_xtrBot->setDecimals(2); m_xtrBot->setSingleStep(0.05);
    transLayout->addRow("Xtr Top:", m_xtrTop);
    transLayout->addRow("Xtr Bot:", m_xtrBot);
    mainLayout->addWidget(transBox);

    // ---- Output Group ----
    QGroupBox* outBox = new QGroupBox("OUTPUT");
    QFormLayout* outLayout = new QFormLayout(outBox);
    m_outputFile = new QLineEdit("polar.dat");
    outLayout->addRow("File:", m_outputFile);
    mainLayout->addWidget(outBox);

    mainLayout->addStretch();

    // ---- Simulate button + progress ----
    m_simButton = new QPushButton("▶  SIMULATE");
    m_simButton->setObjectName("simBtn");
    m_simButton->setFixedHeight(42);
    mainLayout->addWidget(m_simButton);

    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 0); // indeterminate
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(6);
    mainLayout->addWidget(m_progressBar);

    connect(m_simButton, &QPushButton::clicked, this, &MainWindow::onSimulate);

    return panel;
}

QWidget* MainWindow::buildOutputPanel() {
    QWidget* panel = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Chart tabs
    m_chartTabs = new QTabWidget;

    auto makeChartView = [&]() -> QChartView* {
        QChart* chart = new QChart;
        chart->setBackgroundBrush(QBrush(QColor("#0f1117")));
        chart->setPlotAreaBackgroundBrush(QBrush(QColor("#0a0c12")));
        chart->setPlotAreaBackgroundVisible(true);
        QChartView* view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
        view->setStyleSheet("background: #0f1117; border: none;");
        return view;
    };

    m_clAlphaView = makeChartView();
    m_cdAlphaView = makeChartView();
    m_cmAlphaView = makeChartView();
    m_polarView   = makeChartView();
    m_ldView      = makeChartView();

    m_chartTabs->addTab(m_clAlphaView, "CL vs α");
    m_chartTabs->addTab(m_cdAlphaView, "CD vs α");
    m_chartTabs->addTab(m_cmAlphaView, "CM vs α");
    m_chartTabs->addTab(m_polarView,   "Drag Polar");
    m_chartTabs->addTab(m_ldView,      "L/D vs α");

    // Log output at bottom
    m_logOutput = new QTextEdit;
    m_logOutput->setReadOnly(true);
    m_logOutput->setMaximumHeight(130);
    m_logOutput->setPlaceholderText("// Simulation log output will appear here...");

    QSplitter* vSplit = new QSplitter(Qt::Vertical);
    vSplit->addWidget(m_chartTabs);
    vSplit->addWidget(m_logOutput);
    vSplit->setStretchFactor(0, 3);
    vSplit->setStretchFactor(1, 1);
    vSplit->setSizes({640, 130});

    layout->addWidget(vSplit);
    return panel;
}

void MainWindow::buildCharts() {
    // Helper lambda: configure a chart with axes and a series
    auto configChart = [&](QChartView* view, const QString& title,
                           const QString& xLabel, const QString& yLabel,
                           QLineSeries*& series, const QColor& lineColor) {
        QChart* chart = view->chart();
        chart->setTitle(title);
        chart->setTitleBrush(QBrush(QColor("#8892b0")));
        chart->legend()->hide();

        series = new QLineSeries;
        QPen pen(lineColor);
        pen.setWidth(2);
        series->setPen(pen);
        chart->addSeries(series);

        QValueAxis* axisX = new QValueAxis;
        QValueAxis* axisY = new QValueAxis;
        axisX->setTitleText(xLabel);
        axisY->setTitleText(yLabel);
        for (auto* ax : {(QAbstractAxis*)axisX, (QAbstractAxis*)axisY}) {
            ax->setLabelsColor(QColor("#8892b0"));
            ax->setTitleBrush(QBrush(QColor("#64748b")));
            ax->setGridLineColor(QColor("#1e2130"));
            ax->setLinePen(QPen(QColor("#2a2e40")));
        }
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    };

    configChart(m_clAlphaView, "Lift Coefficient",       "α (deg)",  "CL",         m_clSeries,    QColor("#4fc3f7"));
    configChart(m_cdAlphaView, "Drag Coefficient",       "α (deg)",  "CD",         m_cdSeries,    QColor("#ef5350"));
    configChart(m_cmAlphaView, "Moment Coefficient",     "α (deg)",  "CM",         m_cmSeries,    QColor("#ce93d8"));
    configChart(m_polarView,   "Drag Polar",             "CD",       "CL",         m_polarSeries, QColor("#66bb6a"));
    configChart(m_ldView,      "Lift-to-Drag Ratio",     "α (deg)",  "L/D",        m_ldSeries,    QColor("#ffa726"));
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------
void MainWindow::onAirfoilTypeChanged(int index) {
    if (index == 2) { // DAT file
        m_nacaCode->setPlaceholderText("(file path set separately)");
        m_nacaCode->setEnabled(false);
    } else {
        m_nacaCode->setEnabled(true);
        m_nacaCode->setPlaceholderText(index == 0 ? "e.g. 0012" : "e.g. 23012");
    }
}

void MainWindow::onSimulate() {
    // Validate inputs
    if (m_alphaStart->value() >= m_alphaEnd->value()) {
        QMessageBox::warning(this, "Input Error", "Alpha Start must be less than Alpha End.");
        return;
    }

    setSimulating(true);

    // Build airfoil string
    QString airfoilStr;
    int type = m_airfoilType->currentIndex();
    if (type == 0 || type == 1) {
        airfoilStr = "NACA " + m_nacaCode->text().trimmed();
    } else {
        airfoilStr = m_nacaCode->text().trimmed();
    }

    // Configure XFOIL interface
    m_xfoil.setAirfoil(airfoilStr.toStdString());
    m_xfoil.setAlpha(m_alphaStart->value(), m_alphaEnd->value(), m_alphaStep->value());
    m_xfoil.setOutputFile(m_outputFile->text().toStdString());
    m_xfoil.setViscous(m_viscous->isChecked());
    m_xfoil.setReynolds(m_reynolds->value());
    m_xfoil.setMach(m_mach->value());
    m_xfoil.setNcrit(m_nCrit->value());
    m_xfoil.setXtr(m_xtrTop->value(), m_xtrBot->value());

    m_logOutput->append(QString("[%1]  Running: %2  |  α [%3° → %4°, Δ%5°]  |  Re = %6")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(airfoilStr)
                            .arg(m_alphaStart->value())
                            .arg(m_alphaEnd->value())
                            .arg(m_alphaStep->value())
                            .arg(m_reynolds->value(), 0, 'e', 2));

    // Run in background thread
    m_workerThread = new QThread(this);
    m_worker = new XFoilWorker(&m_xfoil);
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started,  m_worker, &XFoilWorker::run);
    connect(m_worker, &XFoilWorker::finished,   this,     &MainWindow::onSimulationFinished);
    connect(m_worker, &XFoilWorker::finished,   m_workerThread, &QThread::quit);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);

    m_workerThread = nullptr; // will be cleaned up automatically
    auto* t = m_worker->thread();
    static_cast<QThread*>(t)->start();
}

void MainWindow::onSimulationFinished() {
    auto alpha = m_xfoil.getAlpha();
    auto CL    = m_xfoil.getCL();
    auto CD    = m_xfoil.getCD();
    auto CM    = m_xfoil.getCM();

    if (alpha.empty()) {
        m_logOutput->append("  [ERROR]  Check inputs.");
        setSimulating(false);
        statusBar()->showMessage("Simulation failed — no data returned");
        return;
    }

    updateCLvsAlpha(alpha, CL);
    updateCDvsAlpha(alpha, CD);
    updateCMvsAlpha(alpha, CM);
    updateCLvsCDPolar(CL, CD);
    updateLDvsAlpha(alpha, CL, CD);

    // Compute some quick stats for log
    double clMax = *std::max_element(CL.begin(), CL.end());
    double cdMin = *std::min_element(CD.begin(), CD.end());
    double ldMax = 0;
    for (size_t i = 0; i < CL.size(); i++)
        if (CD[i] > 1e-8) ldMax = std::max(ldMax, CL[i] / CD[i]);

    m_logOutput->append(QString("  ✓  %1 points  |  CLmax = %2  |  CDmin = %3  |  (L/D)max = %4")
                            .arg(alpha.size())
                            .arg(clMax, 0, 'f', 4)
                            .arg(cdMin, 0, 'f', 5)
                            .arg(ldMax,  0, 'f', 1));

    setSimulating(false);
    statusBar()->showMessage(QString("Done  —  %1 points computed").arg(alpha.size()));
}

// ---------------------------------------------------------------------------
// Chart update helpers
// ---------------------------------------------------------------------------
void MainWindow::updateCLvsAlpha(const std::vector<double>& alpha,
                                 const std::vector<double>& CL) {
    m_clSeries->clear();
    for (size_t i = 0; i < alpha.size(); i++)
        m_clSeries->append(alpha[i], CL[i]);
    m_clAlphaView->chart()->axes(Qt::Horizontal).first()->setRange(alpha.front(), alpha.back());
    double clMin = *std::min_element(CL.begin(), CL.end());
    double clMax = *std::max_element(CL.begin(), CL.end());
    double pad = (clMax - clMin) * 0.1;
    m_clAlphaView->chart()->axes(Qt::Vertical).first()->setRange(clMin - pad, clMax + pad);
}

void MainWindow::updateCDvsAlpha(const std::vector<double>& alpha,
                                 const std::vector<double>& CD) {
    m_cdSeries->clear();
    for (size_t i = 0; i < alpha.size(); i++)
        m_cdSeries->append(alpha[i], CD[i]);
    m_cdAlphaView->chart()->axes(Qt::Horizontal).first()->setRange(alpha.front(), alpha.back());
    double cdMin = *std::min_element(CD.begin(), CD.end());
    double cdMax = *std::max_element(CD.begin(), CD.end());
    double pad = (cdMax - cdMin) * 0.1;
    m_cdAlphaView->chart()->axes(Qt::Vertical).first()->setRange(cdMin - pad, cdMax + pad);
}

void MainWindow::updateCMvsAlpha(const std::vector<double>& alpha,
                                 const std::vector<double>& CM) {
    m_cmSeries->clear();
    for (size_t i = 0; i < alpha.size(); i++)
        m_cmSeries->append(alpha[i], CM[i]);
    m_cmAlphaView->chart()->axes(Qt::Horizontal).first()->setRange(alpha.front(), alpha.back());
    double cmMin = *std::min_element(CM.begin(), CM.end());
    double cmMax = *std::max_element(CM.begin(), CM.end());
    double pad = (cmMax - cmMin) * 0.1;
    m_cmAlphaView->chart()->axes(Qt::Vertical).first()->setRange(cmMin - pad, cmMax + pad);
}

void MainWindow::updateCLvsCDPolar(const std::vector<double>& CL,
                                   const std::vector<double>& CD) {
    m_polarSeries->clear();
    for (size_t i = 0; i < CL.size(); i++)
        m_polarSeries->append(CD[i], CL[i]);
    double cdMin = *std::min_element(CD.begin(), CD.end());
    double cdMax = *std::max_element(CD.begin(), CD.end());
    double clMin = *std::min_element(CL.begin(), CL.end());
    double clMax = *std::max_element(CL.begin(), CL.end());
    m_polarView->chart()->axes(Qt::Horizontal).first()->setRange(cdMin * 0.9, cdMax * 1.1);
    m_polarView->chart()->axes(Qt::Vertical).first()->setRange(clMin - 0.1, clMax + 0.1);
}

void MainWindow::updateLDvsAlpha(const std::vector<double>& alpha,
                                 const std::vector<double>& CL,
                                 const std::vector<double>& CD) {
    m_ldSeries->clear();
    double ldMin =  1e9, ldMax = -1e9;
    for (size_t i = 0; i < alpha.size(); i++) {
        if (CD[i] < 1e-8) continue;
        double ld = CL[i] / CD[i];
        m_ldSeries->append(alpha[i], ld);
        ldMin = std::min(ldMin, ld);
        ldMax = std::max(ldMax, ld);
    }
    if (ldMin < 1e8) {
        m_ldView->chart()->axes(Qt::Horizontal).first()->setRange(alpha.front(), alpha.back());
        double pad = (ldMax - ldMin) * 0.1;
        m_ldView->chart()->axes(Qt::Vertical).first()->setRange(ldMin - pad, ldMax + pad);
    }
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------
void MainWindow::setSimulating(bool simulating) {
    m_simButton->setEnabled(!simulating);
    m_progressBar->setVisible(simulating);
    m_airfoilType->setEnabled(!simulating);
    m_nacaCode->setEnabled(!simulating);
}