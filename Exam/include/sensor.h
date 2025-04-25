
class TempHum {
    public:
    static float temperature;
    static float humidity;

    void SensorLoop();
    void SensorInit();

    private:
    float m_getTemperature();
    float m_getHumidity();
};