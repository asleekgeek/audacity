#pragma once

#include "modularity/ioc.h"

#include "projectscene/iprojectsceneconfiguration.h"

#include "draw/types/color.h"
#include "libraries/lib-track/Track.h"

namespace au::au3 {
class TrackColor : public TrackAttachment
{
    muse::Inject<projectscene::IProjectSceneConfiguration> projectSceneConfiguration;

public:
    static TrackColor& Get(const Track* track);
    static TrackColor& Get(Track* track);

    static void Init(size_t index);

    TrackColor(Track& track);
    void Reparent(const std::shared_ptr<Track>& parent) override;
    void CopyTo(Track& track) const override;
    void WriteXMLAttributes(XMLWriter& writer) const override;
    bool HandleXMLAttribute(const std::string_view& attr, const XMLAttributeValueView& valueView) override;

    muse::draw::Color GetColor() const;
    void SetColor(const muse::draw::Color& color);
private:
    std::weak_ptr<Track> mTrack;
    muse::draw::Color mColor;
};
}
