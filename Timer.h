
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
    void setDuration(int durationInSeconds);
    void setName(String n);
    int getDurationInSeconds() const;
    String getDurationAsString();
    String getName();
    bool manageTimer();

private:

    void count();

    int duration;
    int seconds;
    bool running;
    char name[8];

    unsigned long startingMillis;



};


#endif //CAKETIMERCL_TIMER_H
