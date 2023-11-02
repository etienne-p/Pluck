#include "Sequencer.hpp"

const float Sequencer::MIN_TIME_MUL = .25f;
const float Sequencer::MAX_TIME_MUL = 4.0f;

Sequencer::Sequencer()
{
    selectPattern(patterns.size() - 1);
}

ofXml Sequencer::toXml()
{
    ofXml xml;
    ofXml patternXml;
    ofXml eventXml;
    
    xml.addChild("sequence");
    xml.setTo("sequence");
    xml.addValue("bpm", bpm);
    xml.addChild("patterns");
    xml.setTo("patterns");
    for (const auto& p : patterns)
    {
        patternXml.clear();
        patternXml.addChild("pattern");
        patternXml.setTo("pattern");
        patternXml.addValue("time_mul", p.timeMul);
        
        if (p.events.size() > 0)
        {
            patternXml.addChild("events");
            patternXml.setTo("events");
            
            for (const auto& e : p.events)
            {
                eventXml.clear();
                eventXml.addChild("event");
                eventXml.setTo("event");
                eventXml.addValue("id", e.id);
                eventXml.addValue("time", e.timeRelativeToBpm);
                eventXml.addValue("velocity", e.velocity);
                eventXml.reset();
                patternXml.addXml(eventXml);
            }
        }
        patternXml.reset();
        xml.addXml(patternXml);
    }
    xml.reset();
    return xml;
}

void Sequencer::clear()
{
    for (auto& p : patterns) p.events.clear();
}

void Sequencer::load(ofXml& xml)
{
    clear();
    pause();
    xml.reset();
    xml.setTo("scene");
    xml.setTo("sequence");
    bpm = xml.getValue<float>("bpm");
    xml.setTo("patterns");
    xml.setTo("pattern[0]");
    auto i = 0;
    do
    {
        //ofLogNotice("load pattern");
        patterns[i].timeMul = xml.getValue<float>("time_mul");
        
        if (xml.exists("events"))
        {
            xml.setTo("events");
            xml.setTo("event[0]");

            do
            {
                //ofLogNotice("load event");
                SequencerEvent e;
                e.id = xml.getValue<int>("id");;
                e.velocity = xml.getValue<float>("velocity");
                e.timeRelativeToBpm = xml.getValue<float>("time");
                patterns[i].events.push_back(e);
            }
            while( xml.setToSibling() );
            
            xml.setToParent();
            xml.setToParent();
        }
        ++i;
    }
    while( xml.setToSibling() );
    
    changed.emit();
    notesChanged.emit();
}

bool Sequencer::pushSequencerEvent(int id, float velocity)
{
    if (paused || !recording || selectedPattern == -1) return false;

    SequencerEvent e;
    e.id = id;
    e.velocity = ofClamp(velocity, .0f, 1.0f);
    // not super accurate but it does the job
    e.timeRelativeToBpm = patterns[selectedPattern].timeStampRelativeToBpm;
    
    //ofLogNotice("note: " + ofToString(e.timeRelativeToBpm));
    
    auto sequencerEvents = &(patterns[selectedPattern].events);
    
    // insert SequencerEvent in the SequencerPattern
    // making sure SequencerEvents are always time sorted in the SequencerPattern
    if (sequencerEvents->size() == 0 || (*sequencerEvents)[(*sequencerEvents).size() - 1].timeRelativeToBpm < e.timeRelativeToBpm)
    {
        (*sequencerEvents).push_back(e);
    }
    else if ((*sequencerEvents)[0].timeRelativeToBpm > e.timeRelativeToBpm)
    {
        (*sequencerEvents).insert((*sequencerEvents).begin(), e);
    }
    else
    {
        auto foundItem = std::find_if(sequencerEvents->begin(), sequencerEvents->end(),
                                      [&e](const SequencerEvent& evt) { return evt.timeRelativeToBpm > e.timeRelativeToBpm; });
        
        sequencerEvents->insert(foundItem, e);
    }
    notesChanged.emit();
    return true;
}

void Sequencer::play()
{
    startTime = Poco::Timestamp();
    paused = false;
    for(auto& p : patterns) p.timeStampRelativeToBpm = .0;
    timeStampRelativeToBpm = .0;
    changed.emit();
}

void Sequencer::pause() { paused = true; changed.emit(); }

void Sequencer::setBPM(float bpm_) { bpm = bpm_; changed.emit(); }

float Sequencer::getBPM() const { return bpm; }

void Sequencer::update(float dt)
{
    frameEvents.clear();
    
    if (paused) return;
    
    for(auto& p : patterns)
    {
        const auto barDuration = (60.0f / bpm) * 4.0f * p.timeMul; // assumes 4x4 beat
        const auto scaledDt = dt / barDuration;
        const auto newTimeStampRelativeToBpm = p.timeStampRelativeToBpm + scaledDt;
        const auto newTimeStampRelativeToBpmMod = fmod(newTimeStampRelativeToBpm, 1.0f);
        
        if (p.active)
        {
            const auto loop = newTimeStampRelativeToBpmMod < p.timeStampRelativeToBpm;
            
            for(auto& e : p.events)
            {
                const auto etOffset = (loop &&
                (e.timeRelativeToBpm < newTimeStampRelativeToBpmMod)) ? 1.0f : .0f;
                
                if (e.timeRelativeToBpm + etOffset <= newTimeStampRelativeToBpm &&
                    p.timeStampRelativeToBpm < e.timeRelativeToBpm + etOffset)
                {
                    //ofLogNotice("seq", "evt");
                    frameEvents.push_back(e);
                }
            }
        }
        
        p.timeStampRelativeToBpm = newTimeStampRelativeToBpmMod;
    }
    
    const auto barDuration = (60.0f / bpm) * 4.0f; // assumes 4x4 beat
    const auto scaledDt = dt / barDuration;
    const auto newTimeStampRelativeToBpm = timeStampRelativeToBpm + scaledDt;
    const auto newTimeStampRelativeToBpmMod = fmod(newTimeStampRelativeToBpm, .25f);
    if (newTimeStampRelativeToBpmMod < timeStampRelativeToBpm && tickerEnabled) tick.emit();
    timeStampRelativeToBpm = newTimeStampRelativeToBpmMod;
}

void Sequencer::handleUIAction(UIAction action)
{
    if (selectedPattern == -1) return;
    switch(action)
    {
        case PATTERN_CLEAR:
        {
            patterns[selectedPattern].events.clear();
            notesChanged.emit();
        }
            break;
            
        case PATTERN_INCREASE_TIME_MUL:
        {
            patterns[selectedPattern].timeMul = std::min<float>(MAX_TIME_MUL,
                                                                2.0f * patterns[selectedPattern].timeMul);
        }
            break;
            
        case PATTERN_DECREASE_TIME_MUL:
        {
            patterns[selectedPattern].timeMul = std::max<float>(MIN_TIME_MUL,
                                                                .5f * patterns[selectedPattern].timeMul);
        }
            break;
            
        case PATTERN_ACTIVE:
        {
            patterns[selectedPattern].active = !(patterns[selectedPattern].active);
        }
            break;
   
        default: return;
    }
    changed.emit();
}

/*double Sequencer::getTimeRelativeToBpm(float timeMul) const
{
    const auto dts = startTime.elapsed() / 1000000.0; // time elapsed in seconds;
    const auto barDuration = 4.0 * bpm * timeMul / 60.0; // assumes 4x4 beat
    return fmod(dts / barDuration, 1.0f);
}*/

void Sequencer::selectPattern(size_t index)
{
    if (selectedPattern == index) return;
    selectedPattern = index;
    changed.emit();
}

void Sequencer::filterEvents(const vector<int>& ids)
{
    for(auto& p : patterns)
    {
        if (ids.size() == 0)
        {
            p.events.clear();
            continue;
        }
        
        p.events.erase(std::remove_if(std::begin(p.events), std::end(p.events),
                                      [&ids](const SequencerEvent& e)
                                      {
                                          return std::find(ids.begin(), ids.end(), e.id) == ids.end();
                                      }),
                       std::end(p.events));

    }
}

void Sequencer::setRecording(bool value) { recording = value; changed.emit(); }

bool Sequencer::isRecording() const { return recording; }

bool Sequencer::isPlaying() const { return !paused; }

int Sequencer::getSelectedPattern() const { return selectedPattern; }

const std::array<SequencerPattern, Sequencer::NUM_PATTERNS>& Sequencer::getPatterns() const { return patterns; }

const std::vector<SequencerEvent>& Sequencer::getFrameEvents() const { return frameEvents; };

bool Sequencer::isTickerEnabled() const noexcept { return tickerEnabled; };

void Sequencer::setTickerEnabled(bool val)
{
    tickerEnabled = val;
    //tickerTime = getTimeRelativeToBpm(1.0f);
}
