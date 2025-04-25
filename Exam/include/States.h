enum class State {
    STARTUP     = 1 << 0,
    SHOWALARM   = 1 << 1,
    EDITHOUR    = 1 << 2,
    EDITMINUTE  = 1 << 3,
    EDITENABLED = 1 << 4,
    DATETIME    = 1 << 5,
    TEMPHUMID   = 1 << 6,
    WEATHER     = 1 << 7,
    SETLOC      = 1 << 8,
    NEWS        = 1 << 9,
};

enum class ButtonState{
    LEFT        = 1 << 0,
    UP          = 1 << 1,
    DOWN        = 1 << 2,
    RIGHT       = 1 << 3
};