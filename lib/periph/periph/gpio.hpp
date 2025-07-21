/// @author Blake Freer
/// @date 2023-11-08

#pragma once

namespace macfe::periph {

class DigitalInput {
public:
    virtual bool Read() = 0;
};

class DigitalOutput {
public:
    virtual void SetHigh() = 0;
    virtual void SetLow() = 0;
    virtual void Set(bool value) = 0;
};

class InvertedDigitalInput final : public DigitalInput {
public:
    InvertedDigitalInput(DigitalInput& di) : di_(di) {}

    inline bool Read() override {
        return !di_.Read();
    }

private:
    DigitalInput& di_;
};

class InvertedDigitalOutput final : public DigitalOutput {
public:
    InvertedDigitalOutput(DigitalOutput& digital_output)
        : digital_output_(digital_output) {}

    inline void Set(bool value) override {
        digital_output_.Set(!value);
    }

    inline void SetLow() override {
        digital_output_.SetHigh();
    }

    inline void SetHigh() override {
        digital_output_.SetLow();
    }

private:
    DigitalOutput& digital_output_;
};

}  // namespace macfe::periph