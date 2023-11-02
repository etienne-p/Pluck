//
//  Library.hpp
//  Harp2
//
//  Created by etienne cella on 2016-02-20.
//
//

#pragma once
#include <vector>
#include <functional>
#include "ofMain.h"
#include "Sequencer.hpp"
#include "RopesController.hpp"
#include "Locator.h"

class Library
{
public:
    
    using TSaveScreenshot = std::function<void(const std::string& )>;
    
    static const std::string DIR_UGC;
    
    static constexpr size_t NUM_SLOTS = 6;
    
    Library(TSaveScreenshot saveScreenshot_)
    {
        saveScreenshot = saveScreenshot_;
        
        ropesController = Locator::get<RopesController>();
        sequencer = Locator::get<Sequencer>();
        
        // make sure our backup dir exists (so we can assume it later)
        ofDirectory dir(DIR_UGC);
        if (!dir.isDirectory())
        {
            dir.create();
            dir.setWriteable(true);
        }
        else loadSceneNames();
        
        // TMP
        //loadScene("demo");
    }
    
    void openScene(const std::string& name)
    {
        const auto fileName = DIR_UGC + "/" + name + ".xml";
        ofFile file(fileName);
        if (!file.exists())
        {
            ofLogError("app core", "failed to load scene [name], xml data does not exists.");
            return;
        }
        ofXml xml;
        xml.load(fileName);
        ropesController->fromXml(xml);
        sequencer->load(xml);
    }
    
    void eraseScene(const std::string& name)
    {
        ofFile data(getXmlPath(name));
        if (data.exists()) data.remove();
        ofFile thumb(getThumbnailPath(name));
        if (thumb.exists()) thumb.remove();
    }
    
    void eraseScene(size_t index)
    {
        assert(index <= NUM_SLOTS);
        const auto name = sceneNames[index];
        if (name != "")
        {
            eraseScene(name);
            sceneNames[index] = "";
        }
    }
    
    void openScene(size_t index)
    {
        assert(index <= NUM_SLOTS);
        currentSceneIndex = index;
        const auto name = sceneNames[index];
        sequencer->pause();
        sequencer->setRecording(false);
        if (name != "") openScene(name);
        else
        {
            sequencer->clear();
            ropesController->clear();
        }
    }
    
    void saveCurrentScene(const std::string& name)
    {
        eraseScene(currentSceneIndex);
        sceneNames[currentSceneIndex] = name;
        
        auto xml = ofXml();
        xml.addChild("scene");
        xml.setTo("scene");
        auto ropesXml = ropesController->toXml();
        xml.addXml(ropesXml);
        auto seqXml = sequencer->toXml();
        xml.addXml(seqXml);
        xml.save(getXmlPath(name));
        saveScreenshot(getThumbnailPath(name));
    }
    
    const std::array<string, NUM_SLOTS>& getSceneNames() { return sceneNames; }
    
    std::string getCurrentSceneName()
    {
        return currentSceneIndex > -1 ? sceneNames[currentSceneIndex] : "";
    }
    
    std::string getThumbnailPath(std::string sceneName)
    {
        return DIR_UGC + "/" + sceneName + ".png";
    }
    
    std::string getXmlPath(std::string sceneName)
    {
        return DIR_UGC + "/" + sceneName + ".xml";
    }
    
    void setCurrentSceneIndex(int index) { currentSceneIndex = index; }
    
    int getCurrentSceneIndex() const noexcept { return currentSceneIndex; }
    
private:
    
    void loadSceneNames()
    {
        std::fill(std::begin(sceneNames), std::end(sceneNames), "");
        ofDirectory dir(DIR_UGC);
        dir.allowExt("xml");
        dir.listDir();
        assert(dir.size() <= NUM_SLOTS);
        for(int i = 0; i < dir.size(); i++)
        {
            const auto path = dir.getPath(i);
            ofFile file(path);
            const auto fileName = file.getBaseName();
            ofLogNotice("app core", fileName);
            sceneNames[i] = fileName;
        }
    }
    
    int currentSceneIndex{-1};
    TSaveScreenshot saveScreenshot;
    std::array<std::string, NUM_SLOTS> sceneNames;
    std::shared_ptr<RopesController> ropesController;
    std::shared_ptr<Sequencer> sequencer;
};
