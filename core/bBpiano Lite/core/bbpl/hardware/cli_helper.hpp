#ifndef cli_helper_hpp
#define cli_helper_hpp

#include <stop_token>
int cli_helper(int argc, char *argv[], const char *version, const char *logo,
               std::stop_token external_stop = {});

#endif /* cli_helper_hpp */
