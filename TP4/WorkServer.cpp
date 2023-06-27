#include "../RedesHelper.h"

#include "WorkServer.h"

void func(void* arg){
    cout << "Hello from " << this_thread::get_id() << " " << (size_t) arg << endl;
    this_thread::sleep_for(seconds(1));
}

int main(){
    const size_t nThreads = 3;
    Mx::WorkServer<nThreads> WS(10);

    Mx::WorkUnit wu = {func, (void*) 0};
    WS.put(wu);
    WS.put({func, (void*) 1});
    WS.put({func, (void*) 2});
    WS.put({func, (void*) 3});
    WS.put({func, (void*) 4});
    WS.put({func, (void*) 5});

    cin.get();
    WS.printStats();
    
    WS.put(Mx::FakeWorkUnit{});
    WS.put(Mx::FakeWorkUnit{});
    WS.put(Mx::FakeWorkUnit{});
    WS.put(Mx::FakeWorkUnit{});
    WS.put(Mx::FakeWorkUnit{});

    cin.get();
    WS.printStats();

    return 0;
}