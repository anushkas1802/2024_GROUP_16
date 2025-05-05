/**
 * @file VRRenderThread.h
 * @brief EEEE2046 - Software Engineering & VR Project
 * Template to add VR rendering to your application.
 * @author P Evans 2022
 */

#pragma once

#include <QThread>
#include <vtkActor.h>
#include <vtkActorCollection.h>
#include <vtkCallbackCommand.h>
#include <vtkDataSetMapper.h>
#include <vtkNamedColors.h>
#include <vtkOpenVRCamera.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkOpenVRRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>

class VRRenderThread : public QThread
{
    Q_OBJECT

public:
    /**
     * @brief Command to signal the VR rendering loop to terminate.
     */
    enum Command {
        END_RENDER,
        ROTATE_X,
        ROTATE_Y,
        ROTATE_Z
        // Add more commands as needed
    };

    /**
     * @brief Constructor for the VRRenderThread.
     *
     * This constructor is called by the MainWindow and runs in the primary program thread.
     * It initializes the actor list and command variables. The OpenVR rendering components
     * are initialized in the run() method, as the interactor needs to run in the VR thread.
     *
     * @param parent The parent QObject.
     */
    explicit VRRenderThread(QObject* parent = nullptr);

    /**
     * @brief Destructor for the VRRenderThread.
     *
     * This destructor is crucial for managing resources. It is called when the VR thread
     * is stopped. Ensure that all dynamically allocated memory and VTK objects are properly
     * released here to prevent memory leaks during repeated start/stop cycles of the VR thread.
     */
    ~VRRenderThread() override;

    /**
     * @brief Adds a VTK actor to the list of actors to be rendered in VR.
     *
     * This method adds a given vtkActor to the internal collection of actors that will be
     * part of the VR scene. It also performs an initial transformation to position the
     * actor in a sensible default location within the VR environment. This method is
     * intended to be called from the main GUI thread *before* the VR rendering thread is started
     * or when it's not running to avoid thread safety issues with VTK.
     *
     * @param actor A pointer to the vtkActor object to be added to the VR scene.
     */
    void addActorOffline(vtkSmartPointer<vtkActor> actor);

    /**
     * @brief Issues a command to the VR rendering thread to update rendering parameters.
     *
     * This method allows the main GUI thread to send commands to the VR rendering thread,
     * such as rotation updates or a signal to end the rendering loop. This is a thread-safe
     * way to control the VR scene from the GUI.
     *
     * @param cmd An integer representing the command to be executed (e.g., END_RENDER, ROTATE_X).
     * @param value A double value associated with the command (e.g., rotation angle).
     */
    void issueCommand(int cmd, double value);

protected:
    /**
     * @brief This function is the entry point for the VR rendering thread.
     *
     * It is executed when VRRenderThread::start() is called. This method initializes the
     * VTK OpenVR rendering pipeline, including the renderer, render window, interactor, and camera.
     * It then enters a rendering loop that continues until the interactor is done or an
     * END_RENDER command is received. Within the loop, it updates the VR scene and handles
     * VR events.
     */
    void run() override;

private:
    vtkSmartPointer<vtkActorCollection> actors;
    double rotateX;
    double rotateY;
    double rotateZ;
    volatile bool endRender; // volatile for thread-safe access
    vtkSmartPointer<vtkOpenVRRenderer> renderer;
    vtkSmartPointer<vtkOpenVRRenderWindow> window;
    vtkSmartPointer<vtkOpenVRCamera> camera;
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> interactor;
    long long t_last;
};

// VRRenderThread.cpp
#include "VRRenderThread.h"

VRRenderThread::VRRenderThread(QObject* parent)
    : QThread(parent),
    actors(vtkSmartPointer<vtkActorCollection>::New()),
    rotateX(0.0),
    rotateY(0.0),
    rotateZ(0.0),
    endRender(false),
    renderer(nullptr),
    window(nullptr),
    camera(nullptr),
    interactor(nullptr),
    t_last(0)
{
}

VRRenderThread::~VRRenderThread()
{
    endRender = true;
    wait(); // Wait for the thread to finish
}

void VRRenderThread::addActorOffline(vtkSmartPointer<vtkActor> actor)
{
    /* Check if the render thread is currently running. If not, it's safe to modify the actor list. */
    if (!isRunning()) {
        /* Get the original center (origin) of the actor's geometry. */
        double ac[3];
        actor->GetOrigin(ac);

        /* Apply initial transformations to position the actor in the VR scene.
         * These transformations might need adjustment based on the specific model
         * and desired orientation/position in VR.
         */
        actor->RotateX(-90);                                          /* Rotate the actor -90 degrees around the X-axis. */
        actor->AddPosition(-ac[0] + 0,                               /* Translate along X, centering based on original X. */
            -ac[1] - 100,                             /* Translate along Y, moving it back by 100 units. */
            -ac[2] - 200);                             /* Translate along Z, moving it down by 200 units. */

        /* Add the transformed actor to the collection of actors to be rendered. */
        actors->AddItem(actor);
    }
    /* If the thread is running, it's not safe to modify VTK objects from another thread.
     * Consider using a signal/slot mechanism (or thread-safe data structures) to the VR thread for runtime modifications.
     */
}

void VRRenderThread::issueCommand(int cmd, double value)
{
    /* Update the corresponding class variables based on the received command. */
    switch (cmd) {
        /* Command to signal the VR rendering loop to terminate. */
    case END_RENDER:
        this->endRender = true;
        break;

        /* Command to update the rotation angle around the X-axis. */
    case ROTATE_X:
        this->rotateX = value;
        break;

        /* Command to update the rotation angle around the Y-axis. */
    case ROTATE_Y:
        this->rotateY = value;
        break;

        /* Command to update the rotation angle around the Z-axis. */
    case ROTATE_Z:
        this->rotateZ