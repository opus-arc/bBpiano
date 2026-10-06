/*
   'input.cpp' Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Sun Oct  4 23:30:39 2026
*/
#include<chrono>
#include<condition_variable>
#include<cstdint>
#include<filesystem>
#include<iostream>
#include<mutex>
#include<stop_token>
#include<thread>
void l579(double ln);void bbpiano_shutdown()noexcept;void l494(double
ln);void l504()noexcept;void l438(double ln);void l379()noexcept;void
l359(float*out,int l468,double l823);void l458();void l52(int lk,
double l82)noexcept;void l106(int lk,double l82)noexcept;void l190(
int lk,double l519)noexcept;void l127(double l11)noexcept;void l125(
double l11)noexcept;void l119(double l11)noexcept;void l71(double l11
)noexcept;void l63()noexcept;void l578()noexcept;bool l523(std::
stop_token stop_token)noexcept;bool l207()noexcept;void l392()noexcept
;void l525()noexcept;
#include<string>
static constexpr double ln=44100.0;int l116(int l226,char*l14[],const
char*l17,const char*l30,std::stop_token l237={});void l473(const std
::string&l40,std::stop_token stop_token);void l502(std::stop_token
stop_token);void l441(std::stop_token stop_token);void l565(std::
string l811,std::stop_token stop_token);void l527(std::stop_token
stop_token);void l501(std::stop_token stop_token);void l518(std::
stop_token stop_token);
int l470(int l226,char*l14[],const char*l17,const char*l30,std::
stop_token l237={});
#include<algorithm>
#include<array>
#include<cmath>
#include<fstream>
#include<limits>
#include<stdexcept>
#include<vector>
#include<cstddef>
#include<unordered_set>
#include<utility>
class l9:public std::runtime_error{public:using std::runtime_error::
runtime_error;};class l136{public:struct l8{std::uint64_t l24=0;
double l143=0.0;std::uint8_t l6=0;std::uint8_t l36=0;std::uint8_t l254
=0;std::string l262;int l798()const noexcept{return static_cast<int>(
l6&0x0F);}std::uint8_t l234()const noexcept{return l6&0xF0;}bool l387
()const noexcept{return l234()==0x90&&l254!=0;}bool l354()const
noexcept{return l234()==0x80||(l234()==0x90&&l254==0);}bool l582()const
noexcept{return l234()==0xA0;}bool l602()const noexcept{return l234()==
0xB0;}bool l459()const noexcept{if(!l602()){return false;}return l36
==64||l36==66||l36==67||l36==68;}double l509()const noexcept{return
std::clamp(static_cast<double>(l254)/127.0,0.0,1.0);}};enum class l62
{l241,l310};l136()=delete;static std::vector<l8>l569(const std::
string&l160){auto ls=l731(l777(l778(l160)));std::stable_sort(ls.begin
(),ls.end(),[](const l8&l0,const l8&l1){return l0.l143<l1.l143;});
return ls;}static l62 l729(double l439,double l603,const std::string&
l160,std::stop_token stop_token={}){if(!(l439>0.0)){throw std::
invalid_argument("\x70\x6c\x61\x79\x62\x61\x63\x6b\x5f\x72\x61\x74"
"\x65\x20\x6d\x75\x73\x74\x20\x62\x65\x20\x67\x72\x65\x61\x74\x65\x72"
"\x20\x74\x68\x61\x6e\x20\x7a\x65\x72\x6f\x2e");}const double l452=
std::max(0.0,l603);std::vector<l8>ls=l569(l160);const auto l780=std::
lower_bound(ls.begin(),ls.end(),l452,[](const l8&li,double time){
return li.l143<time;});std::unordered_set<int>l257;std::mutex l616;
std::condition_variable l395;std::stop_callback l557(stop_token,[&
l395]{l395.notify_all();});const auto l717=std::chrono::steady_clock
::now();auto l273=[&l257]{for(int lr:l257){l106(lr,0.0);}l285();};try
{for(auto li=l780;li!=ls.end();++li){if(stop_token.stop_requested()){
l273();return l62::l310;}const double l764=std::max(0.0,li->l143-l452
)/l439;const auto l601=l717+std::chrono::duration_cast<std::chrono::
steady_clock::duration>(std::chrono::duration<double>(l764));{std::
unique_lock lock(l616);l395.wait_until(lock,l601,[&stop_token]{return
stop_token.stop_requested();});}if(stop_token.stop_requested()){l273(
);return l62::l310;}l506( *li);if(li->l387()&&l58(li->l36)){l257.
insert(static_cast<int>(li->l36));}else if(li->l354()){l257.erase(
static_cast<int>(li->l36));}}}catch(...){l273();throw;}l273();return
l62::l241;}static void l506(const l8&li){const int lr=static_cast<int
>(li.l36);const double l184=static_cast<double>(li.l254);if(li.l387()){
if(l58(li.l36)){l52(lr,l184);}}else if(li.l354()){if(l58(li.l36)){
l106(lr,l184);}}else if(li.l582()){if(l58(li.l36)){l190(lr,li.l509());
}}else if(li.l459()){l722(li.l36,li.l509());}}static void l285(){l127
(0.0);l125(0.0);l119(0.0);l71(0.0);}private:struct l151{std::uint64_t
l24=0;double l577=500000.0;std::size_t l480=0;};class l583{public:
explicit l583(std::vector<std::uint8_t>l27):l133(std::move(l27)){}std
::size_t l7()const noexcept{return l64;}std::size_t size()const
noexcept{return l133.size();}void l449(std::size_t l7){if(l7>l133.
size()){throw l9("\x4d\x49\x44\x49\x20\x63\x68\x75\x6e\x6b\x20\x65"
"\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c\x65\x20\x73\x69\x7a\x65\x2e"
);}l64=l7;}std::uint8_t l53(std::size_t l18=std::numeric_limits<std::
size_t>::max()){if(l64>=l133.size()||l64>=l18){throw l9("\x55\x6e\x65"
"\x78\x70\x65\x63\x74\x65\x64\x20\x65\x6e\x64\x20\x6f\x66\x20\x4d\x49"
"\x44\x49\x20\x64\x61\x74\x61\x2e");}return l133[l64++];}std::
uint16_t l384(std::size_t l18=std::numeric_limits<std::size_t>::max()){
const std::uint16_t l430=l53(l18);const std::uint16_t l280=l53(l18);
return static_cast<std::uint16_t>((l430<<8)|l280);}std::uint32_t l531
(std::size_t l18=std::numeric_limits<std::size_t>::max()){const std::
uint32_t l88=l53(l18);const std::uint32_t l89=l53(l18);const std::
uint32_t l163=l53(l18);const std::uint32_t l642=l53(l18);return(l88<<
24)|(l89<<16)|(l163<<8)|l642;}std::uint32_t l400(std::size_t l18){std
::uint32_t lt=0;for(int count=0;count<4;++count){const std::uint8_t
l35=l53(l18);lt=(lt<<7)|static_cast<std::uint32_t>(l35&0x7F);if((l35&
0x80)==0){return lt;}}throw l9("\x49\x6e\x76\x61\x6c\x69\x64\x20\x4d"
"\x49\x44\x49\x20\x76\x61\x72\x69\x61\x62\x6c\x65\x2d\x6c\x65\x6e\x67"
"\x74\x68\x20\x76\x61\x6c\x75\x65\x2e");}std::string l524(std::size_t
count,std::size_t l18=std::numeric_limits<std::size_t>::max()){l393(
count,l18);const auto begin=l133.begin()+static_cast<std::ptrdiff_t>(
l64);l64+=count;return std::string(begin,begin+static_cast<std::
ptrdiff_t>(count));}std::vector<std::uint8_t>l592(std::size_t count,
std::size_t l18){l393(count,l18);const auto begin=l133.begin()+
static_cast<std::ptrdiff_t>(l64);l64+=count;return{begin,begin+
static_cast<std::ptrdiff_t>(count)};}void l761(std::size_t count,std
::size_t l18=std::numeric_limits<std::size_t>::max()){l393(count,l18);
l64+=count;}private:void l393(std::size_t count,std::size_t l18)const
{const std::size_t l533=std::min(l18,l133.size());if(l64>l533||count>
l533-l64){throw l9("\x55\x6e\x65\x78\x70\x65\x63\x74\x65\x64\x20\x65"
"\x6e\x64\x20\x6f\x66\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x2e");}
}std::vector<std::uint8_t>l133;std::size_t l64=0;};static bool l58(
std::uint8_t lr)noexcept{return lr>=21&&lr<=108;}static void l722(std
::uint8_t l117,double l11){switch(l117){case 64:l71(l11);break;case 66
:l119(l11);break;case 67:l127(l11);break;case 68:l125(l11);break;
default:break;}}static std::vector<std::uint8_t>l778(const std::
string&path){std::ifstream l227(path,std::ios::binary|std::ios::ate);
if(!l227){throw l9("\x43\x61\x6e\x6e\x6f\x74\x20\x6f\x70\x65\x6e\x20"
"\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x3a\x20"+path);}const std::
streampos end=l227.tellg();if(end<=0){throw l9("\x4d\x49\x44\x49\x20"
"\x66\x69\x6c\x65\x20\x69\x73\x20\x65\x6d\x70\x74\x79\x3a\x20"+path);
}if(static_cast<std::uintmax_t>(end)>std::numeric_limits<std::size_t>
::max()){throw l9("\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x20\x69\x73"
"\x20\x74\x6f\x6f\x20\x6c\x61\x72\x67\x65\x2e");}std::vector<std::
uint8_t>l27(static_cast<std::size_t>(end));l227.seekg(0,std::ios::beg
);l227.read(reinterpret_cast<char* >(l27.data()),static_cast<std::
streamsize>(l27.size()));if(!l227){throw l9("\x46\x61\x69\x6c\x65\x64"
"\x20\x74\x6f\x20\x72\x65\x61\x64\x20\x4d\x49\x44\x49\x20\x66\x69\x6c"
"\x65\x3a\x20"+path);}return l27;}static std::vector<l8>l777(std::
vector<std::uint8_t>l27){l583 l3(std::move(l27));if(l3.l524(4)!="\x4d"
"\x54\x68\x64"){throw l9("\x4d\x69\x73\x73\x69\x6e\x67\x20\x4d\x49"
"\x44\x49\x20\x68\x65\x61\x64\x65\x72\x20\x63\x68\x75\x6e\x6b\x2e");}
const std::uint32_t l585=l3.l531();if(l585<6){throw l9("\x49\x6e\x76"
"\x61\x6c\x69\x64\x20\x4d\x49\x44\x49\x20\x68\x65\x61\x64\x65\x72\x20"
"\x6c\x65\x6e\x67\x74\x68\x2e");}const std::size_t l176=l3.l7()+l585;
if(l176<l3.l7()||l176>l3.size()){throw l9("\x4d\x49\x44\x49\x20\x68"
"\x65\x61\x64\x65\x72\x20\x65\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c"
"\x65\x20\x73\x69\x7a\x65\x2e");}const std::uint16_t l23=l3.l384(l176
);const std::uint16_t l326=l3.l384(l176);const std::uint16_t l405=l3.
l384(l176);l3.l449(l176);if(l23>1){throw l9("\x4f\x6e\x6c\x79\x20\x4d"
"\x49\x44\x49\x20\x66\x6f\x72\x6d\x61\x74\x20\x30\x20\x61\x6e\x64\x20"
"\x31\x20\x61\x72\x65\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}
if(l326==0||(l23==0&&l326!=1)){throw l9("\x49\x6e\x76\x61\x6c\x69\x64"
"\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x63\x6f\x75\x6e\x74"
"\x2e");}if((l405&0x8000)!=0||l405==0){throw l9("\x53\x4d\x50\x54\x45"
"\x20\x6f\x72\x20\x7a\x65\x72\x6f\x20\x4d\x49\x44\x49\x20\x74\x69\x6d"
"\x65\x20\x64\x69\x76\x69\x73\x69\x6f\x6e\x20\x69\x73\x20\x75\x6e\x73"
"\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}std::vector<l8>ls;std::
vector<l151>l178{{0,500000.0,0}};std::size_t l591=1;for(std::uint16_t
l12=0;l12<l326;++l12){if(l3.l524(4)!="\x4d\x54\x72\x6b"){throw l9(""
"\x4d\x69\x73\x73\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63"
"\x6b\x20\x63\x68\x75\x6e\x6b\x2e");}const std::size_t l453=l3.l531();
const std::size_t l550=l3.l7();if(l453>l3.size()-l550){throw l9("\x4d"
"\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x65\x78\x63\x65\x65\x64\x73"
"\x20\x66\x69\x6c\x65\x20\x73\x69\x7a\x65\x2e");}const std::size_t l57
=l550+l453;std::uint64_t l308=0;std::uint8_t l54=0;std::string l262;
while(l3.l7()<l57){const std::uint32_t l495=l3.l400(l57);if(l495>std
::numeric_limits<std::uint64_t>::max()-l308){throw l9("\x4d\x49\x44"
"\x49\x20\x61\x62\x73\x6f\x6c\x75\x74\x65\x20\x74\x69\x63\x6b\x20\x6f"
"\x76\x65\x72\x66\x6c\x6f\x77\x2e");}l308+=l495;const std::uint8_t
l410=l3.l53(l57);std::uint8_t l6=l410;std::uint8_t l36=0;bool l461=
false;if(l410<0x80){if(l54==0){throw l9("\x4d\x49\x44\x49\x20\x72\x75"
"\x6e\x6e\x69\x6e\x67\x20\x73\x74\x61\x74\x75\x73\x20\x68\x61\x73\x20"
"\x6e\x6f\x20\x73\x74\x61\x74\x75\x73\x20\x62\x79\x74\x65\x2e");}l6=
l54;l36=l410;l461=true;}if(l6==0xFF){l54=0;const std::uint8_t l15=l3.
l53(l57);const std::size_t length=l3.l400(l57);const auto data=l3.
l592(length,l57);if(l15==0x03){l262.assign(data.begin(),data.end());}
else if(l15==0x51&&data.size()==3){const double l189=static_cast<
double>(data[0]) *65536.0+static_cast<double>(data[1]) *256.0+
static_cast<double>(data[2]);if(l189>0.0){l178.push_back({l308,l189,
l591++});}}continue;}if(l6==0xF0||l6==0xF7){l54=0;l3.l761(l3.l400(l57
),l57);continue;}if(l6>=0xF0){throw l9("\x55\x6e\x73\x75\x70\x70\x6f"
"\x72\x74\x65\x64\x20\x73\x79\x73\x74\x65\x6d\x20\x6d\x65\x73\x73\x61"
"\x67\x65\x20\x69\x6e\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x2e"
);}l54=l6;const std::uint8_t l15=l6&0xF0;const std::uint8_t l487=l461
?l36:l3.l53(l57);if((l487&0x80)!=0){throw l9("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62\x79\x74\x65"
"\x2e");}switch(l15){case 0x80:case 0x90:case 0xA0:case 0xB0:{const
std::uint8_t l442=l3.l53(l57);if((l442&0x80)!=0){throw l9("\x49\x6e"
"\x76\x61\x6c\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62"
"\x79\x74\x65\x2e");}ls.push_back({l308,0.0,l6,l487,l442,l262});break
;}case 0xE0:static_cast<void>(l3.l53(l57));break;case 0xC0:case 0xD0:
break;default:throw l9("\x49\x6e\x76\x61\x6c\x69\x64\x20\x4d\x49\x44"
"\x49\x20\x63\x68\x61\x6e\x6e\x65\x6c\x20\x6d\x65\x73\x73\x61\x67\x65"
"\x2e");}}l3.l449(l57);}l785(ls,l178,static_cast<double>(l405));
return ls;}static void l785(std::vector<l8>&ls,std::vector<l151>&l178
,double l562){std::stable_sort(l178.begin(),l178.end(),[](const l151&
l0,const l151&l1){if(l0.l24!=l1.l24){return l0.l24<l1.l24;}return l0.
l480<l1.l480;});std::vector<l151>l118;for(const l151&l189:l178){if(!
l118.empty()&&l118.back().l24==l189.l24){l118.back()=l189;}else{l118.
push_back(l189);}}std::stable_sort(ls.begin(),ls.end(),[](const l8&l0
,const l8&l1){return l0.l24<l1.l24;});std::size_t l407=0;std::
uint64_t l181=0;double l320=0.0;double l364=l118.front().l577;for(l8&
li:ls){while(l407+1<l118.size()&&l118[l407+1].l24<=li.l24){const l151
&l388=l118[++l407];l320+=static_cast<double>(l388.l24-l181) *l364/(
1000000.0*l562);l181=l388.l24;l364=l388.l577;}li.l143=l320+
static_cast<double>(li.l24-l181) *l364/(1000000.0*l562);}}static std
::vector<l8>l731(const std::vector<l8>&ls){std::vector<l8>l279;for(
const l8&li:ls){if(li.l387()||li.l354()||li.l582()||li.l459()){l279.
push_back(li);}}std::vector<l8>l390;for(const l8&li:l279){std::string
l100=li.l262;std::transform(l100.begin(),l100.end(),l100.begin(),[](
unsigned char l73){return static_cast<char>(std::tolower(l73));});if(
l100.find("\x70\x69\x61\x6e\x6f")!=std::string::npos||l100.find("\x6b"
"\x65\x79\x62\x6f\x61\x72\x64")!=std::string::npos||l100.find("\x67"
"\x72\x61\x6e\x64")!=std::string::npos){l390.push_back(li);}}if(!l390
.empty()){return l390;}std::vector<l8>l330;for(const l8&li:l279){if(
li.l798()==0){l330.push_back(li);}}return l330.empty()?l279:l330;}};
class l295{public:enum class l62{l241,l310};l295()=delete;static l62
l666(const std::string&l160,std::filesystem::path lu={},std::uint32_t
ln=44100,double l428=5.0,std::stop_token stop_token={}){if(l160.empty
()){throw std::invalid_argument("\x4e\x6f\x20\x4d\x49\x44\x49\x20\x66"
"\x69\x6c\x65\x20\x70\x61\x74\x68\x20\x70\x72\x6f\x76\x69\x64\x65\x64"
"\x2e");}if(ln==0||ln>std::numeric_limits<std::uint32_t>::max()/
sizeof(float)||!std::isfinite(l428)||l428<0.0){throw std::
invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20\x57\x41\x56\x20"
"\x72\x65\x6e\x64\x65\x72\x69\x6e\x67\x20\x63\x6f\x6e\x66\x69\x67\x75"
"\x72\x61\x74\x69\x6f\x6e\x2e");}std::vector<l136::l8>ls=l136::l569(
l160);if(lu.empty()){lu=std::filesystem::path(l160);lu.
replace_extension("\x2e\x77\x61\x76");}l751(lu);const double l749=ls.
empty()?0.0:ls.back().l143;const long double l265=(static_cast<long
double>(l749)+l428) *ln;constexpr std::uint64_t l649=std::
numeric_limits<std::uint32_t>::max()-36ULL;if(!std::isfinite(l265)||
l265<0.0L||l265>l649/sizeof(float)){throw std::runtime_error("\x57"
"\x41\x56\x20\x65\x78\x63\x65\x65\x64\x73\x20\x74\x68\x65\x20\x52\x49"
"\x46\x46\x20\x33\x32\x2d\x62\x69\x74\x20\x73\x69\x7a\x65\x20\x6c\x69"
"\x6d\x69\x74\x3b\x20\x52\x46\x36\x34\x20\x69\x73\x20\x6e\x6f\x74\x20"
"\x69\x6d\x70\x6c\x65\x6d\x65\x6e\x74\x65\x64\x2e");}const std::
uint64_t l416=static_cast<std::uint64_t>(std::ceil(l265));const std::
uint64_t l272=l416*sizeof(float);std::filesystem::path l39=lu;l39+=""
"\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72\x74";if(std::filesystem::exists
(l39)){throw std::runtime_error("\x54\x65\x6d\x70\x6f\x72\x61\x72\x79"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x61\x6c\x72\x65\x61"
"\x64\x79\x20\x65\x78\x69\x73\x74\x73\x3a\x20"+l39.string());}l63();
try{std::ofstream ll(l39,std::ios::binary);if(!ll){throw std::
runtime_error("\x43\x61\x6e\x6e\x6f\x74\x20\x63\x72\x65\x61\x74\x65"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lu.string());}
l682(ll,ln,static_cast<std::uint32_t>(l272));constexpr std::size_t
l558=512;std::array<float,l558>l137{};std::uint64_t l61=0;std::size_t
l168=0;while(l61<l416){if(stop_token.stop_requested()){ll.close();l63
();std::error_code l202;std::filesystem::remove(l39,l202);return l62
::l310;}while(l168<ls.size()&&l505(ls[l168],ln)<=l61){l136::l506(ls[
l168]);++l168;}std::uint64_t l230=std::min<std::uint64_t>(l416,l61+
l558);if(l168<ls.size()){l230=std::min(l230,l505(ls[l168],ln));}if(
l230==l61){continue;}const auto l477=static_cast<std::size_t>(l230-
l61);l359(l137.data(),static_cast<int>(l477),1.0);ll.write(
reinterpret_cast<const char* >(l137.data()),static_cast<std::
streamsize>(l477*sizeof(float)));if(!ll){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69\x6c\x65\x20\x77\x72\x69\x74"
"\x69\x6e\x67\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lu
.string());}l61=l230;}l63();ll.close();if(!ll){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e"
"\x61\x6c\x69\x7a\x65\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a"
"\x20"+lu.string());}std::filesystem::rename(l39,lu);return l62::l241
;}catch(...){l63();std::error_code l202;std::filesystem::remove(l39,
l202);throw;}}private:static std::uint64_t l505(const l136::l8&li,std
::uint32_t ln)noexcept{return static_cast<std::uint64_t>(std::llround
(li.l143*ln));}static void l751(const std::filesystem::path&lu){if(
std::filesystem::exists(lu)){throw std::runtime_error("\x52\x65\x66"
"\x75\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f\x76\x65\x72\x77\x72\x69\x74"
"\x65\x20\x65\x78\x69\x73\x74\x69\x6e\x67\x20\x57\x41\x56\x20\x66\x69"
"\x6c\x65\x3a\x20"+lu.string());}const std::filesystem::path l411=lu.
parent_path();if(!l411.empty()&&!std::filesystem::exists(l411)){throw
std::runtime_error("\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x64"
"\x69\x72\x65\x63\x74\x6f\x72\x79\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74"
"\x20\x65\x78\x69\x73\x74\x3a\x20"+l411.string());}}static void l269(
std::ostream&ll,std::uint16_t lt){ll.put(static_cast<char>(lt&0xFF));
ll.put(static_cast<char>((lt>>8)&0xFF));}static void l232(std::
ostream&ll,std::uint32_t lt){ll.put(static_cast<char>(lt&0xFF));ll.
put(static_cast<char>((lt>>8)&0xFF));ll.put(static_cast<char>((lt>>16
)&0xFF));ll.put(static_cast<char>((lt>>24)&0xFF));}static void l682(
std::ostream&ll,std::uint32_t ln,std::uint32_t l272){constexpr std::
uint16_t l618=3;constexpr std::uint16_t l790=1;constexpr std::
uint16_t l743=32;constexpr std::uint16_t l490=sizeof(float);constexpr
std::uint32_t l573=16;const std::uint32_t l713=ln*l490;const std::
uint32_t l648=4+(8+l573)+(8+l272);ll.write("\x52\x49\x46\x46",4);l232
(ll,l648);ll.write("\x57\x41\x56\x45\x66\x6d\x74\x20",8);l232(ll,l573
);l269(ll,l618);l269(ll,l790);l232(ll,ln);l232(ll,l713);l269(ll,l490);
l269(ll,l743);ll.write("\x64\x61\x74\x61",4);l232(ll,l272);}};
#include<optional>
#include<CoreMIDI/CoreMIDI.h>
#include<functional>
#include<memory>
#include<span>
class lp{public:using l111=std::span<const std::uint8_t>;using l398=
std::function<void(l111)>;private:struct l148{explicit l148(l398
new_handler):l131(std::move(new_handler)){}void l605(l111 lj)noexcept
{{std::lock_guard lock(mutex);if(!l374){return;}++l432;}try{l131(lj);
}catch(...){}{std::lock_guard lock(mutex);--l432;}l287.notify_all();}
void l698()noexcept{std::unique_lock lock(mutex);l374=false;l287.wait
(lock,[this]{return l432==0;});l131={};}bool l786()const noexcept{std
::lock_guard lock(mutex);return l374;}mutable std::mutex mutex;std::
condition_variable l287;l398 l131;std::size_t l432=0;bool l374=true;}
;public:class l33{public:l33()noexcept=default;l33(const l33&)=delete
;l33&operator=(const l33&)=delete;l33(l33&&l271)noexcept:l108(std::
exchange(l271.l108,{})){}l33&operator=(l33&&l271)noexcept{if(this!=&
l271){reset();l108=std::exchange(l271.l108,{});}return*this;}~l33(){
reset();}void reset()noexcept{if(l108){l108->l698();l108.reset();}}
explicit operator bool()const noexcept{return static_cast<bool>(l108);
}private:friend class lp;explicit l33(std::shared_ptr<l148>l25):l108(
std::move(l25)){}std::shared_ptr<l148>l108;};static constexpr std::
uint8_t l404=64;static constexpr std::uint8_t l408=66;static constexpr
std::uint8_t l336=67;static constexpr std::uint8_t l329=68;lp()=
default;lp(const lp&)=delete;lp&operator=(const lp&)=delete;lp(lp&&)=
delete;lp&operator=(lp&&)=delete;~lp(){l67();}l33 l560(l398 l131){if(
!l131){throw std::invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20"
"\x4d\x49\x44\x49\x20\x6d\x65\x73\x73\x61\x67\x65\x20\x68\x61\x6e\x64"
"\x6c\x65\x72\x2e");}auto l25=std::make_shared<l148>(std::move(l131));
std::lock_guard lock(l114);l771();l142.push_back(l25);return l33(std
::move(l25));}void l72(){std::lock_guard l292(l164);{std::lock_guard
l275(l114);if(l91){return;}}const ItemCount l402=
MIDIGetNumberOfSources();if(l402==0){throw std::runtime_error("\x4e"
"\x6f\x20\x4d\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69"
"\x63\x65\x20\x66\x6f\x75\x6e\x64\x2e");}MIDIClientRef l90=0;
MIDIPortRef l70=0;std::vector<std::unique_ptr<l288>>l305;OSStatus l6=
MIDIClientCreate(CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49"
"\x6e\x70\x75\x74\x20\x48\x75\x62"),nullptr,nullptr,&l90);if(l6!=
noErr||l90==0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20"
"\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d\x49\x44"
"\x49\x20\x63\x6c\x69\x65\x6e\x74\x2e");}l6=MIDIInputPortCreate(l90,
CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49\x6e\x70\x75\x74"
"\x20\x50\x6f\x72\x74"),&lp::l748,nullptr,&l70);if(l6!=noErr||l70==0){
MIDIClientDispose(l90);throw std::runtime_error("\x46\x61\x69\x6c\x65"
"\x64\x20\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d"
"\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x70\x6f\x72\x74\x2e");}l305
.reserve(static_cast<std::size_t>(l402));for(ItemCount lh=0;lh<l402;
++lh){const MIDIEndpointRef l140=MIDIGetSource(lh);if(l140==0){
continue;}auto l43=std::make_unique<l288>();l43->l154=this;l43->l140=
l140;l6=MIDIPortConnectSource(l70,l140,l43.get());if(l6==noErr){l305.
push_back(std::move(l43));}}if(l305.empty()){MIDIPortDispose(l70);
MIDIClientDispose(l90);throw std::runtime_error("\x4e\x6f\x20\x4d\x49"
"\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69\x63\x65\x20\x63"
"\x6f\x75\x6c\x64\x20\x62\x65\x20\x6f\x70\x65\x6e\x65\x64\x2e");}{std
::lock_guard l275(l114);l380=l90;l372=l70;l389=std::move(l305);l351=
true;l91=true;}}void l67()noexcept{std::lock_guard l292(l164);
MIDIClientRef l90=0;MIDIPortRef l70=0;{std::lock_guard l275(l114);if(
!l91){return;}l351=false;l91=false;l90=l380;l70=l372;}for(const auto&
l43:l389){if(l70!=0&&l43&&l43->l140!=0){MIDIPortDisconnectSource(l70,
l43->l140);}}if(l70!=0){MIDIPortDispose(l70);}if(l90!=0){
MIDIClientDispose(l90);}{std::unique_lock l275(l114);l512.wait(l275,[
this]{return l363==0;});l380=0;l372=0;l389.clear();}}bool l646()const
noexcept{std::lock_guard lock(l114);return l91;}private:struct l218{
std::array<std::uint8_t,3>l27{};std::size_t size=0;};struct l500{std
::mutex mutex;std::uint8_t l54=0;std::uint8_t l134=0;std::array<std::
uint8_t,2>data{};std::size_t l186=0;std::size_t l196=0;bool l366=
false;};struct l288{lp*l154=nullptr;MIDIEndpointRef l140=0;l500 lq;};
class l414{public:explicit l414(lp&l154)noexcept:l469(&l154),l161(
l154.l676()){}~l414(){if(l161){l469->l674();}}explicit operator bool(
)const noexcept{return l161;}private:lp*l469;bool l161;};static void
l748(const MIDIPacketList*l348,void* ,void*l760)noexcept{auto*l43=
static_cast<l288* >(l760);if(l348==nullptr||l43==nullptr||l43->l154==
nullptr){return;}l414 l167( *l43->l154);if(!l167){return;}const
MIDIPacket*packet=&l348->packet[0];for(UInt32 lh=0;lh<l348->
numPackets;++lh){l43->l154->l651(l43->lq,packet->data,packet->length);
packet=MIDIPacketNext(packet);}}bool l676()noexcept{std::lock_guard
lock(l114);if(!l351){return false;}++l363;return true;}void l674()noexcept
{{std::lock_guard lock(l114);--l363;}l512.notify_all();}static std::
size_t l447(std::uint8_t l6)noexcept{switch(l6&0xF0){case 0x80:case
0x90:case 0xA0:case 0xB0:case 0xE0:return 2;case 0xC0:case 0xD0:
return 1;default:break;}switch(l6){case 0xF1:case 0xF3:return 1;case
0xF2:return 2;case 0xF6:return 0;default:return 0;}}void l651(l500&lq
,const Byte*data,UInt16 length)noexcept{std::vector<l218>l211;l211.
reserve(static_cast<std::size_t>(length)/2+1);{std::lock_guard l827(
lq.mutex);for(UInt16 lh=0;lh<length;++lh){const std::uint8_t l35=
static_cast<std::uint8_t>(data[lh]);if(l35>=0xF8){l218 lj;lj.l27[0]=
l35;lj.size=1;l211.push_back(lj);continue;}if(lq.l366){if(l35==0xF7){
lq.l366=false;}continue;}if((l35&0x80)!=0){lq.l196=0;lq.l134=0;if(l35
==0xF0){lq.l366=true;lq.l54=0;continue;}if(l35==0xF7){lq.l54=0;
continue;}lq.l134=l35;lq.l186=l447(l35);if(l35<0xF0){lq.l54=l35;}else
{lq.l54=0;}if(lq.l186==0){l218 lj;lj.l27[0]=l35;lj.size=1;l211.
push_back(lj);lq.l134=0;}continue;}if(lq.l134==0){if(lq.l54==0){
continue;}lq.l134=lq.l54;lq.l186=l447(lq.l134);lq.l196=0;}if(lq.l196<
lq.data.size()){lq.data[lq.l196++]=l35;}if(lq.l196==lq.l186){l218 lj;
lj.l27[0]=lq.l134;for(std::size_t l141=0;l141<lq.l186;++l141){lj.l27[
l141+1]=lq.data[l141];}lj.size=lq.l186+1;l211.push_back(lj);lq.l134=0
;lq.l196=0;}}}for(const l218&lj:l211){l604(l111(lj.l27.data(),lj.size
));}}void l604(l111 lj)noexcept{std::vector<std::shared_ptr<l148>>
l324;{std::lock_guard lock(l114);l324.reserve(l142.size());for(const
auto&l350:l142){if(auto l25=l350.lock()){l324.push_back(std::move(l25
));}}}for(const auto&l131:l324){l131->l605(lj);}}void l771(){l142.
erase(std::remove_if(l142.begin(),l142.end(),[](const std::weak_ptr<
l148>&l350){const auto l25=l350.lock();return!l25||!l25->l786();}),
l142.end());}mutable std::mutex l164;mutable std::mutex l114;std::
condition_variable l512;bool l91=false;bool l351=false;std::size_t
l363=0;MIDIClientRef l380=0;MIDIPortRef l372=0;std::vector<std::
unique_ptr<l288>>l389;std::vector<std::weak_ptr<l148>>l142;};class l92
{public:explicit l92(lp&l55)noexcept:l245(l55){}l92(const l92&)=
delete;l92(l92&&)=delete;l92&operator=(l92&&)=delete;~l92(){l67();}
void l72(){if(l20){return;}l20.emplace(l245.l560([](lp::l111 lj){l802
(lj);}));}void l67()noexcept{if(!l20){return;}l20->reset();l20.reset(
);l779();}bool l646()const noexcept{return l20.has_value();}private:
static bool l58(std::uint8_t lr)noexcept{return lr>=21&&lr<=108;}
static void l802(lp::l111 lj){if(lj.empty()){return;}const std::
uint8_t l15=lj[0]&0xF0;if(l15==0x80||l15==0x90||l15==0xA0){if(lj.size
()<3||!l58(lj[1])){return;}const int lr=static_cast<int>(lj[1]);const
double l184=static_cast<double>(lj[2]);if(l15==0x80||(l15==0x90&&lj[2
]==0)){l106(lr,l184);}else if(l15==0x90){l52(lr,l184);}else{l190(lr,
std::clamp(l184/127.0,0.0,1.0));}return;}if(l15!=0xB0||lj.size()<3){
return;}const double l11=std::clamp(static_cast<double>(lj[2])/127.0,
0.0,1.0);switch(lj[1]){case lp::l404:l71(l11);break;case lp::l408:
l119(l11);break;case lp::l336:l127(l11);break;case lp::l329:l125(l11);
break;default:break;}}static void l779()noexcept{l127(0.0);l125(0.0);
l119(0.0);l71(0.0);l63();}lp&l245;std::optional<lp::l33>l20;};
#include<iomanip>
#include<sstream>
class l66{public:struct l103{std::uint64_t l216=0;std::array<std::
uint8_t,3>lj{};std::uint8_t l409=0;};explicit l66(lp&l55)noexcept:
l245(l55){}l66(const l66&)=delete;l66&operator=(const l66&)=delete;
l66(l66&&)=delete;l66&operator=(l66&&)=delete;~l66(){l708();}void l72
(std::filesystem::path lu={}){std::lock_guard l292(l164);if(l20){
return;}{std::lock_guard l370(l249);l166.clear();l166.reserve(4096);
l318=std::move(lu);l572=l77::now();}l20.emplace(l245.l560([this](lp::
l111 lj){l634(lj);}));}std::filesystem::path l67(){std::lock_guard
l292(l164);const bool l633=l20.has_value();if(l20){l20->reset();l20.
reset();}std::vector<l103>l294;std::filesystem::path l325;{std::
lock_guard l370(l249);l294=l166;l325=l318;}if(!l633&&l294.empty()){
return l532;}const std::filesystem::path l403=l325.empty()?std::
filesystem::current_path()/l661(l294):l325;l656(l403,l294);{std::
lock_guard l370(l249);l166.clear();l318.clear();l532=l403;}return l403
;}void l708()noexcept{std::lock_guard l292(l164);if(l20){l20->reset();
l20.reset();}std::lock_guard l370(l249);l166.clear();l318.clear();}
bool l815()const noexcept{std::lock_guard lock(l164);return l20.
has_value();}private:using l77=std::chrono::steady_clock;static
constexpr std::uint16_t l437=480;static constexpr std::uint32_t l672=
500000;void l634(lp::l111 lj){if(!l755(lj)){return;}l103 li;li.l216=
static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::
microseconds>(l77::now()-l572).count());li.l409=static_cast<std::
uint8_t>(lj.size());std::copy(lj.begin(),lj.end(),li.lj.begin());std
::lock_guard lock(l249);l166.push_back(std::move(li));}static bool
l755(lp::l111 lj)noexcept{if(lj.empty()||lj[0]>=0xF0){return false;}
switch(lj[0]&0xF0){case 0x80:case 0x90:case 0xA0:case 0xB0:case 0xE0:
return lj.size()==3;case 0xC0:case 0xD0:return lj.size()==2;default:
return false;}}static std::string l661(const std::vector<l103>&ls){
const std::time_t now=std::time(nullptr);std::tm l486{};localtime_r(&
now,&l486);std::size_t l508=0;std::size_t l528=0;for(const l103&li:ls
){if(li.l409<3){continue;}const std::uint8_t l15=li.lj[0]&0xF0;if(l15
==0x90&&li.lj[2]!=0){++l508;}else if(l15==0xB0&&l659(li.lj[1])){++
l528;}}const std::uint64_t l320=ls.empty()?0:ls.back().l216/
1000000ULL;std::ostringstream l100;l100<<std::put_time(&l486,"\x25"
"\x59\x2d\x25\x6d\x2d\x25\x64\x5f\x25\x48\x2d\x25\x4d\x2d\x25\x53")<<""
"\x5f"<<l508<<"\x2d\x6e\x6f\x74\x65\x73\x5f"<<l528<<"\x2d\x70\x65\x64"
"\x61\x6c\x73\x5f"<<l320<<"\x2d\x73\x65\x63\x6f\x6e\x64\x73\x2e\x6d"
"\x69\x64";return l100.str();}static bool l659(std::uint8_t l117)noexcept
{return l117==lp::l404||l117==lp::l408||l117==lp::l336||l117==lp::
l329;}static void l544(std::vector<std::uint8_t>&ll,std::uint32_t lt){
std::uint8_t l27[5]{};int lh=4;l27[lh]=static_cast<std::uint8_t>(lt&
0x7F);while((lt>>=7)!=0&&lh>0){l27[--lh]=static_cast<std::uint8_t>((
lt&0x7F)|0x80);}for(;lh<5;++lh){ll.push_back(l27[lh]);}}static std::
uint32_t l678(std::uint64_t microseconds)noexcept{const long double
l434=static_cast<long double>(microseconds) *l437/l672;return l434>=
std::numeric_limits<std::uint32_t>::max()?std::numeric_limits<std::
uint32_t>::max():static_cast<std::uint32_t>(l434);}static void l721(
std::vector<std::uint8_t>&l12){const std::uint8_t l809[]={lp::l404,lp
::l408,lp::l336,lp::l329};for(std::uint8_t l117:l809){l544(l12,0);l12
.insert(l12.end(),{0xB0,l117,0});}}static void l418(std::ostream&ll,
std::uint16_t lt){ll.put(static_cast<char>((lt>>8)&0xFF));ll.put(
static_cast<char>(lt&0xFF));}static void l563(std::ostream&ll,std::
uint32_t lt){ll.put(static_cast<char>((lt>>24)&0xFF));ll.put(
static_cast<char>((lt>>16)&0xFF));ll.put(static_cast<char>((lt>>8)&
0xFF));ll.put(static_cast<char>(lt&0xFF));}static void l656(const std
::filesystem::path&lu,std::vector<l103>ls){std::stable_sort(ls.begin(
),ls.end(),[](const l103&l0,const l103&l1){return l0.l216<l1.l216;});
std::vector<std::uint8_t>l12;l12.reserve(ls.size() *4+32);l12.insert(
l12.end(),{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20});std::uint32_t l181=0;
for(const l103&li:ls){const std::uint32_t l24=l678(li.l216);l544(l12,
l24-l181);l181=l24;l12.insert(l12.end(),li.lj.begin(),li.lj.begin()+
li.l409);}l721(l12);l12.insert(l12.end(),{0x00,0xFF,0x2F,0x00});if(
l12.size()>std::numeric_limits<std::uint32_t>::max()){throw std::
runtime_error("\x52\x65\x63\x6f\x72\x64\x65\x64\x20\x4d\x49\x44\x49"
"\x20\x74\x72\x61\x63\x6b\x20\x69\x73\x20\x74\x6f\x6f\x20\x6c\x61\x72"
"\x67\x65\x2e");}if(!lu.parent_path().empty()&&!std::filesystem::
exists(lu.parent_path())){throw std::runtime_error("\x4d\x49\x44\x49"
"\x20\x6f\x75\x74\x70\x75\x74\x20\x64\x69\x72\x65\x63\x74\x6f\x72\x79"
"\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74\x20\x65\x78\x69\x73\x74\x3a\x20"
+lu.parent_path().string());}if(std::filesystem::exists(lu)){throw std
::runtime_error("\x52\x65\x66\x75\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f"
"\x76\x65\x72\x77\x72\x69\x74\x65\x20\x65\x78\x69\x73\x74\x69\x6e\x67"
"\x20\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x3a\x20"+lu.string());}std
::filesystem::path l39=lu;l39+="\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72"
"\x74";if(std::filesystem::exists(l39)){throw std::runtime_error(""
"\x54\x65\x6d\x70\x6f\x72\x61\x72\x79\x20\x4d\x49\x44\x49\x20\x6f\x75"
"\x74\x70\x75\x74\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x65\x78\x69\x73"
"\x74\x73\x3a\x20"+l39.string());}try{std::ofstream ll(l39,std::ios::
binary);if(!ll){throw std::runtime_error("\x43\x61\x6e\x6e\x6f\x74"
"\x20\x63\x72\x65\x61\x74\x65\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70"
"\x75\x74\x3a\x20"+lu.string());}ll.write("\x4d\x54\x68\x64",4);l563(
ll,6);l418(ll,0);l418(ll,1);l418(ll,l437);ll.write("\x4d\x54\x72\x6b"
,4);l563(ll,static_cast<std::uint32_t>(l12.size()));ll.write(
reinterpret_cast<const char* >(l12.data()),static_cast<std::
streamsize>(l12.size()));ll.close();if(!ll){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69\x6c\x65\x20\x77\x72\x69\x74"
"\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"
+lu.string());}std::filesystem::rename(l39,lu);}catch(...){std::
error_code l202;std::filesystem::remove(l39,l202);throw;}}lp&l245;
mutable std::mutex l164;mutable std::mutex l249;std::optional<lp::l33
>l20;std::vector<l103>l166;std::filesystem::path l318;std::filesystem
::path l532;l77::time_point l572{};};
#include<sys/event.h>
#include<sys/time.h>
#include<termios.h>
#include<unistd.h>
#include<atomic>
#include<cctype>
#include<cerrno>
#include<string_view>
class l99{public:l99()=default;l99(const l99&)=delete;l99&operator=(
const l99&)=delete;l99(l99&&)=delete;l99&operator=(l99&&)=delete;void
l817(std::chrono::milliseconds duration)noexcept{const auto l791=std
::clamp(duration.count(),20LL,2000LL);l553.store(static_cast<int>(
l791));}void l796(std::stop_token stop_token={}){l386 l828;l362 l208;
l615(l208.get());std::stop_callback l557(stop_token,[l711=l208.get()]
{struct kevent li{};EV_SET(&li,l424,EVFILT_USER,0,NOTE_TRIGGER,0,
nullptr);static_cast<void>(kevent(l711,&li,1,nullptr,0,nullptr));});
l726();try{while(!stop_token.stop_requested()){l707();const timespec
l766=l639();struct kevent li{};const int count=kevent(l208.get(),
nullptr,0,&li,1,&l766);if(count<0){if(errno==EINTR){continue;}throw
std::runtime_error("\x6b\x71\x75\x65\x75\x65\x20\x77\x61\x69\x74\x20"
"\x66\x61\x69\x6c\x65\x64\x2e");}if(count==0){continue;}if(li.filter
==EVFILT_USER&&li.ident==l424){break;}if(li.filter==EVFILT_READ&&li.
ident==STDIN_FILENO&&!l723()){break;}}}catch(...){l448();l285();throw
;}l448();l285();}private:using l77=std::chrono::steady_clock;static
constexpr uintptr_t l424=1;static constexpr int l688=24;static
constexpr int l732=108;static constexpr int l807=10;class l386{public
:l386(){if(!isatty(STDIN_FILENO)){throw std::runtime_error("\x50\x43"
"\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20\x72\x65\x71\x75\x69\x72\x65"
"\x73\x20\x61\x6e\x20\x69\x6e\x74\x65\x72\x61\x63\x74\x69\x76\x65\x20"
"\x74\x65\x72\x6d\x69\x6e\x61\x6c\x2e");}if(tcgetattr(STDIN_FILENO,&
l396)!=0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x72\x65\x61\x64\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d"
"\x6f\x64\x65\x2e");}termios l281=l396;l281.c_lflag&=static_cast<
tcflag_t>(~(ICANON|ECHO));l281.c_cc[VMIN]=1;l281.c_cc[VTIME]=0;if(
tcsetattr(STDIN_FILENO,TCSANOW,&l281)!=0){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x65\x6e\x74\x65\x72\x20\x72"
"\x61\x77\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d\x6f\x64\x65\x2e"
);}l554=true;}~l386(){if(l554){static_cast<void>(tcsetattr(
STDIN_FILENO,TCSANOW,&l396));}}private:termios l396{};bool l554=false
;};class l362{public:l362():l301(kqueue()){if(l301<0){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x6b\x71\x75\x65\x75\x65\x2e");}}~l362(){close(l301);
}int get()const noexcept{return l301;}private:int l301;};struct l194{
int lr=0;l77::time_point l297{};bool l129=false;};struct l543{int l141
=-1;double l82=0.0;};static void l615(int l208){std::array<struct
kevent,2>l302{};EV_SET(&l302[0],STDIN_FILENO,EVFILT_READ,EV_ADD|
EV_ENABLE,0,0,nullptr);EV_SET(&l302[1],l424,EVFILT_USER,EV_ADD|
EV_CLEAR,0,0,nullptr);if(kevent(l208,l302.data(),static_cast<int>(
l302.size()),nullptr,0,nullptr)!=0){throw std::runtime_error("\x46"
"\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x72\x65\x67\x69\x73\x74\x65\x72"
"\x20\x6b\x71\x75\x65\x75\x65\x20\x65\x76\x65\x6e\x74\x73\x2e");}}
bool l723(){std::array<char,64>l203{};const ssize_t count=read(
STDIN_FILENO,l203.data(),l203.size());if(count==0){return false;}if(
count<0){return errno==EINTR||errno==EAGAIN;}for(ssize_t lh=0;lh<
count;++lh){l640(l203[static_cast<std::size_t>(lh)]);}return true;}
void l640(char l73){if(l73>='1'&&l73<='9'){l341=l688+(static_cast<int
>(l73-'1') *l807);l704("\x50\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64"
"\x20\x62\x61\x73\x65\x20\x4d\x49\x44\x49\x20\x6e\x6f\x74\x65\x3a\x20"
,l341);return;}if(l590(l73)){return;}const l543 l373=l799(l73);const
int lr=l341+l373.l141;if(l373.l141<0||lr<21||lr>l732){return;}l730(lr
,l373.l82);}void l730(int lr,double l82){const auto now=l77::now();
const auto l481=std::chrono::milliseconds(l553.load());l194*l182=
nullptr;for(l194&l21:l317){if(l21.l129&&l21.lr==lr){l21.l297=now+l481
;return;}if(!l21.l129&&l182==nullptr){l182=&l21;}}if(l182==nullptr){
return;}l52(lr,l82);l691("\x4e\x6f\x74\x65\x4f\x6e\x3a\x20\x4d\x49"
"\x44\x49\x20",lr,"\x2c\x20\x76\x65\x6c\x6f\x63\x69\x74\x79\x20",l82);
l182->lr=lr;l182->l297=now+l481;l182->l129=true;}void l707(){const
auto now=l77::now();for(l194&l21:l317){if(l21.l129&&l21.l297<=now){
l106(l21.lr,0.0);l21.l129=false;}}}void l448()noexcept{for(l194&l21:
l317){if(l21.l129){l106(l21.lr,0.0);l21.l129=false;}}}timespec l639()const
noexcept{auto l311=l77::time_point::max();for(const l194&l21:l317){if
(l21.l129){l311=std::min(l311,l21.l297);}}if(l311==l77::time_point::
max()){return timespec{3600,0};}const auto l738=std::max(l77::
duration::zero(),l311-l77::now());const auto nanoseconds=std::chrono
::duration_cast<std::chrono::nanoseconds>(l738).count();return
timespec{static_cast<time_t>(nanoseconds/1000000000LL),static_cast<
long>(nanoseconds%1000000000LL)};}static l543 l799(char l73)noexcept{
const char l385=static_cast<char>(std::tolower(static_cast<unsigned
char>(l73)));constexpr std::string_view l706="\x71\x77\x65\x72\x74"
"\x79\x75\x69\x6f\x70";constexpr std::string_view l631="\x61\x73\x64"
"\x66\x67\x68\x6a\x6b\x6c\x3b";constexpr std::string_view l607="\x7a"
"\x78\x63\x76\x62\x6e\x6d\x2c\x2e\x2f";if(const auto lh=l706.find(
l385);lh!=std::string_view::npos){return{static_cast<int>(lh),112.0};
}if(const auto lh=l631.find(l385);lh!=std::string_view::npos){return{
static_cast<int>(lh),72.0};}if(const auto lh=l607.find(l385);lh!=std
::string_view::npos){return{static_cast<int>(lh),40.0};}return{};}
bool l590(char l73){switch(l73){case'-':l304=!l304;if(l304){l127(1.0);
l115("\x50\x65\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53"
"\x6f\x66\x74\x20\x70\x65\x64\x61\x6c\x20\x28\x75\x6e\x61\x20\x63\x6f"
"\x72\x64\x61\x29\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x70\x72\x65"
"\x73\x73\x65\x64\x2e");}else{l127(0.0);l115("\x50\x65\x64\x61\x6c"
"\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x6f\x66\x74\x20\x70\x65"
"\x64\x61\x6c\x20\x28\x75\x6e\x61\x20\x63\x6f\x72\x64\x61\x29\x20\x68"
"\x61\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e"
);}return true;case'=':l291=!l291;if(l291){l125(1.0);l115("\x50\x65"
"\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x48\x61\x72\x6d\x6f"
"\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65"
"\x6e\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l125(0.0);l115(""
"\x50\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x48\x61"
"\x72\x6d\x6f\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20"
"\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}return
true;case'[':l260=!l260;if(l260){l119(1.0);l115("\x50\x65\x64\x61\x6c"
"\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x6f\x73\x74\x65\x6e\x75\x74"
"\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20"
"\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l119(0.0);l115("\x50\x65"
"\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x6f\x73\x74"
"\x65\x6e\x75\x74\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62"
"\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}return true;
case']':l278=!l278;if(l278){l71(1.0);l115("\x50\x65\x64\x61\x6c\x50"
"\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x75\x73\x74\x61\x69\x6e\x20\x70"
"\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x70\x72\x65"
"\x73\x73\x65\x64\x2e");}else{l71(0.0);l115("\x50\x65\x64\x61\x6c\x52"
"\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x75\x73\x74\x61\x69\x6e\x20"
"\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x72\x65"
"\x6c\x65\x61\x73\x65\x64\x2e");}return true;return true;default:
return false;}}void l285()noexcept{l304=false;l291=false;l260=false;
l278=false;l127(0.0);l125(0.0);l119(0.0);l71(0.0);}static void l726(){
std::cout<<"\x50\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20\x73\x74"
"\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65\x78\x69"
"\x74\x73\x29\x2e\n"<<"\x20\x20\x51\x2d\x50\x20\x2f\x20\x41\x2d\x3b"
"\x20\x2f\x20\x5a\x2d\x2f\x20\x3a\x20\x76\x65\x6c\x6f\x63\x69\x74\x79"
"\x20\x31\x31\x32\x20\x2f\x20\x37\x32\x20\x2f\x20\x34\x30\n"<<"\x20"
"\x20\x31\x2d\x39\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x3a\x20\x73\x65\x6c\x65\x63\x74\x20\x70\x69\x74\x63\x68\x20"
"\x62\x61\x6e\x6b\n"<<"\x20\x20\x2d\x20\x3d\x20\x5b\x20\x5d\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x3a\x20\x73\x6f\x66\x74\x20\x2f"
"\x20\x68\x61\x72\x6d\x6f\x6e\x69\x63\x20\x2f\x20\x73\x6f\x73\x74\x65"
"\x6e\x75\x74\x6f\x20\x2f\x20\x73\x75\x73\x74\x61\x69\x6e\n";}inline
void l115(std::string_view lj){std::cout<<"\r\x1b\x5b\x32\x4b"<<lj<<
std::flush;}inline void l704(std::string_view lj,double lt){std::cout
<<"\r\x1b\x5b\x32\x4b"<<lj<<lt<<std::flush;}inline void l691(std::
string_view l736,double l803,std::string_view l735,double l805){std::
cout<<"\r\x1b\x5b\x32\x4b"<<l736<<l803<<l735<<l805<<std::flush;}std::
atomic<int>l553{350};int l341=60;std::array<l194,30>l317{};bool l304=
false;bool l291=false;bool l260=false;bool l278=false;};namespace{
void l406(std::stop_token stop_token){std::mutex mutex;std::
condition_variable_any l287;std::unique_lock lock(mutex);l287.wait(
lock,stop_token,[]{return false;});}}int l116(int l226,char*l14[],
const char*l17,const char*l30,std::stop_token l237){return l470(l226,
l14,l17,l30,l237);}void l473(const std::string&l40,std::stop_token
stop_token){std::cout<<"\x50\x6c\x61\x79\x69\x6e\x67\x20\x4d\x49\x44"
"\x49\x3a\x20"<<l40<<'\n';const l136::l62 l157=l136::l729(1.0,0.0,l40
,stop_token);if(l157==l136::l62::l241){std::cout<<"\x4d\x49\x44\x49"
"\x20\x70\x6c\x61\x79\x62\x61\x63\x6b\x20\x63\x6f\x6d\x70\x6c\x65\x74"
"\x65\x64\x2e\n";}}void l502(std::stop_token stop_token){lp l55;l92
l79(l55);l79.l72();l55.l72();std::cout<<"\x4d\x49\x44\x49\x20\x70\x69"
"\x61\x6e\x6f\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c"
"\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e\n";l406(stop_token);l55.l67
();l79.l67();}void l441(std::stop_token stop_token){l99 l79;l79.l796(
stop_token);}void l565(std::string l40,std::stop_token stop_token){
std::filesystem::path lu(l40);lu.replace_extension("\x2e\x77\x61\x76"
);const l295::l62 l157=l295::l666(l40,lu,static_cast<std::uint32_t>(
ln),5.0,stop_token);if(l157==l295::l62::l241){std::cout<<"\x45\x78"
"\x70\x6f\x72\x74\x65\x64\x20\x57\x41\x56\x3a\x20"<<lu<<'\n';}}void
l527(std::stop_token stop_token){lp l55;l92 l79(l55);l66 l546(l55);
l79.l72();l546.l72();l55.l72();std::cout<<"\x4d\x49\x44\x49\x20\x72"
"\x65\x63\x6f\x72\x64\x69\x6e\x67\x20\x73\x74\x61\x72\x74\x65\x64\x20"
"\x28\x43\x74\x72\x6c\x2d\x43\x20\x73\x61\x76\x65\x73\x20\x61\x6e\x64"
"\x20\x65\x78\x69\x74\x73\x29\x2e\n";l406(stop_token);l55.l67();const
std::filesystem::path lu=l546.l67();l79.l67();std::cout<<"\x52\x65"
"\x63\x6f\x72\x64\x65\x64\x20\x4d\x49\x44\x49\x3a\x20"<<lu<<'\n';}
void l501(std::stop_token stop_token){std::cout<<"\x54\x65\x73\x74"
"\x20\x73\x65\x72\x76\x69\x63\x65\x20\x73\x74\x61\x72\x74\x65\x64\x2e"
"\n\n";for(int lo=21;lo<=108;++lo){l52(lo,110.0);}std::this_thread::
sleep_for(std::chrono::milliseconds(500));l458();}void l518(std::
stop_token stop_token){std::cout<<"\x49\x6e\x74\x65\x72\x6e\x61\x6c"
"\x20\x74\x65\x73\x74\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74"
"\x72\x6c\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e\n";l71(0.6);l52(69,
110);l406(stop_token);l63();}
#include<numbers>
class l645{float l122=0.0f;float l217=0.0f;public:inline void l29(
float&lm){constexpr float l599=0.020f;constexpr float l632=0.25f;
constexpr float l695=0.38f;constexpr float l456=1.0f-l599;constexpr
float l514=l695*l632;constexpr float l753=1.0f-l514;constexpr float
l638=l456*l753;constexpr float l792=l456*l514;constexpr float l88=
0.292893218813f;constexpr float l89=0.585786437627f;constexpr float
l163=0.292893218813f;constexpr float l153=0.171572875254f;const float
l34=l88*lm+l122;l122=l89*lm+l217;l217=l163*lm-l153*l34;lm=l638*lm+
l792*l34;}inline void lx()noexcept{l122=0.0f;l217=0.0f;}inline float
l51()const noexcept{return 0.5f* (l122*l122+l217*l217);}};
class l507{inline static constexpr std::size_t l446=6;inline static
constexpr std::size_t l830=237;struct la{float l65;float l153;};
struct lc{int lo;double l427;double l819;double l367;double l689;std
::size_t l436;std::array<la,l446>l109;};lc l193;std::array<float,l446
>l174{},l229{},l169{},l206{};public:explicit l507(double l284)noexcept
:l193(l742(l284)){}inline void l29(float&lm)noexcept{for(std::size_t
ld=0;ld<l193.l436;++ld){const auto&l97=l193.l109[ld];const float l34=
l229[ld]+l97.l65* (l174[ld]-l169[ld])+l97.l153* (lm-l206[ld]);l229[ld
]=l174[ld];l174[ld]=lm;l206[ld]=l169[ld];l169[ld]=l34;lm=l34;}}inline
void lx()noexcept{l174.fill(0);l229.fill(0);l169.fill(0);l206.fill(0);
}[[nodiscard]]inline double l51()const noexcept{double l541=0;for(std
::size_t ld=0;ld<l193.l436;++ld)l541+=double(l174[ld]) *l174[ld]+
double(l229[ld]) *l229[ld]+double(l169[ld]) *l169[ld]+double(l206[ld]
) *l206[ld];return l541;}[[nodiscard]]inline double l806()const
noexcept{return l193.l689;}[[nodiscard]]inline double l770()const
noexcept{return l193.l367;}private:[[nodiscard]]inline static const lc
&l742(double l284)noexcept{std::size_t l539=0;double l545=std::abs(
l197[0].l427-l284);for(std::size_t ld=1;ld<l197.size();++ld){double
l130=std::abs(l197[ld].l427-l284);if(l130<l545){l545=l130;l539=ld;}}
return l197[l539];}inline static constexpr std::array<lc,237>l197={{
lc{21,27.3782,5.4850291759431087e-05,44100,1256.1791970538138,5,{{la{
-1.97561741f,0.975828409f},la{-1.97552645f,0.976193845f},la{-
1.97369206f,0.975448072f},la{-1.97081971f,0.974156499f},la{-
1.97242332f,0.978192449f},la{0.0f,0.0f}}}},lc{22,29.010100000000001,
5.3600800116909449e-05,44100,1175.7458552382921,5,{{la{-1.96372128f,
0.964050353f},la{-1.9717077f,0.972444832f},la{-1.96961272f,
0.971636713f},la{-1.96402633f,0.968087614f},la{-1.96275449f,
0.969608963f},la{0.0f,0.0f}}}},lc{23,30.7393,5.2581980993204519e-05,
44100,1150.0474452299304,6,{{la{-1.91192961f,0.913868785f},la{-
1.94920659f,0.950945914f},la{-1.91715014f,0.918866158f},la{-
1.61721599f,0.659784973f},la{-1.95656514f,0.960707247f},la{-
1.95035338f,0.959751129f}}}},lc{24,32.571599999999997,
5.17816694448717e-05,44100,1200.8506824441356,3,{{la{-1.92620552f,
0.928909123f},la{-1.90365779f,0.905978262f},la{-1.91570508f,
0.923792899f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{25,
34.513100000000001,5.1190427994123997e-05,44100,1144.0504372404769,3,
{{la{-1.87815917f,0.882206082f},la{-1.87360358f,0.878423035f},la{-
1.89597535f,0.903852522f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{26,36.570300000000003,5.0801325194743499e-05,44100,
1048.4193343558957,4,{{la{-1.94385779f,0.944940805f},la{-1.93963969f,
0.943285704f},la{-1.92240548f,0.932544053f},la{-1.87991309f,
0.900426209f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{27,38.7502,
5.0609819378936862e-05,44100,917.227027224621,6,{{la{-1.93205214f,
0.955200076f},la{-1.93604088f,0.951059163f},la{-1.94753456f,
0.956511199f},la{-1.95472836f,0.959277093f},la{-1.95814013f,
0.960000515f},la{-1.96135461f,0.961906552f}}}},lc{28,
41.060000000000002,5.0613704795535706e-05,44100,865.67404586467183,6,
{{la{-1.93850732f,0.955186605f},la{-1.94436634f,0.953947306f},la{-
1.92963254f,0.956615448f},la{-1.95022285f,0.955191553f},la{-
1.95876062f,0.95939666f},la{-1.95195532f,0.95412153f}}}},lc{29,
43.5075,5.081302372431709e-05,44100,875.56928074648386,4,{{la{-
1.90810955f,0.910220563f},la{-1.92635751f,0.930483162f},la{-
1.91664648f,0.928137422f},la{-1.89224124f,0.917906106f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{30,46.1008,5.1210071831366423e-05,44100,
770.30917108001245,6,{{la{-1.94940615f,0.952197254f},la{-1.93999875f,
0.94663614f},la{-1.9564203f,0.957157433f},la{-1.93214333f,
0.951343656f},la{-1.93162382f,0.943187416f},la{-1.92634833f,
0.956761956f}}}},lc{31,48.824800000000003,5.180354900715938e-05,44100
,780.05071239915003,5,{{la{-1.84576738f,0.867045403f},la{-1.83324111f
,0.856321812f},la{-1.89549732f,0.903491855f},la{-1.91561091f,
0.9180215f},la{-1.80722249f,0.85793072f},la{0.0f,0.0f}}}},lc{31,
48.848799999999997,5.1809506247468992e-05,44100,780.05009007276419,5,
{{la{-1.80101502f,0.85103637f},la{-1.80756569f,0.834348261f},la{-
1.85942256f,0.879543066f},la{-1.91567767f,0.918087423f},la{-
1.8964113f,0.904385269f},la{0.0f,0.0f}}}},lc{32,51.735100000000003,
5.2610545216462892e-05,44100,770.05006467739531,3,{{la{-1.83070004f,
0.837868094f},la{-1.83421659f,0.860558987f},la{-1.85556376f,
0.865851521f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{32,
51.7605,5.2618306901262375e-05,44100,769.6446342298284,3,{{la{-
1.8307873f,0.83794558f},la{-1.85589695f,0.866165459f},la{-1.83452427f
,0.860902846f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{33,
54.818899999999999,5.3636409104809483e-05,44100,748.95618998032705,2,
{{la{-1.81941128f,0.837222934f},la{-1.79409838f,0.804697275f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{33,
54.845799999999997,5.3646069605784835e-05,44100,748.58648791269366,2,
{{la{-1.81932974f,0.837156832f},la{-1.79400969f,0.804617643f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{34,
58.084000000000003,5.4892370405986803e-05,44100,646.05007144786737,5,
{{la{-1.89789987f,0.901324987f},la{-1.85185313f,0.861660242f},la{-
1.87971151f,0.893190444f},la{-1.87204635f,0.899526238f},la{-
1.86635423f,0.912846982f},la{0.0f,0.0f}}}},lc{34,58.1126,
5.4904088538290463e-05,44100,685.7487941932003,3,{{la{-1.84933233f,
0.885742486f},la{-1.87260377f,0.886826158f},la{-1.88976836f,
0.89396739f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{35,
61.543700000000001,5.6394372616627355e-05,44100,662.67040836747901,2,
{{la{-1.82757664f,0.850194752f},la{-1.85171771f,0.858706355f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{35,
61.573900000000002,5.6408210716939515e-05,44100,662.34615361778719,2,
{{la{-1.82748187f,0.850121617f},la{-1.85164189f,0.858637571f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{36,
65.209400000000002,5.8160810311740388e-05,44100,596.04981384202756,4,
{{la{-1.79989874f,0.871051073f},la{-1.84324586f,0.873099267f},la{-
1.85206687f,0.865746737f},la{-1.87699485f,0.881926f},la{0.0f,0.0f},la
{0.0f,0.0f}}}},lc{36,65.241399999999999,5.8176980625087649e-05,44100,
586.26597416569825,4,{{la{-1.84031534f,0.890000165f},la{-1.86913574f,
0.894349515f},la{-1.89406621f,0.897736728f},la{-1.88252389f,
0.892955363f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{37,69.093400000000003
,6.0213806022023323e-05,44100,590.64592913210822,2,{{la{-1.8506279f,
0.858371317f},la{-1.81322587f,0.842635274f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{37,69.127300000000005,
6.0232509095508895e-05,44100,590.35650285055772,2,{{la{-1.85055399f,
0.858304918f},la{-1.81312394f,0.842561543f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{38,73.208799999999997,
6.2579636542420321e-05,44100,540.96645325977465,3,{{la{-1.77542222f,
0.83026284f},la{-1.83614564f,0.857392848f},la{-1.86275458f,0.8690781f
},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{38,
73.244699999999995,6.2601105219795922e-05,44100,540.70283446011558,3,
{{la{-1.86268044f,0.869010687f},la{-1.77522743f,0.830127835f},la{-
1.83604038f,0.857310891f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{38,73.280699999999996,6.2622647886681856e-05,44100,
543.94986947803545,3,{{la{-1.79897559f,0.826723218f},la{-1.84655499f,
0.854426205f},la{-1.69159698f,0.753333986f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{39,77.569199999999995,
6.5289084468286726e-05,44100,464.55815692925933,5,{{la{-1.86808908f,
0.872439206f},la{-1.89379632f,0.900982618f},la{-1.88958466f,
0.907149732f},la{-1.87972772f,0.915879428f},la{-1.85997808f,
0.923252761f},la{0.0f,0.0f}}}},lc{39,77.607399999999998,
6.5313716505639545e-05,44100,480.05028445125447,5,{{la{-1.8432864f,
0.849597394f},la{-1.81370378f,0.848389804f},la{-1.86131322f,
0.873798668f},la{-1.74893141f,0.803318262f},la{-1.74191105f,
0.825929582f},la{0.0f,0.0f}}}},lc{39,77.645499999999998,
6.5338299358686647e-05,44100,478.05031913935181,5,{{la{-1.83910131f,
0.845573485f},la{-1.66116881f,0.732892215f},la{-1.86410344f,
0.875801086f},la{-1.79301178f,0.856918335f},la{-1.83166659f,
0.862691164f},la{0.0f,0.0f}}}},lc{40,82.189499999999995,
6.8378564983695274e-05,44100,449.94992592826696,6,{{la{-1.83181882f,
0.839642048f},la{-1.82127488f,0.838442802f},la{-1.74467003f,
0.778825104f},la{-1.77838814f,0.831142604f},la{-1.68879676f,
0.768835366f},la{-1.71406794f,0.879858971f}}}},lc{40,
82.229900000000001,6.8406550346842109e-05,44100,477.94996872428879,4,
{{la{-1.76762152f,0.781171978f},la{-1.8027842f,0.822329521f},la{-
1.77790082f,0.830524087f},la{0.212062538f,0.161472589f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{40,82.270200000000003,6.8434483060773974e-05,
44100,440.04997722859707,6,{{la{-1.82780993f,0.835303366f},la{-
1.85174644f,0.862174392f},la{-1.8434484f,0.866375983f},la{-
1.72853351f,0.810030282f},la{-1.81772447f,0.865983486f},la{-
1.70036685f,0.808718562f}}}},lc{41,87.084900000000005,
7.1890266102344605e-05,44100,454.05018390197006,3,{{la{-1.76347291f,
0.777459264f},la{-1.66741228f,0.742573142f},la{-1.77968466f,
0.807876945f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{41,
87.127700000000004,7.1922034626807979e-05,44100,460.04990662643326,5,
{{la{-0.953317821f,0.432814002f},la{-0.733706534f,0.522687376f},la{-
1.0167892f,0.568364084f},la{-1.76161194f,0.79612869f},la{-1.74281156f
,0.759355545f},la{0.0f,0.0f}}}},lc{41,87.170500000000004,
7.1953821532202286e-05,44100,451.02797456738773,3,{{la{-1.77740252f,
0.789789855f},la{-1.8012805f,0.825034499f},la{-1.73827446f,
0.80238843f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{42,
92.271900000000002,7.5873719965610862e-05,44100,402.04987305491164,5,
{{la{-1.8123194f,0.82188946f},la{-1.80629373f,0.825213552f},la{-
1.76036572f,0.795512676f},la{-1.76910222f,0.830251336f},la{-
1.70921075f,0.827708483f},la{0.0f,0.0f}}}},lc{42,92.3172,
7.5909690237425587e-05,44100,432.05002328747639,3,{{la{-1.63367343f,
0.667850912f},la{-1.68208885f,0.727181792f},la{-1.50401282f,
0.565943062f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{42,
92.362499999999997,7.594568091914677e-05,44100,425.95006172308916,3,{
{la{-1.7792064f,0.803349495f},la{-1.74647772f,0.762872577f},la{-
1.72785735f,0.794150054f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{43,97.767799999999994,8.0386576743260001e-05,44100,
414.0498857291692,2,{{la{-1.76342881f,0.780166566f},la{-1.71989155f,
0.764460504f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{43,97.815799999999996,8.0427313370761084e-05,44100,
406.04996444490632,3,{{la{-1.76548958f,0.781960964f},la{-1.71794689f,
0.763104439f},la{-1.37856054f,0.507372677f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{43,97.863900000000001,
8.0468157850107554e-05,44100,413.33409135038357,2,{{la{-1.7280072f,
0.772561789f},la{-1.77124584f,0.787077367f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{44,103.59099999999999,
8.5496148564465589e-05,44100,382.05001468336422,4,{{la{-1.70036209f,
0.744959593f},la{-1.69032812f,0.715087414f},la{-1.11455858f,
0.315571189f},la{-0.195824429f,0.0179404821f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{44,103.642,8.5542394832813175e-05,44100,
370.05000543534283,4,{{la{-1.71443594f,0.735205114f},la{-1.72169161f,
0.752072036f},la{-1.67906368f,0.741820395f},la{-1.53495824f,
0.660775542f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{44,103.693,
8.5588667164501184e-05,44100,371.94996486118373,4,{{la{-1.76678813f,
0.781279266f},la{-1.77562404f,0.80175823f},la{-1.73201358f,
0.798626423f},la{-1.5806005f,0.793981969f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lc{45,109.761,9.1281010195021513e-05,44100,313.95071330612313,6,
{{la{-1.85649168f,0.883714736f},la{-1.87194347f,0.88369137f},la{-
1.87455583f,0.879267097f},la{-1.83531916f,0.887555599f},la{-
1.79912043f,0.887820125f},la{-1.76900101f,0.908342361f}}}},lc{45,
109.815,9.133333879731857e-05,44100,327.94996993667502,6,{{la{-
1.81718612f,0.826586545f},la{-1.81402588f,0.833453596f},la{-
1.77758944f,0.825789869f},la{-1.59512293f,0.675846279f},la{-
1.72433889f,0.820507169f},la{-1.78278208f,0.9509148f}}}},lc{45,
109.869,9.1385697045794626e-05,44100,367.31881904436028,3,{{la{
1.99399447f,0.994009018f},la{-1.70000136f,0.752823293f},la{-
1.74489331f,0.764012933f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{46,116.295,9.7829378225853023e-05,44100,346.05010452595053,2,{{
la{-1.7307018f,0.751758754f},la{-1.68997049f,0.744265735f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{46,116.352,
9.7888438760023548e-05,44100,346.04999119460979,2,{{la{-1.68581378f,
0.739552438f},la{-1.72177672f,0.743926644f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{46,116.41,
9.7948570054725719e-05,44100,338.04995763831658,3,{{la{-1.60123682f,
0.64996171f},la{-1.6085484f,0.678918898f},la{-1.58199167f,
0.625674367f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{47,
123.218,0.00010525133288627102,44100,326.04999048149699,2,{{la{-
1.71862078f,0.741641462f},la{-1.66871953f,0.726938426f},la{0.0f,0.0f}
,la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{47,123.279,
0.0001053189775995625,44100,326.04999597635322,2,{{la{-1.66418004f,
0.722141862f},la{-1.71114182f,0.735150039f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{47,123.339,
0.00010538555204720589,44100,318.04998063235035,3,{{la{-1.56920969f,
0.622857988f},la{-1.5869875f,0.664435506f},la{-1.60620368f,
0.648194611f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{48,
130.553,0.00011367230448717747,44100,304.04997675092665,5,{{la{-
0.132796273f,0.0135793602f},la{1.01230764f,0.827987194f},la{
0.484422207f,0.957755923f},la{-1.66607058f,0.726877987f},la{-
1.71855068f,0.741774797f},la{0.0f,0.0f}}}},lc{48,130.61699999999999,
0.00011374835315072723,44100,306.27143131786733,5,{{la{1.9417578f,
0.986379266f},la{1.93587184f,0.984759152f},la{1.94426513f,
0.987549067f},la{-1.71844828f,0.741689205f},la{-1.66593945f,
0.726791263f},la{0.0f,0.0f}}}},lc{48,130.68199999999999,
0.00011382563612761529,44100,268.05004310068853,6,{{la{-1.69787848f,
0.722729921f},la{-1.69267297f,0.721511245f},la{-1.66998756f,
0.723823667f},la{-1.6159972f,0.690210044f},la{-1.59889317f,
0.721311569f},la{-1.53093338f,0.675382137f}}}},lc{49,
138.32499999999999,0.00012324025444501751,44100,288.33200247673579,2,
{{la{-1.71159673f,0.73588872f},la{-1.65555716f,0.720007896f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{49,138.393,
0.00012332696434276942,44100,288.19258974155349,2,{{la{-1.65534329f,
0.719861567f},la{-1.7114346f,0.735753238f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{49,138.46100000000001,
0.00012341372685106358,44100,288.05380805269107,2,{{la{-1.71126699f,
0.735613167f},la{-1.65512133f,0.719709218f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{50,146.559,
0.00013412721454709035,44100,255.95001268026917,4,{{la{-1.75094259f,
0.768893898f},la{-1.71582925f,0.756156802f},la{-1.66730738f,
0.757412493f},la{-1.53113687f,0.781898737f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{50,146.631,0.00013422590355159281,44100,264.0499473076913
,3,{{la{-1.71829176f,0.741890252f},la{-1.63644683f,0.705939114f},la{-
1.37588906f,0.524810493f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{50,146.703,0.00013432465389000837,44100,270.98122284513789,3,{{
la{1.99399996f,0.994009018f},la{-1.64444637f,0.71491915f},la{-
1.71606958f,0.740185022f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{51,155.28399999999999,0.00014653930235788255,44100,
255.83484194440592,2,{{la{-1.69787824f,0.724877119f},la{-1.6341691f,
0.709071636f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{51,155.36000000000001,0.00014665148916490561,44100,
255.69328190622721,2,{{la{-1.69782519f,0.724830747f},la{-1.63405752f,
0.708950102f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{51,155.43600000000001,0.00014676374729231839,44100,
255.55484765906522,2,{{la{-1.69774973f,0.724766076f},la{-1.63392627f,
0.708822489f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{52,164.52799999999999,0.00016071595204200263,44100,
242.05001160005759,3,{{la{1.31991482f,0.769295216f},la{-1.56788659f,
0.640675426f},la{-1.57678771f,0.62387836f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f}}}},lc{52,164.60900000000001,0.00016084498160109391,
44100,236.04998975497838,3,{{la{-1.46472979f,0.537552297f},la{-
1.52328551f,0.605941415f},la{-1.31221569f,0.43049857f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{52,164.68899999999999,
0.00016097250147062826,44100,236.04999828674812,3,{{la{-1.45680451f,
0.53248477f},la{-1.51357925f,0.597271264f},la{-1.31951165f,
0.435287148f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{53,
174.322,0.00017694240546449472,44100,218.34660497916312,3,{{la{-
1.69234657f,0.719554365f},la{-1.62177324f,0.692202866f},la{-
1.45303488f,0.603346109f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{53,174.40799999999999,0.00017709056309902615,44100,
218.23961366866678,3,{{la{-1.69221604f,0.719446659f},la{-1.6216172f,
0.692102253f},la{-1.45282817f,0.603286564f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{53,174.49299999999999,0.0001772370966770329
,44100,225.95000991137317,2,{{la{-1.67363012f,0.704431653f},la{-
1.60609734f,0.688234985f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{54,184.69900000000001,0.0001951909802984074,44100
,211.94996959014557,2,{{la{-1.6678158f,0.699521005f},la{-1.60801435f,
0.69054091f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{54,184.78999999999999,0.00019535414519352071,44100,
214.05001384122934,2,{{la{-1.54874587f,0.620200098f},la{-1.48762035f,
0.553256631f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{54,184.881,0.00019551736416016424,44100,198.0499959747963,5,{{
la{-1.45533264f,0.531271398f},la{-1.35787296f,0.465750307f},la{-
1.33543551f,0.469496191f},la{-1.44615507f,0.560262322f},la{-
0.336192518f,0.118023083f},la{0.0f,0.0f}}}},lc{55,195.69399999999999,
0.00021529375640180309,44100,190.05000832377476,3,{{la{-1.69136131f,
0.717938304f},la{-1.66310835f,0.72079885f},la{-1.55533886f,
0.69666791f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{55,
195.78999999999999,0.00021547270321706786,44100,180.04997545983952,4,
{{la{-1.73005056f,0.750487506f},la{-1.70790994f,0.752524137f},la{-
1.60336542f,0.716419399f},la{-1.51751494f,0.664682388f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{55,195.886,0.00021565170887217493,44100,
190.04998011160353,3,{{la{-1.68863273f,0.715645313f},la{-1.65999293f,
0.71844393f},la{-1.54738224f,0.691302061f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f}}}},lc{56,207.34399999999999,0.00023743618266172512,
44100,187.47626947145369,2,{{la{-1.65138721f,0.686474442f},la{-
1.57780647f,0.668185413f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{56,207.44499999999999,0.00023763187926342927,
44100,187.37568941541932,2,{{la{-1.57767451f,0.66806972f},la{-
1.65129006f,0.686394453f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{56,207.547,0.00023782957807628698,44100,
187.27588565138433,2,{{la{-1.65117526f,0.686300457f},la{-1.57752264f,
0.667944729f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{57,219.68700000000001,0.00026181988958400476,44100,
150.0500243558038,5,{{la{-1.70141172f,0.725952089f},la{-1.66816759f,
0.710098088f},la{-1.6581291f,0.731994748f},la{-1.58233917f,
0.738512754f},la{-1.41224015f,0.617388666f},la{0.0f,0.0f}}}},lc{57,
219.79400000000001,0.00026203536410030967,44100,158.05000870997708,5,
{{la{-1.37575364f,0.475228071f},la{-1.47461915f,0.551837325f},la{-
1.40214801f,0.506910861f},la{-1.28594911f,0.464166224f},la{-
1.35327566f,0.54272449f},la{0.0f,0.0f}}}},lc{57,219.90199999999999,
0.00026225292323324228,44100,135.74440974360624,5,{{la{-1.82257116f,
0.831843495f},la{-1.81065083f,0.832508028f},la{-1.77790439f,
0.825349808f},la{-1.76070583f,0.840787888f},la{-1.75516939f,
0.891357183f},la{0.0f,0.0f}}}},lc{58,232.768,0.00028867597150438904,
44100,133.94998287161803,5,{{la{-1.69730031f,0.721301079f},la{-
1.70088959f,0.735505879f},la{-1.60505247f,0.7181831f},la{-1.65851092f
,0.728623331f},la{-1.58519483f,0.717450261f},la{0.0f,0.0f}}}},lc{58,
232.88200000000001,0.00028891453989226564,44100,166.49552427208943,2,
{{la{-1.51485682f,0.573697805f},la{-1.55158436f,0.638001084f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{58,
232.99700000000001,0.00028915527950406771,44100,135.19968749557609,4,
{{la{-1.7550329f,0.770863712f},la{-1.77666295f,0.801575243f},la{-
1.76776373f,0.82268852f},la{-1.75065327f,0.855284274f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{59,246.62799999999999,0.00031824458244894441,
44100,158.04999568606524,2,{{la{-1.48859429f,0.558963716f},la{-
1.41516173f,0.529622376f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{59,246.749,0.00031850768825534171,44100,
165.76113005205127,1,{{la{-1.53636789f,0.598389506f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{59,
246.87100000000001,0.00031877305486527742,44100,165.66570789544255,1,
{{la{-1.53665197f,0.59860605f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{60,261.31400000000002,
0.00035079667538140852,44100,156.74746405626604,1,{{la{-1.47983623f,
0.553979218f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{60,261.44200000000001,0.0003510858324722655,
44100,156.67011966663554,1,{{la{-1.47962892f,0.553829432f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{
60,261.57100000000003,0.00035137734297986819,44100,156.59270234038718
,1,{{la{-1.47939885f,0.553662777f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{61,276.87400000000002,
0.00038662562228112331,44100,115.94998641182315,4,{{la{-1.69431853f,
0.719602704f},la{-1.68768716f,0.7322855f},la{-1.63288796f,
0.728120208f},la{-1.59083438f,0.741442859f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{61,277.00999999999999,0.00038694476329759826,44100,
116.05005513457725,4,{{la{-1.66865015f,0.697217286f},la{-1.68972647f,
0.73042208f},la{-1.66343975f,0.748819828f},la{-1.61079967f,
0.772055924f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{61,277.14600000000002
,0.00038726400718821016,44100,137.9499962089794,3,{{la{-1.40533721f,
0.50103724f},la{-1.39200425f,0.501331091f},la{-0.576268554f,
0.364073306f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{62,
293.36099999999999,0.00042605830918566789,44100,125.95000409686831,3,
{{la{-1.43983138f,0.518752396f},la{-1.45462942f,0.563627541f},la{-
1.06789839f,0.369027436f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{62,293.505,0.00042640927717448165,44100,125.95000093629491,3,{{
la{-1.3932029f,0.485260218f},la{-1.05016685f,0.29702577f},la{-
1.41153252f,0.536202371f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{62,293.649,0.0004267603578166185,44100,125.95000444198391,3,{{la
{-1.39345741f,0.485467881f},la{-1.03243852f,0.286306649f},la{-
1.41258848f,0.53706193f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}}
,lc{63,310.82900000000001,0.00046944827536626285,44100,
91.950080171151683,4,{{la{-1.81460905f,0.825380206f},la{-1.80463636f,
0.83428359f},la{-1.77349973f,0.846067429f},la{-1.73728573f,
0.875533581f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{63,310.98200000000003
,0.00046983553170952373,44100,119.90740810030783,2,{{la{-1.63480234f,
0.674715459f},la{-1.55352521f,0.671634018f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{63,311.13400000000001,
0.00047022037995252336,44100,98.050005780673672,4,{{la{-1.75590611f,
0.773005605f},la{-1.75519907f,0.790238082f},la{-1.74280059f,
0.824039221f},la{-1.77896094f,0.938246846f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{64,329.33699999999999,0.00051718777559689097,44100,
122.55913384704931,1,{{la{-1.47278535f,0.551728725f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{64,
329.49900000000002,0.00051761353584727128,44100,122.49892949011617,1,
{{la{-1.47255266f,0.551564276f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{64,329.661,
0.00051803943206598496,44100,85.949921029226502,4,{{la{-1.77746117f,
0.792335093f},la{-1.74501705f,0.782088578f},la{-1.72204816f,
0.794623911f},la{-1.7111454f,0.843837678f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lc{65,348.94799999999998,0.00056970865048497213,44100,
106.04998122524377,2,{{la{-1.56499076f,0.617891788f},la{-1.5072993f,
0.632070899f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{65,349.11900000000003,0.00057017523572402582,44100,
106.04999165819454,2,{{la{-1.56249952f,0.615925074f},la{-1.50446403f,
0.629847646f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{65,349.291,0.00057064469874168445,44100,106.04999053526601,2,{
{la{-1.56028557f,0.614190698f},la{-1.50156343f,0.627655268f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{66,369.726,
0.00062747715154224781,44100,108.34777269365895,1,{{la{-1.46289909f,
0.545959592f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{66,369.90800000000002,0.00062799265582099805,
44100,108.29532729005696,1,{{la{-1.46261728f,0.545761585f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{
66,370.08999999999997,0.00062850832366120868,44100,108.24308999356471
,1,{{la{-1.46232843f,0.545558691f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{67,391.74200000000002,
0.00069101348515588257,44100,102.23536023794553,1,{{la{-1.42246914f,
0.515941143f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{67,391.93400000000003,0.00069157794011855437,
44100,96.049988599892657,2,{{la{-1.51087976f,0.579526603f},la{-
1.28563511f,0.515001059f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{67,392.12700000000001,0.0006921455140092512,44100
,102.1373268153034,1,{{la{-1.4218297f,0.515500247f},la{0.0f,0.0f},la{
0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{68,
415.06799999999998,0.00076087878767108698,44100,90.049996831854457,2,
{{la{-1.40132785f,0.498443961f},la{-1.23531437f,0.42922911f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{68,
415.27199999999999,0.00076150118321163189,44100,90.050003554931877,2,
{{la{-1.40425193f,0.501107633f},la{-1.22184455f,0.419836104f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{68,415.476,
0.00076212377448834864,44100,52.049881959969596,5,{{la{-1.85293555f,
0.860983431f},la{-1.82006705f,0.850245237f},la{-1.77923524f,
0.847789526f},la{-1.73648858f,0.866294324f},la{-1.64221585f,
0.85742408f},la{0.0f,0.0f}}}},lc{69,439.78399999999999,
0.00083769948617548108,44100,64.597930280719012,4,{{la{-1.719594f,
0.743334532f},la{-1.67567897f,0.738277435f},la{-1.60195076f,
0.73939234f},la{-1.513659f,0.776049137f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{69,440,0.00083838330327871036,44100,75.291577639267373,3,{{la{-
1.61215842f,0.655785203f},la{-1.54449737f,0.652785778f},la{-
1.37891185f,0.646372795f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{69,440.21600000000001,0.00083906733450199965,44100,
73.949998325793956,4,{{la{-1.33123612f,0.448966861f},la{-1.26290882f,
0.411909491f},la{-1.27300584f,0.458449602f},la{-1.10220194f,
0.456748247f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{70,465.99400000000003
,0.00092222624045597843,44100,47.950046389724406,4,{{la{-1.85189188f,
0.860898197f},la{-1.80926919f,0.848365605f},la{-1.74589622f,
0.83617872f},la{-1.70455754f,0.86327368f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{70,466.22300000000001,0.0009229784278811439,44100,
47.949863835211467,4,{{la{-1.85002208f,0.859169662f},la{-1.80738449f,
0.846780717f},la{-1.74071598f,0.831943929f},la{-1.70029402f,
0.857420325f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{70,466.452,
0.00092373085009990236,44100,84.815021398302832,1,{{la{-1.38820076f,
0.493289798f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{71,493.767,0.0010151496048357825,44100,
62.050004707954301,4,{{la{-1.43114603f,0.513247907f},la{-1.48725891f,
0.575444043f},la{-1.44056785f,0.617269754f},la{-1.24975526f,
0.684027076f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{71,494.00999999999999
,0.0010159776288841188,44100,69.94999169200679,3,{{la{-1.50191987f,
0.571695924f},la{-1.40049541f,0.562435985f},la{-0.951542079f,
0.527846336f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{71,
494.25200000000001,0.0010168025017225687,44100,62.049994045836215,4,{
{la{-1.41946125f,0.504389226f},la{-1.48749614f,0.575066149f},la{-
1.4392271f,0.615870655f},la{-1.24579155f,0.681757569f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{72,523.19500000000005,0.0011172856755547671,44100
,75.165670375930716,1,{{la{-1.36051321f,0.475399137f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{72,
523.452,0.0011181940327211753,44100,75.123726949064789,1,{{la{-
1.36060619f,0.475469589f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{72,523.70899999999995,
0.0011191026712183991,44100,47.949987968326525,4,{{la{-1.7237488f,
0.747173369f},la{-1.67088199f,0.735702515f},la{-1.57634509f,
0.712215304f},la{-1.55141759f,0.771344721f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{73,554.37599999999998,0.0012295302954572266,44100,
64.049998116280591,2,{{la{-1.36679804f,0.471606493f},la{-1.31398308f,
0.498729795f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{73,554.649,0.0012305310187171968,44100,62.050009773483652,2,{{
la{-1.50772536f,0.575759292f},la{-1.44359136f,0.60292995f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{73,
554.92100000000005,0.0012315283841869743,44100,61.305060527901894,2,{
{la{-1.53759718f,0.598483503f},la{-1.4849304f,0.632818639f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{74,
587.41600000000005,0.0013528730659360278,44100,60.050000407357416,2,{
{la{-1.38644004f,0.487791806f},la{-1.28536797f,0.491276681f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{74,
587.70500000000004,0.0013539716118675233,44100,60.050003071529595,2,{
{la{-1.38402998f,0.486105055f},la{-1.28231752f,0.489054441f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{74,
587.99400000000003,0.0013550704960375276,44100,64.050002696336762,2,{
{la{-1.2861768f,0.424825698f},la{-0.343989342f,0.0521134362f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{75,
622.42600000000004,0.0014883936924585063,44100,56.049997189850302,2,{
{la{-1.35457587f,0.464643419f},la{-1.27596891f,0.479821116f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{75,
622.73199999999997,0.0014895997064160145,44100,56.049996637251709,2,{
{la{-1.3577528f,0.467131346f},la{-1.2708739f,0.477653623f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{75,
623.03700000000003,0.0014908021470083413,44100,56.049996216979928,2,{
{la{-1.35758114f,0.467141688f},la{-1.26698744f,0.475420266f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{76,
659.52099999999996,0.0016372643994772614,44100,24.146808107406216,3,{
{la{-1.91826022f,0.924577594f},la{-1.93160176f,0.968326449f},la{-
1.82575846f,0.958159924f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{76,659.84500000000003,0.0016385882132663129,44100,
32.049921268916975,3,{{la{-1.93342769f,0.942627132f},la{-1.83664703f,
0.909056425f},la{-1.76286805f,0.925528824f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{76,660.16899999999998,0.0016399124310108822
,44100,24.116464758516159,3,{{la{-1.9182862f,0.924626648f},la{-
1.93120396f,0.968017042f},la{-1.82519495f,0.957709849f},la{0.0f,0.0f}
,la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{77,698.82799999999997,
0.0018007898642832853,44100,48.249849873447289,2,{{la{-1.44530022f,
0.532936752f},la{-1.30938339f,0.532243192f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{77,699.17100000000005,
0.0018022425396217285,44100,48.22674104578897,2,{{la{-1.44504714f,
0.532761931f},la{-1.30903041f,0.532088757f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{77,699.51499999999999,
0.0018036998933031727,44100,38.04999180214341,4,{{la{-1.46186352f,
0.54294306f},la{-1.2284348f,0.417786479f},la{-1.17453015f,
0.474301785f},la{-1.21694064f,0.475296915f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lc{78,740.47699999999998,0.0019803801748292742,44100,
47.523774393243798,2,{{la{-1.09188616f,0.298132569f},la{-1.0870961f,
0.338604301f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{78,740.84100000000001,0.0019819779269423863,44100,
47.483135028623437,2,{{la{-1.09257054f,0.298502475f},la{-1.08799922f,
0.339093655f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{78,741.20500000000004,0.0019835761632297278,44100,
47.443837174678947,2,{{la{-1.09317505f,0.298829257f},la{-1.08881748f,
0.339541167f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{79,784.60799999999995,0.0021775887830562946,44100,
48.547499828826787,1,{{la{-1.27058542f,0.421279997f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{79,
784.99400000000003,0.0021793445621545167,44100,48.523575329641297,1,{
{la{-1.27028108f,0.421096712f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{79,785.37900000000002,
0.0021810963210037405,44100,48.499830313935945,1,{{la{-1.2699697f,
0.420909226f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{80,831.37,0.0023941198237539172,44100,
37.950001974274308,2,{{la{-1.47181594f,0.552935421f},la{-1.38261032f,
0.586579382f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{80,831.77800000000002,0.0023960427315172735,44100,
39.949997098975885,2,{{la{-1.35135889f,0.468876839f},la{-1.20124602f,
0.465730965f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{80,832.18700000000001,0.0023979709317304625,44100,
45.863072277814581,1,{{la{-1.22391295f,0.393599957f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{81,
880.91800000000001,0.002631825353009612,44100,37.9499992637883,2,{{la
{-1.23915434f,0.394399941f},la{-1.11627996f,0.395525128f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{81,881.351,
0.0026339395379290798,44100,37.94999726257965,2,{{la{-1.23644924f,
0.392714918f},la{-1.11370862f,0.393554121f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{81,881.78399999999999,
0.0026360543561619763,44100,28.855336845982407,3,{{la{-1.67851853f,
0.715789676f},la{-1.56876552f,0.716429114f},la{-1.34250402f,
0.737495959f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{82,
933.56399999999996,0.0028934788772695488,44100,25.950016862324823,2,{
{la{-1.81193781f,0.835402727f},la{-1.68513715f,0.834208548f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{82,
934.02300000000002,0.0028958005795701332,44100,25.052877472737215,3,{
{la{-1.69068253f,0.726088643f},la{-1.5674473f,0.709040523f},la{-
1.3994149f,0.731583238f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}}
,lc{82,934.48099999999999,0.0028981179144138163,44100,
23.950012091818461,2,{{la{-1.84408128f,0.864561021f},la{-1.70568359f,
0.834620774f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{83,989.35599999999999,0.0031807169710454416,44100,
32.049998814936608,2,{{la{-1.41628051f,0.51657784f},la{-1.22288167f,
0.541632235f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{83,989.84199999999998,0.0031832632707381259,44100,
25.950000246200204,2,{{la{-1.60586166f,0.654705346f},la{-1.56754363f,
0.705698073f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{83,990.32799999999997,0.0031858103268383163,44100,
34.050002313610527,2,{{la{-1.20882189f,0.381606311f},la{-0.76430279f,
0.225313246f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{84,1048.48,0.003495987417455864,44100,31.950001267965064,2,{{
la{-0.999297798f,0.253464848f},la{-0.966494262f,0.296039879f},la{0.0f
,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{84,1049,
0.0034988090267594118,44100,31.94999983245367,2,{{la{-0.988245428f,
0.24735406f},la{-0.969338775f,0.295706987f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{84,1049.51,
0.0035015771931539356,44100,31.95000036343454,2,{{la{-0.975654721f,
0.2403014f},la{-0.973600626f,0.296193272f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{85,1111.1400000000001,
0.0038419949048638536,44100,29.232737399599809,2,{{la{-1.09106624f,
0.301896185f},la{-0.977892458f,0.324562669f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{85,1111.6900000000001,
0.0038450794309375725,44100,29.199636666280007,2,{{la{-1.09417939f,
0.303803355f},la{-0.978356361f,0.32524085f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{85,1112.23,
0.0038481075901828548,44100,29.168747692132186,2,{{la{-1.09693968f,
0.305498838f},la{-0.978756547f,0.32585451f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{86,1177.55,
0.0042119619561208719,44100,29.949999386057133,2,{{la{0.384083152f,
0.0433288142f},la{-1.14902902f,0.357350677f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{86,1178.1199999999999,
0.0042151127347369224,44100,29.949999810849739,2,{{la{0.429935843f,
0.0623645224f},la{-1.16155779f,0.365283489f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{86,1178.7,
0.0042183183075681406,44100,29.94999917052413,2,{{la{0.448067248f,
0.0643512383f},la{-1.16206348f,0.365652114f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{87,1247.9200000000001,
0.0045970393473528538,44100,29.093744880463838,1,{{la{-1.14893627f,
0.355150968f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{87,1248.53,0.0046003399631090302,44100,
29.073644006212714,1,{{la{-1.14948452f,0.355499178f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{87,
1249.1500000000001,0.004603693977777587,44100,15.950010446318959,3,{{
la{-1.81487226f,0.847692072f},la{-1.56732917f,0.77372241f},la{-
1.32511139f,0.831984341f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{88,1322.5,0.0049951145405383321,44100,23.063326449313003,2,{{la{
-1.26668882f,0.420045257f},la{-0.943880856f,0.391119719f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{88,
1323.1500000000001,0.0049985326872045224,44100,23.046738439728571,2,{
{la{-1.266698f,0.420055687f},la{-0.94378227f,0.391024172f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{88,1323.8,
0.0050019499015450716,44100,23.030372023025556,2,{{la{-1.26669502f,
0.420058727f},la{-0.943671107f,0.390927941f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{89,1401.53,
0.0054035704786987411,44100,22.04999991439503,2,{{la{-1.1086365f,
0.320119411f},la{-0.814210057f,0.284173995f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{89,1402.22,
0.0054070707146882764,44100,22.049999373269927,2,{{la{-1.10473061f,
0.317801952f},la{-0.812058091f,0.281852752f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{89,1402.9100000000001,
0.0054105697677488025,44100,22.05000016180983,2,{{la{-1.10295749f,
0.316933215f},la{-0.808148801f,0.279221237f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{90,1485.29,
0.0058195612069360638,44100,20.24240418164247,2,{{la{-1.16862035f,
0.360077918f},la{-0.817855597f,0.310178459f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{90,1486.02,
0.0058231055893683322,44100,20.23369138965937,2,{{la{-1.16814804f,
0.35981071f},la{-0.817106307f,0.309898198f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{90,1486.75,
0.0058266485350518971,44100,20.225020654207832,2,{{la{-1.16767251f,
0.359541744f},la{-0.816351652f,0.309615582f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{91,1574.0599999999999,
0.0062398203557380589,44100,22.534597652165711,1,{{la{-1.03914273f,
0.294344574f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{91,1574.8299999999999,0.006243369216496159,44100
,18.050002277743431,2,{{la{-1.28692412f,0.43690753f},la{-0.944672942f
,0.422444552f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lc{91,1575.5999999999999,0.0062469163912102438,44100,
22.512142462096577,1,{{la{-1.03848088f,0.294029504f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{92,
1668.1199999999999,0.0066607089561924035,88200,36.128143257534461,2,{
{la{-1.31384766f,0.499646187f},la{-1.46118248f,0.541389525f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{92,
1668.9400000000001,0.0066642651179016771,88200,18.050073618784772,4,{
{la{-1.92558289f,0.936454177f},la{-1.90293932f,0.962630987f},la{-
1.73225188f,0.966341853f},la{-1.49239087f,0.916346431f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{92,1669.76,0.0066678193086544425,88200,
11.949999564697556,4,{{la{-1.92404354f,0.931724072f},la{-1.90811646f,
0.963221252f},la{-1.69624138f,0.951371133f},la{-1.76242805f,
0.901863456f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{93,1767.8099999999999
,0.0070785229884286109,88200,34.049999824083464,2,{{la{-1.39061058f,
0.489491194f},la{-1.25199795f,0.447369486f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{93,1768.6800000000001,
0.0070820399260056689,88200,34.050000419490786,2,{{la{-1.38737321f,
0.487151057f},la{-1.25071621f,0.445867151f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{93,1769.55,
0.0070855546206974353,88200,34.04999548008832,2,{{la{-1.38404977f,
0.484751672f},la{-1.24949622f,0.444392771f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{94,1874.5999999999999,
0.0074934700986100814,88200,32.050001959972057,2,{{la{-1.41132057f,
0.508179128f},la{-1.13332355f,0.38280952f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{94,1875.52,0.0074968984776709983,
88200,32.050004530495357,2,{{la{-1.40961003f,0.506987572f},la{-
1.13035929f,0.380449742f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lc{94,1876.45,0.007500361588523464,88200,
32.050000684932343,2,{{la{-1.40787315f,0.505779803f},la{-1.12734687f,
0.378048122f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{95,1987.8399999999999,0.0078968439886875907,88200,
34.050000773820308,2,{{la{-1.40740156f,0.507375896f},la{-1.05532122f,
0.78514415f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{95,1988.8199999999999,0.0079001722277890086,88200,
25.949996712467019,2,{{la{-1.68465018f,0.723492444f},la{-1.51265633f,
0.703982711f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{95,1989.8,0.0079034976998747491,88200,25.94998708296416,2,{{la
{-1.68620276f,0.724880397f},la{-1.51368713f,0.706086874f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{96,
2107.9200000000001,0.0082842751064562091,88200,26.049992003862975,2,{
{la{-1.52102256f,0.590794623f},la{-1.27824211f,0.518641233f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{96,2108.96,
0.0082874532511910421,88200,16.475560521303869,3,{{la{-1.79486322f,
0.816107869f},la{-1.72609103f,0.831490874f},la{-1.48112917f,
0.846359909f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{96,
2109.9899999999998,0.0082905978744504689,88200,18.050012260345365,3,{
{la{-1.70915866f,0.740216672f},la{-1.61312962f,0.73246479f},la{-
1.38371623f,0.743409336f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{97,2235.25,0.0086513811057734348,88200,20.049999422835274,3,{{la
{-1.33764315f,0.452865243f},la{-1.23265481f,0.409275472f},la{-
1.18375885f,0.488856226f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{97,2236.3499999999999,0.0086543623851430551,88200,
16.75332621829341,3,{{la{-1.70733809f,0.73970741f},la{-1.60602009f,
0.736931682f},la{-1.34576941f,0.740814209f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{97,2237.4499999999998,0.0086573404646191551
,88200,16.049968539097616,3,{{la{-1.85847199f,0.882319093f},la{-
1.67582357f,0.81665957f},la{-1.45060456f,0.888110697f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{98,2370.2800000000002,
0.0089938977410334853,88200,24.04999510940327,2,{{la{-1.44367349f,
0.536668599f},la{-1.05527306f,0.418754786f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{98,2371.4400000000001,
0.0089966395612102908,88200,24.050000944036569,2,{{la{-1.44260156f,
0.535899282f},la{-1.05257785f,0.417181641f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{98,2372.6100000000001,
0.0089994016309385607,88200,24.049998097681179,2,{{la{-1.44152284f,
0.535125732f},la{-1.04984868f,0.415592074f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{99,2513.46,
0.0093076518307531313,88200,27.303931738795988,1,{{la{-1.27929449f,
0.424628615f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lc{99,2514.6999999999998,0.0093101571409708352,
88200,27.286592230910919,1,{{la{-1.27938032f,0.424696863f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{
99,2515.9299999999998,0.0093126387443926931,88200,27.269534540694522,
1,{{la{-1.2794528f,0.424756318f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{100,2665.29,
0.009588770619505492,88200,16.049993650337324,3,{{la{-1.41189635f,
0.504743397f},la{-1.32147706f,0.527017891f},la{-0.918336689f,
0.459195286f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{100,
2666.5999999999999,0.0095909775822555152,88200,14.05000921304956,3,{{
la{-1.67238939f,0.714045942f},la{-1.54770339f,0.723699868f},la{-
1.16496634f,0.742832482f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lc{100,2667.9099999999999,0.0095931809174093739,88200,
14.050004699752449,3,{{la{-1.67080188f,0.712590873f},la{-1.54771423f,
0.723582923f},la{-1.16424525f,0.742861569f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{101,2826.29,0.009833681755363196,88200,
18.061293279039187,2,{{la{-1.56582272f,0.637099981f},la{-1.23706043f,
0.600258529f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{101,2827.6799999999998,0.0098355725817048413,88200,
15.95000356431677,3,{{la{-1.73297954f,0.780714512f},la{-1.39156926f,
0.73246944f},la{-0.732175231f,0.744814754f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{101,2829.0700000000002,
0.0098374597092516624,88200,18.062247537041763,2,{{la{-1.56300688f,
0.634850442f},la{-1.2336477f,0.597753167f},la{0.0f,0.0f},la{0.0f,0.0f
},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{102,2997.02,0.010039223629270608,
88200,8.0500294257860858,3,{{la{-1.92696917f,0.968685925f},la{-
1.66416311f,0.856092572f},la{-1.33945107f,0.817759395f},la{0.0f,0.0f}
,la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{102,2998.4899999999998,
0.010040768085133125,88200,10.427679969706709,3,{{la{-1.67182195f,
0.716325402f},la{-1.5028441f,0.689757407f},la{-1.23464954f,
0.721291363f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{102,
2999.9699999999998,0.010042319309200817,88200,14.064003043708219,3,{{
la{-1.65408003f,0.711521387f},la{-1.32032216f,0.624098301f},la{-
0.867125869f,0.724614978f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{103,3178.0599999999999,0.010202684190796315,88200,
16.049997564486684,2,{{la{-1.45802009f,0.556949019f},la{-1.10012186f,
0.507797778f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{103,3179.6199999999999,0.010203867825481026,88200,
16.049994461635109,2,{{la{-1.45615852f,0.55553478f},la{-1.09845221f,
0.506341875f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{103,3181.1799999999998,0.010205047779361105,88200,
7.9500108120458037,3,{{la{-1.85704398f,0.889898777f},la{-1.75982821f,
0.966056645f},la{-1.28446043f,0.854913056f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{104,3370.04,0.010321893182107819,88200,
15.949998830535687,2,{{la{-1.30273044f,0.447959423f},la{-0.935120285f
,0.399429828f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lc{104,3371.6900000000001,0.010322696893368198,88200,
15.950000932357083,2,{{la{-1.30059123f,0.44649905f},la{-0.933342814f,
0.397908658f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lc{104,3373.3499999999999,0.010323501853965826,88200,
8.6223366353627995,3,{{la{-1.76920259f,0.808407784f},la{-1.62415028f,
0.857822061f},la{-1.04090142f,0.779321969f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lc{105,3573.6100000000001,0.010395243004170114
,88200,6.04999566006272,4,{{la{-1.39712834f,0.506596565f},la{-
1.32416666f,0.489707291f},la{-1.13913548f,0.55498606f},la{-
0.795843482f,0.608181298f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{105,
3575.3699999999999,0.010395661433177543,88200,14.050000038758405,2,{{
la{-1.46755624f,0.572719991f},la{-0.991771877f,0.517681658f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{105,
3577.1199999999999,0.010396074005476357,88200,10.049998643098087,3,{{
la{-1.43175745f,0.528265715f},la{-1.24529123f,0.518923283f},la{-
0.864156842f,0.556155503f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{106,3789.48,0.01042174655198037,88200,13.9500005768713,2,{{la{-
1.21271563f,0.38847962f},la{-0.901799798f,0.399888486f},la{0.0f,0.0f}
,la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{106,
3791.3400000000001,0.010421769215013135,88200,14.050001243937039,2,{{
la{-1.29614079f,0.449194372f},la{-0.784460485f,0.40342617f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{106,3793.21,
0.010421788656279837,88200,13.950001107162485,2,{{la{-1.21087635f,
0.387411445f},la{-0.897981465f,0.398319453f},la{0.0f,0.0f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{107,4018.3899999999999,
0.010401042854577084,88200,8.0500002911816608,3,{{la{-1.50740194f,
0.589755476f},la{-1.27144229f,0.590140879f},la{-0.78878653f,
0.614752471f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{107,
4020.3699999999999,0.010400669406307886,88200,8.0500018924282237,3,{{
la{-1.50677586f,0.589403689f},la{-1.26945913f,0.588653088f},la{-
0.788016021f,0.614056766f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lc{107,4022.3400000000001,0.010400294740517305,88200,
6.0500005377427026,4,{{la{-1.36691868f,0.507670105f},la{-1.39220786f,
0.537437439f},la{-1.04930043f,0.686420321f},la{-0.37702921f,
0.730419397f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{108,
4261.1300000000001,0.010333412023005079,88200,12.050000698935637,2,{{
la{-1.27907968f,0.443780214f},la{-0.732009947f,0.412854165f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{108,
4263.2299999999996,0.010332647299167762,88200,12.050000820600349,2,{{
la{-1.27830303f,0.443303198f},la{-0.730007112f,0.412438154f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lc{108,
4265.3199999999997,0.010331883366577739,88200,12.049999616174771,2,{{
la{-1.28984487f,0.4519822f},la{-0.727853715f,0.422457099f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},}};};
class l493{float l65=0.0;float l220=0.0;float l215=0.0;public:bool
l580=false;l493(double l547,double l586){if(std::abs(l547)<1.0e-12){
l580=true;l65=0.0f;return;}const double l130=std::tan(0.5*l586*l547)/
std::tan(0.5*l586);const double l422=(1.0-l130)/(1.0+l130);if(!std::
isfinite(l422)||std::abs(l422)>=1.0){throw std::runtime_error("\x66"
"\x72\x61\x63\x74\x69\x6f\x6e\x61\x6c\x5f\x66\x69\x6c\x74\x65\x72\x3a"
"\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x61\x6c\x6c\x70\x61\x73\x73\x20"
"\x63\x6f\x65\x66\x66\x69\x63\x69\x65\x6e\x74");}l65=static_cast<
float>(l422);}inline void l29(float&lm){if(l580){return;}const float
l34=l220+l65* (lm-l215);l220=lm;l215=l34;lm=l34;}inline void lx(){
l220=0.0;l215=0.0;}inline float l51()const noexcept{return 0.5f* (
l220*l220+l215*l215);}};
#include<complex>
class l251{inline static constexpr int l22=21;inline static constexpr
int l94=108;inline static constexpr std::size_t l315=6;struct lb{
float l88;float l89;float l65;};struct lf{int lo;double l427;double
l818;double l597;std::array<lb,l315>l109;};public:lf l290;private:std
::array<float,l315>l42{};public:explicit l251(int lo)noexcept:l290(
l197[static_cast<std::size_t>(std::clamp(lo,l22,l94)-l22)]){}[[
nodiscard]]inline double l813()const noexcept{return l290.l597;}[[
nodiscard]]inline double l600(double l367,double l768)const{const
double l556=2.0*std::numbers::pi_v<double> *l768/l367;const std::
complex<double>l122=std::polar(1.0,-l556);std::complex<double>l457{
1.0,0.0};for(const auto&l97:l290.l109)l457*=(double(l97.l88)+double(
l97.l89) *l122)/(1.0+double(l97.l65) *l122);return-std::arg(l457)/
l556;}inline void l29(float&lm)noexcept{for(std::size_t ld=0;ld<l315;
++ld){const auto&l97=l290.l109[ld];const float l34=l97.l88*lm+l42[ld]
;l42[ld]=l97.l89*lm-l97.l65*l34;lm=l34;}}inline void lx()noexcept{l42
.fill(0.0f);}[[nodiscard]]inline float l51()const noexcept{float l575
=0.0f;for(float lt:l42)l575+=lt*lt;return l575/static_cast<float>(
l315);}private:inline static constexpr std::array<lf,88>l197={{lf{21,
27.5,0.85742032977504412,0.85741661439239292,{{lb{0.599726796f,
0.555388093f,0.156409457f},lb{0.741163671f,0.295103461f,0.037428502f}
,lb{0.969172835f,0.0165651198f,-0.0131573398f},lb{0.830871403f,-
0.109907508f,-0.278228104f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}}
,lf{22,29.13523509488062,0.75066105224459401,0.7506619571893699,{{lb{
0.580761135f,0.516048372f,0.0978947207f},lb{1.09314692f,-
0.00136377837f,0.0928633288f},lb{1.02259135f,0.00562875113f,
0.029237438f},lb{0.51087898f,0.509291947f,0.0211803075f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{23,30.867706328507751,
1.4800343380408656,1.4800355018819626,{{lb{0.594773233f,0.514169991f,
0.109796979f},lb{0.570261121f,0.531858742f,0.102968365f},lb{
0.610026777f,0.478643805f,0.0895087495f},lb{0.600029767f,0.483035594f
,0.0838992447f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{24,
32.703195662574828,0.67703153472507449,0.67703038097734469,{{lb{
0.815292895f,0.220123217f,0.0368896984f},lb{0.526160359f,0.418647438f
,-0.0538475476f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{25,34.64782887210901,
0.69666080209240655,0.69665830332052181,{{lb{1.02035236f,
0.0121773425f,0.0329578482f},lb{0.994840384f,0.0159894768f,
0.0112490673f},lb{0.997597337f,0.000294383994f,-0.00169446308f},lb{
1.0067879f,-0.0155846532f,-0.00838571787f},lb{0.959713995f,
0.00777110737f,-0.0321136862f},lb{0.425314099f,0.424020171f,-
0.150313497f},}}},lf{26,36.70809598967594,1.3911093481205912,
1.3911121453998017,{{lb{0.630939364f,0.607252359f,0.238663197f},lb{
0.644183218f,0.523969352f,0.16859743f},lb{0.558826864f,0.512817502f,
0.0720524266f},lb{0.531618953f,0.512325168f,0.0443416163f},lb{
1.05408549f,-0.0428682007f,0.0116023384f},lb{1.01614916f,-
0.0253095068f,-0.0087830117f},}}},lf{27,38.890872965260115,
0.47121152885188244,0.47120641378627143,{{lb{0.614674032f,
0.198462442f,-0.185099587f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{28,
41.203444614108747,0.79416646836438987,0.79416265961839339,{{lb{
0.827214897f,0.655734599f,0.483892411f},lb{0.614918172f,0.613052309f,
0.228751227f},lb{0.700333357f,0.118912816f,-0.180232897f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{29,
43.653528929125486,0.6507102218399754,0.64928708002509194,{{lb{
28.9138603f,-27.9596863f,-0.0454978645f},lb{0.920290768f,
0.0338419639f,-0.0455388874f},lb{0.0165325813f,0.0164325684f,-
0.967023492f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f
},}}},lf{30,46.2493028389543,0.55393467480967684,0.5525121155944076,{
{lb{0.52350229f,0.518573523f,0.0424013585f},lb{26.332962f,-
25.3722973f,-0.0390352197f},lb{0.0352337323f,0.00120436365f,-
0.963550508f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f
},}}},lf{31,48.999429497718666,0.31363652975626988,
0.31363132726022519,{{lb{0.726950884f,0.140048683f,-0.13202472f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{32,51.913087197493141,
0.50721345679074092,0.50721219122174621,{{lb{0.604217052f,
0.600735188f,0.205594704f},lb{0.832158327f,0.101522312f,-
0.0658215657f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{33,55,0.41685862762190456,
0.41685618409928427,{{lb{0.657960117f,0.634813428f,0.293416768f},lb{
0.86499089f,0.0160933416f,-0.118477352f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{34,
58.270470189761241,0.32782032425130253,0.32781867185975511,{{lb{
0.739006102f,0.655698419f,0.395332456f},lb{0.868263781f,0.0627665743f
,-0.0685505196f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{35,61.735412657015502,
0.19673697337919718,0.19673383674281522,{{lb{0.807371855f,0.1658521f,
-0.0256482493f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{36,
65.406391325149656,0.5113473044118928,0.51135271387241532,{{lb{
0.788431108f,0.690659821f,0.479401529f},lb{1.05299413f,0.310631841f,
0.363912314f},lb{0.811230659f,0.516055286f,0.327564687f},lb{
0.658967912f,0.625217378f,0.284455001f},lb{0.999790013f,0.0f,0.0f},lb
{0.999790013f,0.0f,0.0f},}}},lf{37,69.295657744218019,
0.28767561951245485,0.28767564787498062,{{lb{0.736167431f,
0.647123337f,0.384161472f},lb{0.902309716f,0.0941902474f,-
0.00287279394f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{38,73.416191979351879,
0.32864823991805908,0.32864931637905326,{{lb{0.944784701f,0.92770493f
,0.873017609f},lb{0.696295857f,0.518881202f,0.215519682f},lb{
0.95420301f,0.045854114f,0.000339105609f},lb{0.995622277f,
0.00233388646f,-0.00176244543f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f}
,}}},lf{39,77.781745930520231,0.30741891154034562,0.30741852714937007
,{{lb{0.999711633f,0.993693054f,0.993905902f},lb{0.887165189f,
0.66502142f,0.5525769f},lb{0.801080048f,0.518886209f,0.320298195f},lb
{0.918724537f,0.0417532027f,-0.0392807312f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},}}},lf{40,82.406889228217494,0.24560145343137246,
0.24560209109043304,{{lb{0.853651464f,0.809978247f,0.66399157f},lb{
0.826794684f,0.269713521f,0.0967467427f},lb{0.999782562f,-
1.48979211e-08f,-0.0f},lb{0.999782503f,-0.0f,-0.0f},lb{0.999782503f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{41,87.307057858250971,
0.11010766725869625,0.11010460952830284,{{lb{0.885708749f,0.14449279f
,0.0310847536f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{42,
92.4986056779086,0.11577367222702861,0.11577102247131661,{{lb{
0.993454754f,0.990474343f,0.984836698f},lb{0.880521715f,0.1755061f,
0.0565108992f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{43,97.998858995437331,
0.18333060700377182,0.18333352737874128,{{lb{0.766832411f,0.45989877f
,0.22698018f},lb{1.0416559f,-0.011029019f,0.0308359936f},lb{
0.978306234f,0.0354222134f,0.013934169f},lb{0.986826181f,0.023579618f
,0.0106108701f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{44,
103.82617439498628,0.040036636502009469,0.039954648942691171,{{lb{
0.949495971f,0.937892497f,0.887607276f},lb{1.00797212f,0.490740716f,
0.498858988f},lb{1.0008918f,-0.000501996838f,0.000487383921f},lb{
1.0008918f,-0.000502061681f,0.000487225421f},lb{1.00089121f,-
0.000501829374f,0.000486933684f},lb{0.993668497f,-0.706793427f,-
0.713096976f},}}},lf{45,110,0.029642786890857257,0.0295971130163044,{
{lb{0.980723321f,0.876643062f,0.857723534f},lb{1.02486253f,
0.190329045f,0.215408474f},lb{0.98601526f,-0.0415588766f,-
0.0553750023f},lb{0.989385605f,-0.578181088f,-0.58872211f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{46,116.54094037952248,
0.16975279795465156,0.16970649964419832,{{lb{0.993798852f,0.98878026f
,0.982801795f},lb{0.940717161f,0.82147032f,0.762343526f},lb{
0.863893092f,0.634422541f,0.498448342f},lb{0.786959469f,0.469338268f,
0.256408989f},lb{1.20874548f,-0.0868035853f,0.122041255f},lb{
0.970886528f,-0.478615552f,-0.507685423f},}}},lf{47,
123.47082531403103,0.035009066660717243,0.034971327764171542,{{lb{
0.97442615f,0.96230185f,0.937287509f},lb{0.989805937f,-0.539001942f,-
0.549065769f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f
},lb{1.0f,0.0f,0.0f},}}},lf{48,130.81278265029931,0.13495712781856303
,0.13424177311227903,{{lb{1.00784063f,0.837106407f,0.845085025f},lb{
0.95027703f,0.562547743f,0.512937903f},lb{0.773342729f,0.270734906f,
0.0441557169f},lb{1.12592971f,-0.115390502f,0.0106148152f},lb{
0.998361766f,-0.890331805f,-0.891961932f},lb{1.0f,0.0f,0.0f},}}},lf{
49,138.59131548843604,0.058746349654687578,0.054686346533082748,{{lb{
0.980364025f,0.187261418f,0.167761028f},lb{0.9718225f,0.0104925521f,-
0.017570883f},lb{0.999158263f,-0.953364372f,-0.954200804f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{50,
146.83238395870379,-0.011214580178518922,0.047779390590984162,{{lb{
0.990663767f,-0.60727644f,-0.615974009f},lb{1.00122774f,-0.996284783f
,-0.995048761f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{51,155.56349186104046,
0.017718800381101373,0.017659713601635239,{{lb{0.992038071f,-
0.571211696f,-0.578966081f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{52,
164.81377845643496,0.013269125713558446,0.013145549636863417,{{lb{
0.995723963f,-0.704598844f,-0.708759844f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{53,174.61411571650194,0.012641015213949393,0.012520407547119734
,{{lb{0.99571532f,-0.693189979f,-0.697340846f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},}}},lf{54,184.9972113558172,0.37868960276100977,
0.378660698344636,{{lb{0.825021029f,0.389514536f,0.214622185f},lb{
0.753541887f,0.300063729f,0.0536808074f},lb{0.998856664f,
0.000536368578f,-0.000535668631f},lb{0.683845937f,0.223737091f,-
0.0923522189f},lb{1.43834972f,-0.606888533f,-0.168479443f},lb{
0.918005168f,-0.459794492f,-0.54175663f},}}},lf{55,195.99771799087463
,0.12701654700847309,0.12654964412045064,{{lb{1.1681329f,-
0.240081087f,-0.0718317628f},lb{0.720318079f,0.207231224f,-
0.0723343343f},lb{0.998930693f,-0.867290199f,-0.868342996f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{56,
207.65234878997256,0.014196194535651423,0.013938705100463874,{{lb{
0.995806456f,-0.729476988f,-0.733569801f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{57,220,0.20465774363289654,0.20419387444158632,{{lb{
0.799209714f,0.404715091f,0.20400995f},lb{0.807737887f,0.226163834f,
0.0339748636f},lb{1.19591594f,-0.217351571f,-0.0213664155f},lb{
0.953136742f,0.0250633992f,-0.021730639f},lb{0.999386489f,-
0.884821177f,-0.885426641f},lb{1.0f,0.0f,0.0f},}}},lf{58,
233.08188075904496,0.038896433622899192,0.038883946816770633,{{lb{
0.988568723f,0.582398355f,0.571201146f},lb{1.0142622f,0.313005775f,
0.327465713f},lb{0.965545893f,0.051009547f,0.016706882f},lb{
0.991001189f,0.0109970998f,0.00214755442f},lb{1.0f,0.0f,0.0f},lb{1.0f
,0.0f,0.0f},}}},lf{59,246.94165062806206,0.032830022749244507,
0.032637969351965064,{{lb{0.990659654f,0.57697314f,0.567822993f},lb{
0.979858339f,0.0123420283f,-0.00767927337f},lb{0.998098254f,-
0.738902152f,-0.740772426f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},}}},lf{60,261.62556530059862,0.012926738428970547,
0.012550762696303477,{{lb{0.996275842f,-0.731645703f,-0.735302925f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{61,277.18263097687208,
0.032165145178381725,0.032143675067840528,{{lb{0.969321072f,-
0.0274190437f,-0.0577470772f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{62,
293.66476791740757,0.017741009471637766,0.017690396344634925,{{lb{
0.954588652f,0.513678193f,0.468474418f},lb{1.14660776f,0.0157275535f,
0.162497997f},lb{0.90009129f,-0.018637307f,-0.118422605f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{63,
311.12698372208092,0.04035237610931805,0.040346030865123311,{{lb{
0.966261804f,0.529893398f,0.496487528f},lb{0.981924295f,0.0124844424f
,-0.00537041062f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{64,329.62755691286992,
0.01715177356241207,0.017120475154654959,{{lb{1.0076642f,0.540508091f
,0.548424304f},lb{1.03198195f,-0.00856817886f,0.0235145818f},lb{
0.950185239f,-0.0221171007f,-0.0718403682f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{65,349.22823143300388,
0.017514410181210166,0.017487510238098584,{{lb{1.02649641f,
0.269840032f,0.296572834f},lb{0.961407363f,0.0465278327f,
0.00811895914f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{66,369.9944227116344,
0.022722093780179364,0.022703605835845998,{{lb{0.976765215f,
0.0324434862f,0.00950561743f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{67,
391.99543598174927,0.014124971719019623,0.014097852447143093,{{lb{
1.01360297f,0.42427209f,0.438141078f},lb{0.976349592f,0.0126638897f,-
0.0108050527f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{68,415.30469757994513,
0.017716359605711548,0.017688442084515751,{{lb{1.00471044f,
0.536005914f,0.540982068f},lb{0.979444802f,-0.004169608f,-
0.0245566461f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{69,440,0.10176631973743934,
0.10188182300953239,{{lb{0.819824994f,0.434237868f,0.254359961f},lb{
1.03941965f,-0.0901459605f,-0.0505014285f},lb{1.0f,0.0f,0.0f},lb{1.0f
,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{70,
466.16376151808993,0.12039282452329729,0.12065213345614838,{{lb{
0.757579207f,0.284194946f,0.0419883803f},lb{1.10212457f,-0.190412983f
,-0.0881009176f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{71,493.88330125612413,
0.025181731279501109,0.025155578041368025,{{lb{0.973125696f,
0.0833870545f,0.0567829385f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{72,
523.25113060119725,0.027911015412448042,0.0279016986888144,{{lb{
0.972450256f,0.317944348f,0.290573001f},lb{0.993223786f,
0.00153785862f,-0.0051008272f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{73,554.36526195374415,
0.042002827886067781,0.042012447538418859,{{lb{0.938014209f,
0.248360977f,0.186526358f},lb{1.01008284f,-0.00341879716f,
0.00679231621f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{74,587.32953583481515,
0.022129090265598016,0.022059124264627698,{{lb{1.0099473f,0.45746699f
,0.467586011f},lb{0.971104026f,0.0205016993f,-0.00827826094f},lb{1.0f
,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},
}}},lf{75,622.25396744416184,0.025545885093157337,
0.025488415178533921,{{lb{0.973884523f,0.0375257097f,0.0116621768f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{76,659.25511382573984,
0.023227019525173656,0.023188578938153494,{{lb{0.974814057f,
0.0975872427f,0.0726766065f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{77,
698.45646286600777,0.025192315111285182,0.02512550936792984,{{lb{
0.973972499f,0.0477763824f,0.0220076013f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{78,739.9888454232688,0.022088628743562998,0.022022096880221408,
{{lb{0.977236152f,0.0444471091f,0.0218490809f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},}}},lf{79,783.99087196349853,0.016272462500420838,
0.016157406570587486,{{lb{1.01221073f,0.496043146f,0.508402884f},lb{
0.975786805f,0.00883241743f,-0.0152838463f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{80,
830.60939515989025,0.01687967199601674,0.016746417190969479,{{lb{
1.01265442f,0.497204214f,0.510058165f},lb{0.974828541f,0.0103882253f,
-0.0146530448f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{81,880,0.021291830495761135,
0.021206022398886581,{{lb{0.977660775f,0.0526175536f,0.0306478254f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{82,932.32752303617985,
0.026945658276763824,0.026823726088244456,{{lb{0.971932948f,
0.0553923808f,0.0276567079f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{83,
987.76660251224826,0.013125877309455318,0.012981252373833442,{{lb{
1.0092535f,0.514246464f,0.523883522f},lb{0.980557621f,0.00677067926f,
-0.0124234464f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{84,1046.5022612023945,
0.013127152809474006,0.012962497238108966,{{lb{1.00960517f,
0.503719687f,0.513614357f},lb{0.980383813f,0.00651569711f,-
0.0129119251f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{85,1108.7305239074883,
0.010336494106209904,0.010184361339962124,{{lb{1.00755727f,
0.496961176f,0.504841506f},lb{0.98449862f,-0.00317055616f,-
0.0184620284f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{86,1174.6590716696303,
0.0094712241497262994,0.0092987001935452753,{{lb{1.0093801f,
0.412381947f,0.422089189f},lb{0.983681977f,0.00238205376f,-
0.0137106208f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{87,1244.5079348883237,
0.02663527389084696,0.026429796975497123,{{lb{0.991836905f,
0.543615341f,0.535686731f},lb{0.978570402f,0.0100743789f,-
0.0112041971f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{88,1318.5102276514797,
0.024242723525079588,0.024112421216637842,{{lb{0.973033249f,
0.124768361f,0.0981109813f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{89,
1396.9129257320155,0.015820217400752193,0.015631776755286413,{{lb{
0.998215139f,0.74948889f,0.748051047f},lb{0.984867454f,0.0102757076f,
-0.00465928577f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{90,1479.9776908465376,
0.011949162604826494,0.011838660788130716,{{lb{0.994577289f,
0.628071725f,0.623101294f},lb{0.990929067f,0.0043034968f,-
0.00449006725f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{91,1567.9817439269971,
0.0094224663703528447,0.0094029046271016572,{{lb{0.98817575f,
0.221790612f,0.210475758f},lb{0.999571681f,3.75041191e-06f,-
3.75212448e-06f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{92,1661.2187903197805,
0.05980265986965088,0.059651421433927286,{{lb{0.965376496f,
0.285068303f,0.250776976f},lb{0.967900574f,0.0145719349f,-
0.0172664709f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{93,1760,0.031335558273157184,
0.031178911490643207,{{lb{0.96814698f,0.0235108417f,-0.00764639117f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{94,1864.6550460723597,
0.038536421715804986,0.038316661095661279,{{lb{0.961406112f,
0.0244043134f,-0.0137004927f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{95,
1975.5332050244961,0.026272560648429276,0.026108283424544517,{{lb{
0.972850621f,0.0222706478f,-0.00395919289f},lb{1.0f,0.0f,0.0f},lb{
1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},}}},lf{96,2093.004522404789,0.078584138919259847,
0.078459934557733438,{{lb{0.926423907f,0.920865417f,0.847760558f},lb{
0.96115613f,0.0294361282f,-0.00915507972f},lb{1.0f,0.0f,0.0f},lb{1.0f
,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{97,
2217.4610478149766,0.074208225893439422,0.074119103851506607,{{lb{
0.924043834f,0.918499589f,0.843072057f},lb{0.966758668f,0.0262892637f
,-0.00666718977f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,
0.0f},lb{1.0f,0.0f,0.0f},}}},lf{98,2349.3181433392601,
0.040668510748089269,0.040306363796772555,{{lb{0.999687552f,
0.984731495f,0.984975755f},lb{0.959391832f,0.028217487f,-0.012113628f
},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},}}},lf{99,2489.0158697766474,0.0087108935035869594,
0.0086563702963833918,{{lb{0.989404142f,0.082995683f,0.0737152323f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{100,2637.0204553029598,
0.001076057116530133,0.0010687145280212321,{{lb{0.997546852f,
0.0832279176f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{101,
2793.8258514640311,0.0010528550964885128,0.0010447837373837056,{{lb{
0.997409046f,0.0831896812f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{102,2959.9553816930752,0.00076717842403185919,
0.00076056264574003122,{{lb{0.997653842f,0.0828748196f,0.0821666643f}
,lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{103,3135.9634878539946,
0.00078442827509554649,0.00077682824987908696,{{lb{0.997455895f,
0.0828791335f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{104,
3322.437580639561,0.00084422519217250045,0.00083503615180350362,{{lb{
0.997120202f,0.0829221159f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{105,3520,0.00082491670391699856,0.00081482458100261561,{{lb{
0.997010171f,0.0828909054f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},lf{106,3729.3100921447194,0.00090940322020846348,
0.00089690178229269554,{{lb{0.996842861f,0.082977213f,0.0821666643f},
lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f
,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{107,3951.0664100489921,
0.0015049289270278963,0.0014841647661936032,{{lb{0.995968819f,
0.0981342122f,0.0967058763f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb
{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}}},lf{108,
4186.009044809578,0.00081468721060068069,0.0008005229738574707,{{lb{
0.996237099f,0.0828170404f,0.0821666643f},lb{1.0f,0.0f,0.0f},lb{1.0f,
0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},lb{1.0f,0.0f,0.0f},}
}},}};};
namespace lv::l4{enum class l45{l394,l680,l797,l776,l718,l620};
constexpr int l22=21;constexpr int l94=108;constexpr int l180=l94-l22
+1;static_assert(l180==88);constexpr int l655=69;constexpr double l570
=440.0;using l59=std::array<double,l180>;using l289=std::array<double
,l180>;using l314=std::array<double,l180>;enum class l75{l0=1,l331=2,
l1=3};constexpr double l773=1.7;}class l47{l47(const l47&)=delete;l47
&operator=(const l47&)=delete;public:using l45=lv::l4::l45;using l59=
lv::l4::l59;using l75=lv::l4::l75;using l289=lv::l4::l289;using l314=
lv::l4::l314;constexpr l47()noexcept=default;double l683(int lk,l45
l112=l45::l394,l75 l725=l75::l331)const noexcept{if(lk<lv::l4::l22||
lk>lv::l4::l94){return 0.0;}const int lh=lk-lv::l4::l22;const l200&lz
=l296();const double l261=l737(lz,lh,l112);switch(l725){case l75::l0:
return l261-lz.l221[lh];case l75::l331:return l261;case l75::l1:
return l261+lz.l221[lh];}return l261;}const l59&l825()const noexcept{
return l296().l37;}double l450(int lk)const noexcept{if(lk<lv::l4::
l22||lk>lv::l4::l94){return 0.0;}const int lh=lk-lv::l4::l22;return
l296().l491[lh];}const l314&l822()const noexcept{return l296().l491;}
private:struct l200{l59 l37{};l59 l462{};l59 l535{};l59 l485{};l59
l440{};l59 l433{};l289 l221{};l314 l491{};};static l200 l740(){l200 lz
{};l679(lz.l37);lz.l462=lz.l37;lz.l535=lz.l37;lz.l485=lz.l37;lz.l440=
lz.l37;lz.l433=lz.l37;l606(lz.l37,lz.l221);return lz;}static const
l200&l296(){static const l200 lz=l740();return lz;}static double l426
(int lk){return lv::l4::l570*std::pow(2.0,static_cast<double>(lk-lv::
l4::l655)/12.0);}static double l670(double l660){constexpr double l789
=-1.3333333333333333;constexpr double l788=1.3333333333333333;
constexpr double l628=-6.5760753629956712;constexpr double l627=
1.9137732290077178;constexpr double l626=0.27614763151078042;
constexpr double l625=-0.28572738557770405;constexpr double l623=-
0.30709648310494486;const double lm=std::log2(l660/440.0);double l343
=l628+l627*lm+l626*lm*lm+l625*std::pow(std::max(lm-l789,0.0),2.0)+
l623*std::pow(std::max(lm-l788,0.0),2.0);l343=std::clamp(l343,std::
log(5e-6),std::log(2e-2));return std::exp(l343);}static double l250(
int lk){return l670(l426(lk));}static double l464(int l152){constexpr
double l774=3.5;constexpr double l765=60.0;constexpr double l78=25.0;
const double lm=(static_cast<double>(l152)-l765)/l78;return(l774*0.5) *
(1.0-std::erf(lm))+1.0;}static double l801(int l152,int l369,double
l657){const double l246=l464(l152);const double l223=l246*l246;const
double l417=l250(l152);const double l352=l250(l369);return 2.0*l657*
std::sqrt((1.0+l417*4.0*l223)/(1.0+l352*l223));}static double l609(
int l152,int l369,double l734){const double l246=l464(l152);const
double l223=l246*l246;const double l417=l250(l152);const double l352=
l250(l369);const double l684=std::sqrt((1.0+l417*4.0*l223)/(1.0+l352*
l223));return l734/(2.0*l684);}static double l794(int lk,const std::
array<int,8>&l16,const std::array<double,8>&l150){if(lk<=l16.front()){
return l150.front();}for(std::size_t ld=0;ld+1<l16.size();++ld){const
int l488=l16[ld];const int l517=l16[ld+1];if(lk<=l517){const double
l629=static_cast<double>(lk-l488)/static_cast<double>(l517-l488);
return l150[ld]+l629* (l150[ld+1]-l150[ld]);}}const std::size_t l244=
l16.size()-1;const std::size_t l454=l244-1;const double l752=(l150[
l244]-l150[l454])/static_cast<double>(l16[l244]-l16[l454]);return l150
[l244]+l752*static_cast<double>(lk-l16[l244]);}static void l679(l59&
l37){l59 l538{};for(int lo=lv::l4::l22;lo<=lv::l4::l94;++lo){const int
lh=lo-lv::l4::l22;l538[lh]=l426(lo);}constexpr std::array<int,8>l16={
21,33,45,57,69,81,93,105};std::array<double,l16.size()>l183{};std::
array<double,l16.size()>l466{};constexpr int l413=4;l183[l413]=lv::l4
::l570;for(int ld=l413+1;ld<static_cast<int>(l16.size());++ld){l183[
ld]=l801(l16[ld-1],l16[ld],l183[ld-1]);}for(int ld=l413-1;ld>=0;--ld){
l183[ld]=l609(l16[ld],l16[ld+1],l183[ld+1]);}for(std::size_t ld=0;ld<
l16.size();++ld){l466[ld]=1200.0*std::log2(l183[ld]/l426(l16[ld]));}
for(int lo=lv::l4::l22;lo<=lv::l4::l94;++lo){const int lh=lo-lv::l4::
l22;const double l637=l794(lo,l16,l466);l37[lh]=l538[lh] *std::pow(
2.0,l637/1200.0);}}static double l737(const l200&lz,int lh,l45 l112)noexcept
{switch(l112){case l45::l394:return lz.l37[lh];case l45::l680:return
lz.l462[lh];case l45::l797:return lz.l535[lh];case l45::l776:return lz
.l485[lh];case l45::l718:return lz.l440[lh];case l45::l620:return lz.
l433[lh];}return lz.l37[lh];}static int l619(int lk)noexcept{if(lk<=
28){return 1;}if(lk<=33){return 2;}return 3;}static double l694(
double l101,int l102){if(l102<=1){return 0.0;}const double l715=std::
pow(2.0,lv::l4::l773/1200.0);const double l787=l101*l715;const double
l677=l787-l101;return l677*0.5;}static void l606(const l59&l37,l289&
l221){for(int lo=lv::l4::l22;lo<=lv::l4::l94;++lo){const int lh=lo-lv
::l4::l22;const int l102=l619(lo);const double l101=l37[lh];l221[lh]=
l694(l101,l102);}}};
namespace lv::l357::l346{struct le{double l759,l756,l240,l228;double
l746,l205,l745,l204;double l702,l763;};inline constexpr std::array<le
,88>l598{{le{1.90124055258e+15,1375613.66073,0.119408639313,
38.0826056266,4.13048457387,2.3661379754,1.84042891408,0.752770660711
,0.0110001,-0.000149802114365},le{9.59155564422e+15,1203516.60213,
0.336605009809,42.7975403467,4.3217167608,2.13689865174,1.87282283511
,0.846590795985,0.0109264,-0.000166014912801},le{2.60495506681e+16,
4766224.352,0.680080500023,43.8041537575,4.43248325664,2.02184788517,
2.00582859629,0.882320819001,0.0108529,-0.000132863768194},le{
1.99698309552e+16,13924813.291,0.690558976978,42.7497316804,
4.41477849949,2.05016826179,2.09363392848,0.869484001743,0.0107796,-
0.000163429243251},le{8.12371683584e+15,10074715.5664,0.500826731448,
41.752426156,4.28611951718,2.12302448773,2.04460267567,0.857263030672
,0.0107065,-0.000136441301655},le{6.28225497142e+15,9016749.14465,
1.33590430622,41.6112520286,4.24669728008,2.14487920175,1.88943153814
,0.851462584024,0.0106336,-0.000136552779455},le{1.07968035525e+16,
2704775.11953,0.822794057432,35.3120056655,4.29546731673,
2.12303565723,1.77570820998,0.864135668989,0.0105609,-
0.000135379752219},le{8.9091042996e+15,2207178.48042,0.563376468068,
37.0341337886,4.26124977415,2.05617607923,1.82048610449,
0.904995431651,0.0104884,-0.000116150999085},le{8.53567720522e+15,
16539450.8839,0.378958708451,48.1140075828,4.24924103727,
1.97298528202,1.89889780684,0.968287636118,0.0104161,-
0.000160978275395},le{1.97079971635e+16,11716508.0714,1.01676591528,
54.7853828661,4.32921359654,1.86305537124,1.92334683335,1.00177512769
,0.010344,-0.000504397989134},le{2.43941407626e+16,2509784.20042,
0.91437413231,51.6672100607,4.38098336984,1.69544031575,1.91200131707
,0.981753094032,0.0102721,-0.000317725913316},le{1.08083268369e+16,
4265860.33642,0.977110846001,49.7863985178,4.28618459335,1.5201450234
,1.91014588707,0.950445267132,0.0102004,-0.000203022072646},le{
7.46766237692e+15,8141565.71313,0.642531064534,44.244546139,
4.21901048949,1.47609829611,1.96529179691,0.939866028905,0.0101289,-
0.000128452961692},le{2.24847450514e+16,4988494.93476,0.166314024304,
47.4859641528,4.35104734796,1.51202563395,2.00643838807,
0.925925602717,0.0100576,-0.000119826877057},le{1.15153139959e+17,
4119685.0189,0.398230846254,40.9874728747,4.56044593576,1.59089095524
,1.96575748133,0.892274281148,0.0099865,-0.000120796129225},le{
5.97006130445e+17,1614025.85489,1.11643970939,41.305850937,
4.74783933855,1.76776079661,1.85061335816,0.879405355389,0.0099156,-
0.000119602779202},le{2.93661522715e+18,303123.896986,0.732342028387,
41.2024689357,4.93348828936,2.00596330823,1.65453597875,
0.872580344006,0.0098449,-0.000105054800329},le{1.07745332812e+19,
272586.97748,2.0533668725,37.6134048809,5.09015948451,2.25323482166,
1.53646772938,0.833646690938,0.0097744,-9.22221054168e-05},le{
1.63210828375e+19,208252.936695,1.48142793424,33.7012539807,
5.15057732939,2.41769691578,1.57750620148,0.78896669122,0.0097041,-
9.69041021875e-05},le{1.83086253625e+19,318761.09555,2.32410984379,
39.2999268334,5.14246320381,2.45757009513,1.61667445683,
0.789028770014,0.009634,-9.94000349481e-05},le{3.09363114974e+19,
417603.491112,4.00831322361,35.2516917672,5.21462388212,2.44817823536
,1.62930657254,0.810520898878,0.0095641,-0.000141677382787},le{
2.70158172408e+20,1612931.50992,4.79542548161,32.2919627132,
5.42617462016,2.47220686626,1.7081592721,0.820639552793,0.0094944,-
8.33410396604e-05},le{1.19205748909e+21,2364284.45159,4.8610119504,
32.6651261608,5.63080749806,2.49970685481,1.77884466561,
0.801668586098,0.0094249,-6.4170804766e-05},le{2.69550334639e+21,
1484457.14381,5.65780250493,30.5722817433,5.70689112757,2.4186482948,
1.78092483058,0.76697888569,0.0093556,-7.44144226127e-05},le{
2.30917598291e+21,1440842.58677,3.8672163874,28.1104330807,
5.68725315675,2.23823261956,1.77914365649,0.753684846433,0.0092865,-
6.23805667313e-05},le{8.3853281128e+20,1315746.52895,3.74477742445,
30.9201466664,5.55940564079,2.13115292583,1.74361856527,0.78397726502
,0.0092176,-6.56586095084e-05},le{2.05746243457e+20,235749.468223,
3.376147697,29.4134184667,5.3318978151,2.11621801165,1.62679044939,
0.810408836341,0.0091489,-5.269308545e-05},le{1.44875631151e+19,
77625.9945432,3.29138819921,26.6205540825,5.02336056677,2.04273528386
,1.50440683859,0.794756202943,0.0090804,-6.37478055802e-05},le{
1.94197195099e+18,76335.343837,5.12458315846,28.7434163259,
4.78787637622,1.90868313949,1.43147535346,0.779681773405,0.0090121,-
6.2806313395e-05},le{1.09252606132e+18,44097.5014844,2.08784844498,
26.4140837749,4.75777849303,1.7665349973,1.45580748673,0.776330871603
,0.008944,-4.16355793184e-05},le{2.15339417951e+18,83457.9946799,
4.2219353228,27.1273467111,4.78739346586,1.68226393051,1.55375237691,
0.788663104904,0.0088761,-5.72457250329e-05},le{1.99910863391e+18,
196003.572152,3.59281318911,31.7845508829,4.80939333133,1.65090988139
,1.6513739311,0.788851264288,0.0088084,-5.67521213917e-05},le{
6.05655546297e+18,256406.626699,1.55332182867,21.1563127178,
4.90671427297,1.67347519019,1.68270195371,0.7608748089,0.0087409,-
3.25426048289e-05},le{1.25811838756e+19,395796.470854,0.738748456995,
21.5790650408,5.0068813294,1.89946525684,1.67608432359,0.752596233909
,0.0086736,-3.81005451984e-05},le{1.04260284885e+19,387143.601204,
1.84331128269,27.9860962212,4.98847216481,2.1735206265,1.68104717617,
0.763397936992,0.0086065,-5.35087040523e-05},le{5.31889066379e+18,
440714.218483,2.9640174542,28.2328333013,4.89380592203,2.28665214119,
1.68378930656,0.771083753699,0.0085396,-6.10062309639e-05},le{
3.8991327591e+18,624999.367894,1.96576664678,22.8379379543,
4.84466641641,2.30547103254,1.65171092866,0.800300960365,0.0084729,-
4.26791313419e-05},le{2.81902105616e+18,240588.237061,4.60570255449,
26.1595000027,4.78346141945,2.36680526719,1.63177387398,
0.841895192828,0.0084064,-5.21700552012e-05},le{1.48173314182e+18,
338288.593047,1.02056918203,25.23717991,4.68932935519,2.46659275508,
1.66364294526,0.829070378947,0.0083401,-2.56516875243e-05},le{
1.20291840144e+18,494809.113567,1.10908032934,22.0169056935,
4.69763023758,2.57531514807,1.73518653837,0.791491183682,0.008274,-
3.77659084397e-05},le{2.97823105717e+18,1429908.23488,2.7775221225,
27.9156157484,4.80781968524,2.64854148324,1.86148438256,
0.793149852153,0.0082081,-4.2695280883e-05},le{9.33515253331e+18,
3804401.88228,4.7913686661,25.1660042169,4.90881733218,2.62500877784,
1.97065263392,0.815715922247,0.0081424,-4.03540187213e-05},le{
3.33080492707e+19,2369795.75647,2.98308433682,25.99899469,
5.03476734492,2.47031578862,1.96340698925,0.831190372432,0.0080769,-
5.06830022518e-05},le{7.05858908872e+19,1204200.93663,3.70802288176,
25.8393068801,5.14725245202,2.20832738139,1.85234012253,
0.817900256065,0.0080116,-5.46829029568e-05},le{5.28088613703e+19,
466769.92944,2.94548563106,25.8865441327,5.1022010374,1.99599348054,
1.75397644688,0.799847676124,0.0079465,-0.000145142204853},le{
1.6827420192e+19,618369.897105,4.24950414417,26.2119102199,
4.96939458864,1.98390068574,1.73670544127,0.78487917451,0.0078816,-
0.000115104965335},le{9.06969848954e+18,744146.436259,2.6688840612,
25.0071588243,4.8984659227,2.08935279964,1.72980213548,0.77563580592,
0.0078169,-5.71533919881e-05},le{1.34542366837e+19,462859.430068,
4.26533788213,23.9939244608,4.92771067589,2.14193759332,1.72322315434
,0.765569236097,0.0077524,-4.33189041665e-05},le{3.3479137075e+19,
349539.006332,2.69579120257,22.4760237905,5.03854920411,2.12131931738
,1.6960995783,0.75790325128,0.0076881,-5.24611276553e-05},le{
1.88125157346e+20,462823.272766,3.29182989912,21.4138626342,
5.22611414597,2.09425915356,1.6819033962,0.749823099457,0.007624,-
4.28785081806e-05},le{1.19523779118e+21,262730.399132,3.02943917924,
24.5967599624,5.43473834035,2.14286837587,1.63414329327,
0.741435774512,0.0075601,-8.11549738401e-05},le{4.15578594012e+21,
327152.881339,3.80004688879,22.2157020775,5.60002039788,2.2772175074,
1.57834228461,0.735617111497,0.0074964,-7.48231411051e-05},le{
6.91932963346e+21,426419.877375,3.2653735401,23.6357916739,
5.66216566821,2.37752616899,1.58761438073,0.746157564391,0.0074329,-
5.51166470958e-05},le{3.98517504672e+21,267406.158826,3.0916807582,
21.8643935308,5.58515053626,2.41528923023,1.6909645123,0.759977525464
,0.0073696,-3.40807076608e-05},le{1.35539073976e+21,1043628.64232,
3.62941499761,23.2872028291,5.43308454427,2.44830842497,1.851704686,
0.766285029711,0.0073065,-3.84660155355e-05},le{1.92622779627e+20,
2256882.81384,1.54673126708,21.4998732961,5.20399927168,2.41900746695
,1.9377698801,0.783451063761,0.0072436,-3.53734170468e-05},le{
3.60551896733e+19,1740069.86187,2.72593196205,25.6361249991,
4.98846806538,2.336881587,1.87192558247,0.819054597983,0.0071809,-
2.62197692688e-05},le{2.0319390508e+19,385017.840365,3.56597481611,
21.1262935454,4.90959889104,2.27376879685,1.74095522556,
0.833786831943,0.0071184,-2.08366966287e-05},le{2.70304438908e+19,
333204.351113,2.93778591247,22.4657998721,4.95015522978,2.28867922868
,1.68783777912,0.82666779629,0.0070561,-2.63466522507e-05},le{
2.01406157963e+19,810208.179239,4.66246104354,20.6956677999,
4.92739567587,2.44208327708,1.74643660529,0.789432967705,0.006994,-
3.71491330704e-05},le{7.65920808025e+18,3254646.82757,5.09475391607,
23.9578580978,4.82215223877,2.56470397575,1.88804256835,
0.759373821503,0.0069321,-5.65952730224e-05},le{9.69375116876e+18,
3475672.18973,3.0607607207,21.3878430914,4.8128422123,2.54958789556,
1.97410168331,0.757344353993,0.0068704,-3.97501710497e-05},le{
3.52350163621e+19,3008116.8505,1.71989324322,18.7741548293,
4.95822815771,2.48946216533,1.97840710945,0.763572355606,0.0068089,-
2.80583328194e-05},le{2.52346163895e+20,4835980.26307,2.41705464632,
19.5947137301,5.1896391866,2.48587807784,1.96738727913,0.759852763097
,0.0067476,-4.10829509604e-05},le{9.11605412669e+20,4071954.91555,
2.55583148843,21.5423226242,5.347430544,2.53881064712,1.97375689409,
0.74574036088,0.0066865,-4.64706395524e-05},le{1.29911829327e+21,
4030239.95118,4.91704701955,21.199729105,5.38066929043,2.60202721757,
1.95275484686,0.763473061333,0.0066256,-4.36511056284e-05},le{
1.66401399799e+21,1662423.01281,4.55222772533,21.0436563843,
5.4128217747,2.59183053025,1.86763398198,0.806784113439,0.0065649,-
3.7928141205e-05},le{4.78872087411e+21,323815.222049,2.00728185241,
20.6222953605,5.51498491617,2.48274729869,1.72104741638,
0.825016527325,0.0065044,-5.3056483016e-05},le{1.11431733284e+22,
88306.4846515,3.02689484137,24.5394324977,5.62119113328,2.30632363875
,1.59139570297,0.809832449626,0.0064441,-5.10067899288e-05},le{
2.1467483623e+22,66857.74539,1.81239841328,21.2139576723,
5.67581661429,2.17637473548,1.5542171282,0.767797106242,0.006384,-
4.50770759898e-05},le{3.03087346701e+22,245951.077705,1.03713583045,
26.2526218185,5.73455089345,2.21027557819,1.64875137876,
0.734227174795,0.0063241,-5.00803389621e-05},le{4.10988590708e+22,
1267462.13502,0.570362374096,21.9323677239,5.77971601378,
2.34555635797,1.80519739425,0.724944535076,0.0062644,-
4.58401271204e-05},le{2.47550109362e+22,2401181.21523,2.06870253593,
26.6025610843,5.6836542421,2.46012924543,1.86668315037,0.727860138668
,0.0062049,-5.46223765275e-05},le{5.09745438073e+21,1295813.25068,
2.68413985791,18.765082944,5.4767812401,2.48441085215,1.84351492016,
0.749017042135,0.0061456,-4.40839881267e-05},le{9.53264159811e+20,
1273793.79349,4.04450415038,20.9411480072,5.29775344247,2.37402879555
,1.80976881891,0.784201553926,0.0060865,-4.30413573051e-05},le{
5.00886621257e+20,654997.951258,4.54776062685,21.6198813525,
5.20558028662,2.18188391562,1.74017339915,0.792347378204,0.0060276,-
3.20406072321e-05},le{6.38318441143e+20,350693.000742,3.86950635439,
21.1464707465,5.21889630676,2.04708460856,1.67881007955,0.76863945222
,0.0059689,-3.38654598586e-05},le{6.76824527473e+20,388313.655311,
4.29273073948,18.8457290445,5.23663791848,2.00910094401,1.72053329875
,0.750055536868,0.0059104,-3.00130231674e-05},le{5.27519557108e+20,
1395370.75036,1.37346258138,25.2976644765,5.21336489289,1.93907677528
,1.80741270252,0.752896801988,0.0058521,-5.03653594614e-05},le{
4.24032406187e+20,1387147.09661,1.25319881825,25.1594282591,
5.18580099943,1.81260481036,1.83665334036,0.750740469208,0.005794,-
4.99108661826e-05},le{7.40432430877e+20,1731186.82269,1.20699664838,
24.2410095938,5.24537421261,1.72919044337,1.84769529021,
0.742154632073,0.0057361,-4.79571150316e-05},le{3.53702785333e+21,
1296549.72386,1.41797975196,24.0219773005,5.42032229641,1.76379022909
,1.81346414965,0.744184015328,0.0056784,-4.67711580002e-05},le{
2.77773610086e+22,493483.981895,4.21067971479,15.6703613767,
5.61085009355,1.90832208452,1.70557676263,0.756992091208,0.0056209,-
2.27752187732e-05},le{3.66101160631e+22,295164.27301,3.5910145855,
17.0294442298,5.67029076271,2.10900400858,1.6233261335,0.753285727058
,0.0055636,-2.07577336328e-05},le{2.66407039403e+22,333379.555005,
2.26129253675,16.0621991637,5.62102584522,2.24881190183,1.63207079832
,0.743638675314,0.0055065,-2.58049619446e-05},le{1.00105475962e+22,
565928.906547,4.45972613332,19.0633020496,5.51171770059,2.32450129811
,1.69962250219,0.738250585904,0.0054496,-4.19822327871e-05},le{
1.81498777858e+21,1734900.42258,2.68297604694,15.3194641998,
5.36195538131,2.42473764446,1.79906929787,0.737991989173,0.0053929,-
2.89087289707e-05},le{1.12747490485e+21,2065125.04097,3.16029681519,
15.4911603298,5.27542872582,2.65325822518,1.90840588135,
0.755604694289,0.0053364,-3.19222563179e-05},}};}class l192{l192(
const l192&)=delete;l192&operator=(const l192&)=delete;public:using lc
=lv::l357::l346::le;using l235=std::array<lc,88>;constexpr l192()noexcept
=default;const lc*l381(int lk)const noexcept{if(lk<21||lk>108)return
nullptr;return&l236()[static_cast<std::size_t>(lk-21)];}const l235&
l754()const noexcept{return l236();}private:static constexpr const
l235&l236()noexcept{return lv::l357::l346::l598;}};
namespace lv::l460::l496{struct lg{double l769;};inline constexpr std
::array<lg,88>l720{{lg{12.3988206919},lg{12.8891815684},lg{
13.1332105294},lg{13.0839327734},lg{12.8890542964},lg{12.3334372967},
lg{11.5713724036},lg{10.6439459031},lg{9.67353066531},lg{
8.69475953779},lg{7.75861813737},lg{6.90352041365},lg{6.12894607314},
lg{5.42385158532},lg{4.83877578739},lg{4.35742291689},lg{
3.96601763469},lg{3.66669615581},lg{3.44305916601},lg{3.25519545620},
lg{2.48500589816},lg{2.42870645282},lg{2.36987715335},lg{
2.36555046436},lg{2.30916471578},lg{2.30054038444},lg{2.29927159747},
lg{2.29950268773},lg{2.29970769619},lg{2.29838304084},lg{
2.29704308482},lg{2.28984360988},lg{2.27965770670},lg{2.26948218038},
lg{2.25350012030},lg{2.23460609259},lg{2.21576495128},lg{
2.19482505542},lg{2.17249421243},lg{2.15023958991},lg{2.12661526438},
lg{2.10239760722},lg{2.08038575212},lg{2.05845122567},lg{
2.03659430135},lg{2.01832734084},lg{2.00011508361},lg{1.98336683563},
lg{1.96874177635},lg{1.95347555206},lg{1.94238693842},lg{
1.93063556045},lg{1.92096807626},lg{1.91131151156},lg{1.90510595470},
lg{1.89683187889},lg{1.89062632204},lg{1.88370791961},lg{
1.87749789350},lg{1.87263363374},lg{1.86775947868},lg{1.86154052370},
lg{1.85324858372},lg{1.84702962875},lg{1.83873768877},lg{
1.83044574880},lg{1.82084619918},lg{1.81125761501},lg{1.79831447070},
lg{1.78746145235},lg{1.77250647871},lg{1.75836059104},lg{
1.74425126497},lg{1.72685989245},lg{1.70622057731},lg{1.68895544985},
lg{1.66721274056},lg{1.64681393361},lg{1.62400957623},lg{
1.60055424045},lg{1.57796485708},lg{1.55227685138},lg{1.52476666739},
lg{1.49936735910},lg{1.47095957610},lg{1.44393685561},lg{
1.41467684717},lg{1.38608153033},}};}class l195{l195(const l195&)=
delete;l195&operator=(const l195&)=delete;public:using lc=lv::l460::
l496::lg;using l235=std::array<lc,88>;constexpr l195()noexcept=
default;const lc*l381(int lk)const noexcept{constexpr int l22=21;
constexpr int l94=108;if(lk<l22||lk>l94){return nullptr;}const std::
size_t lh=static_cast<std::size_t>(lk-l22);return&l236()[lh];}double
l450(int lk)const noexcept{const lc*l549=l381(lk);if(l549==nullptr){
return 0.0;}return l549->l769;}const l235&l754()const noexcept{return
l236();}private:static constexpr const l235&l236()noexcept{return lv
::l460::l496::l720;}};class l81{l81(const l81&)=delete;l81&operator=(
const l81&)=delete;public:static constexpr l47 l668{};static constexpr
l192 l50{};static constexpr l195 l775{};constexpr l81()noexcept=
default;};class l239{public:struct l476{int size=0;int l521=0;double
l7=0.0;double l334=0.0;int l135=0;int l177=0;float l188=1.0f;float
l222=0.0f;l476(int size,double l7):size(size),l7(l7){if(size<2)throw
std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c"
"\x3a\x20\x73\x69\x7a\x65\x20\x69\x73\x20\x74\x6f\x6f\x20\x73\x6d\x61"
"\x6c\x6c\x2c\x20\x73\x69\x7a\x65\x3a\x20"+std::to_string(size));if(
l7>1.0||l7<=0.0)throw std::runtime_error("\x73\x74\x72\x69\x6e\x67"
"\x5f\x6d\x6f\x64\x65\x6c\x3a\x20\x70\x6f\x73\x69\x74\x69\x6f\x6e\x20"
"\x69\x73\x6e\x27\x74\x20\x67\x6f\x6f\x64\x2c\x20\x70\x6f\x73\x69\x74"
"\x69\x6f\x6e\x3a\x20"+std::to_string(l7));l521=size-1;l334=l521*l7;
l135=std::ceil(l334);l177=l135-1;l188=l334-l177;l222=1-l188;}};public
:const l81*l335=nullptr;int l32=69;double l101=440.0;double l60=
44100.0;double l267=44100.0;l251 l233;l507 l172;double l696=0.0;
double l820=0.0;double l170=0.0;int l76=0;double l484=0.0;int l96=0;
std::vector<float>l0;std::vector<float>l1;int l44=0;int l38=0;float*
l158=nullptr;float*l274=nullptr;double l767=1.0/9.4;l476 l13;double
l248=0.0;l645 l361;bool l201=false;l493 l268;bool l128=false;double
l564=0.0;public:l239(double ln,int lk,l47::l45 l112,l47::l75 l739,
const l81*l46):l335(l46),l32(lk),l101(l46->l668.l683(l32,l112,l739)),
l60(ln),l267(l32>=92?2.0*l60:l60),l233(l32),l172(l101),l696(l233.l600
(l267,l101)),l170(l172.l806()/2.0),l76(static_cast<int>(std::floor(
l170))+1),l484(2.0* (l170-std::floor(l170))),l96(l76-1),l13(l76,l767),
l268(l484,2.0*std::numbers::pi_v<double> *l101/l267){if(l76<4)throw
std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c"
"\x3a\x20\x64\x65\x6c\x61\x79\x5f\x69\x6e\x74\x20\x69\x73\x20\x74\x6f"
"\x6f\x20\x73\x6d\x61\x6c\x6c\x3a\x20"+std::to_string(l76));if(l13.
l135+1>l96||l13.l135-1<0){throw std::runtime_error("\x73\x74\x72\x69"
"\x6e\x67\x5f\x6d\x6f\x64\x65\x6c\x3a\x20\x6e\x65\x78\x74\x5f\x69\x6e"
"\x64\x65\x78\x20\x64\x6f\x65\x73\x6e\x27\x74\x20\x65\x78\x69\x73\x74"
"\x3a\x20"+std::to_string(l76));}if(std::abs(l172.l770()-l267)>0.5){
throw std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64"
"\x65\x6c\x3a\x20\x64\x69\x73\x70\x65\x72\x73\x69\x6f\x6e\x20\x70\x72"
"\x65\x73\x65\x74\x20\x73\x61\x6d\x70\x6c\x65\x20\x72\x61\x74\x65\x20"
"\x6d\x69\x73\x6d\x61\x74\x63\x68");}l0.resize(l76,0.0f);l1.resize(
l76,0.0f);l44=0;l38=0;l158=&l0[l44];l274=&l1[l28(l96,l38)];l248=l46->
l775.l450(l32);l564=1.0/(2.0*l248);}inline void l750(){if(l44==l96)l44
=0;else l44++;if(l38==0)l38=l96;else l38--;l1[l28(0,l38)]=-l0[l28(0,
l44)];l0[l28(l96,l44)]=-l1[l28(l96,l38)];l158=&l1[l28(0,l38)];l274=&
l0[l28(l96,l44)];}inline int l28(int l355,int l382){return l355+l382
<=l96?l355+l382:l355+l382-l76;}inline void l419(double l85){const bool
l568=l85>0.0;if(l568){l727(l85*l564);}l750();filter();l340();if(l568){
l128=true;}}inline void l727(double l286){l0[l28(l13.l135,l44)]+=l286
 *l13.l188;l0[l28(l13.l177,l44)]+=l286*l13.l222;l1[l28(l13.l135,l38)]
+=l286*l13.l188;l1[l28(l13.l177,l38)]+=l286*l13.l222;}inline double
l423(){return-2.0*l248*static_cast<double>( *l274);}inline double l647
(){return l13.l188*l0[l28(l13.l135,l44)]+l13.l222*l0[l28(l13.l177,l44
)]+l13.l188*l1[l28(l13.l135,l38)]+l13.l222*l1[l28(l13.l177,l38)];}
inline void lx(){l44=0;l38=0;l128=false;l201=false;l344=0;l323=0;std
::fill(l0.begin(),l0.end(),0.0f);std::fill(l1.begin(),l1.end(),0.0f);
l158=&l0[l44];l274=&l1[l28(l96,l38)];l233.lx();l268.lx();l361.lx();
l172.lx();}private:inline void filter(){l268.l29( *l158);l233.l29( *
l158);l172.l29( *l158);if(l201){l361.l29( *l158);}}int l344=0;int l323
=0;inline void l340(){const int l608=l32>=92?128:64;constexpr int l781
=8;if(++l323<l608){return;}l323=0;constexpr float l542=1.0e-5f;
constexpr float l687=l542*l542;if(l705()<l687){if(++l344>=l781){l128=
false;lx();}}else{l344=0;l128=true;}}inline float l705(){float l353=
0.0f;for(int lh=0;lh<l76;++lh){l353+=l0[static_cast<std::size_t>(lh)]
 *l0[static_cast<std::size_t>(lh)]+l1[static_cast<std::size_t>(lh)] *
l1[static_cast<std::size_t>(lh)];}l353/=static_cast<float>(2*l76);
float l51=std::max(l233.l51(),l268.l51());l51=std::max(l51,
static_cast<float>(l172.l51()));if(l201){l51=std::max(l51,l361.l51());
}return std::max(l353,l51);}};
class l483{public:int l32=69;const lv::l357::l346::le*l50=nullptr;
double l155=0.0;double l74=0.0;double l69=0.0;double l110=0.0;double
l198=0.0;double l185=0.0;bool l238=false;double l463=0.0;public:l483(
double ln,int lk,const l81*l46):l155((1.0/2.0)/ln),l32(lk),l50(l46->
l50.l381(l32)){l463=l155/l50->l702;}inline double l529(double l175){
if(!l238)return 0.0;if(l74<0.0||l69<0.0){lx();return 0.0;}double l85=
l728(l175);if(!std::isfinite(l85)||l85<=0.0){lx();return 0.0;}if(l110
<0||l185<0.0){l185+=l110*l155;if(l185>0.0){l185=0.0;}}if(l185<l50->
l763){lx();return 0.0;}l74=l74+(l198-l175) *l155;l69=l69+(l110-l198) *
l155;constexpr double l445=1.0e-12;if(!std::isfinite(l74)||!std::
isfinite(l69)||l74<-l445||l69<-l445){lx();return 0.0;}l74=std::max(
0.0,l74);l69=std::max(0.0,l69);if(l85>0.0)l110-=l85*l463;return l85;}
inline void l322(double l782){if(l238)return;lx();l110=l782;l238=true
;}inline void lx(){l74=0.0;l69=0.0;l110=0.0;l198=0.0;l185=0.0;l238=
false;}private:inline double l559(double lm,double l482){return lm>=0
?std::pow(lm,l482):-std::pow(-lm,l482);}inline double l537(double l465
,double l783){return std::pow(l465>0.0?l465:0.0,l783);}inline double
l728(double l175){double l126=l175-l74/l155;double l139=l110+l69/l155
;if(!(l126<=l139)){return 0.0;}const double l498=l50->l759*l537(l74,
l50->l746);const double l503=l50->l756*l537(l69,l50->l745);const
double l240=l50->l240;const double l228=l50->l228;const double l205=
l50->l205;const double l204=l50->l204;const auto l467=[&](double l455
){const double l307=l498+l240*l559(l455-l175,l205);const double l399=
l503+l228*l559(l110-l455,l204);return l307-l399;};const double l530=
l467(l126);const double l510=l467(l139);if(!std::isfinite(l530)||!std
::isfinite(l510)||l530>0.0||l510<0.0){return 0.0;}double l147=std::
clamp(l198,l126,l139);constexpr int l681=8;constexpr double l612=
1.0e-10;constexpr double l669=1.0e-10;constexpr double l663=1.0e-14;
double l492=0.0;for(int ld=0;ld<l681;++ld){const double l551=l147-
l175;const double l548=l110-l147;const double l401=std::abs(l551);
const double l345=std::abs(l548);const double l471=std::pow(l401,l205
);const double l587=std::pow(l345,l204);const double l685=std::
copysign(l471,l551);const double l692=std::copysign(l587,l548);const
double l307=l498+l240*l685;const double l399=l503+l228*l692;const
double l316=l307-l399;if(!std::isfinite(l316)){return 0.0;}l492=l307;
if(l316>0.0){l139=l147;}else{l126=l147;}if(std::abs(l316)<=l612){
break;}if((l139-l126)<=l669){break;}double l209=0.0;if(l401>0.0){l209
+=l240*l205* (l471/l401);}if(l345>0.0){l209+=l228*l204* (l587/l345);}
double l263;if(std::isfinite(l209)&&l209>l663){const double l266=l147
-l316/l209;if(std::isfinite(l266)&&l266>l126&&l266<l139){l263=l266;}
else{l263=(l126+l139) *0.5;}}else{l263=(l126+l139) *0.5;}l147=l263;}
l198=l147;return l492;}};
class BridgeModel{public:BridgeModel(){}void l29()const{}};
class SoundboardModel{static constexpr bool l617=false;
SoundboardModel(const SoundboardModel&)=delete;SoundboardModel&
operator=(const SoundboardModel&)=delete;static constexpr std::size_t
l180=88;static constexpr std::size_t l132=8;static constexpr std::
size_t l571=721;static constexpr std::array<std::size_t,l132>l520{37,
87,181,271,359,492,687,721};struct l653{std::array<float,l571>data{};
std::size_t length=1;std::size_t lh=0;inline void l772(std::size_t
l594)noexcept{length=std::clamp<std::size_t>(l594,1,l571);lh=0;data.
fill(0.0f);}inline float read()const noexcept{return data[lh];}inline
void write(float lm)noexcept{data[lh]=lm;++lh;if(lh>=length){lh=0;}}
inline void reset()noexcept{data.fill(0.0f);lh=0;}};static constexpr
std::size_t l709=8;struct l624{float l88=1.0f;float l89=0.0f;float
l163=0.0f;float l65=0.0f;float l153=0.0f;float l283=0.0f;float l264=
0.0f;inline float l29(float lm)noexcept{const float l34=l88*lm+l283;
l283=l89*lm-l65*l34+l264;l264=l163*lm-l153*l34;return l34;}inline void
reset()noexcept{l283=0.0f;l264=0.0f;}};struct l251{std::array<l624,
l709>l109{};inline float l29(float lm)noexcept{for(auto&l26:l109){lm=
l26.l29(lm);}return lm;}inline void reset()noexcept{for(auto&l26:l109
){l26.reset();}}};struct l415{float l78=0.0f;float l300=1.0f;float
l282=1.0f;float l299=0.0f;inline float l29(float lm)noexcept{l299=l78
 *l299+(1.0f-l78) *lm;const float l280=l299;const float l430=lm-l280;
return l300*l280+l282*l430;}inline void reset()noexcept{l299=0.0f;}};
std::array<l653,l132>l276{};std::array<l251,l132>l306{};l415 l225{};
l415 l247{};l415 l224{};double l60=44100.0;public:mutable std::array<
float,l180>l121{};explicit SoundboardModel(double ln=44100.0):l60(ln){
l686();l697();l664();}inline float l219()noexcept{if(!l617){float l516
=0.0;for(int ld=0;ld<l121.size();ld++){l516+=l121[ld];}return l516;}
float l526=0.0f;float l489=0.0f;float l561=0.0f;for(std::size_t ld=0;
ld<20;++ld){l526+=l121[ld];}for(std::size_t ld=20;ld<55;++ld){l489+=
l121[ld];}for(std::size_t ld=55;ld<88;++ld){l561+=l121[ld];}const
float l443=l225.l29(l526)+l247.l29(l489)+l224.l29(l561);std::array<
float,l132>l80{};float l536=0.0f;for(std::size_t ld=0;ld<l132;++ld){
l80[ld]=l306[ld].l29(l276[ld].read());l536+=l80[ld];}constexpr float
l130=0.3535533905932738f;const float l593=l130* (l80[0]-l80[1]+l80[2]
-l80[3]+l80[4]-l80[5]+l80[6]-l80[7]);const float l667=0.25f*l536;
const float l203=l130*l443;for(std::size_t ld=0;ld<l132;++ld){const
std::size_t l808=(ld+1)&7;const float l703=l80[l808]-l667;l276[ld].
write(l703+l203);}constexpr float l643=0.18f;return l593+l643*l443;}
inline void lx()noexcept{l121.fill(0.0f);for(auto&l170:l276){l170.
reset();}for(auto&l675:l306){l675.reset();}l225.reset();l247.reset();
l224.reset();}private:inline void l686()noexcept{for(std::size_t ld=0
;ld<l132;++ld){l276[ld].l772(l520[ld]);}}inline void l697()noexcept{
constexpr double l804=0.34;constexpr float l552=0.075f;for(std::
size_t ld=0;ld<l132;++ld){const double l641=static_cast<double>(l520[
ld])/l60;const double l710=std::pow(10.0,-3.0*l641/l804);for(auto&l26
:l306[ld].l109){l26.l88=1.0f;l26.l89=0.0f;l26.l163=0.0f;l26.l65=0.0f;
l26.l153=0.0f;l26.l283=0.0f;l26.l264=0.0f;}auto&l26=l306[ld].l109[0];
l26.l88=static_cast<float>(l710) * (1.0f-l552);l26.l89=0.0f;l26.l163=
0.0f;l26.l65=-l552;l26.l153=0.0f;}}inline void l664()noexcept{
constexpr double l630=1800.0;const float l78=static_cast<float>(std::
exp(-2.0*3.14159265358979323846*l630/l60));l225.l78=l78;l225.l300=
1.00f;l225.l282=0.82f;l247.l78=l78;l247.l300=1.00f;l247.l282=1.00f;
l224.l78=l78;l224.l300=0.95f;l224.l282=1.28f;}};class l210{public:
double l636=0.0;int l32;int l56=3;const BridgeModel*l337;const
SoundboardModel*l144;l483 l104;std::array<l239,3>l19;std::array<
double,3>l270={0.0,0.0,0.0};bool l212=false;bool l113=false;bool l231
=false;const l81*l335=nullptr;double l255=0.0;double l478=0.0;double
l584=0.0;std::array<double,3>l243={0.0,0.0,0.0};std::array<float,3>
l420{};l210(int lk,double ln,int l102,l47::l45 l112,const
SoundboardModel*l665,const BridgeModel*l701,const l81*l46):l32(lk),
l56(l102),l636(ln),l104(ln,lk,l46),l19{l239(ln,lk,l112,l47::l75::l0,
l46),l239(ln,lk,l112,l47::l75::l331,l46),l239(ln,lk,l112,l47::l75::l1
,l46),},l144(l665),l337(l701),l335(l46){for(int ld=0;ld<l56;++ld){
l255+=l19[ld].l248;}if(l255>0.0){l478=1.0/l255;l584=1.0/(l255*2.0);
for(int ld=0;ld<l56;++ld){l243[ld]=l19[ld].l248*l478;}}}inline void
l700(){l650();double l377=0.0;for(int ld=0;ld<l56;++ld){l270[ld]=l19[
ld].l647();l377+=l243[ld] *l270[ld];}double l349=l104.l529(l377);
double l515=l104.l529(l377+l349*l584);double l85=(l349+l515)/2.0;for(
int ld=0;ld<l56;ld++){if(l19[ld].l32<92){l19[ld].l419(l85*l243[ld]);}
else{l19[ld].l419(l349*l243[ld]);l420[ld]=l19[ld].l423();l19[ld].l419
(l515*l243[ld]);}}l712();l340();if(!l113){l144->l121[l32-21]=0.0f;
l420.fill(0.0f);}}inline void l322(double l588){for(int ld=0;ld<l56;
ld++){l19[ld].l128=true;l270[ld]=0.0;}l104.l322(l588);}inline void
l712(){float l157=0.0;for(int ld=0;ld<l56;ld++){if(l19[ld].l32<92){
l157+=l19[ld].l423();}else{l157+=(l19[ld].l423()+l420[ld])/2.0;}}l144
->l121[l32-21]=l157;}inline void lx(){for(int ld=0;ld<l56;ld++){l19[
ld].lx();}l104.lx();l212=false;l113=false;l270.fill(0.0);l231=false;}
private:inline void l340(){l113=l104.l238;for(int ld=0;ld<l56;++ld){
l113=l113||l19[ld].l128;}}inline void l650(){const bool l596=!l212&&!
l231;for(int lh=0;lh<l56;++lh){l19[lh].l201=l596;}}};class l83{l83(
const l83&)=delete;l83&operator=(const l83&)=delete;public:static
constexpr l81 l46{};BridgeModel l337;SoundboardModel l144;std::array<
l210* ,88>l10;l83(double ln){for(std::size_t lh=0;lh<l10.size();++lh){
const int lk=static_cast<int>(lh)+21;int l102=1;if(lk>=34){l102=3;}
else if(lk>=29){l102=2;}l10[lh]=new l210(lk,ln,l102,l47::l45::l394,&
l144,&l337,&l46);}}inline void l356(){for(int ld=0;ld<l10.size();ld++
){if(l10[ld]->l113)l10[ld]->l700();}}inline float l219(){return 0.01*
l144.l219();}inline void l71(bool l128){if(l128){for(int ld=0;ld<l10.
size();ld++){l10[ld]->l231=true;}}else{for(int ld=0;ld<l10.size();ld
++){l10[ld]->l231=false;}}}inline void lx(){for(int ld=0;ld<l10.size(
);ld++){l10[ld]->lx();}l144.lx();}~l83()noexcept{for(int ld=0;ld<l10.
size();ld++){delete l10[ld];}}};
#include<AudioToolbox/AudioToolbox.h>
class l107{public:class l149{public:explicit l149(l107&l435)noexcept:
l576(&l435),l161(l435.l716()){}l149(const l149&)=delete;l149&operator
=(const l149&)=delete;~l149(){if(l161){l576->l693();}}explicit
operator bool()const noexcept{return l161;}private:l107*l576;bool l161
;};l107()noexcept=default;l107(const l107&)=delete;l107&operator=(
const l107&)=delete;void open()noexcept{l42.store(0,std::
memory_order_release);}void l567()noexcept{std::uint32_t l138=l42.
fetch_or(l179,std::memory_order_acq_rel)|l179;while((l138&l328)!=0){
l42.wait(l138,std::memory_order_acquire);l138=l42.load(std::
memory_order_acquire);}}bool l816()const noexcept{return(l42.load(std
::memory_order_acquire)&l179)==0;}private:static constexpr std::
uint32_t l179=0x80000000U;static constexpr std::uint32_t l328=~l179;
bool l716()noexcept{std::uint32_t l138=l42.load(std::
memory_order_acquire);while((l138&l179)==0){if((l138&l328)==l328){
return false;}if(l42.compare_exchange_weak(l138,l138+1,std::
memory_order_acq_rel,std::memory_order_acquire)){return true;}}return
false;}void l693()noexcept{l42.fetch_sub(1,std::memory_order_release);
l42.notify_all();}std::atomic<std::uint32_t>l42{l179};};class l31{l31
(l31&&)=delete;l31&operator=(l31&&)=delete;AudioUnit l49=nullptr;l107
l256;bool l91=false;public:l31(const l31&)=delete;l31&operator=(const
l31&)=delete;explicit l31(double ln){try{AudioComponentDescription
l187{};l187.componentType=kAudioUnitType_Output;l187.componentSubType
=kAudioUnitSubType_DefaultOutput;l187.componentManufacturer=
kAudioUnitManufacturer_Apple;l187.componentFlags=0;l187.
componentFlagsMask=0;AudioComponent l574=AudioComponentFindNext(
nullptr,&l187);if(l574==nullptr){throw std::runtime_error("\x46\x61"
"\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e\x64\x20\x61\x75\x64\x69"
"\x6f\x20\x63\x6f\x6d\x70\x6f\x6e\x65\x6e\x74\x2e");}OSStatus l671=
AudioComponentInstanceNew(l574,&l49);if(l671!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}
AudioStreamBasicDescription l23{};l23.mSampleRate=ln;l23.mFormatID=
kAudioFormatLinearPCM;l23.mFormatFlags=
kAudioFormatFlagsNativeFloatPacked;l23.mBitsPerChannel=32;l23.
mChannelsPerFrame=1;l23.mBytesPerFrame=sizeof(Float32) *l23.
mChannelsPerFrame;l23.mFramesPerPacket=1;l23.mBytesPerPacket=l23.
mBytesPerFrame*l23.mFramesPerPacket;l23.mReserved=0;OSStatus l757=
AudioUnitSetProperty(l49,kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Input,0,&l23,sizeof(l23));if(l757!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74"
"\x20\x73\x74\x72\x65\x61\x6d\x20\x66\x6f\x72\x6d\x61\x74\x2e");}
AURenderCallbackStruct l167{};l167.inputProc=&l31::l714;l167.
inputProcRefCon=this;OSStatus l744=AudioUnitSetProperty(l49,
kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&l167,
sizeof(l167));if(l744!=noErr){throw std::runtime_error("\x46\x61\x69"
"\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74\x20\x72\x65\x6e\x64\x65\x72"
"\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x2e");}}catch(...){
AudioComponentInstanceDispose(l49);l49=nullptr;throw;}}static OSStatus
l714(void*l719,AudioUnitRenderActionFlags*l98,const AudioTimeStamp*
l824,UInt32 l829,UInt32 l589,AudioBufferList*l342)noexcept{auto*l146=
static_cast<l31* >(l719);if(l146==nullptr||l342==nullptr){return
kAudio_ParamError;}l107::l149 l613(l146->l256);if(!l613){l298(l98,
l342);return noErr;}return l146->l522(l98,l589,l342);}OSStatus l522(
AudioUnitRenderActionFlags*l98,UInt32 l259,AudioBufferList*data)noexcept
;void l72(){if(l91)return;OSStatus l6=AudioUnitInitialize(l49);if(l6
!=noErr){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x20\x61\x75\x64\x69"
"\x6f\x20\x75\x6e\x69\x74\x2e");}l256.open();l6=AudioOutputUnitStart(
l49);if(l6!=noErr){l256.l567();AudioUnitUninitialize(l49);throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x74\x61"
"\x72\x74\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}l91=true
;}void l67()noexcept{if(!l91)return;l256.l567();AudioOutputUnitStop(
l49);AudioUnitUninitialize(l49);l91=false;}static void l298(
AudioUnitRenderActionFlags*l98,AudioBufferList*data)noexcept;~l31()noexcept
{l67();if(l49!=nullptr){AudioComponentInstanceDispose(l49);l49=
nullptr;}}};
#include<mach/mach_time.h>
class l338{private:uint64_t l444=0;double l60=44100.0;double l120=0.0
;public:explicit l338(double ln):l60(ln){}inline void l690()noexcept{
l444=clock_gettime_nsec_np(CLOCK_UPTIME_RAW);}inline void l622(int
l468)noexcept{const uint64_t end=clock_gettime_nsec_np(
CLOCK_UPTIME_RAW);const uint64_t l376=end-l444;const double l724=
1000000000.0*static_cast<double>(l468)/l60;const double l662=
static_cast<double>(l376)/l724;l120=0.9*l120+0.1*l662;}[[nodiscard]]
inline double l810()const noexcept{return l120;}[[nodiscard]]inline
double l826()const noexcept{return l120*100.0;}[[nodiscard]]inline
double l812()const noexcept{if(l120<=0.0){return 0.0;}return 1.0/l120
;}[[nodiscard]]inline bool realtime_capable()const noexcept{return
l120<=1.0;}[[nodiscard]]double l644(std::unique_ptr<l83>&l95,int l673
=512,int l499=8192,int l242=5)const{constexpr int l511=31;l242=std::
clamp(l242,1,l511);std::array<double,l511>l313{};volatile float l277=
0.0f;for(int l327=0;l327<l242;++l327){for(int l61=0;l61<l673;++l61){
l95->l356();l277=l277+l95->l219();}const uint64_t l72=
clock_gettime_nsec_np(CLOCK_UPTIME_RAW);for(int l61=0;l61<l499;++l61){
l95->l356();l277=l277+l95->l219();}const uint64_t end=
clock_gettime_nsec_np(CLOCK_UPTIME_RAW);const double l376=static_cast
<double>(end-l72);const double l733=1000000000.0*static_cast<double>(
l499)/l60;l313[l327]=l376/l733;}std::sort(l313.begin(),l313.begin()+
l242);return l313[static_cast<std::size_t>(l242/2)];}inline void reset
()noexcept{l120=0.0;}void l303(std::unique_ptr<l83>&l95,std::string
l800)const{const double l319=l644(l95);const double l534=1000000.0/
l60;const double l654=l534*l319;std::cout<<std::fixed<<std::
setprecision(2)<<"\x20\x20\x20\x20\x46\x72\x61\x6d\x65\x3a\x20"<<l654
<<"\x20\x75\x73\x20\x2f\x20"<<l534<<"\x20\x75\x73\n"<<"\x20\x20\x20"
"\x20\x4f\x63\x63\x75\x70\x61\x6e\x63\x79\x3a\x20"<<l319*100.0<<"\x25"
"\n"<<"\x20\x20\x20\x20\x53\x70\x65\x65\x64\x3a\x20"<<(1.0/l319)<<""
"\x78\x20\x72\x65\x61\x6c\x74\x69\x6d\x65\n"<<"\x20\x20\x20\x20"+l800
+"\x20\x72\x65\x61\x6c\x74\x69\x6d\x65\x3a\x20"<<(l319<=1.0?"\x50\x41"
"\x53\x53":"\x46\x41\x49\x4c")<<"\n\n";}};
#include<type_traits>
enum class l5:std::uint8_t{l52,l106,l190,l566,l475,l540,l451,l63,};
struct l93{l5 l15=l5::l63;int lr=0;double lt=0.0;};template<std::
size_t l124>class l171{static_assert(l124>=2&&(l124&(l124-1))==0);
static_assert(std::is_trivially_copyable_v<l93>);public:l171()noexcept
{for(std::size_t lh=0;lh<l124;++lh){l375[lh].l105.store(lh,std::
memory_order_relaxed);}}l171(const l171&)=delete;l171&operator=(const
l171&)=delete;bool l741(l93 l2)noexcept{std::size_t l7=l391.load(std
::memory_order_relaxed);for(;;){l371&l25=l375[l7&(l124-1)];const std
::size_t l105=l25.l105.load(std::memory_order_acquire);const auto l312
=static_cast<std::intptr_t>(l105)-static_cast<std::intptr_t>(l7);if(
l312==0){if(l391.compare_exchange_weak(l7,l7+1,std::
memory_order_relaxed)){l25.l2=l2;l25.l105.store(l7+1,std::
memory_order_release);return true;}}else if(l312<0){return false;}
else{l7=l391.load(std::memory_order_relaxed);}}}bool l383(l93&l2)noexcept
{l371&l25=l375[l309&(l124-1)];const std::size_t l105=l25.l105.load(
std::memory_order_acquire);const auto l312=static_cast<std::intptr_t>
(l105)-static_cast<std::intptr_t>(l309+1);if(l312!=0){return false;}
l2=l25.l2;l25.l105.store(l309+l124,std::memory_order_release);++l309;
return true;}void clear()noexcept{l93 l2;while(l383(l2)){}}private:
struct l371{std::atomic<std::size_t>l105{0};l93 l2{};};alignas(64)std
::array<l371,l124>l375{};alignas(64)std::atomic<std::size_t>l391{0};
alignas(64)std::size_t l309=0;};class PianoCommandBus{public:bool l784
(l93 l2)noexcept{if(l293.l741(l2)){return true;}l412.store(true,std::
memory_order_release);return false;}template<class l611>void l658(
l611&&l472){l93 l2;if(l412.exchange(false,std::memory_order_acq_rel)){
while(l293.l383(l2)){}l472(l93{l5::l63,0,0.0});}while(l293.l383(l2)){
l472(l2);}}void clear()noexcept{l293.clear();l412.store(false,std::
memory_order_release);}private:l171<4096>l293;std::atomic<bool>l412{
false};};std::unique_ptr<l83>ly;std::unique_ptr<l31>l146;std::
unique_ptr<l338>l84;namespace{constexpr int l191=21;constexpr int l429
=108;PianoCommandBus l165;std::atomic<bool>l347{false};std::atomic<
std::uint32_t>l258{0};bool l58(int l253)noexcept{return l253>=l191&&
l253<=l429;}void l474()noexcept{l258.fetch_add(1,std::
memory_order_release);l258.notify_all();}void l123(l5 l15,int l253,
double lt)noexcept{static_cast<void>(l165.l784({l15,l253,lt}));}void
l214(const l93&l2){if(!ly){throw std::logic_error("\x50\x69\x61\x6e"
"\x6f\x20\x65\x6e\x67\x69\x6e\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x69"
"\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x2e");}switch(l2.l15){case
l5::l52:{if(!l58(l2.lr))break;l210*l68=ly->l10[l2.lr-l191];l68->l212=
true;for(int ld=0;ld<l68->l56;++ld){l68->l19[ld].l201=false;}l68->
l113=true;const double l793=std::pow(2,(l2.lt-52.0)/25.0);l68->l322(
l793);break;}case l5::l106:{if(l58(l2.lr)){ly->l10[l2.lr-l191]->l212=
false;}l210*l68=ly->l10[l2.lr-l191];l68->l212=false;if(!l68->l231){
for(int ld=0;ld<l68->l56;++ld){l68->l19[ld].l201=true;}}l68->l113=
true;break;}case l5::l451:ly->l71(l2.lt>0.01);break;case l5::l63:ly->
lx();break;case l5::l190:case l5::l566:case l5::l475:case l5::l540:
break;}}}void l579(double ln){ly=std::make_unique<l83>(ln);}void
bbpiano_shutdown()noexcept{ly.reset();}void l494(double ln){l84=std::
make_unique<l338>(ln);}void l504()noexcept{l84.reset();}void l438(
double ln){l146=std::make_unique<l31>(ln);l146->l72();}void l379()noexcept
{l146.reset();}void l359(float*out,int l358,double l795){if(out==
nullptr||l358<0){throw std::invalid_argument("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x61\x75\x64\x69\x6f\x20\x6f\x75\x74\x70\x75\x74\x20\x62"
"\x75\x66\x66\x65\x72\x2e");}if(!ly||!l84){throw std::logic_error(""
"\x50\x69\x61\x6e\x6f\x20\x65\x6e\x67\x69\x6e\x65\x20\x69\x73\x20\x6e"
"\x6f\x74\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x2e");}l165
.l658([](const l93&l2){l214(l2);});l84->l690();for(int ld=0;ld<l358;
++ld){ly->l356();float lm=ly->l219();if(!std::isfinite(lm)){std::cerr
<<"\x4f\x55\x54\x50\x55\x54\x20\x41\x42\x4e\x4f\x52\x4d\x41\x4c\n"<<""
"\x73\x61\x6d\x70\x6c\x65\x20\x3d\x20"<<lm<<'\n'<<"\x66\x72\x61\x6d"
"\x65\x20\x3d\x20"<<ld<<'\n';for(int ld=0;ld<ly->l10.size();ld++){if(
ly->l10[ld]->l113){std::cout<<"\x68\x61\x6d\x6d\x65\x72\x5f\x2e\x77"
"\x5f\x61\x5f\x31\x5f"<<ly->l10[ld]->l104.l74<<"\n";std::cout<<"\x68"
"\x61\x6d\x6d\x65\x72\x5f\x2e\x77\x5f\x62\x5f\x31\x5f"<<ly->l10[ld]->
l104.l69<<"\n";std::cout<<"\x68\x61\x6d\x6d\x65\x72\x5f\x2e\x6d\x69"
"\x64\x64\x6c\x65\x5f\x76\x5f"<<ly->l10[ld]->l104.l198<<"\n";}}std::
cout<<"\n";lm=0.0f;}out[ld]=lm*l795;}l84->l622(l358);}void l52(int lk
,double l82)noexcept{if(l58(lk)){l123(l5::l52,lk,l82);}}void l106(int
lk,double l82)noexcept{if(l58(lk)){l123(l5::l106,lk,l82);}}void l190(
int lk,double l519)noexcept{if(l58(lk)){l123(l5::l190,lk,l519);}}void
l127(double l11)noexcept{l123(l5::l566,0,l11);}void l125(double l11)noexcept
{l123(l5::l475,0,l11);}void l119(double l11)noexcept{l123(l5::l540,0,
l11);}void l71(double l11)noexcept{l123(l5::l451,0,l11);}void l63()noexcept
{l123(l5::l63,0,0.0);}void l578()noexcept{l165.clear();if(ly){ly->lx(
);}}void l392()noexcept{l347.store(true,std::memory_order_release);
l474();}bool l207()noexcept{return l347.load(std::
memory_order_acquire);}void l525()noexcept{l347.store(false,std::
memory_order_release);}bool l523(std::stop_token stop_token)noexcept{
std::stop_callback l557(stop_token,[]{l474();});while(!stop_token.
stop_requested()&&!l207()){const std::uint32_t l610=l258.load(std::
memory_order_acquire);if(stop_token.stop_requested()||l207()){break;}
l258.wait(l610,std::memory_order_acquire);}return l207();}void l458(){
if(!ly||!l84){throw std::logic_error("\x50\x69\x61\x6e\x6f\x20\x65"
"\x6e\x67\x69\x6e\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x69\x74"
"\x69\x61\x6c\x69\x7a\x65\x64\x2e");}l165.clear();ly->lx();for(int lo
=79;lo<=l429;++lo){l214({l5::l52,lo,110.0});}l84->l303(ly,"\x48\x69"
"\x67\x68\x2d\x72\x61\x6e\x67\x65");l165.clear();ly->lx();for(int lo=
48;lo<=78;++lo){l214({l5::l52,lo,110.0});}l84->l303(ly,"\x4d\x69\x64"
"\x2d\x72\x61\x6e\x67\x65");l165.clear();ly->lx();for(int lo=l191;lo
<=47;++lo){l214({l5::l52,lo,110.0});}l84->l303(ly,"\x42\x61\x73\x73"
"\x2d\x72\x61\x6e\x67\x65");l165.clear();ly->lx();for(int lo=l191;lo
<=l429;++lo){l214({l5::l52,lo,110.0});}l84->l303(ly,"\x38\x38\x2d\x6b"
"\x65\x79\x73");ly->lx();}
#include<cstring>
OSStatus l31::l522(AudioUnitRenderActionFlags*l98,UInt32 l259,
AudioBufferList*data)noexcept{if(data==nullptr||data->mNumberBuffers
!=1||data->mBuffers[0].mData==nullptr||data->mBuffers[0].
mNumberChannels!=1||data->mBuffers[0].mDataByteSize<l259*sizeof(
Float32)||l259>static_cast<UInt32>(std::numeric_limits<int>::max())){
l392();l298(l98,data);return noErr;}AudioBuffer&l137=data->mBuffers[0
];auto*l699=static_cast<Float32* >(l137.mData);try{l359(l699,
static_cast<int>(l259),0.90);return noErr;}catch(...){l392();l298(l98
,data);return noErr;}}void l31::l298(AudioUnitRenderActionFlags*l98,
AudioBufferList*data)noexcept{if(data!=nullptr){for(UInt32 ld=0;ld<
data->mNumberBuffers;++ld){AudioBuffer&l137=data->mBuffers[ld];if(
l137.mData!=nullptr){std::memset(l137.mData,0,l137.mDataByteSize);}}}
if(l98!=nullptr){ *l98|=kAudioUnitRenderAction_OutputIsSilence;}}
#include<csignal>
#include<cstdlib>
#include<exception>
#include<getopt.h>
#include<pthread.h>
#include<signal.h>
namespace{void l87(const char*l17,const char*l30);void l199();void
l581(bool l173);void l333();class l162{public:explicit l162(bool l173
){l581(l173);}l162(const l162&)=delete;l162&operator=(const l162&)=
delete;~l162(){l333();}};}int l470(int l226,char*l14[],const char*l17
,const char*l30,std::stop_token l237){sigset_t l159;sigemptyset(&l159
);sigaddset(&l159,SIGINT);sigaddset(&l159,SIGTERM);sigaddset(&l159,
SIGUSR1);const int l595=pthread_sigmask(SIG_BLOCK,&l159,nullptr);if(
l595!=0){std::cerr<<"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x62\x6c"
"\x6f\x63\x6b\x20\x70\x72\x6f\x63\x65\x73\x73\x20\x73\x69\x67\x6e\x61"
"\x6c\x73\x2e\n";return EXIT_FAILURE;}static const option l635[]={{""
"\x68\x65\x6c\x70",no_argument,nullptr,'h'},{"\x76\x65\x72\x73\x69"
"\x6f\x6e",no_argument,nullptr,'v'},{"\x6d\x69\x64\x69",
required_argument,nullptr,'m'},{"\x70\x69\x61\x6e\x6f",no_argument,
nullptr,'p'},{"\x6b\x65\x79\x62\x6f\x61\x72\x64",no_argument,nullptr,
'k'},{"\x65\x78\x70\x6f\x72\x74",required_argument,nullptr,'e'},{""
"\x72\x65\x63\x6f\x72\x64",no_argument,nullptr,'r'},{"\x74\x65\x73"
"\x74",no_argument,nullptr,'t'},{"\x69\x6e\x74\x65\x72\x6e\x61\x6c"
"\x2d\x74\x65\x73\x74",no_argument,nullptr,'i'},{nullptr,0,nullptr,0}
};int l497;struct l762{bool l199=false;bool l17=false;std::optional<
std::string>lo=std::nullopt;bool l95=false;bool l79=false;std::
optional<std::string>l213=std::nullopt;bool l332=false;bool l252=
false;bool l321=false;};l762 lw;while((l497=getopt_long(l226,l14,""
"\x68\x76\x6d\x3a\x70\x6b\x65\x3a\x72\x74\x69",l635,nullptr))!=-1){
switch(l497){case'h':lw.l199=true;break;case'v':lw.l17=true;break;
case'm':lw.lo=optarg;break;case'p':lw.l95=true;break;case'k':lw.l79=
true;break;case'e':lw.l213=optarg;break;case'r':lw.l332=true;break;
case't':lw.l252=true;break;case'i':lw.l321=true;break;default:return
EXIT_FAILURE;}}if(!(lw.l199||lw.l17||lw.lo||lw.l95||lw.l79||lw.l213||
lw.l332||lw.l252||lw.l321)){l87(l17,l30);return EXIT_FAILURE;}if(lw.
l199){l199();return EXIT_SUCCESS;}if(lw.l17){l87(l17,l30);return
EXIT_SUCCESS;}std::stop_source l86;const std::stop_token stop_token=
l86.get_token();std::stop_callback external_stop_callback(l237,[&l86]
{l86.request_stop();});const bool l173=!lw.l213.has_value()&&!lw.l252
;l162 l814(l173);std::jthread l513([l159,&l86](std::stop_token l614){
const pthread_t l479=pthread_self();std::stop_callback l821(l614,[
l479]{static_cast<void>(pthread_kill(l479,SIGUSR1));});int l555=0;if(
sigwait(&l159,&l555)==0&&l555!=SIGUSR1){l86.request_stop();}});std::
jthread l758([&l86]{const bool l621=l523(l86.get_token());if(l621&&!
l86.stop_requested()){l86.request_stop();}});std::exception_ptr l431;
try{if(lw.lo){l87(l17,l30);l473( *lw.lo,stop_token);}else if(lw.l95){
l87(l17,l30);l502(stop_token);}else if(lw.l79){l87(l17,l30);l441(
stop_token);}else if(lw.l213){l87(l17,l30);l565( *lw.l213,stop_token);
}else if(lw.l332){l87(l17,l30);l527(stop_token);}else if(lw.l252){l87
(l17,l30);l501(stop_token);}else if(lw.l321){l87(l17,l30);l518(
stop_token);}}catch(...){l431=std::current_exception();}l86.
request_stop();l513.request_stop();l513.join();l758.join();l379();
const bool l747=l207();if(l431){try{std::rethrow_exception(l431);}
catch(const std::exception&l652){std::cerr<<"\x62\x62\x70\x6c\x3a\x20"
<<l652.what()<<'\n';}catch(...){std::cerr<<"\x62\x62\x70\x6c\x3a\x20"
"\x75\x6e\x6b\x6e\x6f\x77\x6e\x20\x73\x65\x72\x76\x69\x63\x65\x20\x66"
"\x61\x69\x6c\x75\x72\x65\x2e\n";}return EXIT_FAILURE;}if(l747){std::
cerr<<"\x54\x68\x65\x20\x61\x75\x64\x69\x6f\x20\x72\x65\x6e\x64\x65"
"\x72\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x20\x66\x61\x69\x6c\x65\x64"
"\x2e\n";return EXIT_FAILURE;}return EXIT_SUCCESS;}namespace{void l87
(const char*l17,const char*l30){std::cout<<l30;}void l199(){std::cout
<<"\n\x20\x20\x20\x20\x62\x62\x70\x6c\x20\x2d\x20\x62\x42\x70\x69\x61"
"\x6e\x6f\x20\x70\x68\x79\x73\x69\x63\x61\x6c\x2d\x6d\x6f\x64" "\x65"
"\x6c\x69\x6e\x67\x20\x70\x69\x61\x6e\x6f\x20\x65\x6e\x67\x69\x6e\x65"
"\n\n\x20\x20\x20\x20\x55\x73\x61\x67\x65\x3a\n\x20" "\x20\x20\x20"
"\x20\x20\x62\x62\x70\x6c\x20\x3c\x63\x6f\x6d\x6d\x61\x6e\x64\x3e\n"
"\n\x20\x20\x20\x20\x50\x6c\x61\x79\x62\x61\x63" "\x6b\x3a\n\x20\x20"
"\x20\x20\x20\x20\x2d\x6b\x2c\x20\x2d\x2d\x6b\x65\x79\x62\x6f\x61\x72"
"\x64\x20\x20\x20\x20\x20\x20\x20\x20\x20" "\x20\x50\x6c\x61\x79\x20"
"\x75\x73\x69\x6e\x67\x20\x74\x68\x65\x20\x63\x6f\x6d\x70\x75\x74\x65"
"\x72\x20\x6b\x65\x79\x62\x6f\x61\x72" "\x64\n\x20\x20\x20\x20\x20"
"\x20\x2d\x70\x2c\x20\x2d\x2d\x70\x69\x61\x6e\x6f\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20" "\x50\x6c\x61\x79\x20\x75\x73\x69"
"\x6e\x67\x20\x61\x20\x63\x6f\x6e\x6e\x65\x63\x74\x65\x64\x20\x4d\x49"
"\x44\x49\x20\x6b\x65\x79\x62" "\x6f\x61\x72\x64\n\x20\x20\x20\x20"
"\x20\x20\x2d\x6d\x2c\x20\x2d\x2d\x6d\x69\x64\x69\x20\x3c\x66\x69\x6c"
"\x65\x2e\x6d\x69\x64\x3e" "\x20\x20\x20\x50\x6c\x61\x79\x20\x61\x20"
"\x53\x74\x61\x6e\x64\x61\x72\x64\x20\x4d\x49\x44\x49\x20\x46\x69\x6c"
"\x65\n\n\x20\x20" "\x20\x20\x52\x65\x63\x6f\x72\x64\x69\x6e\x67\x20"
"\x61\x6e\x64\x20\x65\x78\x70\x6f\x72\x74\x3a\n\x20\x20\x20\x20\x20"
"\x20\x2d\x72" "\x2c\x20\x2d\x2d\x72\x65\x63\x6f\x72\x64\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x50\x6c\x61\x79\x20\x66\x72\x6f"
"\x6d\x20" "\x61\x20\x4d\x49\x44\x49\x20\x6b\x65\x79\x62\x6f\x61\x72"
"\x64\x20\x61\x6e\x64\x20\x72\x65\x63\x6f\x72\x64\x20\x74\x6f\x20\x4d"
"\x49" "\x44\x49\n\x20\x20\x20\x20\x20\x20\x2d\x65\x2c\x20\x2d\x2d"
"\x65\x78\x70\x6f\x72\x74\x20\x3c\x66\x69\x6c\x65\x2e\x6d\x69\x64\x3e" ""
"\x20\x52\x65\x6e\x64\x65\x72\x20\x61\x20\x4d\x49\x44\x49\x20\x66\x69"
"\x6c\x65\x20\x74\x6f\x20\x57\x41\x56\n\n\x20\x20\x20\x20" "\x44\x69"
"\x61\x67\x6e\x6f\x73\x74\x69\x63\x73\x3a\n\x20\x20\x20\x20\x20\x20"
"\x2d\x74\x2c\x20\x2d\x2d\x74\x65\x73\x74\x20\x20\x20" "\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x52\x75\x6e\x20\x74\x68\x65\x20\x73"
"\x65\x72\x76\x69\x63\x65\x2d\x6c\x69\x66\x65\x63" "\x79\x63\x6c\x65"
"\x20\x73\x6d\x6f\x6b\x65\x20\x74\x65\x73\x74\n\x20\x20\x20\x20\x20"
"\x20\x2d\x69\x2c\x20\x2d\x2d\x69\x6e\x74\x65" "\x72\x6e\x61\x6c\x2d"
"\x74\x65\x73\x74\x20\x20\x20\x20\x20\x50\x6c\x61\x79\x20\x74\x68\x65"
"\x20\x62\x75\x69\x6c\x74\x2d\x69\x6e\x20" "\x41\x34\x20\x65\x6e\x67"
"\x69\x6e\x65\x20\x74\x65\x73\x74\n\n\x20\x20\x20\x20\x47\x65\x6e\x65"
"\x72\x61\x6c\x3a\n\x20\x20\x20" "\x20\x20\x20\x2d\x68\x2c\x20\x2d"
"\x2d\x68\x65\x6c\x70\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x53\x68\x6f\x77\x20" "\x74\x68\x69\x73\x20\x68\x65\x6c\x70"
"\x20\x61\x6e\x64\x20\x65\x78\x69\x74\n\x20\x20\x20\x20\x20\x20\x2d"
"\x76\x2c\x20\x2d\x2d\x76" "\x65\x72\x73\x69\x6f\x6e\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x53\x68\x6f\x77\x20\x76\x65\x72\x73\x69"
"\x6f\x6e\x20\x69\x6e" "\x66\x6f\x72\x6d\x61\x74\x69\x6f\x6e\x20\x61"
"\x6e\x64\x20\x65\x78\x69\x74\n\n\x20\x20\x20\x20\x45\x78\x61\x6d\x70"
"\x6c\x65\x73" "\x3a\n\x20\x20\x20\x20\x20\x20\x62\x62\x70\x6c\x20"
"\x2d\x2d\x6b\x65\x79\x62\x6f\x61\x72\x64\n\x20\x20\x20\x20\x20\x20"
"\x62\x62" "\x70\x6c\x20\x2d\x2d\x70\x69\x61\x6e\x6f\n\x20\x20\x20"
"\x20\x20\x20\x62\x62\x70\x6c\x20\x2d\x2d\x72\x65\x63\x6f\x72\x64\n"
"\x20" "\x20\x20\x20\x20\x20\x62\x62\x70\x6c\x20\x2d\x2d\x6d\x69\x64"
"\x69\x20\"\x70\x65\x72\x66\x6f\x72\x6d\x61\x6e\x63\x65\x2e\x6d\x69" ""
"\x64\"\n\x20\x20\x20\x20\x20\x20\x62\x62\x70\x6c\x20\x2d\x2d\x65\x78"
"\x70\x6f\x72\x74\x20\"\x70\x65\x72\x66\x6f\x72\x6d\x61" "\x6e\x63"
"\x65\x2e\x6d\x69\x64\"\n\n\x20\x20\x20\x20\x4e\x6f\x74\x65\x73\x3a"
"\n\x20\x20\x20\x20\x20\x20\x53\x70\x65\x63\x69" "\x66\x79\x20\x65"
"\x78\x61\x63\x74\x6c\x79\x20\x6f\x6e\x65\x20\x63\x6f\x6d\x6d\x61\x6e"
"\x64\x20\x70\x65\x72\x20\x69\x6e\x76\x6f\x63" "\x61\x74\x69\x6f\x6e"
"\x2e\n\x20\x20\x20\x20\x20\x20\x50\x72\x65\x73\x73\x20\x43\x74\x72"
"\x6c\x2d\x43\x20\x74\x6f\x20\x73\x74\x6f" "\x70\x20\x61\x20\x72\x75"
"\x6e\x6e\x69\x6e\x67\x20\x73\x65\x72\x76\x69\x63\x65\x20\x63\x6c\x65"
"\x61\x6e\x6c\x79\x2e\n\x20\x20\x20" "\x20\x20\x20\x2d\x2d\x72\x65"
"\x63\x6f\x72\x64\x20\x63\x72\x65\x61\x74\x65\x73\x20\x61\x20\x74\x69"
"\x6d\x65\x73\x74\x61\x6d\x70\x65" "\x64\x20\x4d\x49\x44\x49\x20\x66"
"\x69\x6c\x65\x20\x69\x6e\x20\x74\x68\x65\x20\x63\x75\x72\x72\x65\x6e"
"\x74\x20\x64\x69\x72\x65\x63" "\x74\x6f\x72\x79\x2e\n\x20\x20\x20"
"\x20\x20\x20\x2d\x2d\x65\x78\x70\x6f\x72\x74\x20\x77\x72\x69\x74\x65"
"\x73\x20\x3c\x66\x69\x6c" "\x65\x2e\x77\x61\x76\x3e\x20\x62\x65\x73"
"\x69\x64\x65\x20\x74\x68\x65\x20\x69\x6e\x70\x75\x74\x20\x66\x69\x6c"
"\x65\x20\x77\x69\x74" "\x68\x6f\x75\x74\x20\x6f\x70\x65\x6e\x69\x6e"
"\x67\n\x20\x20\x20\x20\x20\x20\x74\x68\x65\x20\x61\x75\x64\x69\x6f"
"\x20\x6f\x75\x74" "\x70\x75\x74\x20\x64\x65\x76\x69\x63\x65\x2e\x20"
"\x45\x78\x69\x73\x74\x69\x6e\x67\x20\x6f\x75\x74\x70\x75\x74\x20\x66"
"\x69\x6c\x65" "\x73\x20\x61\x72\x65\x20\x6e\x6f\x74\x20\x6f\x76\x65"
"\x72\x77\x72\x69\x74\x74\x65\x6e\x2e\n\n\x20\x20\x20\x20";}void l581
(bool l173){l525();try{l579(ln);l494(ln);if(l173){l438(ln);}}catch(
...){l333();throw;}}void l333(){l379();l578();l504();bbpiano_shutdown
();}}extern "C" {void midi_service_start(const char*l40);void
midi_service_stop(void);void piano_service_start(void);void
piano_service_stop(void);void keyboard_service_start(void);void
keyboard_service_stop(void);void export_service_start(const char*l40);
void export_service_stop(void);void record_service_start(void);void
record_service_stop(void);void test_service_start(void);void
test_service_stop(void);void internal_test_service_start(void);void
internal_test_service_stop(void);void close_all_service(void);}
namespace{static constexpr const char*l145="\x4c\x31\x2d\x43\x6c\x61"
"\x76\x69\x65\x72";static constexpr const char*l156="\n\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x8c\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2" "\x94\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80" "\xe2\x94\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94" "\x80\xe2\x94\x80\xe2\x94\x90"
"\xe2\x94\x80\xe2\x94\x80\xe2\x95\xad\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x95\xae\n" "\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\xe2\x94\x82\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb2\xe2\x95\xb1\xe2"
"\x95\xb2\xe2\x95\xb1\xe2" "\x94\x82\n\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\xe2\x94\x82\x20\x20\x62\x42\x70\x69\x61\x6e\x6f"
"\x20\x31\x20\x20\x20" "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb1\xe2\x95\xb2"
"\xe2\x95\xb1\xe2" "\x95\xb2\xe2\x94\x82\n\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x20\x20\x4c\x31\x2d\x43\x6c\x61"
"\x76\x69\x65" "\x72\x2f\x32\x36\x30\x39\x30\x33\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb2\xe2\x95"
"\xb1\xe2" "\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x20\x20\x20\x20\x20\x20\x20"
"\x20" "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb1\xe2" ""
"\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x20\x20\x50\x68\x79" "\x73"
"\x69\x63\x61\x6c\x20\x4d\x6f\x64\x65\x6c\x69\x6e\x67\x20\x50\x69\x61"
"\x6e\x6f\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2" "\x95\xb2"
"\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x20\x20" "\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2" "\x94\x82\xe2\x95"
"\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94" "\x82\x20\x20\x44\x65"
"\x76\x65\x6c\x6f\x70\x65\x64\x20\x62\x79\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82" "\x3d\x3d\xe2\x94\x82\xe2"
"\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20" "\x20\xe2\x94\x82\x20\x20\x20"
"\x20\x5a\x69\x79\x61\x6e\x67\x20\x54\x61\x6e\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20" "\xe2\x94\x82\x3d\x3d\xe2\x94\x82"
"\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20"
"\x20\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\x20\xe2\x94\x82\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94"
"\x82\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n"
"\x20\x20\x20\x20\x20" "\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x20"
"\x20\x62\x42\x53\x6f\x6e\x69\x63\x4c\x61\x62\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20" "\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x3d\x3d\xe2"
"\x94\x82\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82"
"\n\x20\x20" "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x94"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2" "\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80" "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94" ""
"\x80\xe2\x94\x98\xe2\x94\x80\xe2\x94\x80\xe2\x95\xb0\xe2\x94\x80\xe2"
"\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x95\xaf\n\n\x20\x20" "\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x57\x69\x74\x68\x20\x53\x70\x65\x63"
"\x69\x61\x6c\x20\x54\x68\x61\x6e\x6b\x73\x20\x74\x6f" "\n\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x5a\x68\x75\x6f\x72\x61\x6e"
"\x20\x43\x68\x65\x6e\n\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\x20"
"\x20\x20\x66\x6f\x72\x20\x74\x68\x65\x20\x63\x6f\x6e\x76\x65\x72\x73"
"\x61\x74\x69\x6f\x6e\x73\x20\x61\x6e\x64\x20" "\x69\x6e\x73\x70\x69"
"\x72\x61\x74\x69\x6f\x6e\n\n\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x62\x42\x53\x6f\x6e\x69\x63" "\x4c\x61\x62\x20\x54\x65\x63"
"\x68\x6e\x6f\x6c\x6f\x67\x79\n\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x43\x6f\x70\x79\x72" "\x69\x67\x68\x74\x20\x28\x63\x29"
"\x20\x32\x30\x32\x36\x20\x5a\x69\x79\x61\x6e\x67\x20\x54\x61\x6e\x2e"
"\n\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\x20\x20\x20\x41\x6c\x6c"
"\x20\x72\x69\x67\x68\x74\x73\x20\x72\x65\x73\x65\x72\x76\x65\x64\x2e"
"\n\n\x20\x20\x20\x20" "\x20\x20\x20\x20";std::jthread l421;std::
jthread l360;std::jthread l397;std::jthread l425;std::jthread l365;
std::jthread l368;std::jthread l339;void l41(std::jthread&l378){if(!
l378.joinable()){return;}l378.request_stop();l378.join();}}void
midi_service_start(const char*l40){if(l40==nullptr){return;}l41(l421);
std::string path(l40);l421=std::jthread([path=std::move(path)](std::
stop_token stop_token)mutable{char l48[]="\x62\x62\x70\x6c";char
option[]="\x2d\x6d";char*l14[]={l48,option,path.data()};l116(3,l14,
l145,l156,stop_token);});}void midi_service_stop(void){l41(l421);}
void piano_service_start(void){l41(l360);l360=std::jthread([](std::
stop_token stop_token){char l48[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x70";char*l14[]={l48,option};l116(2,l14,l145,l156,stop_token);}
);}void piano_service_stop(void){l41(l360);}void
keyboard_service_start(void){l41(l397);l397=std::jthread([](std::
stop_token stop_token){char l48[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x6b";char*l14[]={l48,option};l116(2,l14,l145,l156,stop_token);}
);}void keyboard_service_stop(void){l41(l397);}void
export_service_start(const char*l40){if(l40==nullptr){return;}l41(
l425);std::string path(l40);l425=std::jthread([path=std::move(path)](
std::stop_token stop_token)mutable{char l48[]="\x62\x62\x70\x6c";char
option[]="\x2d\x65";char*l14[]={l48,option,path.data()};l116(3,l14,
l145,l156,stop_token);});}void export_service_stop(void){l41(l425);}
void record_service_start(void){l41(l365);l365=std::jthread([](std::
stop_token stop_token){char l48[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x72";char*l14[]={l48,option};l116(2,l14,l145,l156,stop_token);}
);}void record_service_stop(void){l41(l365);}void test_service_start(
void){l41(l368);l368=std::jthread([](std::stop_token stop_token){char
l48[]="\x62\x62\x70\x6c";char option[]="\x2d\x74";char*l14[]={l48,
option};l116(2,l14,l145,l156,stop_token);});}void test_service_stop(
void){l41(l368);}void internal_test_service_start(void){l41(l339);
l339=std::jthread([](std::stop_token stop_token){char l48[]="\x62\x62"
"\x70\x6c";char option[]="\x2d\x69";char*l14[]={l48,option};l116(2,
l14,l145,l156,stop_token);});}void internal_test_service_stop(void){
l41(l339);}void close_all_service(void){midi_service_stop();
piano_service_stop();keyboard_service_stop();export_service_stop();
record_service_stop();test_service_stop();internal_test_service_stop(
);}
