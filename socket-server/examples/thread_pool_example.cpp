#include <server/thread_pool/thread_pool.h>
#include <logging/logging.h>

using namespace socket_server;

int main() {
    lLog(lDebug) << "ThreadPool example";

    ThreadPool pool;

    lLog(lDebug) << pool.Count() << " threads ready";
    
    return 0;
}