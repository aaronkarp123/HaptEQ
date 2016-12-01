/*
 Audealize

 Author: Michael Donovan
 
 http://music.cs.northwestern.edu
 http://github.com/interactiveaudiolab/audealize-plugin

 Licensed under the GNU GPLv2 <https://opensource.org/licenses/GPL-2.0>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LookAndFeel_h
#define LookAndFeel_h

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

namespace LookAndFeelHelpers  // from juce_LookAndFeel_V2.cpp
{
static Colour createBaseColour (Colour buttonColour, bool hasKeyboardFocus, bool isMouseOverButton,
                                bool isButtonDown) noexcept
{
    const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
    const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

    if (isButtonDown) return baseColour.contrasting (0.2f);
    if (isMouseOverButton) return baseColour.contrasting (0.1f);

    return baseColour;
}

static TextLayout layoutTooltipText (const String& text, Colour colour) noexcept
{
    const float tooltipFontSize = 13.0f;
    const int maxToolTipWidth = 400;

    AttributedString s;
    s.setJustification (Justification::centred);
    s.append (text, Font (tooltipFontSize, Font::bold), colour);

    TextLayout tl;
    tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
    return tl;
}
}

namespace Audealize
{
/// A juce::LookAndFeel for Audealize plugins with a light color scheme
class AudealizeLookAndFeel : public LookAndFeel_V3
{
public:
    AudealizeLookAndFeel ();

    ~AudealizeLookAndFeel ();

    bool isDarkModeActive ()
    {
        return isDarkMode;
    }

    bool willDrawOutlines ()
    {
        return shouldDrawOutlines;
    }

    void setOutlines (bool enabled);

    void drawTabAreaBehindFrontButton (TabbedButtonBar& bar, Graphics& g, const int w, const int h) override;

    void drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override;

    void drawButtonShape (Graphics& g, const Path& outline, Colour baseColour, float height);

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton,
                               bool isButtonDown) override;

    void drawTickBox (Graphics& g, Component& component, float x, float y, float w, float h, const bool ticked,
                      const bool isEnabled, const bool isMouseOverButton, const bool isButtonDown) override;

    void drawLinearSliderThumb (Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                                float maxSliderPos, const Slider::SliderStyle style, Slider& slider) override;

    void drawLinearSliderBackground (Graphics& g, int x, int y, int width, int height, float /*sliderPos*/,
                                     float /*minSliderPos*/, float /*maxSliderPos*/,
                                     const Slider::SliderStyle /*style*/, Slider& slider) override;

    void drawLinearSlider (Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                           float maxSliderPos, const Slider::SliderStyle style, Slider& slider) override;

    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;

    void drawRotarySliderCentered (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                   const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider);
    void drawCornerResizer (Graphics& g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/) override;

    void drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor) override;

    int getTabButtonOverlap (int /*tabDepth*/) override
    {
        return -4;
    }

    int getTabButtonSpaceAroundImage () override
    {
        return 0;
    }

    int getTabButtonBestWidth (TabBarButton& button, int tabDepth) override;

    void createTabTextLayout (const TabBarButton& button, float length, float depth, Colour colour,
                              TextLayout& textLayout);

protected:
    bool isDarkMode;
    bool shouldDrawOutlines;
    Colour outline, tickBoxFill;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudealizeLookAndFeel);
};

/// A juce::LookAndFeel for Audealize plugins with a dark color scheme
class AudealizeLookAndFeelDark : public AudealizeLookAndFeel
{
public:
    AudealizeLookAndFeelDark ();
    ~AudealizeLookAndFeelDark (){};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudealizeLookAndFeelDark);
};
}

#endif /* LookAndFeel_h */
