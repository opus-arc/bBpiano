#include "bbpl_c_api.h"

#include <chrono>
#include <thread>

int main() {

    // midi_service_start(
    //     "./build/bbpl_SDK/example/Nocturne No. 6 in D-Flat Major, Op. 63.midi"
    // );
    // std::this_thread::sleep_for(std::chrono::seconds(20));
    // midi_service_stop();
    test_service_start();

    return 0;
}
