/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2017 - ROLI Ltd.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/

#pragma once

namespace OpenGLUtil
{

static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& context,
                                                    OpenGLShaderProgram& shaderProgram,
                                                    const String& uniformName)
{
    if (context.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName.toRawUTF8()) < 0)
        return nullptr;

    return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName.toRawUTF8());
}

/** A class that makes it easier to work with a OpenGLShaderProgram::Uniform.
    It permanently associates a particular name with the uniform which is usually
    how Uniform objects tend to be used. This makes using a Uniform a bit cleaner
    instead of having the rewrite name strings in multiple places.
 
    I hope functionality such as this gets integrated into the juce::Uniform class
    in the future,
 */
class UniformWrapper
{
public:
    UniformWrapper (const String & uniformName)
    {
        this->uniformName = uniformName;
    }
    
    operator bool() const noexcept
    {
        return uniform.get() != nullptr;
    }
    
    OpenGLShaderProgram::Uniform * operator->() const noexcept
    {
        return uniform.get();
    }
    
    void connectToShaderProgram (OpenGLContext & context, OpenGLShaderProgram & shaderProgram)
    {
        uniform.reset (createUniform (context, shaderProgram, uniformName));
    }
    
    void disconnectFromShaderProgram()
    {
        uniform.reset();
    }
    
private:
    String uniformName;
    std::unique_ptr<OpenGLShaderProgram::Uniform> uniform;

};

//==============================================================================

/** Vertex data to be passed to the shaders.
     For the purposes of this demo, each vertex will have a 3D position, a colour and a
     2D texture co-ordinate. Of course you can ignore these or manipulate them in the
     shader programs but are some useful defaults to work from.
*/
struct Vertex
{
    float position[3];
    float normal[3];
    float colour[4];
    float texCoord[2];
};

//==============================================================================
// This class just manages the attributes that the shaders use.
struct Attributes
{
    Attributes (OpenGLContext& context, OpenGLShaderProgram& shaderProgram)
    {
        position      .reset (createAttribute (context, shaderProgram, "position"));
        normal        .reset (createAttribute (context, shaderProgram, "normal"));
        sourceColour  .reset (createAttribute (context, shaderProgram, "sourceColour"));
        textureCoordIn.reset (createAttribute (context, shaderProgram, "textureCoordIn"));
    }

    void enable (OpenGLContext& context)
    {
        if (position.get() != nullptr)
        {
            context.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
            context.extensions.glEnableVertexAttribArray (position->attributeID);
        }

        if (normal.get() != nullptr)
        {
            context.extensions.glVertexAttribPointer (normal->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
            context.extensions.glEnableVertexAttribArray (normal->attributeID);
        }

        if (sourceColour.get() != nullptr)
        {
            context.extensions.glVertexAttribPointer (sourceColour->attributeID, 4, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 6));
            context.extensions.glEnableVertexAttribArray (sourceColour->attributeID);
        }

        if (textureCoordIn.get() != nullptr)
        {
            context.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 10));
            context.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
        }
    }

    void disable (OpenGLContext& context)
    {
        if (position.get() != nullptr)       context.extensions.glDisableVertexAttribArray (position->attributeID);
        if (normal.get() != nullptr)         context.extensions.glDisableVertexAttribArray (normal->attributeID);
        if (sourceColour.get() != nullptr)   context.extensions.glDisableVertexAttribArray (sourceColour->attributeID);
        if (textureCoordIn.get() != nullptr) context.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
    }

    std::unique_ptr<OpenGLShaderProgram::Attribute> position, normal, sourceColour, textureCoordIn;

private:
    static OpenGLShaderProgram::Attribute* createAttribute (OpenGLContext& context,
                                                            OpenGLShaderProgram& shader,
                                                            const String& attributeName)
    {
        if (context.extensions.glGetAttribLocation (shader.getProgramID(), attributeName.toRawUTF8()) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Attribute (shader, attributeName.toRawUTF8());
    }
};

//==============================================================================
// This class just manages the uniform values that the demo shaders use.
/*
class Uniforms
{
public:
    Uniforms (OpenGLContext& context, OpenGLShaderProgram& shaderProgram)
    {
        projectionMatrix.reset (createUniform (context, shaderProgram, "projectionMatrix"));
        viewMatrix.reset (createUniform (context, shaderProgram, "viewMatrix"));
    }

private:

    std::map<std::string, std::unique_ptr<OpenGLShaderProgram::Uniform>> uniforms;
    
    
    static OpenGLShaderProgram::Uniform* createUniform (OpenGLContext& context,
                                                        OpenGLShaderProgram& shaderProgram,
                                                        const String& uniformName)
    {
        if (context.extensions.glGetUniformLocation (shaderProgram.getProgramID(), uniformName.toRawUTF8()) < 0)
            return nullptr;

        return new OpenGLShaderProgram::Uniform (shaderProgram, uniformName.toRawUTF8());
    }
};*/

} // OpenGLUtil
