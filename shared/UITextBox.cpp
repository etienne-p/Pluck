//
//  UITextBox.cpp
//  Harp2
//
//  Created by etienne cella on 2015-12-23.
//
//

#include "UITextBox.hpp"

UITextBox::UITextBox()
{
    ipDisplay = 1.0f;
    color = Settings::getLightColor();
}

std::string UITextBox::getText() const { return rawText; }

void UITextBox::setText(string text_)
{
    text = rawText = text_;
    fitSize();
}

void UITextBox::setFont(ofTrueTypeFont* font_)
{
    font = font_;
    fitSize();
}

void UITextBox::setSize(const ofVec2f& v)
{
    Transform2D::setSize(v);
    fitSize();
}

void UITextBox::fitSize()
{
    if (!font)
    {
        ofLogError("UITextBox fitSize failed, no font set.");
        return;
    }
    
    const auto boxWidth = std::max<int>(12, getSize().x);
    constexpr auto CR = '\n';
    
    std::stack<char> input;
    std::string r = rawText;
    std::reverse(r.begin(), r.end());
    for (const auto c : r) input.push(c);
    std::vector<std::string> lines;
    
    while(input.size() > 0)
    {
        std::string line = "";
        
        while(input.size() > 0)
        {
            auto newLine = false;
            line.push_back(input.top());
            input.pop();
            
            const auto bb = font->getStringBoundingBox(line, 0, 0);
            
            if (line.size() > 0 && bb.width > boxWidth)
            {
                // intercept special case:
                // line is not breakable
                auto lineIsBreakable = false;
                for (auto c : line)
                {
                    if (canBreak(c))
                    {
                        lineIsBreakable = true;
                        break;
                    }
                }
                // if line is not breakable
                // force new line
                if (!lineIsBreakable)
                {
                    while(input.size() > 0 && !canBreak(input.top()))
                    {
                        line.push_back(input.top());
                        input.pop();
                    }
                    while (line.back() == ' ') line.pop_back();
                    while (input.size() > 0 && input.top() == ' ') input.pop();
                    newLine = true;
                }
                
                while(!newLine)
                {
                    input.push(line.back());
                    line.pop_back();
                    
                    if (canBreak(line.back()))
                    {
                        // clear spaces around break point
                        while (line.back() == ' ') line.pop_back();
                        while (input.top() == ' ') input.pop();
                        newLine = true;
                    }
                }
            }
            if (newLine || input.size() == 0)
            {
                lines.push_back(line);
                break;
            }
        }
    }
    
    text = "";
    for (auto l : lines) text += l + CR;
    const auto bb = font->getStringBoundingBox(text, 0, 0);
    Transform2D::setSize(ofVec2f(std::max<float>(bb.width, .0f),
                                 std::max<float>(bb.height, .0f) + font->getLineHeight()));
    
    auto parentLayout = dynamic_cast<Layout*>(getParent());
    if (parentLayout != nullptr && parentLayout->getLayoutType() != LayoutType::DEFAULT)
    {
        setLayoutPositionDirty();
    }
}

bool UITextBox::canBreak(char c)
{
    bool rv = false;
    switch(c)
    {
        case '.':
        case ' ':
        case '!':
        case '?':
        case ';':
        case ':':
            rv = true; break;
            
        default: break;
    }
    return rv;
}

void UITextBox::update(float dt)
{
    ipDisplay.update(dt);
}

void UITextBox::draw()
{
    if (!font)
    {
        ofLogError("UITextBox draw failed, no font set.");
        return;
    }
    ofColor c = color;
    color.a = 255.0f * ipDisplay;
    ofSetColor(c);
    font->drawString(text, 0, font->getLineHeight());
}

void UITextBox::show(float delay)
{
    ipDisplay = .0f;
    ipDisplay.to(1.0f, .4f, Linear(), 0, delay);
}

void UITextBox::hide(float delay)
{
    ipDisplay.to(.0f, .4f, Linear(), 0, delay);
}