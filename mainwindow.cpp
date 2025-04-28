#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ModelPart.h"
#include "ModelPartList.h"
#include "optiondialog.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->addButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(ui->openOptions, &QPushButton::released, this, &MainWindow::handleOpenOptions);
    connect(this, &MainWindow::statusUpdateMessageSignal, ui->statusbar, &QStatusBar::showMessage);

    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    setupVTK();

    // Load folder path from environment variable
    QString folderPath = qEnvironmentVariable("LEVEL0_PATH");

    // If environment variable is not set or empty, ask user to select a folder
    if (folderPath.isEmpty()) {
        folderPath = QFileDialog::getExistingDirectory(
            this,
            "Select Initial Parts Folder",
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );
    }

    // If still empty (user cancelled), use fallback
    if (folderPath.isEmpty()) {
        folderPath = "D:/Level0"; // Default fallback
        QMessageBox::warning(this, "Folder Not Selected", "No folder selected. Using default path: D:/Level0");
    }

    qDebug() << "Using folder path:" << folderPath;
    loadInitialPartsFromFolder(folderPath);

    emit statusUpdateMessageSignal("Loaded Level0 parts (invisible)", 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupVTK()
{
    if (!ui->vtkWidget) {
        qWarning("vtkWidget is not initialized in the UI file!");
        return;
    }

    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);

    renderer->SetBackground(0.1, 0.1, 0.1);
    renderWindow->Render();
}

void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

void MainWindow::handleButton()
{
    QMessageBox msgBox;
    msgBox.setText("Add button was clicked");
    msgBox.exec();
    emit statusUpdateMessageSignal("Add button was clicked", 2000);
}

void MainWindow::on_actionItemOptions_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (!selectedPart) {
        QMessageBox::warning(this, "No Selection", "Please select an item first.");
        return;
    }

    OptionDialog optionDialog(this);
    QColor currentColor(selectedPart->getColourR(), selectedPart->getColourG(), selectedPart->getColourB());
    optionDialog.setValues(selectedPart->data(0).toString(), currentColor, selectedPart->visible());

    if (optionDialog.exec() == QDialog::Accepted) {
        selectedPart->setData(0, optionDialog.getName());

        QColor chosenColor = optionDialog.getColor();
        selectedPart->setColour(chosenColor.red(), chosenColor.green(), chosenColor.blue());

        selectedPart->setVisible(optionDialog.isVisible());
        partList->dataChanged(index, index);

        updateRender();
        emit statusUpdateMessageSignal("Updated item options", 2000);
    }
}

void MainWindow::handleOpenOptions()
{
    OptionDialog optionDialog(this);
    optionDialog.exec();
    emit statusUpdateMessageSignal("Open Options button was clicked", 2000);
}

void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessageSignal("Selected item: " + text, 2000);
    }
}

void MainWindow::on_actionOpenFile_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "Open File", "C:/", "STL Files (*.stl);;Text Files (*.txt)");

    if (!fileName.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();

        ModelPart* newPart = new ModelPart({ fileName, 0 });
        partList->getRootItem()->appendChild(newPart);
        newPart->loadSTL(fileName);
        newPart->setVisible(true);

        updateRender();

        QFileInfo fileInfo(fileName);
        emit statusUpdateMessageSignal("Opened file: " + fileInfo.fileName(), 2000);
    }
    else {
        emit statusUpdateMessageSignal("File open canceled", 2000);
    }
}

void MainWindow::showContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid()) return;

    ui->treeView->setCurrentIndex(index); // Make sure context menu affects selected part
    QMenu contextMenu(this);
    contextMenu.addAction(ui->actionItemOptions);
    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();

    int topLevelCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelCount; ++i) {
        QModelIndex topIndex = partList->index(i, 0, QModelIndex());
        updateRenderFromTree(topIndex);
    }

    if (renderer->GetActors()->GetNumberOfItems() > 0) {
        renderer->ResetCamera();
    }

    renderWindow->Render();
}

void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        qDebug() << "Checking part:" << selectedPart->data(0).toString()
            << "| Visible:" << selectedPart->visible()
            << "| Actor:" << (actor != nullptr);

        if (actor) {
            renderer->AddActor(actor);
        }
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

void MainWindow::loadInitialPartsFromFolder(const QString& folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << folderPath;
        return;
    }

    QStringList filters;
    filters << "*.stl" << "*.STL";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        ModelPart* part = new ModelPart({ fileInfo.fileName(), 0 }, partList->getRootItem());
        partList->getRootItem()->appendChild(part);

        part->loadSTL(filePath);
        part->setVisible(false);  // Default invisible

        qDebug() << "Loaded" << filePath << "and set to invisible.";
    }

    updateRender();
}
