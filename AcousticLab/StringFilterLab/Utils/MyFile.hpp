//
//  MyFile.hpp
//  bBpiano
//
//  Created by opus arc on 2026/5/19.
//

#ifndef MyFile_hpp
#define MyFile_hpp

#include <iostream>

class MyFile {
public:
    
    static std::string findPitchName(const std::string& fileName) {
        size_t pos = fileName.find('_');
        if (pos != std::string::npos)
            return fileName.substr(0, pos);
        return "A4";
    }
    
    static std::string findVelocity(const std::string& fileName) {
        size_t pos = fileName.find('v');
        if (pos != std::string::npos)
            return fileName.substr(pos, fileName.size());
        return "80";
    }
    
};

#endif /* MyFile_hpp */
