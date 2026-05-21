#include "hsd.hpp"

#include "bindings.hpp"
// TODO: ensure correct to fix conversion for channels
// TODO: do we need a delay between setting isense high and reading voltage

namespace hsd {

struct Reading {
    float current_ma;
    float fault;
}

class hsd {
public:
    virtual Reading Read(uint8_t channel) = 0;
    virtual ~HSD() = default;
};

class HSD2Channel : public HSD {
public:
    HSD2Channel(AnalogInput& isense, DigitalOutput& en, DigitalOutput& sel)
        : isense_(isense), en_(en), sel_(sel) {}
    Reading Read(uint8_t channel) override;

private:
    AnalogInput& isense_;
    DigitalOutput& en_;
    DigitalOutput& sel_;
};

class HSD4Channel : public HSD {
public:
    HSD4Channel(AnalogInput& isense, DigitalOutput& en, DigitalOutput& sel0,
                DigitalOutput& sel1)
        : isense_(isense), en_(en), sel0_(sel0), sel1_(sel1) {}
    Reading Read(uint8_t channel) override;

private:
    AnalogInput& isense_;
    DigitalOutput& en_;
    DigitalOutput& sel0_;
    DigitalOutput& sel1_;
}

}  // namespace hsd