
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
    void setDuration(int durationInSeconds);
    int getDurationInSeconds() const;
    String getDurationAsString();
    bool manageTimer();

private:

    void count();

    int duration;
    int seconds;
    bool running;



};


#endif //CAKETIMERCL_TIMER_H
