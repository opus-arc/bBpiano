// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// [ZERO AI-GENERATED CODE]
// Every line in this file is written and understood by its author. Every result is
// reproducible, every assumption is open to inspection, and every implementation
// stands open to criticism and challenge.
// AI may be used for non-core, replaceable, engineering work;
// this file, however, contains core logic that the author considers
// necessary to understand firsthand, explain line by line, and take full responsibility for,
// and is therefore implemented entirely by hand.
// ---------------------------------------------------------------------------
// [本文件承诺不含任何 AI 生成代码]
// 每一行代码均由作者亲自编写，并确知其意义。一切结果可以复现，一切假设可经受检验，一切实现经得起批评与质疑。
// AI 可用于非核心、可替代的工程工作；
// 本文件承载作者认为必须亲自理解、能够逐行解释并为之负责的核心逻辑，因此刻意保持完全人工实现。
//
// Ziyang Tan
// 2026-09-04
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#include "./piano_controller.hpp"
#include "./hardware/soundcard_helper.hpp"
#include "./eval/engine_rate/engine_rate_eval.hpp"

std::unique_ptr<PianoModel> bBpiano;
std::unique_ptr<Soundcard> soundcard;
std::unique_ptr<EngineEval> engine_eval;

// ======================== ======================== ========================
// Initialize
// 初始化
// ======================== ======================== ========================
void bbpiano_init(double sample_rate) {
    bBpiano = std::make_unique<PianoModel>(sample_rate);
}	
void bbpiano_shutdown() {
    bBpiano.reset();
}
void eval_init(double sample_rate) {
    engine_eval = std::make_unique<EngineEval>(sample_rate);
}
void eval_shutdown() {
    engine_eval.reset();
}
void soundcard_init(double sample_rate) {
    soundcard = std::make_unique<Soundcard>(sample_rate);
    soundcard->start();
}
void soundcard_shutdown() {
    soundcard.reset();
}

// ======================== ======================== ========================
// Hardware callback and test
// 硬件回调与测试
// ======================== ======================== ========================
int counter__ = 0;
void get_next_buffer(float* out, int frameCount, double amplitudeLimiter) {
    
    counter__++;
    if(counter__ > 20) {
        counter__ = 0;
//        std::cout << "engineRate: " << get_engine_rate() << "\n";
    }
    engine_eval->start_timing();
    
    for (int i = 0; i < frameCount; ++i) {
        bBpiano->piano_movement();
      
        float x = bBpiano->get_sample();
        out[i] = x * amplitudeLimiter;
    }
    
    engine_eval->end_timing(frameCount);
    
}
double get_engine_rate() {
    return engine_eval->engine_rate();
}

// ======================== ======================== ========================
// Piano gesture
// 钢琴手势
// ======================== ======================== ========================
void note_on(int midi_n, double velocity) {
    bBpiano->test_key_a4->trigger(velocity);
}
void note_off(int midi_n, double velocity) {
    
}
void note_aftertouch(int midi_n, double pressure) {
    
}


void softpedal_control(double depth) {
    
}
void harmonicpedal_control(double depth) {
    
}
void sostenutopedal_control(double depth) {
    
}
void sustainpedal_control(double depth) {
    
}

void all_silence() {
    
}
