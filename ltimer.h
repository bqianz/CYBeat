#ifndef LTIMER_H
#define LTIMER_H

#include <iostream>

//The application time based timer
class LTimer
{
    public:
        //Initializes variables
        LTimer()
        {
            //Initialize the variables
            adjustedStartTime = 0;
            pauseDisplayTime = 0;

            mPaused = false;
            mStarted = false;
        }

        //The various clock actions
        void start()
        {
            //Start the timer
            mStarted = true;

            //Unpause the timer
            mPaused = false;

            //Get the current clock time
            adjustedStartTime = SDL_GetTicks();
            pauseDisplayTime = 0;

        }

        void stop()
        {
            //Stop the timer
            mStarted = false;

            //Unpause the timer
            mPaused = false;

            //Clear tick variables
            adjustedStartTime = 0;
            pauseDisplayTime = 0;
        }

        void pause()
        {
            //If the timer is running and isn't already paused
            if( mStarted && !mPaused )
            {
                //Pause the timer
                mPaused = true;

                //Calculate the paused ticks
                pauseDisplayTime = SDL_GetTicks() - adjustedStartTime;
                adjustedStartTime = 0;
            }
        }

        void unpause()
        {
            //If the timer is running and paused
            if( mStarted && mPaused )
            {
                //Unpause the timer
                mPaused = false;

                //Reset the starting ticks
                adjustedStartTime = SDL_GetTicks() - pauseDisplayTime; 
                // mStartTicks is how much time has passed since pressing pause

                //Reset the paused ticks
                pauseDisplayTime = 0;
            }
        }

        //Gets the timer's time
        Uint32 getTicks()
        {
            //The actual timer time
            time = 0;

            //If the timer is running
            if( mStarted )
            {
                //If the timer is paused
                if( mPaused )
                {
                    time = pauseDisplayTime;
                }
                else
                {
                    //Return the current time minus the start time
                    time = SDL_GetTicks() - adjustedStartTime;

                }
            }
            return time;
        }

        //Checks the status of the timer
        bool isStarted()
        {
            //Timer is running and paused or unpaused
            return mStarted;
        }

        bool isPaused()
        {
            //Timer is running and paused
            return mPaused && mStarted;
        }

    private:
        // SUM OF EFFECTIVE RUNNING TIME BETWEEN CURRENT SESSION'S START AND LAST PAUSE
        // Updated at [pause] to (system time at pause) - (adjustedStartTime)
        Uint32 pauseDisplayTime;

        // SUM OF INEFFECTIVE RUNNING TIME SINCE SYSTEM START
        // i.e system start time of current session adjusted w.r.t pauses, stops, previous sessions (which can be seen as stops).
        // Updated at [start] to system time.
        // Updated at [unpause] to (system time at unpause) - (pauseDisplayTime)
        // which is also (system time of current sessions's start) + (sum of all pauses)
        Uint32 adjustedStartTime;

        // ACTUAL TIME FOR DISPLAY
        // General fomrula is (system time) - (adjustedStartTime)
        // when [running], adjustedStartTime stays same, system time increases
        // when [paused], adjustedStartTime and system time are increasing at the same rate theoretically, giving a constant display time.
        Uint32 time;
      
        //The timer status
        bool mPaused;
        bool mStarted;
};

#endif