#include "lvgl.h"

class ButtonHandler {
private:
    struct ButtonState {
        bool current_state;     
        bool previous_state;    
        bool raw_state;         
        uint32_t last_change;   
        uint32_t debounce_time; 
    };

    ButtonState scroll_;
    ButtonState select_;
    
    bool updateButton(ButtonState& btn, bool raw_input);

public:
    ButtonHandler(uint32_t debounce_delay = 50);
    void update(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);
    
    bool getScrollState() const;
    bool getSelectState() const;
    bool isScrollPressed() const;
    bool isSelectPressed() const;
    bool isScrollReleased() const;
    bool isSelectReleased() const;
};