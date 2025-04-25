class Display
{
public:
    void Init();
    void EventLoop();

private:                            // The functions are set as private because they
    void m_displayStartup();        // are only being used by the public methods in the class
    void m_displayDateTime();
    void m_displayTempHum();
    void m_displayWeather();
    void m_displayNews();
    void m_displayAlarm();
    void m_editHour();
    void m_editMinute();
    void m_setLocation();
};