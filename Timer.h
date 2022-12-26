
#ifndef CAKETIMERCL_TIMER_H
#define CAKETIMERCL_TIMER_H



class Timer{

public:
    Timer();
    explicit Timer(int durationInSeconds);
    ~Timer();

    void start();
    void reset();
    void stop();
    void toggle();
    void addTime(int amountOfSeconds);
    void setDuration(int durationInSeconds);
    void setName(String n);
    int getDurationInSeconds() const;
    String getDurationAsString();
    String getName();
    bool manageTimer();

private:

    int duration;
    int seconds;
    bool running;
    String name;

    unsigned long startingMillis;



};


#endif //CAKETIMERCL_TIMER_H
