//
//  OpenGLComponent.cpp
//  OpenGLAppTutorial - App
//
//  Created by Tim Arterbury on 3/21/20.
//  Copyright © 2020 TesserAct Music Technology LLC. All rights reserved.
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
    openGLContext.setContinuousRepainting (true); // Enable rendering

    // Setup OpenGL GUI Overlay Label: Status of Shaders, compiler errors, etc.
    addAndMakeVisible (openGLStatusLabel);
    openGLStatusLabel.setJustificationType (Justification::topLeft);
    openGLStatusLabel.setFont (Font (14.0f));
}

OpenGLComponent::~OpenGLComponent()
{
    openGLContext.setContinuousRepainting (false);
    openGLContext.detach();
}

// OpenGLRenderer Callbacks ================================================
void OpenGLComponent::newOpenGLContextCreated()
{
    compileOpenGLShaderProgram();
    
    vertices = ShapeVertices::generateTriangle(); // Setup vertices
    
    // Generate opengl vertex objects ==========================================
    openGLContext.extensions.glGenVertexArrays(1, &VAO); // Vertex Array Object
    openGLContext.extensions.glGenBuffers (1, &VBO);     // Vertex Buffer Object
    
    
    // Bind opengl vertex objects to data ======================================
    openGLContext.extensions.glBindVertexArray (VAO);
    
    // Fill VBO buffer with vertices array
    openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, VBO);
    openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                           sizeof (GLfloat) * vertices.size() * 3,
                                           vertices.data(),
                                           GL_STATIC_DRAW);


    // Define that our vertices are laid out as groups of 3 GLfloats
    openGLContext.extensions.glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE,
                                                    3 * sizeof (GLfloat), NULL);
    openGLContext.extensions.glEnableVertexAttribArray (0);
    
    
    // Optional OpenGL styling commands ========================================
    
    // glEnable (GL_BLEND); // Enable alpha blending, allowing for transparency of objects
    // glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Paired with above line
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Show wireframe
}

void OpenGLComponent::openGLContextClosing()
{
    // Add any OpenGL related cleanup code here . . .
}

void OpenGLComponent::renderOpenGL()
{
    jassert (OpenGLHelpers::isContextActive());
    
    // Scale viewport
    const float renderingScale = (float) openGLContext.getRenderingScale();
    glViewport (0, 0, roundToInt (renderingScale * getWidth()), roundToInt (renderingScale * getHeight()));

    // Set background color
    OpenGLHelpers::clear (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    // Select shader program
    shaderProgram->use();

    // Setup the Uniforms for use in the Shader
    if (projectionMatrix)
        projectionMatrix->setMatrix4 (calculateProjectionMatrix().mat, 1, false);
    if (viewMatrix)
        viewMatrix->setMatrix4 (calculateViewMatrix().mat, 1, false);
    
    // Draw Vertices
    openGLContext.extensions.glBindVertexArray (VAO);
    glDrawArrays (GL_TRIANGLES, 0, (int) vertices.size());
    openGLContext.extensions.glBindVertexArray (0);
}

// JUCE Component Callbacks ====================================================
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

void OpenGLComponent::handleAsyncUpdate()
{
    openGLStatusLabel.setText (openGLStatusText, dontSendNotification);
}

// OpenGL Related Member Functions =============================================
void OpenGLComponent::compileOpenGLShaderProgram()
{
    std::unique_ptr<OpenGLShaderProgram> shaderProgramAttempt
        = std::make_unique<OpenGLShaderProgram> (openGLContext);

    // Attempt to compile the program
    if (shaderProgramAttempt->addVertexShader ({ BinaryData::BasicVertex_glsl })
        && shaderProgramAttempt->addFragmentShader ({ BinaryData::BasicFragment_glsl })
        && shaderProgramAttempt->link())
    {
        projectionMatrix.disconnectFromShaderProgram();
        viewMatrix.disconnectFromShaderProgram();
        
        shaderProgram.reset (shaderProgramAttempt.release());
        
        projectionMatrix.connectToShaderProgram (openGLContext, *shaderProgram);
        viewMatrix.connectToShaderProgram (openGLContext, *shaderProgram);
        
        openGLStatusText = "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2);
    }
    else
    {
        openGLStatusText = shaderProgramAttempt->getLastError();
    }

    triggerAsyncUpdate(); // Update status text
}


Matrix3D<GLfloat> OpenGLComponent::calculateProjectionMatrix() const
{
    float w = 1.0f / (0.5f + 0.1f);
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    return Matrix3D<GLfloat>::fromFrustum (-w, w, -h, h, 4.0f, 30.0f);
}


Matrix3D<GLfloat> OpenGLComponent::calculateViewMatrix() const
{
    Matrix3D<GLfloat> scale (AffineTransform::scale (3.0f));
    Matrix3D<GLfloat> rotate = draggableOrientation.getRotationMatrix();
    Matrix3D<GLfloat> translate (Vector3D<GLfloat> (0.0f, 0.0f, -10.0f));
    return rotate * scale * translate;
}
