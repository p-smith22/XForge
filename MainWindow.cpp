#include "MainWindow.h"
#include <QtWidgets>
#include <algorithm>


// Color palette
static const char* BG_DEEP    = "#0d0d0d";
static const char* BG_PANEL   = "#0d0d0d";
static const char* BG_INPUT   = "#0d0d0d";
static const char* MAROON     = "#621d1d";
static const char* GOLD       = "#d4af37";
static const char* GOLD_HOVER = "#e8c34d";
static const char* BEIGE      = "#e8dcc4";
static const char* BEIGE_DIM  = "#9b8e76";

// Series colors
static const char* C_CL      = "#d4af37";
static const char* C_CD      = "#a0303f";
static const char* C_CM      = "#c8a878";
static const char* C_POLAR   = "#c9962e";
static const char* C_LD2D    = "#d4af37";
static const char* C_LD3D    = "#a0303f";
static const char* C_CL3D    = "#c8a878";


// Stylesheet for the entire app
static QString stylesheet() {
    QString s;
    s += QString("QMainWindow, QWidget { background:%1; color:%2; }").arg(BG_DEEP, BEIGE);
    s += QString("QGroupBox { background:%1; border:1px solid %2; border-radius:4px;"
                 " margin-top:14px; padding-top:10px; font-size:10px; font-weight:bold;"
                 " letter-spacing:2px; color:%3; }").arg(BG_PANEL, MAROON, GOLD);
    s += "QGroupBox::title { subcontrol-origin:margin; left:10px; padding:0 6px; }";
    s += QString("QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {"
                 " background:%1; border:1px solid %2; border-radius:3px;"
                 " padding:5px 8px; color:%3; }").arg(BG_INPUT, MAROON, BEIGE);
    s += QString("QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {"
                 " border:1px solid %1; }").arg(GOLD);
    s += QString("QCheckBox { color:%1; }").arg(BEIGE);
    s += QString("QCheckBox::indicator { width:14px; height:14px;"
                 " border:1px solid %1; background:%2; }").arg(MAROON, BG_INPUT);
    s += QString("QCheckBox::indicator:checked { background:%1; }").arg(GOLD);
    s += QString("QPushButton { background:%1; color:%2; border:none; border-radius:3px;"
                 " padding:10px 18px; font-weight:bold; font-size:11px;"
                 " letter-spacing:2px; }").arg(GOLD, BG_DEEP);
    s += QString("QPushButton:hover { background:%1; }").arg(GOLD_HOVER);
    s += QString("QPushButton:disabled { background:%1; color:%2; }").arg(MAROON, BEIGE_DIM);
    s += QString("QTabWidget::pane { background:%1; border:1px solid %2; }").arg(BG_PANEL, MAROON);
    s += QString("QTabBar::tab { background:%1; color:%2; padding:8px 16px;"
                 " border:1px solid %3; font-size:10px; letter-spacing:1px; }").arg(BG_DEEP, BEIGE_DIM, MAROON);
    s += QString("QTabBar::tab:selected { background:%1; color:%2; }").arg(BG_PANEL, GOLD);
    s += QString("QTextEdit { background:%1; color:%2; border:1px solid %3;"
                 " border-radius:3px; font-family:Consolas,monospace; font-size:11px; padding:6px; }")
             .arg(BG_DEEP, BEIGE, MAROON);
    s += QString("QSplitter::handle { background:%1; }").arg(MAROON);
    return s;
}


// Chart Interface
static void styleChart(QChart* chart) {
    chart->setBackgroundBrush(QColor(BG_PANEL));
    chart->setPlotAreaBackgroundBrush(QColor(BG_DEEP));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setTitleBrush(QColor(GOLD));
    chart->legend()->hide();

    QFont titleFont;
    titleFont.setPointSize(11);
    titleFont.setBold(true);
    chart->setTitleFont(titleFont);

    // Set up chart axes
    for (auto* ax : chart->axes(Qt::Horizontal)) {
        ax->setLabelsBrush(QColor(BEIGE_DIM));
        ax->setTitleBrush(QColor(BEIGE));
        ax->setLinePen(QPen(QColor(MAROON)));
        ax->setGridLinePen(QPen(QColor(MAROON)));
    }
    for (auto* ax : chart->axes(Qt::Vertical)) {
        ax->setLabelsBrush(QColor(BEIGE_DIM));
        ax->setTitleBrush(QColor(BEIGE));
        ax->setLinePen(QPen(QColor(MAROON)));
        ax->setGridLinePen(QPen(QColor(MAROON)));
    }
}


// Constructor
MainWindow::MainWindow() : m_thread(nullptr), m_worker(nullptr) {
    setWindowTitle("XForge — XFOIL Analyzer");
    resize(1500, 950);
    setStyleSheet(stylesheet());
    setupUI();
}

// Destructor
MainWindow::~MainWindow() {
    if (m_thread && m_thread->isRunning()) {
        m_thread->quit();
        m_thread->wait();
    }
}


// This object builds the interface
void MainWindow::setupUI() {

    // Left panel

    QWidget* inputs = new QWidget;
    QVBoxLayout* inLay = new QVBoxLayout(inputs);
    inLay->setContentsMargins(14, 14, 14, 14);
    inLay->setSpacing(10);

    // Title
    QLabel* title = new QLabel("XFORGE");
    title->setStyleSheet(QString("color:%1; font-size:24px; font-weight:bold;"
                                 "letter-spacing:8px;").arg(GOLD));
    inLay->addWidget(title);

    // Airfoil section
    QGroupBox* gAir = new QGroupBox("AIRFOIL");
    QFormLayout* fAir = new QFormLayout(gAir);
    m_naca = new QLineEdit("0012");
    fAir->addRow("NACA code:", m_naca);
    inLay->addWidget(gAir);

    // Angle of attack section
    QGroupBox* gAlpha = new QGroupBox("ANGLE OF ATTACK SWEEP");
    QFormLayout* fAlpha = new QFormLayout(gAlpha);
    m_aStart = new QDoubleSpinBox; m_aStart->setRange(-30, 30); m_aStart->setValue(-5); m_aStart->setSuffix("\u00B0");
    m_aEnd   = new QDoubleSpinBox; m_aEnd  ->setRange(-30, 30); m_aEnd  ->setValue(15); m_aEnd  ->setSuffix("\u00B0");
    m_aStep  = new QDoubleSpinBox; m_aStep ->setRange(0.05, 5); m_aStep ->setValue(0.5); m_aStep ->setSuffix("\u00B0");
    m_aStep->setSingleStep(0.1);
    fAlpha->addRow("alpha start:", m_aStart);
    fAlpha->addRow("alpha end:",   m_aEnd);
    fAlpha->addRow("step:",    m_aStep);
    inLay->addWidget(gAlpha);

    //Flow conditions section
    QGroupBox* gFlow = new QGroupBox("FLOW CONDITIONS");
    QFormLayout* fFlow = new QFormLayout(gFlow);
    m_re   = new QDoubleSpinBox; m_re  ->setRange(1e3, 1e9); m_re  ->setValue(1e6); m_re->setDecimals(0); m_re->setSingleStep(1e5);
    m_mach = new QDoubleSpinBox; m_mach->setRange(0, 0.95);  m_mach->setValue(0);   m_mach->setDecimals(3);
    m_ncrit = new QSpinBox;      m_ncrit->setRange(1, 20);    m_ncrit->setValue(9);
    m_visc  = new QCheckBox("Viscous"); m_visc->setChecked(true);
    fFlow->addRow("Reynolds:", m_re);
    fFlow->addRow("Mach:",     m_mach);
    fFlow->addRow("N crit:",   m_ncrit);
    fFlow->addRow(m_visc);
    inLay->addWidget(gFlow);

    // Wing geometry section
    QGroupBox* gWing = new QGroupBox("WING GEOMETRY (3D)");
    QFormLayout* fWing = new QFormLayout(gWing);
    m_chord   = new QDoubleSpinBox; m_chord  ->setRange(0.01, 20);  m_chord  ->setValue(0.20); m_chord  ->setDecimals(3); m_chord  ->setSuffix(" m");
    m_span    = new QDoubleSpinBox; m_span   ->setRange(0.05, 200); m_span   ->setValue(2.0);  m_span   ->setDecimals(3); m_span   ->setSuffix(" m");
    m_eOswald = new QDoubleSpinBox; m_eOswald->setRange(0.5, 1.0);  m_eOswald->setValue(0.85); m_eOswald->setDecimals(2); m_eOswald->setSingleStep(0.01);
    m_rho     = new QDoubleSpinBox; m_rho    ->setRange(0.001, 10); m_rho    ->setValue(1.225);m_rho    ->setDecimals(3); m_rho    ->setSuffix(" kg/m\u00B3");
    m_vinf    = new QDoubleSpinBox; m_vinf   ->setRange(0.1, 500);  m_vinf   ->setValue(30);   m_vinf   ->setDecimals(2); m_vinf   ->setSuffix(" m/s");
    fWing->addRow("Chord c:",    m_chord);
    fWing->addRow("Wingspan b:", m_span);
    fWing->addRow("Oswald e:",   m_eOswald);
    fWing->addRow("Density ρ:",  m_rho);
    fWing->addRow("Vinf:",         m_vinf);
    inLay->addWidget(gWing);

    // Run button
    m_runBtn = new QPushButton("RUN SIMULATION");
    m_runBtn->setMinimumHeight(44);
    connect(m_runBtn, &QPushButton::clicked, this, &MainWindow::onSimulate); // Runs simulation on click

    inLay->addSpacing(6);
    inLay->addWidget(m_runBtn);
    inLay->addStretch(1);


    // Right Panel

    // Helper to build a chart with one series
    auto buildChart = [](QChartView*& view, QLineSeries*& series,
                         const QString& title, const QString& xLabel,
                         const QString& yLabel, const char* color)
    {
        series = new QLineSeries;
        series->setColor(QColor(color));

        QChart* chart = new QChart;
        chart->setTitle(title);
        chart->addSeries(series);

        QValueAxis* axX = new QValueAxis; axX->setTitleText(xLabel);
        QValueAxis* axY = new QValueAxis; axY->setTitleText(yLabel);
        chart->addAxis(axX, Qt::AlignBottom);
        chart->addAxis(axY, Qt::AlignLeft);
        series->attachAxis(axX);
        series->attachAxis(axY);

        styleChart(chart);

        view = new QChartView(chart);
        view->setRenderHint(QPainter::Antialiasing);
    };

    // Build all 9 charts
    buildChart(m_clView,    m_clSer,    "Lift Coefficient (2D)",   "alpha (deg)",    "CL",   C_CL);
    buildChart(m_cdView,    m_cdSer,    "Drag Coefficient",        "alpha (deg)",    "CD",   C_CD);
    buildChart(m_cmView,    m_cmSer,    "Moment Coefficient",      "alpha (deg)",    "CM",   C_CM);
    buildChart(m_polarView, m_polarSer, "Drag Polar",              "CD",         "CL",   C_POLAR);
    buildChart(m_ld2dView,  m_ld2dSer,  "Lift-to-Drag Ratio (2D)", "alpha (deg)",    "L/D",  C_LD2D);
    buildChart(m_ld3dView,  m_ld3dSer,  "Lift-to-Drag Ratio (3D)", "alpha (deg)",    "L/D",  C_LD3D);
    buildChart(m_cl3dView,  m_cl3dSer,  "Lift Curve (3D)",         "alpha 3D (deg)", "CL",   C_CL3D);
    buildChart(m_schrenkView, m_schrenkSer, "Schrenk Spanwise Lift",       "y / (b/2)", "Lift (N/m)", "#d4c89a");
    buildChart(m_lltView,     m_lltSer,     "Prandtl Lifting-Line Theory", "y / b",     "Lift (N/m)", "#b8941f");

    // Tabs for each chart
    QTabWidget* tabs = new QTabWidget;
    tabs->addTab(m_clView,    "CL vs alpha (2D)");
    tabs->addTab(m_cl3dView,  "CL vs alpha (3D)");
    tabs->addTab(m_cdView,    "CD vs alpha");
    tabs->addTab(m_cmView,    "CM vs alpha");
    tabs->addTab(m_polarView, "Drag Polar");
    tabs->addTab(m_ld2dView,  "L/D 2D");
    tabs->addTab(m_ld3dView,  "L/D 3D");
    tabs->addTab(m_schrenkView, "Spanwise Lift");
    tabs->addTab(m_lltView,     "Lifting Line");

    // Analysis data
    QGroupBox* gMetrics = new QGroupBox("ANALYSIS RESULTS");
    QGridLayout* mGrid = new QGridLayout(gMetrics);
    mGrid->setHorizontalSpacing(24);
    mGrid->setVerticalSpacing(4);

    QString lblStyle  = QString("color:%1; font-size:9px; letter-spacing:1.5px;").arg(BEIGE_DIM);
    QString valStyle  = "font-size:15px; font-weight:600; font-family:Consolas;";

    // Avoids repeating 9 seperate rows of data
    auto addRow = [&](int row, int col, const QString& name, QLabel*& out, const char* color) {
        QLabel* nameLbl = new QLabel(name);
        nameLbl->setStyleSheet(lblStyle);
        out = new QLabel("—");
        out->setStyleSheet(valStyle + QString("color:%1;").arg(color));
        mGrid->addWidget(nameLbl, row * 2,     col);
        mGrid->addWidget(out,     row * 2 + 1, col);
    };

    // Column 0: 2D metrics
    addRow(0, 0, "CL,max",          m_lblClMax,  GOLD);
    addRow(1, 0, "CD,min",          m_lblCdMin,  C_CD);
    addRow(2, 0, "(L/D)max  2D",    m_lblLd2,    GOLD);
    addRow(3, 0, "alpha @ CL,max",      m_lblStall,  BEIGE);
    addRow(4, 0, "CM range",        m_lblCm,     C_CM);
    // Column 1: 3D metrics
    addRow(0, 1, "Aspect Ratio",    m_lblAR,     GOLD);
    addRow(1, 1, "a3D  (1/rad)",    m_lblA3D,    GOLD);
    addRow(2, 1, "(L/D)max  3D",    m_lblLd3,    C_LD3D);
    addRow(3, 1, "alpha @ (L/D)max,3D", m_lblLdOpt,  BEIGE);
    mGrid->setRowStretch(10, 1);

    // Log
    m_log = new QTextEdit;
    m_log->setReadOnly(true);
    m_log->setPlaceholderText("Simulation log...");

    // Format charts, anaylsis, tabs, and input areas
    QSplitter* bottomSplit = new QSplitter(Qt::Horizontal);
    bottomSplit->addWidget(gMetrics);
    bottomSplit->addWidget(m_log);
    bottomSplit->setSizes({440, 440});

    QSplitter* rightSplit = new QSplitter(Qt::Vertical);
    rightSplit->addWidget(tabs);
    rightSplit->addWidget(bottomSplit);
    rightSplit->setSizes({580, 220});

    QSplitter* mainSplit = new QSplitter(Qt::Horizontal);
    mainSplit->addWidget(inputs);
    mainSplit->addWidget(rightSplit);
    mainSplit->setSizes({370, 1130});
    setCentralWidget(mainSplit);
}


// Push values into XFOIL and evaluate using Plane class objects
void MainWindow::onSimulate() {

    // Disable button during run
    m_runBtn->setEnabled(false);
    m_runBtn->setText("RUNNING...");

    // Push to XFOIL
    QString airfoil = "naca" + m_naca->text();
    m_plane.setAirfoil   (airfoil.toStdString());
    m_plane.setAlpha     (m_aStart->value(), m_aEnd->value(), m_aStep->value());
    m_plane.setOutputFile("polar.txt");
    m_plane.setViscous   (m_visc->isChecked());
    m_plane.setReynolds  (m_re->value());
    m_plane.setMach      (m_mach->value());
    m_plane.setNcrit     (m_ncrit->value());
    m_plane.setXtr       (1.0, 1.0);

    // Push 3D wing parameters
    m_plane.setChord   (m_chord->value());
    m_plane.setWingspan(m_span->value());
    m_plane.setEpsilon (m_eOswald->value());
    m_plane.setRho     (m_rho->value());
    m_plane.setVinf    (m_vinf->value());
    m_plane.calculateAspectRatio();

    // Spin up the worker thread
    m_thread = new QThread;
    m_worker = new XFoilWorker(&m_plane);
    m_worker->moveToThread(m_thread);
    connect(m_thread, &QThread::started,      m_worker, &XFoilWorker::run);
    connect(m_worker, &XFoilWorker::finished, this,     &MainWindow::onSimulationFinished);
    connect(m_worker, &XFoilWorker::finished, m_thread, &QThread::quit);
    connect(m_worker, &XFoilWorker::finished, m_worker, &QObject::deleteLater);
    connect(m_thread, &QThread::finished,     m_thread, &QObject::deleteLater);
    m_thread->start();
}


// Pull results from XFOIL and Plane class
void MainWindow::onSimulationFinished() {
    m_runBtn->setEnabled(true);
    m_runBtn->setText("RUN SIMULATION");

    auto alpha = m_plane.getAlpha();
    auto CL    = m_plane.getCL();
    auto CD    = m_plane.getCD();
    auto CM    = m_plane.getCM();

    if (alpha.empty()) {
        m_log->append("ERROR: XFOIL didn't converge!!!");
        return;
    }

    // 2D plots
    plot(m_clView,    m_clSer,    alpha, CL);
    plot(m_cdView,    m_cdSer,    alpha, CD);
    plot(m_cmView,    m_cmSer,    alpha, CM);
    plot(m_polarView, m_polarSer, CD,    CL);

    // L/D 2D
    std::vector<double> ld2(alpha.size());
    for (size_t i = 0; i < alpha.size(); ++i)
        ld2[i] = (CD[i] > 1e-8) ? CL[i] / CD[i] : 0.0;
    plot(m_ld2dView, m_ld2dSer, alpha, ld2);

    // L/D 3D
    std::vector<double> ld3(alpha.size());
    double ld3Best = 0, aLDopt = 0;
    for (size_t i = 0; i < alpha.size(); ++i) {
        double cd3 = m_plane.calculatedrag3D(CD[i], CL[i]);
        ld3[i] = (cd3 > 1e-8) ? CL[i] / cd3 : 0.0;
        if (ld3[i] > ld3Best) { ld3Best = ld3[i]; aLDopt = alpha[i]; }
    }
    plot(m_ld3dView, m_ld3dSer, alpha, ld3);

    // CL vs alpha 3D
    std::vector<double> alpha3D(alpha.size());
    for (size_t i = 0; i < alpha.size(); ++i)
        alpha3D[i] = m_plane.calculate3DAoA(alpha[i], CL[i]);
    plot(m_cl3dView, m_cl3dSer, alpha3D, CL);

    // Schrenk and LLT - delegated entirely to Plane
    auto schrenk = m_plane.schrenkApproximation();
    std::vector<double> sx(schrenk.size());
    for (size_t i = 0; i < schrenk.size(); ++i)
        sx[i] = (i + 0.5) / schrenk.size();          // y / (b/2) in [0, 1]
    plot(m_schrenkView, m_schrenkSer, sx, schrenk);

    auto llt = m_plane.liftingLineTheory(30);
    std::vector<double> lx(llt.size());
    const double PI = 3.14159265358979;
    for (size_t i = 0; i < llt.size(); ++i)
        lx[i] = -0.5 * std::cos((i + 1) * PI / (llt.size() + 1));   // y / b in [-0.5, 0.5]
    plot(m_lltView, m_lltSer, lx, llt);

    // Numerical results
    auto clMaxIt = std::max_element(CL.begin(), CL.end());
    double clMax  = *clMaxIt;
    double aStall = alpha[std::distance(CL.begin(), clMaxIt)];
    double cdMin  = *std::min_element(CD.begin(), CD.end());
    double cmMin  = *std::min_element(CM.begin(), CM.end());
    double cmMax  = *std::max_element(CM.begin(), CM.end());

    double ld2Best = 0;
    for (size_t i = 0; i < CL.size(); ++i)
        if (CD[i] > 1e-8) ld2Best = std::max(ld2Best, CL[i] / CD[i]);

    double AR  = m_plane.calculateAspectRatio();
    double a3D = m_plane.calculate3DLiftSlope();

    m_lblClMax->setText(QString::number(clMax,    'f', 4));
    m_lblCdMin->setText(QString::number(cdMin,    'f', 5));
    m_lblLd2  ->setText(QString::number(ld2Best,  'f', 1));
    m_lblStall->setText(QString::number(aStall,   'f', 2) + " \u00B0");
    m_lblCm   ->setText(QString("%1 -> %2").arg(cmMin, 0, 'f', 3).arg(cmMax, 0, 'f', 3));
    m_lblAR   ->setText(QString::number(AR,       'f', 2));
    m_lblA3D  ->setText(QString::number(a3D,      'f', 3));
    m_lblLd3  ->setText(QString::number(ld3Best,  'f', 1));
    m_lblLdOpt->setText(QString::number(aLDopt,   'f', 2) + " °");

    m_log->append(QString("%1 points  |  done").arg(alpha.size()));
}


// plots
void MainWindow::plot(QChartView* view, QLineSeries* series,
                      const std::vector<double>& x,
                      const std::vector<double>& y)
{
    series->clear();
    if (x.empty() || x.size() != y.size()) return;

    double xMin = x[0], xMax = x[0], yMin = y[0], yMax = y[0];
    for (size_t i = 0; i < x.size(); ++i) {
        series->append(x[i], y[i]);
        if (x[i] < xMin) xMin = x[i];
        if (x[i] > xMax) xMax = x[i];
        if (y[i] < yMin) yMin = y[i];
        if (y[i] > yMax) yMax = y[i];
    }
    double padX = std::max(1e-6, (xMax - xMin) * 0.05);
    double padY = std::max(1e-6, (yMax - yMin) * 0.05);
    view->chart()->axes(Qt::Horizontal).first()->setRange(xMin - padX, xMax + padX);
    view->chart()->axes(Qt::Vertical)  .first()->setRange(yMin - padY, yMax + padY);
}