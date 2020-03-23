//
//  OpenGLComponent.hpp
//  OpenGLAppTutorial - App
//
//  Created by Tim Arterbury on 3/21/20.
//  Copyright © 2020 TesserAct Music Technology LLC. All rights reserved.
//

#pragma once

#include <JuceHeader.h>
#include "OpenGLUtil/OpenGLUtil.hpp"
#include "ShapeVertices.hpp"

/** A custom JUCE Component which renders using OpenGL. You can use this class
    as a template for any Component you want to create which renders OpenGL inside
    of its view. If you were creating a spectrum visualizer, you might name this
    class SpectrumVisualizer.
 */
class OpenGLComponent : public Component,
                        private OpenGLRenderer,
                        private AsyncUpdater
{
public:

    OpenGLComponent();
    ~OpenGLComponent();
    
    // OpenGLRenderer Callbacks ================================================
    void newOpenGLContextCreated() override;
    void openGLContextClosing() override;
    void renderOpenGL() override;
    
    // Component Callbacks =====================================================
    void paint (Graphics& g) override;
    void resized () override;
    
    // Used to connect Draggable3DOrientation to mouse movments
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    
    // AsyncUpdater Callback ===================================================
    /** If the OpenGLRenderer thread needs to update some form JUCE GUI object
        reserved for the JUCE Message Thread, this callback allows the message
        thread to handle the update. */
    void handleAsyncUpdate() override;

private:
    
    /** Attempts to compile the OpenGL program at runtime and setup OpenGL variables. */
    void compileOpenGLShaderProgram();
    
    Matrix3D<GLfloat> calculateProjectionMatrix() const;
    Matrix3D<GLfloat> calculateViewMatrix() const;

    // OpenGL Variables
    OpenGLContext openGLContext;
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    
    OpenGLUtil::UniformWrapper projectionMatrix { "projectionMatrix" };
    OpenGLUtil::UniformWrapper viewMatrix {"viewMatrix" };
    
    GLuint VAO, VBO;
    std::vector<Vector3D<GLfloat>> vertices;
    
    // GUI Mouse Drag Interaction
    Draggable3DOrientation draggableOrientation;
    
    // GUI overlay status text
    String openGLStatusText;
    Label openGLStatusLabel;
};

