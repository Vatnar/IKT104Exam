enum class State {
    STARTUP     = 1 << 0,
    SHOWALARM   = 1 << 1,
    EDITHOUR    = 1 << 2,
    EDITMINUTE  = 1 << 3,
    DATETIME    = 1 << 4,
    TEMPHUMID   = 1 << 5,
    WEATHER     = 1 << 6,
    SETLOC      = 1 << 7,
    NEWS        = 1 << 8
};

enum class ButtonState{
    LEFT        = 1 << 0,
    UP          = 1 << 1,
    DOWN        = 1 << 2,
    RIGHT       = 1 << 3
};