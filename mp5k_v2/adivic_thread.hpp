#ifndef _ADIVIC_THREAD_H_
#define _ADIVIC_THREAD_H_
#include <QObject>
#include <QDebug>
#include <QThread>
#include "AdivicGlobal.hpp"
#include <queue>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>
#ifdef _DEBUG
    #define BOOST_DISABLE_ASSERTS
#endif
#include <boost/lockfree/queue.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <QThread>
using std::queue;


typedef enum {
    CRITICAL      = QThread::TimeCriticalPriority,
    HIGHEST       = QThread::HighestPriority,
    _ABOVE_NORMAL = QThread::HighPriority,
    NORMAL        = QThread::NormalPriority,
    BELOW_NORMAL  = QThread::LowPriority,
    LOWEST        = QThread::LowestPriority,
    IDLE          = QThread::IdlePriority
}_tPriority;

class TaskElement
{

public:
    TaskElement( ) :done(true) {};
    TaskElement( const TaskElement &obj ) :done(true) {};

    virtual ~TaskElement() {};
public  :
    virtual DWORD run(void) = 0;
    virtual void stop(void) = 0;
    bool IsReady(void) { return (!done); };
    bool WaitForReady(void);
protected:
    boost::atomic<bool> done;
private:
    
};

class AdivicTaskBase : public internal::no_copy
{
public:
    AdivicTaskBase() {}
    ~AdivicTaskBase() {}
    virtual bool create(_tPriority nPriority, DWORD StackSize) { return false; }
    virtual bool destroy( DWORD &exit ) { 
        exit = 0;
        return false;
    }
private:
};

template <class T> class AdivicTask : public AdivicTaskBase
{
public:

    AdivicTask() :thread_(NULL), obj(NULL) {}
    AdivicTask(T &t) :thread_(NULL) { obj = new T(t);}

    ~AdivicTask() { if(obj !=NULL) delete obj; }
    bool assign(const T& t) { 
        if ( obj != NULL) return false;
        obj = new T(t);
        return true;
    }

    void release(void) { 
        if ( obj == NULL) return;
        delete obj;
        obj = NULL;
    }

    T& task(void) { return *obj; }
    
    bool create(QThread::Priority nPriority, DWORD StackSize){
        if( obj == NULL ) return false;
        obj->movetoThread(thread_);
       // thread_ = CreateThread( NULL, StackSize , ThreadProc, obj, CREATE_SUSPENDED, NULL );
        if( thread_ == NULL ) return false;
       // SetThreadPriority( thread_, nPriority );
       // ResumeThread( thread_ );
        thread_->setPriority( nPriority);
        thread_->setStackSize( StackSize);
        thread_->start();
        return obj->WaitForReady();
    }
/*
    bool destroy( DWORD &exit ) {
        int retry_num = 0;
        if( thread_ == NULL ) return true;
_retry_:
        obj->stop();
        if ( !GetExitCodeThread( thread_, &exit ) ) return false;
        retry_num--;
        if( exit == STILL_ACTIVE && retry_num != 0 )goto _retry_;
        CloseHandle(thread_);
        thread_ = NULL;
        return (retry_num!=0);
    }
*/
    bool isEmpty() {
        return (obj == nullptr);
    }

    operator bool() {
        return !isEmpty();
    }

private:
   /*
    static DWORD WINAPI ThreadProc(  LPVOID lpParameter ) {
        T* tObj = (T*)lpParameter;
        ExitThread(tObj->run());
        return 0;
    }
    */
private:
    QThread* thread_;

   // HANDLE thread_;
      T* obj;
};


#endif

