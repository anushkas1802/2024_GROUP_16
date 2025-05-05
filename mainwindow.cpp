}

/**
 * @brief Destructor. Stops the VR thread if it is running and cleans up resources.
 */
MainWindow::~MainWindow()
{
    if (vrThread && vrThread->isRunning()) {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0);
        vrThread->wait();
    }
    delete vrThread;
    delete ui;
}

/**
 * @brief Initializes the VTK rendering window and renderer.
 */
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

/**
 * @brief Updates the status bar with a message.
 * @param message Message to display.
 * @param timeout Duration in milliseconds.
 */
void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

/**
 * @brief Dummy button used for initial testing.
 */
void MainWindow::handleButton()
{
    QMessageBox msgBox;
    msgBox.setText("Add button was clicked");
    msgBox.exec();
    emit statusUpdateMessageSignal("Add button was clicked", 2000);
}

/**
 * @brief Opens the option dialog for the selected tree item.
 */
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

        if (vrThread && vrThread->isRunning()) {
            vtkActor* actor = selectedPart->getActor();
        }

        selectedPart->setVisible(optionDialog.isVisible());
        partList->dataChanged(index, index);
        updateRender();
        emit statusUpdateMessageSignal("Updated item options", 2000);
    }
}

/**
 * @brief Opens the option dialog (for testing purposes).
 */
void MainWindow::handleOpenOptions()
{
    OptionDialog optionDialog(this);
    optionDialog.exec();
    emit statusUpdateMessageSignal("Open Options button was clicked", 2000);
}

/**
 * @brief Emits a status message when a tree item is clicked.
 */
void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessageSignal("Selected item: " + text, 2000);
    }
}

/**
 * @brief Handles opening a directory and loading all STL files within.
 */
void MainWindow::on_actionOpenFile_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Repositry Folder", QDir::homePath());

    if (!folderPath.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();
        loadInitialPartsFromFolder(folderPath);
    }
}

/**
 * @brief Shows a context menu when right-clicking on a tree view item.
 * @param pos The position in the tree view.
 */
void MainWindow::showContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid()) return;

    ui->treeView->setCurrentIndex(index);
    QMenu contextMenu(this);
    contextMenu.addAction(ui->actionItemOptions);
    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

/**
 * @brief Updates the render window with currently visible parts.
 */
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

/**
 * @brief Recursively adds visible parts to the VTK renderer.
 * @param index Current index in the model tree.
 */
void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();

        if (actor) {
            renderer->AddActor(actor);
        }
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

/**
 * @brief Loads model parts from the given folder and its subfolders.
 * @param folderPath The path to the directory.
 */
void MainWindow::loadInitialPartsFromFolder(const QString& folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << folderPath;
        return;
    }

    loadPartsRecursively(dir, partList->getRootItem());
    updateRender();
}