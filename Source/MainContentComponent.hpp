//
//  MainContentComponent.hpp
//  OpenGLAppTutorial - App
//
//  Created by Tim Arterbury on 3/21/20.
//  Copyright © 2020 TesserAct Music Technology LLC. All rights reserved.
//

#pragma once

#include "OpenGLComponent.hpp"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component
{
public:
    //==============================================================================
    MainContentComponent()
    {
        // Stylin'
        getLookAndFeel().setColour (ResizableWindow::backgroundColourId, Colour (0xFF292a30));
        
        addAndMakeVisible (openGLComponent);
        setSize (800, 600);
    }

    void resized() override
    {
        openGLComponent.setBounds (getLocalBounds());
    }

private:
    OpenGLComponent openGLComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
