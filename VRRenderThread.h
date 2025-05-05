/**
 * @file VRRenderThread.h
 * @brief EEEE2046 - Software Engineering & VR Project
 * Template to add VR rendering to your application.
 * @author P Evans 2022
 */
#ifndef VR_RENDER_THREAD_H
#define VR_RENDER_THREAD_H

 /* Project headers */

 /* Qt headers */
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

/* Vtk headers */
#include <vtkActor.h>
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkOpenVRRenderer.h>
#include <vtkOpenVRCamera.h>
#include <vtkActorCollection.h>
#include <vtkCommand.h>

/**
 * @brief This class inherits from the Qt class QThread which allows it to be a parallel thread
 * to the main() thread, and also from vtkCommand which allows it to act as a "callback" for the
 * vtkRenderWindowInteractor. This callback functionallity means that once the renderWindowInteractor
 * takes control of this thread to enable VR, it can callback to a function in the class to check to see
 * if the user has requested any changes
 */
class VRRenderThread : public QThread {
    Q_OBJECT

public:
    /**
     * @brief List of command names
     */
    enum {
        END_RENDER,     /**< Command to end the rendering loop */
        ROTATE_X,       /**< Command to rotate around the X-axis */
        ROTATE_Y,       /**< Command to rotate around the Y-axis */
        ROTATE_Z        /**< Command to rotate around the Z-axis */
    } Command;

    /**
     * @brief Constructor
     *
     * Constructs a VRRenderThread object. Initializes member variables.
     *
     * @param parent The parent QObject (optional).
     */
    VRRenderThread(QObject* parent = nullptr);

    /**
     * @brief Destructor
     *
     * Destroys the VRRenderThread object. Ensures proper cleanup of resources.
     */
    ~VRRenderThread();

    /**
     * @brief Adds a VTK actor to the VR renderer before the VR interactor starts.
     *
     * This method allows adding vtkActor objects to the internal collection
     * before the VR rendering loop and interactor are initialized. This is useful
     * for setting up the initial VR scene.
     *
     * @param actor A pointer to the vtkActor to be added.
     */
    void addActorOffline(vtkActor* actor);

    /**
     * @brief Issues a command to the VR thread in a thread-safe manner.
     *
     * This function sets internal variables to indicate the type of action
     * (e.g., animation, visual effect) to perform in the rendering thread.
     * The rendering thread will then implement the requested action.
     *
     * @param cmd An integer representing the command to be executed (from the Command enum).
     * @param value A double value associated with the command (e.g., rotation angle).
     */
    void issueCommand(int cmd, double value);

protected:
    /**
     * @brief Re-implementation of the QThread::run() function.
     *
     * This function is the entry point for the VR rendering thread. It initializes
     * the VTK VR rendering components and starts the main rendering loop.
     */
    void run() override;

private:
    /* Standard VTK VR Classes */
    vtkSmartPointer<vtkOpenVRRenderWindow>         window;     /**< The OpenVR render window */
    vtkSmartPointer<vtkOpenVRRenderWindowInteractor> interactor; /**< The OpenVR render window interactor */
    vtkSmartPointer<vtkOpenVRRenderer>             renderer;   /**< The OpenVR renderer */
    vtkSmartPointer<vtkOpenVRCamera>               camera;     /**< The OpenVR camera */

    /* Use to synchronise passing of data to VR thread */
    QMutex                                          mutex;      /**< Mutex for thread synchronization */
    QWaitCondition                                  condition;  /**< Wait condition for thread synchronization */

    /**
     * @brief List of actors that will need to be added to the VR scene
     */
    vtkSmartPointer<vtkActorCollection>             actors;     /**< Collection of actors in the VR scene */

    /**
     * @brief A timer to help implement animations and visual effects
     */
    std::chrono::time_point<std::chrono::steady_clock> t_last;    /**< Last time point for animation timing */

    /**
     * @brief Flag to indicate if the rendering loop should end.
     *
     * This will be set to false by the constructor. If set to true by the GUI,
     * the rendering loop in the run() method will terminate.
     */
    bool                                            endRender;

    /* Some variables to indicate animation actions to apply.
     *
     */
    double rotateX;     /**< Degrees to rotate around X axis (per time-step) */
    double rotateY;     /**< Degrees to rotate around Y axis (per time-step) */
    double rotateZ;     /**< Degrees to rotate around Z axis (per time-step) */
};

#endif // VR_RENDER_THREAD_H