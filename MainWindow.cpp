#include "MainWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QScrollArea>
#include <QFont>
#include <QFrame>
#include <QPalette>
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QtCharts>
#include <cmath>
#include <Eigen/Dense>

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
        QScrollArea { background: #0f1117; border: none; }
        QScrollArea > QWidget > QWidget { background: #0f1117; }
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

    // Wrap the input panel in a scroll area — with the new 3D wing geometry
    // group there are now 7 input groups, which can overflow short windows.
    QScrollArea* inputScroll = new QScrollArea;
    inputScroll->setWidget(buildInputPanel());
    inputScroll->setWidgetResizable(true);
    inputScroll->setFrameShape(QFrame::NoFrame);
    inputScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    inputScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    inputScroll->setMinimumWidth(360);
    inputScroll->setMaximumWidth(420);

    splitter->addWidget(inputScroll);
    splitter->addWidget(buildOutputPanel());
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({370, 1030});
    setCentralWidget(splitter);
    buildCharts();
}

QWidget* MainWindow::buildInputPanel() {
    QWidget* panel = new QWidget;
    panel->setMinimumWidth(340);
    QVBoxLayout* mainLayout = new QVBoxLayout(panel);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(10);

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

    // ---- Wing Geometry Group (3-D extension via Plane) ----
    QGroupBox* wingBox = new QGroupBox("WING GEOMETRY (3D)");
    QFormLayout* wingLayout = new QFormLayout(wingBox);
    wingLayout->setSpacing(8);

    m_chord = new QDoubleSpinBox;
    m_chord->setRange(0.01, 20.0);  m_chord->setValue(0.20);
    m_chord->setDecimals(3);        m_chord->setSingleStep(0.01);
    m_chord->setSuffix(" m");
    m_chord->setToolTip("Chord length (rectangular wing assumption)");

    m_wingspan = new QDoubleSpinBox;
    m_wingspan->setRange(0.05, 200.0); m_wingspan->setValue(2.0);
    m_wingspan->setDecimals(3);        m_wingspan->setSingleStep(0.1);
    m_wingspan->setSuffix(" m");
    m_wingspan->setToolTip("Total wingspan, b");

    m_epsilon = new QDoubleSpinBox;
    m_epsilon->setRange(0.50, 1.00);   m_epsilon->setValue(0.85);
    m_epsilon->setDecimals(2);         m_epsilon->setSingleStep(0.01);
    m_epsilon->setToolTip("Oswald efficiency factor (1.0 = elliptical ideal)");

    m_rho = new QDoubleSpinBox;
    m_rho->setRange(0.001, 10.0);      m_rho->setValue(1.225);
    m_rho->setDecimals(3);             m_rho->setSingleStep(0.005);
    m_rho->setSuffix(" kg/m³");
    m_rho->setToolTip("Air density (sea-level ISA = 1.225 kg/m³)");

    m_vinf = new QDoubleSpinBox;
    m_vinf->setRange(0.1, 500.0);      m_vinf->setValue(30.0);
    m_vinf->setDecimals(2);            m_vinf->setSingleStep(1.0);
    m_vinf->setSuffix(" m/s");
    m_vinf->setToolTip("Freestream velocity, V∞");

    m_lltN = new QSpinBox;
    m_lltN->setRange(2, 200);          m_lltN->setValue(30);
    m_lltN->setToolTip("Number of collocation points for Prandtl Lifting-Line solve");

    wingLayout->addRow("Chord c:",     m_chord);
    wingLayout->addRow("Wingspan b:",  m_wingspan);
    wingLayout->addRow("Oswald e:",    m_epsilon);
    wingLayout->addRow("Density ρ:",   m_rho);
    wingLayout->addRow("V∞:",          m_vinf);
    wingLayout->addRow("LLT N:",       m_lltN);
    mainLayout->addWidget(wingBox);

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
    m_schrenkView = makeChartView();
    m_lltView     = makeChartView();

    m_chartTabs->addTab(m_clAlphaView, "CL vs α");
    m_chartTabs->addTab(m_cdAlphaView, "CD vs α");
    m_chartTabs->addTab(m_cmAlphaView, "CM vs α");
    m_chartTabs->addTab(m_polarView,   "Drag Polar");
    m_chartTabs->addTab(m_ldView,      "L/D  (2D vs 3D)");
    m_chartTabs->addTab(m_schrenkView, "Spanwise Lift  (Schrenk)");
    m_chartTabs->addTab(m_lltView,     "Lifting-Line Theory");

    // Log output at bottom
    m_logOutput = new QTextEdit;
    m_logOutput->setReadOnly(true);
    m_logOutput->setMaximumHeight(200);
    m_logOutput->setPlaceholderText("// Simulation log output will appear here...");

    // ---- Numerical Results panel ----
    QGroupBox* metricsBox = new QGroupBox("ANALYSIS RESULTS");
    QGridLayout* metricsLayout = new QGridLayout(metricsBox);
    metricsLayout->setHorizontalSpacing(24);
    metricsLayout->setVerticalSpacing(4);
    metricsLayout->setContentsMargins(14, 18, 14, 14);

    auto makeMetric = [&](int row, int col,
                          const QString& label, QLabel*& valueOut,
                          const QString& valueColor = "#e8eaf0") {
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color:#8892b0; font-size:9px; font-weight:600; "
                           "letter-spacing:1.5px; text-transform:uppercase;");
        valueOut = new QLabel("—");
        valueOut->setStyleSheet(QString("color:%1; font-size:15px; font-weight:600; "
                                        "font-family:'Consolas',monospace;").arg(valueColor));
        metricsLayout->addWidget(lbl,      row * 2,     col, Qt::AlignLeft);
        metricsLayout->addWidget(valueOut, row * 2 + 1, col, Qt::AlignLeft);
    };

    // Column 0 — 2D (XFOIL) metrics
    makeMetric(0, 0, "CL,max",          m_metricCLmax,      "#4fc3f7");
    makeMetric(1, 0, "CD,min",          m_metricCDmin,      "#ef5350");
    makeMetric(2, 0, "(L/D)max  2D",    m_metricLDmax2D,    "#ffa726");
    makeMetric(3, 0, "α @ CL,max",      m_metricStallAlpha, "#e8eaf0");
    makeMetric(4, 0, "CM range",        m_metricCMrange,    "#ce93d8");

    // Column 1 — 3D (wing) metrics
    makeMetric(0, 1, "Aspect Ratio",    m_metricAR,         "#26c6da");
    makeMetric(1, 1, "a3D  (1/rad)",    m_metricA3D,        "#26c6da");
    makeMetric(2, 1, "(L/D)max  3D",    m_metricLDmax3D,    "#ffa726");
    makeMetric(3, 1, "α @ (L/D)max,3D", m_metricLDopt,      "#e8eaf0");
    metricsLayout->setRowStretch(10, 1);   // push content to top

    // Bottom area: metrics on left, log on right (resizable)
    QSplitter* bottomSplit = new QSplitter(Qt::Horizontal);
    bottomSplit->addWidget(metricsBox);
    bottomSplit->addWidget(m_logOutput);
    bottomSplit->setStretchFactor(0, 1);
    bottomSplit->setStretchFactor(1, 1);
    bottomSplit->setSizes({440, 440});

    QSplitter* vSplit = new QSplitter(Qt::Vertical);
    vSplit->addWidget(m_chartTabs);
    vSplit->addWidget(bottomSplit);
    vSplit->setStretchFactor(0, 3);
    vSplit->setStretchFactor(1, 1);
    vSplit->setSizes({560, 220});

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
    configChart(m_schrenkView, "Schrenk Spanwise Lift",  "y / (b/2)", "Lift  (N/m)", m_schrenkSeries, QColor("#26c6da"));
    configChart(m_lltView,     "Prandtl Lifting-Line Theory", "Spanwise station", "Lift  (N/m)", m_lltSeries, QColor("#ab47bc"));

    // ---- L/D chart: overlay a second series for 3-D L/D ----
    m_ldSeries->setName("2D (XFOIL)");
    m_ld3DSeries = new QLineSeries;
    m_ld3DSeries->setName("3D (induced-drag corrected)");
    QPen pen3d(QColor("#ef5350"));
    pen3d.setWidth(2);
    pen3d.setStyle(Qt::DashLine);
    m_ld3DSeries->setPen(pen3d);
    m_ldView->chart()->addSeries(m_ld3DSeries);
    // Re-attach to existing axes so the new series uses the same scale
    auto ldAxesX = m_ldView->chart()->axes(Qt::Horizontal);
    auto ldAxesY = m_ldView->chart()->axes(Qt::Vertical);
    if (!ldAxesX.isEmpty()) m_ld3DSeries->attachAxis(ldAxesX.first());
    if (!ldAxesY.isEmpty()) m_ld3DSeries->attachAxis(ldAxesY.first());
    // Show the legend now that we have two series
    m_ldView->chart()->legend()->show();
    m_ldView->chart()->legend()->setLabelColor(QColor("#8892b0"));
    m_ldView->chart()->legend()->setAlignment(Qt::AlignBottom);
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
    m_plane.setAirfoil(airfoilStr.toStdString());
    m_plane.setAlpha(m_alphaStart->value(), m_alphaEnd->value(), m_alphaStep->value());
    m_plane.setOutputFile(m_outputFile->text().toStdString());
    m_plane.setViscous(m_viscous->isChecked());
    m_plane.setReynolds(m_reynolds->value());
    m_plane.setMach(m_mach->value());
    m_plane.setNcrit(m_nCrit->value());
    m_plane.setXtr(m_xtrTop->value(), m_xtrBot->value());

    // Configure 3-D wing parameters (Plane)
    m_plane.setChord(m_chord->value());
    m_plane.setWingspan(m_wingspan->value());
    m_plane.setEpsilon(m_epsilon->value());
    m_plane.setRho(m_rho->value());
    m_plane.setVinf(m_vinf->value());
    // Recompute AR explicitly — setChord/setWingspan don't do this themselves
    m_plane.calculateAspectRatio();

    m_logOutput->append(QString("[%1]  Running: %2  |  α [%3° → %4°, Δ%5°]  |  Re = %6")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(airfoilStr)
                            .arg(m_alphaStart->value())
                            .arg(m_alphaEnd->value())
                            .arg(m_alphaStep->value())
                            .arg(m_reynolds->value(), 0, 'e', 2));

    // Run in background thread
    m_workerThread = new QThread(this);
    m_worker = new XFoilWorker(&m_plane);
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
    auto alpha = m_plane.getAlpha();
    auto CL    = m_plane.getCL();
    auto CD    = m_plane.getCD();
    auto CM    = m_plane.getCM();

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

    // ---- 3-D analysis ----
    // These now compute everything self-contained from the XFOIL alpha/CL data
    // and the geometry spinboxes — they do NOT depend on Plane::polar, so the
    // shadowing bug in Plane.h cannot make them silently empty anymore.
    try {
        updateLD3D(alpha, CL, CD);
        updateSchrenk(CL);
        updateLiftingLine(alpha);

        double a3D = m_plane.calculate3DLiftSlope();
        double AR  = m_plane.calculateAspectRatio();
        m_logOutput->append(
            QString("  ✓  3D:  AR = %1   a₃D = %2 /rad   e = %3   ρ = %4 kg/m³   V∞ = %5 m/s")
                .arg(AR,                  0, 'f', 2)
                .arg(a3D,                 0, 'f', 3)
                .arg(m_epsilon->value(),  0, 'f', 2)
                .arg(m_rho->value(),      0, 'f', 3)
                .arg(m_vinf->value(),     0, 'f', 1));
    } catch (const std::exception& e) {
        m_logOutput->append(QString("  [3D ERROR]  %1").arg(e.what()));
    }

    // ---- Numerical results panel ----
    updateMetrics(alpha, CL, CD, CM);

    // Compute some quick stats for log
    double clMax = *std::max_element(CL.begin(), CL.end());
    double cdMin = *std::min_element(CD.begin(), CD.end());
    double ldMax = 0;
    for (size_t i = 0; i < CL.size(); i++)
        if (CD[i] > 1e-8) ldMax = std::max(ldMax, CL[i] / CD[i]);

    m_logOutput->append(QString("  ✓  %1 points  |  CLmax = %2  |  CDmin = %3  |  (L/D)max,2D = %4")
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
// Chart update helpers — 3D
// ---------------------------------------------------------------------------

// Overlays 3-D L/D on top of the existing 2-D L/D curve.
// IMPORTANT: this version does NOT call calculateLift2Drag(i), because that
// indexes Plane::polar — which may be the (empty) shadowed copy if Plane.h
// still re-declares its own `polar`. Instead we use calculatedrag3D(CD, CL),
// which is pure arithmetic over Plane's aspectRatio and epsilon members
// (set by the GUI in onSimulate). So this works regardless of the polar bug.
void MainWindow::updateLD3D(const std::vector<double>& alpha,
                            const std::vector<double>& CL,
                            const std::vector<double>& CD) {
    m_ld3DSeries->clear();
    double ldMin =  1e9, ldMax = -1e9;

    // Track 2-D extremes too so the overlaid axes stay sensible
    for (size_t i = 0; i < alpha.size(); i++) {
        if (CD[i] < 1e-8) continue;
        double ld2 = CL[i] / CD[i];
        ldMin = std::min(ldMin, ld2);
        ldMax = std::max(ldMax, ld2);
    }

    double ld3Best = 0.0;
    int    ld3BestIdx = -1;
    for (size_t i = 0; i < alpha.size(); i++) {
        // Pure-arithmetic 3-D drag correction:  CD3D = CD2D + CL^2/(π·AR·e)
        double cd3D = m_plane.calculatedrag3D(CD[i], CL[i]);
        if (cd3D < 1e-8) continue;
        double ld3D = CL[i] / cd3D;
        m_ld3DSeries->append(alpha[i], ld3D);
        ldMin = std::min(ldMin, ld3D);
        ldMax = std::max(ldMax, ld3D);
        if (ld3D > ld3Best) { ld3Best = ld3D; ld3BestIdx = static_cast<int>(i); }
    }

    // Re-scale axes to fit both 2D and 3D series
    if (!alpha.empty()) {
        m_ldView->chart()->axes(Qt::Horizontal).first()->setRange(alpha.front(), alpha.back());
        double pad = (ldMax - ldMin) * 0.1;
        m_ldView->chart()->axes(Qt::Vertical).first()->setRange(ldMin - pad, ldMax + pad);
    }

    if (ld3BestIdx >= 0) {
        m_logOutput->append(QString("  ✓  (L/D)max,3D = %1  at α = %2°")
                                .arg(ld3Best,           0, 'f', 1)
                                .arg(alpha[ld3BestIdx], 0, 'f', 2));
    }
}

// Spanwise lift distribution from Schrenk approximation.
// Self-contained: computes CLavg from the XFOIL CL data (no dependence
// on Plane::polar) and reads chord/wingspan/rho/Vinf from the GUI spinboxes.
void MainWindow::updateSchrenk(const std::vector<double>& CL) {
    m_schrenkSeries->clear();
    if (CL.empty()) return;

    // Average CL over the polar (same definition Plane::schrenkApproximation uses)
    double CLavg = 0.0;
    for (double cl : CL) CLavg += cl;
    CLavg /= static_cast<double>(CL.size());

    const double rho   = m_rho->value();
    const double Vinf  = m_vinf->value();
    const double c     = m_chord->value();
    const double b2    = m_wingspan->value() / 2.0;
    const double q     = 0.5 * rho * Vinf * Vinf;     // dynamic pressure
    const double PI    = 3.14159265358979323846;
    const int    N     = 50;
    const double dy    = b2 / N;

    double yMin =  1e30, yMax = -1e30;
    for (int i = 0; i < N; i++) {
        double y      = (i + 0.5) * dy;
        double ratio  = y / b2;
        double q_rect  = q * c * CLavg;
        double q_ellip = q * c * CLavg * (4.0 / PI)
                         * std::sqrt(std::max(0.0, 1.0 - ratio * ratio));
        double L      = 0.5 * (q_rect + q_ellip);     // N/m
        double xNorm  = (i + 0.5) / static_cast<double>(N);  // y / (b/2)
        m_schrenkSeries->append(xNorm, L);
        yMin = std::min(yMin, L);
        yMax = std::max(yMax, L);
    }

    m_schrenkView->chart()->axes(Qt::Horizontal).first()->setRange(0.0, 1.0);
    double pad = std::max(1e-3, (yMax - yMin) * 0.1);
    m_schrenkView->chart()->axes(Qt::Vertical).first()
        ->setRange(std::min(0.0, yMin - pad), yMax + pad);
}

// Sectional lift across the wing from Prandtl Lifting-Line Theory.
// Self-contained: computes the average geometric AoA from XFOIL alpha data
// (no dependence on Plane::polar) and reads chord/wingspan/rho/Vinf and N
// from the GUI spinboxes. Same math as Plane::liftingLineTheory.
void MainWindow::updateLiftingLine(const std::vector<double>& alpha) {
    using namespace Eigen;

    m_lltSeries->clear();
    if (alpha.empty()) return;

    const double PI = 3.14159265358979323846;

    // Average geometric AoA in radians
    double alphaGeom = 0.0;
    for (double a : alpha) alphaGeom += a;
    alphaGeom /= static_cast<double>(alpha.size());
    alphaGeom *= PI / 180.0;

    const double a2D       = 2.0 * PI;          // thin-airfoil 2-D lift slope
    const double alpha_L0  = 0.0;
    const double c         = m_chord->value();
    const double b         = m_wingspan->value();
    const double rho       = m_rho->value();
    const double Vinf      = m_vinf->value();
    const int    N         = m_lltN->value();
    if (N < 2 || b < 1e-9) return;

    const double mu = (c * a2D) / (4.0 * b);

    // θ collocation points
    std::vector<double> theta(N);
    for (int i = 0; i < N; i++)
        theta[i] = (i + 1) * PI / (N + 1);

    // Build N×N system  M·A = (alphaGeom − alpha_L0)·1
    MatrixXd M(N, N);
    for (int i = 0; i < N; i++) {
        double sinT = std::sin(theta[i]);
        for (int n = 0; n < N; n++) {
            int nOdd = 2 * n + 1;
            M(i, n) = std::sin(nOdd * theta[i])
                      * (static_cast<double>(nOdd) / sinT + 1.0 / mu);
        }
    }
    VectorXd rhs(N);
    rhs.setConstant(alphaGeom - alpha_L0);
    VectorXd A = M.colPivHouseholderQr().solve(rhs);

    // Sectional lift L'(y) = ρ·V·Γ(y)
    double yMin =  1e30, yMax = -1e30;
    for (int i = 0; i < N; i++) {
        double Gamma_i = 0.0;
        for (int n = 0; n < N; n++) {
            int nOdd = 2 * n + 1;
            Gamma_i += A(n) * std::sin(nOdd * theta[i]);
        }
        Gamma_i *= 2.0 * b * Vinf;             // m^2/s
        double L      = rho * Vinf * Gamma_i;  // N/m
        double yOverB = -0.5 * std::cos(theta[i]);   // ∈ (−0.5, 0.5)
        m_lltSeries->append(yOverB, L);
        yMin = std::min(yMin, L);
        yMax = std::max(yMax, L);
    }

    m_lltView->chart()->axes(Qt::Horizontal).first()->setRange(-0.5, 0.5);
    double pad = std::max(1e-3, (yMax - yMin) * 0.1);
    m_lltView->chart()->axes(Qt::Vertical).first()
        ->setRange(std::min(0.0, yMin - pad), yMax + pad);
}

// ---------------------------------------------------------------------------
// Numerical results panel
// ---------------------------------------------------------------------------
void MainWindow::updateMetrics(const std::vector<double>& alpha,
                               const std::vector<double>& CL,
                               const std::vector<double>& CD,
                               const std::vector<double>& CM) {
    if (alpha.empty()) return;

    // ---- 2D metrics ----
    auto clMaxIt = std::max_element(CL.begin(), CL.end());
    double clMax = *clMaxIt;
    double alphaAtCLmax = alpha[std::distance(CL.begin(), clMaxIt)];

    double cdMin = *std::min_element(CD.begin(), CD.end());

    double cmMin = *std::min_element(CM.begin(), CM.end());
    double cmMax = *std::max_element(CM.begin(), CM.end());

    double ld2Best = 0.0;
    for (size_t i = 0; i < CL.size(); i++)
        if (CD[i] > 1e-8) ld2Best = std::max(ld2Best, CL[i] / CD[i]);

    // ---- 3D metrics ----
    double AR  = m_plane.calculateAspectRatio();
    double a3D = m_plane.calculate3DLiftSlope();   // (1/rad)

    double ld3Best       = 0.0;
    double alphaAtLD3max = 0.0;
    for (size_t i = 0; i < alpha.size(); i++) {
        double cd3 = m_plane.calculatedrag3D(CD[i], CL[i]);
        if (cd3 < 1e-8) continue;
        double ld3 = CL[i] / cd3;
        if (ld3 > ld3Best) {
            ld3Best       = ld3;
            alphaAtLD3max = alpha[i];
        }
    }

    // ---- Push to labels ----
    m_metricCLmax     ->setText(QString::number(clMax, 'f', 4));
    m_metricCDmin     ->setText(QString::number(cdMin, 'f', 5));
    m_metricLDmax2D   ->setText(QString::number(ld2Best, 'f', 1));
    m_metricStallAlpha->setText(QString::number(alphaAtCLmax, 'f', 2) + " °");
    m_metricCMrange   ->setText(QString("%1 → %2")
                                 .arg(cmMin, 0, 'f', 3)
                                 .arg(cmMax, 0, 'f', 3));
    m_metricAR        ->setText(QString::number(AR, 'f', 2));
    m_metricA3D       ->setText(QString::number(a3D, 'f', 3));
    m_metricLDmax3D   ->setText(QString::number(ld3Best, 'f', 1));
    m_metricLDopt     ->setText(QString::number(alphaAtLD3max, 'f', 2) + " °");
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------
void MainWindow::setSimulating(bool simulating) {
    m_simButton->setEnabled(!simulating);
    m_progressBar->setVisible(simulating);
    m_airfoilType->setEnabled(!simulating);
    m_nacaCode->setEnabled(!simulating);
    // Lock 3-D geometry inputs while running too
    m_chord->setEnabled(!simulating);
    m_wingspan->setEnabled(!simulating);
    m_epsilon->setEnabled(!simulating);
    m_rho->setEnabled(!simulating);
    m_vinf->setEnabled(!simulating);
    m_lltN->setEnabled(!simulating);
}