#include "../RedesHelper.h"
#include "barrier.h"

using namespace std;
using namespace std::chrono;

namespace Mx{

typedef void (*ProcFunc)(void *ctx);
using WorkUnitId = unsigned int;

struct WorkUnit{
    ProcFunc fun;
    void *context;
    time_point<system_clock> submit;
};

template <size_t nWorkers>
class WorkServer{
public:

    WorkServer(size_t maxWU_)
        :maxWU(maxWU_){
        for(size_t i = 0; i < nWorkers; ++i)
            workers[i] = new WorkerThread(this);
        
    }
    
    void put(const WorkUnit &work_unit);

    void printStats(){
        lock_guard <mutex> guarda (stats_mtx);
        printf("Se corrieron %d Work Units!\nQueued time: %f\nRuning Time: %f\n", count, QueuedDuration.count(),RunDuration.count());
    }

private:
    struct WorkerThread{
        WorkerThread(WorkServer *WS_)
        :WS(WS_), 
        th{& WorkerThread::threadFunction, this} {}

        void threadFunction(){
            while(1){
                WorkUnit wu;
                WS -> get(&wu);
                auto start = system_clock::now();
                wu.fun(wu.context);
                WS -> updateStats(start - wu.submit, system_clock::now() - start);
            }
        }

        WorkServer *WS;
        thread th;
    };

    void get (WorkUnit *pWorkUnit){
        unique_lock<mutex> lck{mtx};
        while(WUs.size() == 0){
            getcv.wait(lck);
        }
        *pWorkUnit = WUs.front();
        WUs.pop_front();
        putcv.notify_one();
    }

    void updateStats(duration<double, milli> queued, duration<double, milli> run){
        lock_guard <mutex> guarda (stats_mtx);
        QueuedDuration += queued;
        RunDuration += run;
        count++;
    }


    mutex mtx;
    mutex stats_mtx;
    condition_variable putcv; 
    condition_variable getcv;


    deque <WorkUnit> WUs;
    WorkerThread* workers[nWorkers] = {nullptr};
    const size_t maxWU;

    duration<double, milli>  QueuedDuration;
    duration<double, milli>  RunDuration;
    unsigned int count = 0;

};

template <size_t nWorkers>
void WorkServer<nWorkers>::put(const WorkUnit& work_unit) {
    unique_lock<mutex> lck{mtx};
    while(WUs.size() == maxWU){
        putcv.wait(lck);
    }
    WUs.push_back(work_unit);
    WUs[WUs.size() - 1].submit = system_clock::now();
    getcv.notify_one();
}

class FakeWorkUnit: public WorkUnit{
public:
    FakeWorkUnit(){
        size_t x = rand()%100;
        context = (void*) x;
        fun = [](void* arg){
            printf("Hello from Fake(0x%02x): my argument is %zu\n", this_thread::get_id(), (size_t) arg);
            this_thread::sleep_for(seconds(2));
        };
    }
};

} // namespace Mx