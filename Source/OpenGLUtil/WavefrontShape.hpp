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

#include "WavefrontObjFile.hpp"
#include "OpenGLBasicUtils.hpp"

/** A 3D Shape created from a WaveFrontObjFile

    This loads a 3D model from an OBJ file and converts it into some vertex buffers
    that we can draw.
 
    This utility was extracted from JUCE's OpenGL tutorial on their website:
    https://docs.juce.com/master/tutorial_open_gl_application.html
    It is included here as a library-like utility.
*/
struct Shape
{
    Shape (OpenGLContext& context)
    {
        auto dir = File::getCurrentWorkingDirectory();

        int numTries = 0;

        while (! dir.getChildFile ("Resources").exists() && numTries++ < 15)
            dir = dir.getParentDirectory();

        if (shapeFile.load (dir.getChildFile ("Resources").getChildFile ("teapot.obj")).wasOk())
            for (auto* s : shapeFile.shapes)
                vertexBuffers.add (new VertexBuffer (context, *s));
    }

    void draw (OpenGLContext& context, Attributes& glAttributes)
    {
        for (auto* vertexBuffer : vertexBuffers)
        {
            vertexBuffer->bind();

            glAttributes.enable (context);
            glDrawElements (GL_TRIANGLES, vertexBuffer->numIndices, GL_UNSIGNED_INT, 0);
            glAttributes.disable (context);
        }
    }

private:
    struct VertexBuffer
    {
        VertexBuffer (OpenGLContext& context, WavefrontObjFile::Shape& aShape) : openGLContext (context)
        {
            numIndices = aShape.mesh.indices.size();

            openGLContext.extensions.glGenBuffers (1, &vertexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);

            Array<Vertex> vertices;
            createVertexListFromMesh (aShape.mesh, vertices, Colours::green);

            openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                                   static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                                   vertices.getRawDataPointer(), GL_STATIC_DRAW);

            openGLContext.extensions.glGenBuffers (1, &indexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                                   static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                                                   aShape.mesh.indices.getRawDataPointer(), GL_STATIC_DRAW);
        }

        ~VertexBuffer()
        {
            openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
            openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
        }

        void bind()
        {
            openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
            openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        }

        GLuint vertexBuffer, indexBuffer;
        int numIndices;
        OpenGLContext& openGLContext;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VertexBuffer)
    };

    WavefrontObjFile shapeFile;
    OwnedArray<VertexBuffer> vertexBuffers;

    static void createVertexListFromMesh (const WavefrontObjFile::Mesh& mesh, Array<Vertex>& list, Colour colour)
    {
        auto scale = 0.2f;
        WavefrontObjFile::TextureCoord defaultTexCoord { 0.5f, 0.5f };
        WavefrontObjFile::Vertex defaultNormal { 0.5f, 0.5f, 0.5f };

        for (auto i = 0; i < mesh.vertices.size(); ++i)
        {
            const auto& v = mesh.vertices.getReference (i);
            const auto& n = i < mesh.normals.size() ? mesh.normals.getReference (i) : defaultNormal;
            const auto& tc = i < mesh.textureCoords.size() ? mesh.textureCoords.getReference (i) : defaultTexCoord;

            list.add ({ { scale * v.x, scale * v.y, scale * v.z, },
                        { scale * n.x, scale * n.y, scale * n.z, },
                        { colour.getFloatRed(), colour.getFloatGreen(), colour.getFloatBlue(), colour.getFloatAlpha() },
                        { tc.x, tc.y } });
        }
    }
};
