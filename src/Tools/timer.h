#ifndef TIMER_H
#define TIMER_H

#include <QThread>


/**
 * \class Timer
 *
 * \brief This class contains helper functions for platform independent timers and sleeps.
 *
 * This class implements some helper functions, that provide platform independent
 * (Linux / Windows / ...) support for sleeping and timers.
 *
 * \date 21.05.2012
 * \author MWI
 */
class Timer : public QThread
{
    public:

        /**
         * \brief pauses the calling thread for n seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulSecs = seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void sleep(const unsigned long& ulSecs)
        {
            QThread::sleep(ulSecs);
        }

        /**
         * \brief pauses the calling thread for n milli seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulMSecs = milli seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void msleep(const unsigned long& ulMSecs)
        {
            QThread::msleep(ulMSecs);
        }

        /**
         * \brief pauses the calling thread for n micro seconds
         *
         * -
         *
         * @param[in] const unsigned long& ulUSecs = micro seconds to sleep
         *
         * \date 21.05.2012
         * \author MWI
         */
        static void usleep(const unsigned long& ulUSecs)
        {
            QThread::usleep(ulUSecs);
        }
};

#endif // TIMER_H
