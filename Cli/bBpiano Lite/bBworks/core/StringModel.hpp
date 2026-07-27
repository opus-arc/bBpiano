// 必须库
#include <iostream>
#include <algorithm>

// 我的目的是要设计一根能够对应现实世界的完美钢琴琴弦
class StringModel {

private:
    
    double delay;
    int delay_int;
    double delay_frac;
    
    std::vector<float> left;
    std::vector<float> right;
    int leftHead;
    int rightHead;
    
    double strikePosition;
    double pickupPosition;
    struct SpatialPort {
        int index0 = 0;
        int index1 = 0;
        double weight0 = 0.0f;
        double weight1 = 0.0f;
        SpatialPort(double position, int maxIndex) {
            position = std::clamp(position, 0.0, 1.0);
            maxIndex = std::max(maxIndex, 1);
            double originalIndex = position * maxIndex;
            index0 = std::floor(originalIndex);
            index1 = std::ceil(originalIndex);
            weight0 = originalIndex - index0;
            weight1 = 1- weight0;
        }
        SpatialPort() = default;
    };
    SpatialPort strikePort;
    SpatialPort pickupPort;
    
public:
    
    StringModel() {
        
        left.assign(10000, 0);
        right.assign(10000, 0);
        leftHead = 0;
        rightHead = 0;
        
        delay = 44100.0 / (440.0 * 2);
        if(delay < 10) throw std::runtime_error("delay 过小，琴弦初始化失败");
        delay_int = floor(delay - 1);
        delay_frac = delay - delay_int;
        
        strikePosition = 1.0 / 9.4;
        pickupPosition = 6.0 / 7.0;
        strikePort = SpatialPort(strikePosition, delay_int);
        pickupPort = SpatialPort(pickupPosition, delay_int);
        
        
    }
    
    ~StringModel() = default;
    
    inline void propagate() {
        leftHead--;
        rightHead++;
    }
    
    inline void inject() {
        
    }
    
    
    
};
