//
//  main.cpp
//  Playground
//
//  Created by opus arc on 2026/7/10.
//

#include <iostream>

#include <functional>

#include <codecvt>
#include <locale>

#include <random>
#include <algorithm>


std::u32string encode(std::u32string input, std::vector<int> password);
std::u32string decode(std::u32string input, std::vector<int> password);
bool isChinese(char32_t c);
char32_t getChineseBase(char32_t c);
int getChineseRange(char32_t c);

int getShift(int i, std::vector<int> password);

// 由密钥生成确定性的排列和逆排列
std::pair<std::vector<size_t>, std::vector<size_t>>
generatePermutation(size_t size, uint64_t seed) {   // 直接使用整数种子
    std::mt19937 rng(seed);

    // 生成排列 perm
    std::vector<size_t> perm(size);
    for (size_t i = 0; i < size; ++i) perm[i] = i;
    for (size_t i = size - 1; i > 0; --i) {
        std::uniform_int_distribution<size_t> dist(0, i);
        size_t j = dist(rng);
        std::swap(perm[i], perm[j]);
    }

    // 生成逆排列
    std::vector<size_t> inv(size);
    for (size_t i = 0; i < size; ++i) {
        inv[perm[i]] = i;
    }
    return {perm, inv};
}

void scramble_u32(std::u32string& text, uint64_t seed) {
    auto [perm, _] = generatePermutation(text.size(), seed);
    std::u32string temp = text;
    for (size_t i = 0; i < text.size(); ++i)
        text[i] = temp[perm[i]];
}

void unscramble_u32(std::u32string& text, uint64_t seed) {
    auto [_, inv] = generatePermutation(text.size(), seed);
    std::u32string temp = text;
    for (size_t i = 0; i < text.size(); ++i)
        text[i] = temp[inv[i]];
}

std::vector<int> decimalToVector(const std::string& decimal) {
    std::vector<int> result;
    result.reserve(decimal.size());
    for (char c : decimal)
    {
        if (c < '0' || c > '9')
            throw std::invalid_argument("Input contains non-decimal characters");
        result.push_back(static_cast<uint8_t>(c - '0'));
    }
    return result;
}

int main(int argc, const char * argv[]) {
    std::string input_utf8 = "趴奔歬祅劸戔戔邫导愑、秶夊狴盼（绮狰且速下莺夨伡位糁耈皌係愨履扇仜斵雊亖耍，之皉扁，皍爸律并圫覉圫皍临、怭厢奕迡）皈宒臯昰织，纭纭亽仙券皇刹革跶五。愣仞朁违迟玵，狯证而狳逝糁祦挄伣词祥仜皌丒世讹绗仜糃耍、圹亮劧戔躮杭触惎皋井）怯电皌窂旤且耊级喇帯祅丑躮僒乧仚朵；掙惋、伞乓皅丑斿扅為觅糇兽躲徺旣、闷迢祅，乀伧科吔臫祢愕亿耆缽昸，乂昲兹吐丽町到丕吒怯恏旨敚栻苫浀仗，乔丕孟奔祡（產讲亍甸臰忍躯佛恌彗彘乎渪昲耎恒臺杨測柘奔卽扇呟朲串亾纮，皈昵父雇孧孭皋刲丗昲，导乒兯戗扆惍疶昖臫咐亭夶孙佖，，從丐皇纮皇纯甦寄爷脅刊脅贩寢幼臯、忛桀臱央寅叭串皇，逛秂焼皌惉仭惆吕虠朎逛缽戚奘昶挄丑，產聝仝圯庬甧、讬下祢恑仛忶丐，镣旧祡仧孬兺贱浚、套皌爵乂劜惉，毰，云纯皋皋仙绖禗敦糇叔审乑疙纪亖渪戔仜皉狭佟孭栾秔玲耈耍皇三乁叞，乗甬地囡互衧丒迈导昸柗而皇睃地皇亗臱抜仫奞焺丈厼孜皊、祟耈戟井皋、；朴郫惴皋万覇贰朰亖挂扄圮孝怨炼仹遚丑亽奔愗幻焿迎臱昵臲杩肺贩專遙皉昿皇乃吓昶耈亪皌労渲圁，扆刡讨奞绅纪觘皉鲝糁奚迅贯糁夊丒奔戚伡迎朣孠慴朴虛臮夠臯杦云圱歎且丐戙竓戓戙皋仁绳亖伣地昰呷孞靧侞會乘墊";
    std::u32string output_unicode = {};
    std::string mode = "decode";
    std::string key = "20080903";
    
    uint64_t key_hash = static_cast<uint64_t>(std::hash<std::string>{}(key));
    
//    std::cout << "key_hash: " << key_hash << '\n';
    std::string password_string = std::to_string(key_hash);
//    password_string = "333333333";
    std::vector<int> password = decimalToVector(password_string);
    
    std::wstring_convert<
        std::codecvt_utf8<char32_t>,
        char32_t
    > conv;
    std::u32string input_unicode = conv.from_bytes(input_utf8);
    
    if(mode == "encode") {
        // 秘钥打乱
        scramble_u32(input_unicode, key_hash);
        output_unicode = encode(input_unicode, password);
    } else if(mode == "decode") {
        output_unicode = decode(input_unicode, password);
        // 打乱复原
        unscramble_u32(output_unicode, key_hash);
    }
    

    
    std::cout << "mode: " << mode << "\n";
    std::cout << "password: " << password_string << "\n\n";
    
    std::cout << "input: " << input_utf8 << "\n\n";
    

    std::string output_utf8 = conv.to_bytes(output_unicode);
    
    std::cout << "output: " << output_utf8 << "\n\n\n";

    return EXIT_SUCCESS;
}


std::u32string encode(std::u32string input, std::vector<int> password) {
    std::u32string output = input;
    int i = 1;
    
    for(char32_t &c : output) {
        
        int shift = getShift(i, password);
        i++;
        
        if(!(c >= 'A' && c <= 'Z') && !(c >='a' && c <= 'z') && !isChinese(c)) continue;
        
        if(c >= 'a' && c <= 'z'){
            c -= 'a' - 'A';
        }
        
        if(!isChinese(c)) {
            int base = 'A';
            int range = 26;
            if (range > 0) {
                c = base + ((c - base + shift + range) % range);
            }
        } else {
            char32_t base = getChineseBase(c);
            int range = getChineseRange(c);
            if (range > 0) {
                c = base + ((c - base + shift + range) % range);
            }
        }
    }
    return output;
}

std::u32string decode(std::u32string input, std::vector<int> password) {
    std::u32string output = input;
    int i = 1;
    for(char32_t &c : output) {
        
        int shift = getShift(i, password);
        i++;
        
        if(!(c >= 'A' && c <= 'Z') && !(c >='a' && c <= 'z') && !isChinese(c)) continue;
        
        if(c >= 'A' && c <= 'Z'){
            c += 'a' - 'A';
        }
        
        if(!isChinese(c)) {
            int base = 'a';
            int range = 26;
            if (range > 0) {
                c = base + ((c - base - shift + range) % range);
            }
        } else {
            char32_t base = getChineseBase(c);
            int range = getChineseRange(c);
            if (range > 0) {
                c = base + ((c - base - shift + range) % range);
            }
        }
    }
    
    // 首字母大写
    if(output[0] >= 'a' && output[0] <= 'z'){
        output[0] -= 'a' - 'A';
    }
    
    return output;
}

bool isChinese(char32_t c)
{
    return
        (c >= 0x3400  && c <= 0x4DBF) ||
        (c >= 0x4E00  && c <= 0x9FFF) ||
        (c >= 0x20000 && c <= 0x2A6DF) ||
        (c >= 0x2A700 && c <= 0x2B73F) ||
        (c >= 0x2B740 && c <= 0x2B81F) ||
        (c >= 0x2B820 && c <= 0x2CEAF) ||
        (c >= 0x2CEB0 && c <= 0x2EBEF) ||
        (c >= 0x30000 && c <= 0x3134F);
}

char32_t getChineseBase(char32_t c)
{
    if (c >= 0x3400  && c <= 0x4DBF) return 0x3400;
    if (c >= 0x4E00  && c <= 0x9FFF) return 0x4E00;
    if (c >= 0x20000 && c <= 0x2A6DF) return 0x20000;
    if (c >= 0x2A700 && c <= 0x2B73F) return 0x2A700;
    if (c >= 0x2B740 && c <= 0x2B81F) return 0x2B740;
    if (c >= 0x2B820 && c <= 0x2CEAF) return 0x2B820;
    if (c >= 0x2CEB0 && c <= 0x2EBEF) return 0x2CEB0;
    if (c >= 0x30000 && c <= 0x3134F) return 0x30000;

    return 0;
}

int getChineseRange(char32_t c)
{
    if (c >= 0x3400  && c <= 0x4DBF) return 0x4DBF - 0x3400 + 1;
    if (c >= 0x4E00  && c <= 0x9FFF) return 0x9FFF - 0x4E00 + 1;
    if (c >= 0x20000 && c <= 0x2A6DF) return 0x2A6DF - 0x20000 + 1;
    if (c >= 0x2A700 && c <= 0x2B73F) return 0x2B73F - 0x2A700 + 1;
    if (c >= 0x2B740 && c <= 0x2B81F) return 0x2B81F - 0x2B740 + 1;
    if (c >= 0x2B820 && c <= 0x2CEAF) return 0x2CEAF - 0x2B820 + 1;
    if (c >= 0x2CEB0 && c <= 0x2EBEF) return 0x2EBEF - 0x2CEB0 + 1;
    if (c >= 0x30000 && c <= 0x3134F) return 0x3134F - 0x30000 + 1;

    return 0;
}


int getShift(int i, std::vector<int> password) {
    if(password.empty()) return 3;
    int idx = (i - 1) % password.size();
    return password[idx];
}
