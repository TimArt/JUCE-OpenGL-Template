//
//  OpenGLComponent.hpp
//  OpenGLAppTutorial - App
//
//  Created by Tim Arterbury on 3/21/20.
//  Copyright © 2020 JUCE. All rights reserved.
//

#pragma once

#include <JuceHeader.h>
#include "OpenGLBasicUtils.hpp"

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
    
    void setRenderingActive (bool shouldContinuouslyRender);
    
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
    
    
    std::vector<Vector3D<GLfloat>> generateTriangleVertices();
    std::vector<Vector3D<GLfloat>> generateCubeVertices();

private:
    
    /** Attempts to compile the OpenGL program at runtime and setup OpenGL variables. */
    void compileOpenGLShaderProgram();
    
    
    /** Calculates and returns the Projection Matrix.
     */
    void refreshProjectionMatrix()
    {
        float w = 1.0f / (0.5f + 0.1f);
        float h = w * getLocalBounds().toFloat().getAspectRatio (false);
        projectionMatrix = Matrix3D<GLfloat>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
    }
    
    /** Calculates and returns the View Matrix.
     */
    void refreshViewMatrix()
    {
        Matrix3D<GLfloat> scale (AffineTransform::scale (3.0f));
        Matrix3D<GLfloat> rotate = draggableOrientation.getRotationMatrix();
        Matrix3D<GLfloat> translate (Vector3D<GLfloat> (0.0f, 0.0f, -10.0f));
        
        viewMatrix = rotate * scale * translate;
    }
    

    // OpenGL Variables
    OpenGLContext openGLContext;
    GLuint VBO, VAO; /* EBO */
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    std::unique_ptr<Uniforms> uniforms;
    
    std::vector<Vector3D<GLfloat>> vertices;

    Matrix3D<GLfloat> projectionMatrix;
    Matrix3D<GLfloat> viewMatrix;
    
//    GLfloat vertices[9] = {
//        -0.5f, -0.5f, 0.0f,
//         0.5f, -0.5f, 0.0f,
//         0.0f,  0.5f, 0.0f
//    };
    
    // GUI Mouse Drag Interaction
    Draggable3DOrientation draggableOrientation;
    
    // GUI overlay status text
    String openGLStatusText;
    Label openGLStatusLabel;

};

