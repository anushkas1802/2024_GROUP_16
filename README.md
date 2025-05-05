@mainpage EEEE2046 - Software Engineering & VR Project

@section Introduction Introduction

This project represents a collaborative effort to integrate Virtual Reality (VR) rendering capabilities into a software application, enhancing its user interface and functionality. The project involved the development of a multi-threaded application, leveraging Qt for the primary GUI and VTK for 3D rendering and VR integration. The application allows users to interact with 3D models and visualize data within a VR environment.

@section Team_Members Team Members

The team members and their respective contributions are detailed below:

* Anushka Shrivastava

* Isaac Dooley

* Erika Tanigaki

@section Project_Description Project Description

The project aimed to create a robust and extensible application that incorporates VR functionality. This involved a combination of GUI development, 3D scene management, VR rendering pipeline implementation, and thorough documentation. The application allows users to interact with 3D models and visualize data within a VR environment.

Individual Contributions
Anushka Shrivastava
@section Anushka_Shrivastava Anushka Shrivastava

Anushka's primary focus was on the development and enhancement of the application's Qt-based Graphical User Interface (GUI). This involved:

@subsection Core_Qt_GUI_Development Core Qt GUI Development

* Building the fundamental structure of the application's interface using Qt widgets, layouts, and signals/slots.

This included creating windows, dialogs, menus, and other interactive elements.

@subsection Feature_Addition_to_the_GUI Feature Addition to the GUI

* Implementing new features to improve the user experience and application functionality.

This could include adding new controls, data display mechanisms, user input handling, and workflow enhancements.

@subsection Integration_with_VR_Functionality Integration with VR Functionality

* Ensuring seamless integration between the Qt GUI and the VR rendering components developed by Isaac.

This involved passing data, triggering VR rendering updates, and handling user input from both the GUI and VR environments.

@subsection GUI_Thread_Management GUI Thread Management

* Managing the Qt GUI thread to ensure responsiveness and prevent blocking operations, especially when interacting with the VR rendering thread.

Isaac Dooley
@section Isaac_Dooley Isaac Dooley

Isaac's main contribution centered around the integration of Virtual Reality (VR) rendering using VTK, and the projection of the 3D GUI. This encompassed:

@subsection VR_Rendering_Pipeline VR Rendering Pipeline

* Designing and implementing the VR rendering pipeline using VTK's OpenVR integration.

This involved setting up the renderer, render window, interactor, and camera for VR display.

@subsection 3D_Scene_Management 3D Scene Management

* Managing the 3D scene within the VR environment, including loading and displaying 3D models (e.g., STL files), handling transformations, and controlling object properties.

@subsection 3D_GUI_Projection 3D GUI Projection

* Implementing the logic to project 2D GUI elements into the 3D VR environment.

This allows users to interact with traditional GUI controls within the VR space, enhancing usability.

@subsection Multi_threading Multi-threading

* Developing a separate rendering thread to handle the VR rendering process, ensuring that the main GUI thread remains responsive.

This involved thread synchronization, data transfer, and command handling between the GUI and rendering threads.

@subsection VR_Device_Interaction VR Device Interaction

* Handling input from VR devices (headsets, controllers) and translating those inputs into meaningful actions within the application.

Erika Tanigaki
@section Erika_Tanigaki Erika Tanigaki

Erika's contributions spanned both documentation and VR feature development:

@subsection Documentation

* Creating comprehensive documentation for the project, including API documentation, user manuals, and design specifications.

This ensures that the project is well-documented, maintainable, and easy to understand for other developers and users.

@subsection VR_Features VR Features

* Contributing to the development of specific VR features, such as advanced interaction techniques, haptic feedback integration, or specialized rendering effects.

@subsection VR_Testing_and_Validation VR Testing and Validation

* Testing and validating the VR functionality to ensure proper operation, performance, and user experience within the VR environment.

@subsection VR_Scene_Optimization VR Scene Optimization

* Optimizing the 3D scene and rendering pipeline for VR performance, ensuring smooth frame rates and a comfortable VR experience.

@subsection Collaboration_with_Isaac Collaboration with Isaac

* Working closely with Isaac to integrate new VR features into the rendering pipeline and ensure compatibility with the overall application architecture.