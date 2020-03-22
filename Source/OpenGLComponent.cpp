//
//  OpenGLComponent.cpp
//  OpenGLAppTutorial - App
//
//  Created by Tim Arterbury on 3/21/20.
//  Copyright © 2020 JUCE. All rights reserved.
//

#include "OpenGLComponent.hpp"


OpenGLComponent::OpenGLComponent()
{
    // Sets the OpenGL version to 3.2
    openGLContext.setOpenGLVersionRequired (OpenGLContext::OpenGLVersion::openGL3_2);

    // Set default 3D orientation for the draggable GUI tool
    draggableOrientation.reset ({ 0.0, 1.0, 0.0 });

    // Attach the OpenGL context
    openGLContext.setRenderer (this);
    openGLContext.attachTo (*this);

    // Setup OpenGL GUI Overlay Label: Status of Shaders, compiler errors, etc.
    addAndMakeVisible (openGLStatusLabel);
    openGLStatusLabel.setJustificationType (Justification::topLeft);
    openGLStatusLabel.setFont (Font (14.0f));
    
    
    vertices = generateCubeVertices();
}

OpenGLComponent::~OpenGLComponent()
{
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

void OpenGLComponent::setRenderingActive (bool shouldContinuouslyRender)
{
    openGLContext.setContinuousRepainting (shouldContinuouslyRender);
    openGLContext.setComponentPaintingEnabled (shouldContinuouslyRender);
}

// OpenGLRenderer Callbacks ================================================
void OpenGLComponent::newOpenGLContextCreated()
{
    compileOpenGLShaderProgram();
    
    // Generate vertex objects
    openGLContext.extensions.glGenBuffers (1, &VBO); // Vertex Buffer Object
    openGLContext.extensions.glGenVertexArrays(1, &VAO); // Vertex Array Object
    
    
    // Bind vertex objects to data =============================================
    
    // VAO (Vertex Buffer Object)
    openGLContext.extensions.glBindVertexArray (VAO);
    
    // VBO (Vertex Buffer Object) - Bind and Write to Buffer
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER, sizeof (GLfloat) * vertices.size() * 3,
                                           vertices.data(), GL_STATIC_DRAW);


    // Define how vertices are laid out
    openGLContext.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
                                                    3 * sizeof (GLfloat), NULL);
    openGLContext.extensions.glEnableVertexAttribArray (0);
    
    
    // OpenGL styling preferences
    glEnable (GL_BLEND); // Enable alpha blending, allowing for transparency of objects
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show wireframe
}

void OpenGLComponent::openGLContextClosing()
{
    shaderProgram.reset();
    uniforms.reset();
}

void OpenGLComponent::renderOpenGL()
{
    jassert (OpenGLHelpers::isContextActive());
    
    // Scale viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    glViewport (0, 0, roundToInt (renderingScale * getWidth()), roundToInt (renderingScale * getHeight()));

    // Set background Color
    OpenGLHelpers::clear (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // Select shader program
    shaderProgram->use();

    // Setup the Uniforms for use in the Shader
    if (uniforms->projectionMatrix != nullptr)
    {
        refreshProjectionMatrix();
        uniforms->projectionMatrix->setMatrix4 (projectionMatrix.mat, 1, false);
    }

    if (uniforms->viewMatrix != nullptr)
    {
        refreshViewMatrix();
        uniforms->viewMatrix->setMatrix4 (viewMatrix.mat, 1, false);
    }
    
    // Draw Vertices
    openGLContext.extensions.glBindVertexArray (VAO);
    glDrawArrays (GL_TRIANGLES, 0, (int) vertices.size());

//    // Reset the element buffers so child Components draw correctly
//    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
//    openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
//    openGLContext.extensions.glBindVertexArray (0);
}

// Component Callbacks =====================================================
void OpenGLComponent::paint (Graphics& g)
{
    // You can optionally paint any JUCE graphics over the top of your OpenGL graphics
}

void OpenGLComponent::resized ()
{
    draggableOrientation.setViewport (getLocalBounds());
    openGLStatusLabel.setBounds (getLocalBounds().reduced (4).removeFromTop (75));
}


void OpenGLComponent::mouseDown (const MouseEvent& e)
{
    draggableOrientation.mouseDown (e.getPosition());
}

void OpenGLComponent::mouseDrag (const MouseEvent& e)
{
    draggableOrientation.mouseDrag (e.getPosition());
}


// AsyncUpdater Callback =======================================================
void OpenGLComponent::handleAsyncUpdate()
{
    openGLStatusLabel.setText (openGLStatusText, dontSendNotification);
}

std::vector<Vector3D<GLfloat>> OpenGLComponent::generateTriangleVertices()
{
    return { { -0.5f, -0.5f, 0.0f }, { 0.5f, -0.5f, 0.0f }, { 0.0f,  0.5f, 0.0f } };
}

std::vector<Vector3D<GLfloat>> OpenGLComponent::generateCubeVertices()
{
    std::vector<Vector3D<GLfloat>> vertices;
    
    const float offset = 0.5f;
    for (int face = 0; face < 6; face++)
    {
        vertices.push_back ({  });
    }
    return { { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f },
             { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f },
             { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f },
             { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, 0.5f },
             { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }
    
    
    };
    return vertices;
}

void OpenGLComponent::compileOpenGLShaderProgram()
{
    std::unique_ptr<OpenGLShaderProgram> shaderProgramAttempt
        = std::make_unique<OpenGLShaderProgram> (openGLContext);

    // Attempt to compile the program
    if (shaderProgramAttempt->addVertexShader ({ BinaryData::BasicVertex_glsl })
        && shaderProgramAttempt->addFragmentShader ({ BinaryData::BasicFragment_glsl })
        && shaderProgramAttempt->link())
    {
        uniforms.reset (nullptr);
        
        shaderProgram.reset (shaderProgramAttempt.release());
        shaderProgram->use();
        
        uniforms = std::make_unique<Uniforms> (openGLContext, *shaderProgram);
        
        openGLStatusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        openGLStatusText = shaderProgramAttempt->getLastError();
    }

    triggerAsyncUpdate();
}
