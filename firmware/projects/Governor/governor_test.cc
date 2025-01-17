#include <cassert>
#include <iostream>

#include "governor.hpp"

Governor CycleToRunning() {
    Governor g;
    Governor::Input in{
        .bm_sts = BmSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::INIT,
    };

    g.Update(in, 0);

    in.di_sts = DiSts::HV_START_REQ;
    g.Update(in, 2000);

    in.bm_sts = BmSts::RUNNING;
    g.Update(in, 2001);

    in.di_sts = DiSts::READY_TO_DRIVE_REQ;
    g.Update(in, 2002);

    in.mi_sts = MiSts::RUNNING;
    g.Update(in, 2003);

    in.di_sts = DiSts::RUNNING;
    auto out = g.Update(in, 2004);

    assert(out.gov_sts == GovSts::RUNNING);

    return g;
}

void test_normal_sequence() {
    Governor g;

    Governor::Input in{
        .bm_sts = BmSts::INIT,
        .mi_sts = MiSts::INIT,
        .di_sts = DiSts::INIT,
    };

    int time = 0;

    {
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.di_cmd == DiCmd::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
    }

    in.di_sts = DiSts::HV_START_REQ;
    time += 1999;

    {  // Shouldn't go yet
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::INIT);
    }

    time += 1;

    {  // 2s have elapsed -> enter startup
        auto out = g.Update(in, time);
        assert(out.gov_sts == GovSts::STARTUP_HV);
        assert(out.bm_cmd == BmCmd::HV_STARTUP);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_HV);
    }

    in.bm_sts = BmSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_READY_TO_DRIVE);
        assert(out.di_cmd == DiCmd::HV_ON);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::READY_TO_DRIVE_REQ;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_MOTOR);
        assert(out.mi_cmd == MiCmd::STARTUP);

        assert(g.Update(in, ++time).gov_sts == GovSts::STARTUP_MOTOR);
    }

    in.mi_sts = MiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::STARTUP_SEND_READY_TO_DRIVE);
        assert(out.di_cmd == DiCmd::READY_TO_DRIVE);

        assert(g.Update(in, ++time).gov_sts ==
               GovSts::STARTUP_SEND_READY_TO_DRIVE);
    }

    in.di_sts = DiSts::RUNNING;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::RUNNING);
    }

    std::cout << "Passed test_normal_sequence!" << std::endl;
}

void test_shutdown_hvil() {
    Governor g = CycleToRunning();

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };

    int time = 5000;
    assert(g.Update(in, time).gov_sts == GovSts::RUNNING);

    in.bm_sts = BmSts::HVIL_INTERRUPT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::SHUTDOWN);
        assert(out.mi_cmd == MiCmd::SHUTDOWN);
        assert(out.di_cmd == DiCmd::SHUTDOWN);

        assert(g.Update(in, ++time).gov_sts == GovSts::SHUTDOWN);
    }

    in.bm_sts = BmSts::INIT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
        assert(out.di_cmd == DiCmd::INIT);
    }

    std::cout << "Passed test_shutdown_hvil!" << std::endl;
}

void test_shutdown_low_soc() {
    Governor g = CycleToRunning();

    Governor::Input in{
        .bm_sts = BmSts::RUNNING,
        .mi_sts = MiSts::RUNNING,
        .di_sts = DiSts::RUNNING,
    };

    int time = 5000;
    assert(g.Update(in, time).gov_sts == GovSts::RUNNING);

    in.bm_sts = BmSts::LOW_SOC;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::SHUTDOWN);
        assert(out.mi_cmd == MiCmd::SHUTDOWN);
        assert(out.di_cmd == DiCmd::SHUTDOWN);

        assert(g.Update(in, ++time).gov_sts == GovSts::SHUTDOWN);
    }

    in.bm_sts = BmSts::INIT;

    {
        auto out = g.Update(in, ++time);
        assert(out.gov_sts == GovSts::INIT);
        assert(out.bm_cmd == BmCmd::INIT);
        assert(out.di_cmd == DiCmd::INIT);
    }

    std::cout << "Passed test_shutdown_low_soc!" << std::endl;
}

int main() {
    test_normal_sequence();
    test_shutdown_hvil();
    test_shutdown_low_soc();

    std::cout << "All tests passed!" << std::endl;

    return 0;
}