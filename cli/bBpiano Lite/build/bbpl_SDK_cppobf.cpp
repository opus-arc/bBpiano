/*
   'input.cpp' Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Mon Sep 21 16:14:04 2026
*/
#include<chrono>
#include<condition_variable>
#include<cstdint>
#include<filesystem>
#include<iostream>
#include<mutex>
#include<stop_token>
#include<thread>
void l577(double ln);void l535()noexcept;void l493(double ln);void
l502()noexcept;void l436(double ln);void l372()noexcept;void l373(
float*out,int l584,double l818);void l454();void l49(int lk,double l94
)noexcept;void l98(int lk,double l94)noexcept;void l187(int lk,double
l516)noexcept;void l125(double l11)noexcept;void l119(double l11)noexcept
;void l127(double l11)noexcept;void sustainpedal_control(double l11)noexcept
;void l59()noexcept;void l576()noexcept;bool l520(std::stop_token
stop_token)noexcept;bool l197()noexcept;void l386()noexcept;void l521
()noexcept;
#include<string>
static constexpr double ln=44100.0;int l116(int l204,char*l18[],const
char*l13,const char*l29,std::stop_token l238={});void l560(const std
::string&l44,std::stop_token stop_token);void l500(std::stop_token
stop_token);void l437(std::stop_token stop_token);void l561(std::
string l804,std::stop_token stop_token);void l537(std::stop_token
stop_token);void l505(std::stop_token stop_token);void l475(std::
stop_token stop_token);
int l466(int l204,char*l18[],const char*l13,const char*l29,std::
stop_token l238={});
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
runtime_error;};class l130{public:struct l7{std::uint64_t l26=0;
double l136=0.0;std::uint8_t l4=0;std::uint8_t l32=0;std::uint8_t l248
=0;std::string l255;int l793()const noexcept{return static_cast<int>(
l4&0x0F);}std::uint8_t l237()const noexcept{return l4&0xF0;}bool l381
()const noexcept{return l237()==0x90&&l248!=0;}bool l345()const
noexcept{return l237()==0x80||(l237()==0x90&&l248==0);}bool l583()const
noexcept{return l237()==0xA0;}bool l601()const noexcept{return l237()==
0xB0;}bool l455()const noexcept{if(!l601()){return false;}return l32
==64||l32==66||l32==67||l32==68;}double l507()const noexcept{return
std::clamp(static_cast<double>(l248)/127.0,0.0,1.0);}};enum class l56
{l214,l294};l130()=delete;static std::vector<l7>l567(const std::
string&l151){auto ls=l728(l773(l774(l151)));std::stable_sort(ls.begin
(),ls.end(),[](const l7&l0,const l7&l1){return l0.l136<l1.l136;});
return ls;}static l56 l727(double l443,double l602,const std::string&
l151,std::stop_token stop_token={}){if(!(l443>0.0)){throw std::
invalid_argument("\x70\x6c\x61\x79\x62\x61\x63\x6b\x5f\x72\x61\x74"
"\x65\x20\x6d\x75\x73\x74\x20\x62\x65\x20\x67\x72\x65\x61\x74\x65\x72"
"\x20\x74\x68\x61\x6e\x20\x7a\x65\x72\x6f\x2e");}const double l460=
std::max(0.0,l602);std::vector<l7>ls=l567(l151);const auto l776=std::
lower_bound(ls.begin(),ls.end(),l460,[](const l7&li,double time){
return li.l136<time;});std::unordered_set<int>l251;std::mutex l615;
std::condition_variable l390;std::stop_callback l551(stop_token,[&
l390]{l390.notify_all();});const auto l711=std::chrono::steady_clock
::now();auto l270=[&l251]{for(int lq:l251){l98(lq,0.0);}l279();};try{
for(auto li=l776;li!=ls.end();++li){if(stop_token.stop_requested()){
l270();return l56::l294;}const double l759=std::max(0.0,li->l136-l460
)/l443;const auto l599=l711+std::chrono::duration_cast<std::chrono::
steady_clock::duration>(std::chrono::duration<double>(l759));{std::
unique_lock lock(l615);l390.wait_until(lock,l599,[&stop_token]{return
stop_token.stop_requested();});}if(stop_token.stop_requested()){l270(
);return l56::l294;}l503( *li);if(li->l381()&&l63(li->l32)){l251.
insert(static_cast<int>(li->l32));}else if(li->l345()){l251.erase(
static_cast<int>(li->l32));}}}catch(...){l270();throw;}l270();return
l56::l214;}static void l503(const l7&li){const int lq=static_cast<int
>(li.l32);const double l173=static_cast<double>(li.l248);if(li.l381()){
if(l63(li.l32)){l49(lq,l173);}}else if(li.l345()){if(l63(li.l32)){l98
(lq,l173);}}else if(li.l583()){if(l63(li.l32)){l187(lq,li.l507());}}
else if(li.l455()){l719(li.l32,li.l507());}}static void l279(){l125(
0.0);l119(0.0);l127(0.0);sustainpedal_control(0.0);}private:struct
l146{std::uint64_t l26=0;double l574=500000.0;std::size_t l476=0;};
class l580{public:explicit l580(std::vector<std::uint8_t>l22):l126(
std::move(l22)){}std::size_t l8()const noexcept{return l55;}std::
size_t size()const noexcept{return l126.size();}void l446(std::size_t
l8){if(l8>l126.size()){throw l9("\x4d\x49\x44\x49\x20\x63\x68\x75\x6e"
"\x6b\x20\x65\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c\x65\x20\x73\x69"
"\x7a\x65\x2e");}l55=l8;}std::uint8_t l60(std::size_t l16=std::
numeric_limits<std::size_t>::max()){if(l55>=l126.size()||l55>=l16){
throw l9("\x55\x6e\x65\x78\x70\x65\x63\x74\x65\x64\x20\x65\x6e\x64"
"\x20\x6f\x66\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x2e");}return
l126[l55++];}std::uint16_t l376(std::size_t l16=std::numeric_limits<
std::size_t>::max()){const std::uint16_t l384=l60(l16);const std::
uint16_t l260=l60(l16);return static_cast<std::uint16_t>((l384<<8)|
l260);}std::uint32_t l526(std::size_t l16=std::numeric_limits<std::
size_t>::max()){const std::uint32_t l163=l60(l16);const std::uint32_t
l191=l60(l16);const std::uint32_t l335=l60(l16);const std::uint32_t
l639=l60(l16);return(l163<<24)|(l191<<16)|(l335<<8)|l639;}std::
uint32_t l395(std::size_t l16){std::uint32_t lt=0;for(int count=0;
count<4;++count){const std::uint8_t l33=l60(l16);lt=(lt<<7)|
static_cast<std::uint32_t>(l33&0x7F);if((l33&0x80)==0){return lt;}}
throw l9("\x49\x6e\x76\x61\x6c\x69\x64\x20\x4d\x49\x44\x49\x20\x76"
"\x61\x72\x69\x61\x62\x6c\x65\x2d\x6c\x65\x6e\x67\x74\x68\x20\x76\x61"
"\x6c\x75\x65\x2e");}std::string l491(std::size_t count,std::size_t
l16=std::numeric_limits<std::size_t>::max()){l388(count,l16);const
auto begin=l126.begin()+static_cast<std::ptrdiff_t>(l55);l55+=count;
return std::string(begin,begin+static_cast<std::ptrdiff_t>(count));}
std::vector<std::uint8_t>l592(std::size_t count,std::size_t l16){l388
(count,l16);const auto begin=l126.begin()+static_cast<std::ptrdiff_t>
(l55);l55+=count;return{begin,begin+static_cast<std::ptrdiff_t>(count
)};}void l756(std::size_t count,std::size_t l16=std::numeric_limits<
std::size_t>::max()){l388(count,l16);l55+=count;}private:void l388(
std::size_t count,std::size_t l16)const{const std::size_t l528=std::
min(l16,l126.size());if(l55>l528||count>l528-l55){throw l9("\x55\x6e"
"\x65\x78\x70\x65\x63\x74\x65\x64\x20\x65\x6e\x64\x20\x6f\x66\x20\x4d"
"\x49\x44\x49\x20\x64\x61\x74\x61\x2e");}}std::vector<std::uint8_t>
l126;std::size_t l55=0;};static bool l63(std::uint8_t lq)noexcept{
return lq>=21&&lq<=108;}static void l719(std::uint8_t l113,double l11
){switch(l113){case 64:sustainpedal_control(l11);break;case 66:l127(
l11);break;case 67:l125(l11);break;case 68:l119(l11);break;default:
break;}}static std::vector<std::uint8_t>l774(const std::string&path){
std::ifstream l244(path,std::ios::binary|std::ios::ate);if(!l244){
throw l9("\x43\x61\x6e\x6e\x6f\x74\x20\x6f\x70\x65\x6e\x20\x4d\x49"
"\x44\x49\x20\x66\x69\x6c\x65\x3a\x20"+path);}const std::streampos end
=l244.tellg();if(end<=0){throw l9("\x4d\x49\x44\x49\x20\x66\x69\x6c"
"\x65\x20\x69\x73\x20\x65\x6d\x70\x74\x79\x3a\x20"+path);}if(
static_cast<std::uintmax_t>(end)>std::numeric_limits<std::size_t>::
max()){throw l9("\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x20\x69\x73\x20"
"\x74\x6f\x6f\x20\x6c\x61\x72\x67\x65\x2e");}std::vector<std::uint8_t
>l22(static_cast<std::size_t>(end));l244.seekg(0,std::ios::beg);l244.
read(reinterpret_cast<char* >(l22.data()),static_cast<std::streamsize
>(l22.size()));if(!l244){throw l9("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x72\x65\x61\x64\x20\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x3a"
"\x20"+path);}return l22;}static std::vector<l7>l773(std::vector<std
::uint8_t>l22){l580 l2(std::move(l22));if(l2.l491(4)!="\x4d\x54\x68"
"\x64"){throw l9("\x4d\x69\x73\x73\x69\x6e\x67\x20\x4d\x49\x44\x49"
"\x20\x68\x65\x61\x64\x65\x72\x20\x63\x68\x75\x6e\x6b\x2e");}const std
::uint32_t l582=l2.l526();if(l582<6){throw l9("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x4d\x49\x44\x49\x20\x68\x65\x61\x64\x65\x72\x20\x6c\x65"
"\x6e\x67\x74\x68\x2e");}const std::size_t l167=l2.l8()+l582;if(l167<
l2.l8()||l167>l2.size()){throw l9("\x4d\x49\x44\x49\x20\x68\x65\x61"
"\x64\x65\x72\x20\x65\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c\x65\x20"
"\x73\x69\x7a\x65\x2e");}const std::uint16_t l25=l2.l376(l167);const
std::uint16_t l323=l2.l376(l167);const std::uint16_t l405=l2.l376(
l167);l2.l446(l167);if(l25>1){throw l9("\x4f\x6e\x6c\x79\x20\x4d\x49"
"\x44\x49\x20\x66\x6f\x72\x6d\x61\x74\x20\x30\x20\x61\x6e\x64\x20\x31"
"\x20\x61\x72\x65\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}if(
l323==0||(l25==0&&l323!=1)){throw l9("\x49\x6e\x76\x61\x6c\x69\x64"
"\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x63\x6f\x75\x6e\x74"
"\x2e");}if((l405&0x8000)!=0||l405==0){throw l9("\x53\x4d\x50\x54\x45"
"\x20\x6f\x72\x20\x7a\x65\x72\x6f\x20\x4d\x49\x44\x49\x20\x74\x69\x6d"
"\x65\x20\x64\x69\x76\x69\x73\x69\x6f\x6e\x20\x69\x73\x20\x75\x6e\x73"
"\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}std::vector<l7>ls;std::
vector<l146>l156{{0,500000.0,0}};std::size_t l589=1;for(std::uint16_t
l12=0;l12<l323;++l12){if(l2.l491(4)!="\x4d\x54\x72\x6b"){throw l9(""
"\x4d\x69\x73\x73\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63"
"\x6b\x20\x63\x68\x75\x6e\x6b\x2e");}const std::size_t l449=l2.l526();
const std::size_t l552=l2.l8();if(l449>l2.size()-l552){throw l9("\x4d"
"\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x65\x78\x63\x65\x65\x64\x73"
"\x20\x66\x69\x6c\x65\x20\x73\x69\x7a\x65\x2e");}const std::size_t l64
=l552+l449;std::uint64_t l282=0;std::uint8_t l62=0;std::string l255;
while(l2.l8()<l64){const std::uint32_t l492=l2.l395(l64);if(l492>std
::numeric_limits<std::uint64_t>::max()-l282){throw l9("\x4d\x49\x44"
"\x49\x20\x61\x62\x73\x6f\x6c\x75\x74\x65\x20\x74\x69\x63\x6b\x20\x6f"
"\x76\x65\x72\x66\x6c\x6f\x77\x2e");}l282+=l492;const std::uint8_t
l411=l2.l60(l64);std::uint8_t l4=l411;std::uint8_t l32=0;bool l461=
false;if(l411<0x80){if(l62==0){throw l9("\x4d\x49\x44\x49\x20\x72\x75"
"\x6e\x6e\x69\x6e\x67\x20\x73\x74\x61\x74\x75\x73\x20\x68\x61\x73\x20"
"\x6e\x6f\x20\x73\x74\x61\x74\x75\x73\x20\x62\x79\x74\x65\x2e");}l4=
l62;l32=l411;l461=true;}if(l4==0xFF){l62=0;const std::uint8_t l14=l2.
l60(l64);const std::size_t length=l2.l395(l64);const auto data=l2.
l592(length,l64);if(l14==0x03){l255.assign(data.begin(),data.end());}
else if(l14==0x51&&data.size()==3){const double l182=static_cast<
double>(data[0]) *65536.0+static_cast<double>(data[1]) *256.0+
static_cast<double>(data[2]);if(l182>0.0){l156.push_back({l282,l182,
l589++});}}continue;}if(l4==0xF0||l4==0xF7){l62=0;l2.l756(l2.l395(l64
),l64);continue;}if(l4>=0xF0){throw l9("\x55\x6e\x73\x75\x70\x70\x6f"
"\x72\x74\x65\x64\x20\x73\x79\x73\x74\x65\x6d\x20\x6d\x65\x73\x73\x61"
"\x67\x65\x20\x69\x6e\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x2e"
);}l62=l4;const std::uint8_t l14=l4&0xF0;const std::uint8_t l479=l461
?l32:l2.l60(l64);if((l479&0x80)!=0){throw l9("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62\x79\x74\x65"
"\x2e");}switch(l14){case 0x80:case 0x90:case 0xA0:case 0xB0:{const
std::uint8_t l439=l2.l60(l64);if((l439&0x80)!=0){throw l9("\x49\x6e"
"\x76\x61\x6c\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62"
"\x79\x74\x65\x2e");}ls.push_back({l282,0.0,l4,l479,l439,l255});break
;}case 0xE0:static_cast<void>(l2.l60(l64));break;case 0xC0:case 0xD0:
break;default:throw l9("\x49\x6e\x76\x61\x6c\x69\x64\x20\x4d\x49\x44"
"\x49\x20\x63\x68\x61\x6e\x6e\x65\x6c\x20\x6d\x65\x73\x73\x61\x67\x65"
"\x2e");}}l2.l446(l64);}l781(ls,l156,static_cast<double>(l405));
return ls;}static void l781(std::vector<l7>&ls,std::vector<l146>&l156
,double l556){std::stable_sort(l156.begin(),l156.end(),[](const l146&
l0,const l146&l1){if(l0.l26!=l1.l26){return l0.l26<l1.l26;}return l0.
l476<l1.l476;});std::vector<l146>l110;for(const l146&l182:l156){if(!
l110.empty()&&l110.back().l26==l182.l26){l110.back()=l182;}else{l110.
push_back(l182);}}std::stable_sort(ls.begin(),ls.end(),[](const l7&l0
,const l7&l1){return l0.l26<l1.l26;});std::size_t l408=0;std::
uint64_t l183=0;double l313=0.0;double l357=l110.front().l574;for(l7&
li:ls){while(l408+1<l110.size()&&l110[l408+1].l26<=li.l26){const l146
&l385=l110[++l408];l313+=static_cast<double>(l385.l26-l183) *l357/(
1000000.0*l556);l183=l385.l26;l357=l385.l574;}li.l136=l313+
static_cast<double>(li.l26-l183) *l357/(1000000.0*l556);}}static std
::vector<l7>l728(const std::vector<l7>&ls){std::vector<l7>l302;for(
const l7&li:ls){if(li.l381()||li.l345()||li.l583()||li.l455()){l302.
push_back(li);}}std::vector<l7>l380;for(const l7&li:l302){std::string
l102=li.l255;std::transform(l102.begin(),l102.end(),l102.begin(),[](
unsigned char l75){return static_cast<char>(std::tolower(l75));});if(
l102.find("\x70\x69\x61\x6e\x6f")!=std::string::npos||l102.find("\x6b"
"\x65\x79\x62\x6f\x61\x72\x64")!=std::string::npos||l102.find("\x67"
"\x72\x61\x6e\x64")!=std::string::npos){l380.push_back(li);}}if(!l380
.empty()){return l380;}std::vector<l7>l322;for(const l7&li:l302){if(
li.l793()==0){l322.push_back(li);}}return l322.empty()?l302:l322;}};
class l285{public:enum class l56{l214,l294};l285()=delete;static l56
l663(const std::string&l151,std::filesystem::path lw={},std::uint32_t
ln=44100,double l387=5.0,std::stop_token stop_token={}){if(l151.empty
()){throw std::invalid_argument("\x4e\x6f\x20\x4d\x49\x44\x49\x20\x66"
"\x69\x6c\x65\x20\x70\x61\x74\x68\x20\x70\x72\x6f\x76\x69\x64\x65\x64"
"\x2e");}if(ln==0||ln>std::numeric_limits<std::uint32_t>::max()/
sizeof(float)||!std::isfinite(l387)||l387<0.0){throw std::
invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20\x57\x41\x56\x20"
"\x72\x65\x6e\x64\x65\x72\x69\x6e\x67\x20\x63\x6f\x6e\x66\x69\x67\x75"
"\x72\x61\x74\x69\x6f\x6e\x2e");}std::vector<l130::l7>ls=l130::l567(
l151);if(lw.empty()){lw=std::filesystem::path(l151);lw.
replace_extension("\x2e\x77\x61\x76");}l748(lw);const double l744=ls.
empty()?0.0:ls.back().l136;const long double l259=(static_cast<long
double>(l744)+l387) *ln;constexpr std::uint64_t l646=std::
numeric_limits<std::uint32_t>::max()-36ULL;if(!std::isfinite(l259)||
l259<0.0L||l259>l646/sizeof(float)){throw std::runtime_error("\x57"
"\x41\x56\x20\x65\x78\x63\x65\x65\x64\x73\x20\x74\x68\x65\x20\x52\x49"
"\x46\x46\x20\x33\x32\x2d\x62\x69\x74\x20\x73\x69\x7a\x65\x20\x6c\x69"
"\x6d\x69\x74\x3b\x20\x52\x46\x36\x34\x20\x69\x73\x20\x6e\x6f\x74\x20"
"\x69\x6d\x70\x6c\x65\x6d\x65\x6e\x74\x65\x64\x2e");}const std::
uint64_t l417=static_cast<std::uint64_t>(std::ceil(l259));const std::
uint64_t l268=l417*sizeof(float);std::filesystem::path l35=lw;l35+=""
"\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72\x74";if(std::filesystem::exists
(l35)){throw std::runtime_error("\x54\x65\x6d\x70\x6f\x72\x61\x72\x79"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x61\x6c\x72\x65\x61"
"\x64\x79\x20\x65\x78\x69\x73\x74\x73\x3a\x20"+l35.string());}l59();
try{std::ofstream ll(l35,std::ios::binary);if(!ll){throw std::
runtime_error("\x43\x61\x6e\x6e\x6f\x74\x20\x63\x72\x65\x61\x74\x65"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lw.string());}
l683(ll,ln,static_cast<std::uint32_t>(l268));constexpr std::size_t
l563=512;std::array<float,l563>buffer{};std::uint64_t l53=0;std::
size_t l178=0;while(l53<l417){if(stop_token.stop_requested()){ll.
close();l59();std::error_code l194;std::filesystem::remove(l35,l194);
return l56::l294;}while(l178<ls.size()&&l501(ls[l178],ln)<=l53){l130
::l503(ls[l178]);++l178;}std::uint64_t l225=std::min<std::uint64_t>(
l417,l53+l563);if(l178<ls.size()){l225=std::min(l225,l501(ls[l178],ln
));}if(l225==l53){continue;}const auto l472=static_cast<std::size_t>(
l225-l53);l373(buffer.data(),static_cast<int>(l472),1.0);ll.write(
reinterpret_cast<const char* >(buffer.data()),static_cast<std::
streamsize>(l472*sizeof(float)));if(!ll){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69\x6c\x65\x20\x77\x72\x69\x74"
"\x69\x6e\x67\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lw
.string());}l53=l225;}l59();ll.close();if(!ll){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e"
"\x61\x6c\x69\x7a\x65\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a"
"\x20"+lw.string());}std::filesystem::rename(l35,lw);return l56::l214
;}catch(...){l59();std::error_code l194;std::filesystem::remove(l35,
l194);throw;}}private:static std::uint64_t l501(const l130::l7&li,std
::uint32_t ln)noexcept{return static_cast<std::uint64_t>(std::llround
(li.l136*ln));}static void l748(const std::filesystem::path&lw){if(
std::filesystem::exists(lw)){throw std::runtime_error("\x52\x65\x66"
"\x75\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f\x76\x65\x72\x77\x72\x69\x74"
"\x65\x20\x65\x78\x69\x73\x74\x69\x6e\x67\x20\x57\x41\x56\x20\x66\x69"
"\x6c\x65\x3a\x20"+lw.string());}const std::filesystem::path l352=lw.
parent_path();if(!l352.empty()&&!std::filesystem::exists(l352)){throw
std::runtime_error("\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x64"
"\x69\x72\x65\x63\x74\x6f\x72\x79\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74"
"\x20\x65\x78\x69\x73\x74\x3a\x20"+l352.string());}}static void l277(
std::ostream&ll,std::uint16_t lt){ll.put(static_cast<char>(lt&0xFF));
ll.put(static_cast<char>((lt>>8)&0xFF));}static void l210(std::
ostream&ll,std::uint32_t lt){ll.put(static_cast<char>(lt&0xFF));ll.
put(static_cast<char>((lt>>8)&0xFF));ll.put(static_cast<char>((lt>>16
)&0xFF));ll.put(static_cast<char>((lt>>24)&0xFF));}static void l683(
std::ostream&ll,std::uint32_t ln,std::uint32_t l268){constexpr std::
uint16_t l616=3;constexpr std::uint16_t l786=1;constexpr std::
uint16_t l740=32;constexpr std::uint16_t l487=sizeof(float);constexpr
std::uint32_t l568=16;const std::uint32_t l710=ln*l487;const std::
uint32_t l643=4+(8+l568)+(8+l268);ll.write("\x52\x49\x46\x46",4);l210
(ll,l643);ll.write("\x57\x41\x56\x45\x66\x6d\x74\x20",8);l210(ll,l568
);l277(ll,l616);l277(ll,l786);l210(ll,ln);l210(ll,l710);l277(ll,l487);
l277(ll,l740);ll.write("\x64\x61\x74\x61",4);l210(ll,l268);}};
#include<optional>
#include<CoreMIDI/CoreMIDI.h>
#include<functional>
#include<memory>
#include<span>
class lp{public:using l106=std::span<const std::uint8_t>;using l404=
std::function<void(l106)>;private:struct l149{explicit l149(l404
new_handler):l129(std::move(new_handler)){}void l604(l106 lj)noexcept
{{std::lock_guard lock(mutex);if(!l363){return;}++l391;}try{l129(lj);
}catch(...){}{std::lock_guard lock(mutex);--l391;}l275.notify_all();}
void l694()noexcept{std::unique_lock lock(mutex);l363=false;l275.wait
(lock,[this]{return l391==0;});l129={};}bool l782()const noexcept{std
::lock_guard lock(mutex);return l363;}mutable std::mutex mutex;std::
condition_variable l275;l404 l129;std::size_t l391=0;bool l363=true;}
;public:class l30{public:l30()noexcept=default;l30(const l30&)=delete
;l30&operator=(const l30&)=delete;l30(l30&&l264)noexcept:l101(std::
exchange(l264.l101,{})){}l30&operator=(l30&&l264)noexcept{if(this!=&
l264){reset();l101=std::exchange(l264.l101,{});}return*this;}~l30(){
reset();}void reset()noexcept{if(l101){l101->l694();l101.reset();}}
explicit operator bool()const noexcept{return static_cast<bool>(l101);
}private:friend class lp;explicit l30(std::shared_ptr<l149>l24):l101(
std::move(l24)){}std::shared_ptr<l149>l101;};static constexpr std::
uint8_t l402=64;static constexpr std::uint8_t l409=66;static constexpr
std::uint8_t l331=67;static constexpr std::uint8_t l326=68;lp()=
default;lp(const lp&)=delete;lp&operator=(const lp&)=delete;lp(lp&&)=
delete;lp&operator=(lp&&)=delete;~lp(){l67();}l30 l554(l404 l129){if(
!l129){throw std::invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20"
"\x4d\x49\x44\x49\x20\x6d\x65\x73\x73\x61\x67\x65\x20\x68\x61\x6e\x64"
"\x6c\x65\x72\x2e");}auto l24=std::make_shared<l149>(std::move(l129));
std::lock_guard lock(l95);l767();l133.push_back(l24);return l30(std::
move(l24));}void l68(){std::lock_guard l266(l147);{std::lock_guard
l256(l95);if(l93){return;}}const ItemCount l399=
MIDIGetNumberOfSources();if(l399==0){throw std::runtime_error("\x4e"
"\x6f\x20\x4d\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69"
"\x63\x65\x20\x66\x6f\x75\x6e\x64\x2e");}MIDIClientRef l81=0;
MIDIPortRef l77=0;std::vector<std::unique_ptr<l280>>l290;OSStatus l4=
MIDIClientCreate(CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49"
"\x6e\x70\x75\x74\x20\x48\x75\x62"),nullptr,nullptr,&l81);if(l4!=
noErr||l81==0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20"
"\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d\x49\x44"
"\x49\x20\x63\x6c\x69\x65\x6e\x74\x2e");}l4=MIDIInputPortCreate(l81,
CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49\x6e\x70\x75\x74"
"\x20\x50\x6f\x72\x74"),&lp::l754,nullptr,&l77);if(l4!=noErr||l77==0){
MIDIClientDispose(l81);throw std::runtime_error("\x46\x61\x69\x6c\x65"
"\x64\x20\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d"
"\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x70\x6f\x72\x74\x2e");}l290
.reserve(static_cast<std::size_t>(l399));for(ItemCount lh=0;lh<l399;
++lh){const MIDIEndpointRef l134=MIDIGetSource(lh);if(l134==0){
continue;}auto l41=std::make_unique<l280>();l41->l154=this;l41->l134=
l134;l4=MIDIPortConnectSource(l77,l134,l41.get());if(l4==noErr){l290.
push_back(std::move(l41));}}if(l290.empty()){MIDIPortDispose(l77);
MIDIClientDispose(l81);throw std::runtime_error("\x4e\x6f\x20\x4d\x49"
"\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69\x63\x65\x20\x63"
"\x6f\x75\x6c\x64\x20\x62\x65\x20\x6f\x70\x65\x6e\x65\x64\x2e");}{std
::lock_guard l256(l95);l329=l81;l361=l77;l382=std::move(l290);l429=
true;l93=true;}}void l67()noexcept{std::lock_guard l266(l147);
MIDIClientRef l81=0;MIDIPortRef l77=0;{std::lock_guard l256(l95);if(!
l93){return;}l429=false;l93=false;l81=l329;l77=l361;}for(const auto&
l41:l382){if(l77!=0&&l41&&l41->l134!=0){MIDIPortDisconnectSource(l77,
l41->l134);}}if(l77!=0){MIDIPortDispose(l77);}if(l81!=0){
MIDIClientDispose(l81);}{std::unique_lock l256(l95);l508.wait(l256,[
this]{return l350==0;});l329=0;l361=0;l382.clear();}}bool l644()const
noexcept{std::lock_guard lock(l95);return l93;}private:struct l218{
std::array<std::uint8_t,3>l22{};std::size_t size=0;};struct l498{std
::mutex mutex;std::uint8_t l62=0;std::uint8_t l111=0;std::array<std::
uint8_t,2>data{};std::size_t l171=0;std::size_t l186=0;bool l359=
false;};struct l280{lp*l154=nullptr;MIDIEndpointRef l134=0;l498 lr;};
class l416{public:explicit l416(lp&l154)noexcept:l465(&l154),l141(
l154.l678()){}~l416(){if(l141){l465->l673();}}explicit operator bool(
)const noexcept{return l141;}private:lp*l465;bool l141;};static void
l754(const MIDIPacketList*l341,void* ,void*l755)noexcept{auto*l41=
static_cast<l280* >(l755);if(l341==nullptr||l41==nullptr||l41->l154==
nullptr){return;}l416 l158( *l41->l154);if(!l158){return;}const
MIDIPacket*packet=&l341->packet[0];for(UInt32 lh=0;lh<l341->
numPackets;++lh){l41->l154->l649(l41->lr,packet->data,packet->length);
packet=MIDIPacketNext(packet);}}bool l678()noexcept{std::lock_guard
lock(l95);if(!l429){return false;}++l350;return true;}void l673()noexcept
{{std::lock_guard lock(l95);--l350;}l508.notify_all();}static std::
size_t l444(std::uint8_t l4)noexcept{switch(l4&0xF0){case 0x80:case
0x90:case 0xA0:case 0xB0:case 0xE0:return 2;case 0xC0:case 0xD0:
return 1;default:break;}switch(l4){case 0xF1:case 0xF3:return 1;case
0xF2:return 2;case 0xF6:return 0;default:return 0;}}void l649(l498&lr
,const Byte*data,UInt16 length)noexcept{std::vector<l218>l205;l205.
reserve(static_cast<std::size_t>(length)/2+1);{std::lock_guard l823(
lr.mutex);for(UInt16 lh=0;lh<length;++lh){const std::uint8_t l33=
static_cast<std::uint8_t>(data[lh]);if(l33>=0xF8){l218 lj;lj.l22[0]=
l33;lj.size=1;l205.push_back(lj);continue;}if(lr.l359){if(l33==0xF7){
lr.l359=false;}continue;}if((l33&0x80)!=0){lr.l186=0;lr.l111=0;if(l33
==0xF0){lr.l359=true;lr.l62=0;continue;}if(l33==0xF7){lr.l62=0;
continue;}lr.l111=l33;lr.l171=l444(l33);if(l33<0xF0){lr.l62=l33;}else
{lr.l62=0;}if(lr.l171==0){l218 lj;lj.l22[0]=l33;lj.size=1;l205.
push_back(lj);lr.l111=0;}continue;}if(lr.l111==0){if(lr.l62==0){
continue;}lr.l111=lr.l62;lr.l171=l444(lr.l111);lr.l186=0;}if(lr.l186<
lr.data.size()){lr.data[lr.l186++]=l33;}if(lr.l186==lr.l171){l218 lj;
lj.l22[0]=lr.l111;for(std::size_t l137=0;l137<lr.l171;++l137){lj.l22[
l137+1]=lr.data[l137];}lj.size=lr.l171+1;l205.push_back(lj);lr.l111=0
;lr.l186=0;}}}for(const l218&lj:l205){l603(l106(lj.l22.data(),lj.size
));}}void l603(l106 lj)noexcept{std::vector<std::shared_ptr<l149>>
l316;{std::lock_guard lock(l95);l316.reserve(l133.size());for(const
auto&l344:l133){if(auto l24=l344.lock()){l316.push_back(std::move(l24
));}}}for(const auto&l129:l316){l129->l604(lj);}}void l767(){l133.
erase(std::remove_if(l133.begin(),l133.end(),[](const std::weak_ptr<
l149>&l344){const auto l24=l344.lock();return!l24||!l24->l782();}),
l133.end());}mutable std::mutex l147;mutable std::mutex l95;std::
condition_variable l508;bool l93=false;bool l429=false;std::size_t
l350=0;MIDIClientRef l329=0;MIDIPortRef l361=0;std::vector<std::
unique_ptr<l280>>l382;std::vector<std::weak_ptr<l149>>l133;};class
MidiKeyboard{public:explicit MidiKeyboard(lp&l54)noexcept:l230(l54){}
MidiKeyboard(const MidiKeyboard&)=delete;MidiKeyboard(MidiKeyboard&&)=
delete;MidiKeyboard&operator=(MidiKeyboard&&)=delete;~MidiKeyboard(){
l67();}void l68(){if(l21){return;}l21.emplace(l230.l554([](lp::l106 lj
){l796(lj);}));}void l67()noexcept{if(!l21){return;}l21->reset();l21.
reset();l775();}bool l644()const noexcept{return l21.has_value();}
private:static bool l63(std::uint8_t lq)noexcept{return lq>=21&&lq<=
108;}static void l796(lp::l106 lj){if(lj.empty()){return;}const std::
uint8_t l14=lj[0]&0xF0;if(l14==0x80||l14==0x90||l14==0xA0){if(lj.size
()<3||!l63(lj[1])){return;}const int lq=static_cast<int>(lj[1]);const
double l173=static_cast<double>(lj[2]);if(l14==0x80||(l14==0x90&&lj[2
]==0)){l98(lq,l173);}else if(l14==0x90){l49(lq,l173);}else{l187(lq,
std::clamp(l173/127.0,0.0,1.0));}return;}if(l14!=0xB0||lj.size()<3){
return;}const double l11=std::clamp(static_cast<double>(lj[2])/127.0,
0.0,1.0);switch(lj[1]){case lp::l402:sustainpedal_control(l11);break;
case lp::l409:l127(l11);break;case lp::l331:l125(l11);break;case lp::
l326:l119(l11);break;default:break;}}static void l775()noexcept{l125(
0.0);l119(0.0);l127(0.0);sustainpedal_control(0.0);l59();}lp&l230;std
::optional<lp::l30>l21;};
#include<iomanip>
#include<sstream>
class l74{public:struct l97{std::uint64_t l215=0;std::array<std::
uint8_t,3>lj{};std::uint8_t l410=0;};explicit l74(lp&l54)noexcept:
l230(l54){}l74(const l74&)=delete;l74&operator=(const l74&)=delete;
l74(l74&&)=delete;l74&operator=(l74&&)=delete;~l74(){l708();}void l68
(std::filesystem::path lw={}){std::lock_guard l266(l147);if(l21){
return;}{std::lock_guard l346(l201);l157.clear();l157.reserve(4096);
l309=std::move(lw);l569=l76::now();}l21.emplace(l230.l554([this](lp::
l106 lj){l630(lj);}));}std::filesystem::path l67(){std::lock_guard
l266(l147);const bool l628=l21.has_value();if(l21){l21->reset();l21.
reset();}std::vector<l97>l284;std::filesystem::path l319;{std::
lock_guard l346(l201);l284=l157;l319=l309;}if(!l628&&l284.empty()){
return l527;}const std::filesystem::path l401=l319.empty()?std::
filesystem::current_path()/l659(l284):l319;l653(l401,l284);{std::
lock_guard l346(l201);l157.clear();l309.clear();l527=l401;}return l401
;}void l708()noexcept{std::lock_guard l266(l147);if(l21){l21->reset();
l21.reset();}std::lock_guard l346(l201);l157.clear();l309.clear();}
bool l810()const noexcept{std::lock_guard lock(l147);return l21.
has_value();}private:using l76=std::chrono::steady_clock;static
constexpr std::uint16_t l434=480;static constexpr std::uint32_t l670=
500000;void l630(lp::l106 lj){if(!l750(lj)){return;}l97 li;li.l215=
static_cast<std::uint64_t>(std::chrono::duration_cast<std::chrono::
microseconds>(l76::now()-l569).count());li.l410=static_cast<std::
uint8_t>(lj.size());std::copy(lj.begin(),lj.end(),li.lj.begin());std
::lock_guard lock(l201);l157.push_back(std::move(li));}static bool
l750(lp::l106 lj)noexcept{if(lj.empty()||lj[0]>=0xF0){return false;}
switch(lj[0]&0xF0){case 0x80:case 0x90:case 0xA0:case 0xB0:case 0xE0:
return lj.size()==3;case 0xC0:case 0xD0:return lj.size()==2;default:
return false;}}static std::string l659(const std::vector<l97>&ls){
const std::time_t now=std::time(nullptr);std::tm l483{};localtime_r(&
now,&l483);std::size_t l506=0;std::size_t l523=0;for(const l97&li:ls){
if(li.l410<3){continue;}const std::uint8_t l14=li.lj[0]&0xF0;if(l14==
0x90&&li.lj[2]!=0){++l506;}else if(l14==0xB0&&l655(li.lj[1])){++l523;
}}const std::uint64_t l313=ls.empty()?0:ls.back().l215/1000000ULL;std
::ostringstream l102;l102<<std::put_time(&l483,"\x25\x59\x2d\x25\x6d"
"\x2d\x25\x64\x5f\x25\x48\x2d\x25\x4d\x2d\x25\x53")<<"\x5f"<<l506<<""
"\x2d\x6e\x6f\x74\x65\x73\x5f"<<l523<<"\x2d\x70\x65\x64\x61\x6c\x73"
"\x5f"<<l313<<"\x2d\x73\x65\x63\x6f\x6e\x64\x73\x2e\x6d\x69\x64";
return l102.str();}static bool l655(std::uint8_t l113)noexcept{return
l113==lp::l402||l113==lp::l409||l113==lp::l331||l113==lp::l326;}
static void l541(std::vector<std::uint8_t>&ll,std::uint32_t lt){std::
uint8_t l22[5]{};int lh=4;l22[lh]=static_cast<std::uint8_t>(lt&0x7F);
while((lt>>=7)!=0&&lh>0){l22[--lh]=static_cast<std::uint8_t>((lt&0x7F
)|0x80);}for(;lh<5;++lh){ll.push_back(l22[lh]);}}static std::uint32_t
l665(std::uint64_t microseconds)noexcept{const long double l432=
static_cast<long double>(microseconds) *l434/l670;return l432>=std::
numeric_limits<std::uint32_t>::max()?std::numeric_limits<std::
uint32_t>::max():static_cast<std::uint32_t>(l432);}static void l717(
std::vector<std::uint8_t>&l12){const std::uint8_t l803[]={lp::l402,lp
::l409,lp::l331,lp::l326};for(std::uint8_t l113:l803){l541(l12,0);l12
.insert(l12.end(),{0xB0,l113,0});}}static void l414(std::ostream&ll,
std::uint16_t lt){ll.put(static_cast<char>((lt>>8)&0xFF));ll.put(
static_cast<char>(lt&0xFF));}static void l557(std::ostream&ll,std::
uint32_t lt){ll.put(static_cast<char>((lt>>24)&0xFF));ll.put(
static_cast<char>((lt>>16)&0xFF));ll.put(static_cast<char>((lt>>8)&
0xFF));ll.put(static_cast<char>(lt&0xFF));}static void l653(const std
::filesystem::path&lw,std::vector<l97>ls){std::stable_sort(ls.begin(),
ls.end(),[](const l97&l0,const l97&l1){return l0.l215<l1.l215;});std
::vector<std::uint8_t>l12;l12.reserve(ls.size() *4+32);l12.insert(l12
.end(),{0x00,0xFF,0x51,0x03,0x07,0xA1,0x20});std::uint32_t l183=0;for
(const l97&li:ls){const std::uint32_t l26=l665(li.l215);l541(l12,l26-
l183);l183=l26;l12.insert(l12.end(),li.lj.begin(),li.lj.begin()+li.
l410);}l717(l12);l12.insert(l12.end(),{0x00,0xFF,0x2F,0x00});if(l12.
size()>std::numeric_limits<std::uint32_t>::max()){throw std::
runtime_error("\x52\x65\x63\x6f\x72\x64\x65\x64\x20\x4d\x49\x44\x49"
"\x20\x74\x72\x61\x63\x6b\x20\x69\x73\x20\x74\x6f\x6f\x20\x6c\x61\x72"
"\x67\x65\x2e");}if(!lw.parent_path().empty()&&!std::filesystem::
exists(lw.parent_path())){throw std::runtime_error("\x4d\x49\x44\x49"
"\x20\x6f\x75\x74\x70\x75\x74\x20\x64\x69\x72\x65\x63\x74\x6f\x72\x79"
"\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74\x20\x65\x78\x69\x73\x74\x3a\x20"
+lw.parent_path().string());}if(std::filesystem::exists(lw)){throw std
::runtime_error("\x52\x65\x66\x75\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f"
"\x76\x65\x72\x77\x72\x69\x74\x65\x20\x65\x78\x69\x73\x74\x69\x6e\x67"
"\x20\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x3a\x20"+lw.string());}std
::filesystem::path l35=lw;l35+="\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72"
"\x74";if(std::filesystem::exists(l35)){throw std::runtime_error(""
"\x54\x65\x6d\x70\x6f\x72\x61\x72\x79\x20\x4d\x49\x44\x49\x20\x6f\x75"
"\x74\x70\x75\x74\x20\x61\x6c\x72\x65\x61\x64\x79\x20\x65\x78\x69\x73"
"\x74\x73\x3a\x20"+l35.string());}try{std::ofstream ll(l35,std::ios::
binary);if(!ll){throw std::runtime_error("\x43\x61\x6e\x6e\x6f\x74"
"\x20\x63\x72\x65\x61\x74\x65\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70"
"\x75\x74\x3a\x20"+lw.string());}ll.write("\x4d\x54\x68\x64",4);l557(
ll,6);l414(ll,0);l414(ll,1);l414(ll,l434);ll.write("\x4d\x54\x72\x6b"
,4);l557(ll,static_cast<std::uint32_t>(l12.size()));ll.write(
reinterpret_cast<const char* >(l12.data()),static_cast<std::
streamsize>(l12.size()));ll.close();if(!ll){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69\x6c\x65\x20\x77\x72\x69\x74"
"\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"
+lw.string());}std::filesystem::rename(l35,lw);}catch(...){std::
error_code l194;std::filesystem::remove(l35,l194);throw;}}lp&l230;
mutable std::mutex l147;mutable std::mutex l201;std::optional<lp::l30
>l21;std::vector<l97>l157;std::filesystem::path l309;std::filesystem
::path l527;l76::time_point l569{};};
#include<sys/event.h>
#include<sys/time.h>
#include<termios.h>
#include<unistd.h>
#include<atomic>
#include<cctype>
#include<cerrno>
#include<string_view>
class l89{public:l89()=default;l89(const l89&)=delete;l89&operator=(
const l89&)=delete;l89(l89&&)=delete;l89&operator=(l89&&)=delete;void
l812(std::chrono::milliseconds duration)noexcept{const auto l787=std
::clamp(duration.count(),20LL,2000LL);l547.store(static_cast<int>(
l787));}void l792(std::stop_token stop_token={}){l379 l824;l348 l199;
l613(l199.get());std::stop_callback l551(stop_token,[l706=l199.get()]
{struct kevent li{};EV_SET(&li,l422,EVFILT_USER,0,NOTE_TRIGGER,0,
nullptr);static_cast<void>(kevent(l706,&li,1,nullptr,0,nullptr));});
l721();try{while(!stop_token.stop_requested()){l707();const timespec
l762=l634();struct kevent li{};const int count=kevent(l199.get(),
nullptr,0,&li,1,&l762);if(count<0){if(errno==EINTR){continue;}throw
std::runtime_error("\x6b\x71\x75\x65\x75\x65\x20\x77\x61\x69\x74\x20"
"\x66\x61\x69\x6c\x65\x64\x2e");}if(count==0){continue;}if(li.filter
==EVFILT_USER&&li.ident==l422){break;}if(li.filter==EVFILT_READ&&li.
ident==STDIN_FILENO&&!l718()){break;}}}catch(...){l445();l279();throw
;}l445();l279();}private:using l76=std::chrono::steady_clock;static
constexpr uintptr_t l422=1;static constexpr int l685=24;static
constexpr int l729=108;static constexpr int l801=10;class l379{public
:l379(){if(!isatty(STDIN_FILENO)){throw std::runtime_error("\x50\x43"
"\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20\x72\x65\x71\x75\x69\x72\x65"
"\x73\x20\x61\x6e\x20\x69\x6e\x74\x65\x72\x61\x63\x74\x69\x76\x65\x20"
"\x74\x65\x72\x6d\x69\x6e\x61\x6c\x2e");}if(tcgetattr(STDIN_FILENO,&
l403)!=0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x72\x65\x61\x64\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d"
"\x6f\x64\x65\x2e");}termios l278=l403;l278.c_lflag&=static_cast<
tcflag_t>(~(ICANON|ECHO));l278.c_cc[VMIN]=1;l278.c_cc[VTIME]=0;if(
tcsetattr(STDIN_FILENO,TCSANOW,&l278)!=0){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x65\x6e\x74\x65\x72\x20\x72"
"\x61\x77\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d\x6f\x64\x65\x2e"
);}l546=true;}~l379(){if(l546){static_cast<void>(tcsetattr(
STDIN_FILENO,TCSANOW,&l403));}}private:termios l403{};bool l546=false
;};class l348{public:l348():l307(kqueue()){if(l307<0){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x6b\x71\x75\x65\x75\x65\x2e");}}~l348(){close(l307);
}int get()const noexcept{return l307;}private:int l307;};struct l190{
int lq=0;l76::time_point l283{};bool l115=false;};struct l539{int l137
=-1;double l94=0.0;};static void l613(int l199){std::array<struct
kevent,2>l291{};EV_SET(&l291[0],STDIN_FILENO,EVFILT_READ,EV_ADD|
EV_ENABLE,0,0,nullptr);EV_SET(&l291[1],l422,EVFILT_USER,EV_ADD|
EV_CLEAR,0,0,nullptr);if(kevent(l199,l291.data(),static_cast<int>(
l291.size()),nullptr,0,nullptr)!=0){throw std::runtime_error("\x46"
"\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x72\x65\x67\x69\x73\x74\x65\x72"
"\x20\x6b\x71\x75\x65\x75\x65\x20\x65\x76\x65\x6e\x74\x73\x2e");}}
bool l718(){std::array<char,64>l206{};const ssize_t count=read(
STDIN_FILENO,l206.data(),l206.size());if(count==0){return false;}if(
count<0){return errno==EINTR||errno==EAGAIN;}for(ssize_t lh=0;lh<
count;++lh){l638(l206[static_cast<std::size_t>(lh)]);}return true;}
void l638(char l75){if(l75>='1'&&l75<='9'){l424=l685+(static_cast<int
>(l75-'1') *l801);l702("\x50\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64"
"\x20\x62\x61\x73\x65\x20\x4d\x49\x44\x49\x20\x6e\x6f\x74\x65\x3a\x20"
,l424);return;}if(l588(l75)){return;}const l539 l368=l794(l75);const
int lq=l424+l368.l137;if(l368.l137<0||lq<21||lq>l729){return;}l726(lq
,l368.l94);}void l726(int lq,double l94){const auto now=l76::now();
const auto l478=std::chrono::milliseconds(l547.load());l190*l176=
nullptr;for(l190&l20:l306){if(l20.l115&&l20.lq==lq){l20.l283=now+l478
;return;}if(!l20.l115&&l176==nullptr){l176=&l20;}}if(l176==nullptr){
return;}l49(lq,l94);l689("\x4e\x6f\x74\x65\x4f\x6e\x3a\x20\x4d\x49"
"\x44\x49\x20",lq,"\x2c\x20\x76\x65\x6c\x6f\x63\x69\x74\x79\x20",l94);
l176->lq=lq;l176->l283=now+l478;l176->l115=true;}void l707(){const
auto now=l76::now();for(l190&l20:l306){if(l20.l115&&l20.l283<=now){
l98(l20.lq,0.0);l20.l115=false;}}}void l445()noexcept{for(l190&l20:
l306){if(l20.l115){l98(l20.lq,0.0);l20.l115=false;}}}timespec l634()const
noexcept{auto l295=l76::time_point::max();for(const l190&l20:l306){if
(l20.l115){l295=std::min(l295,l20.l283);}}if(l295==l76::time_point::
max()){return timespec{3600,0};}const auto l734=std::max(l76::
duration::zero(),l295-l76::now());const auto nanoseconds=std::chrono
::duration_cast<std::chrono::nanoseconds>(l734).count();return
timespec{static_cast<time_t>(nanoseconds/1000000000LL),static_cast<
long>(nanoseconds%1000000000LL)};}static l539 l794(char l75)noexcept{
const char l378=static_cast<char>(std::tolower(static_cast<unsigned
char>(l75)));constexpr std::string_view l705="\x71\x77\x65\x72\x74"
"\x79\x75\x69\x6f\x70";constexpr std::string_view l631="\x61\x73\x64"
"\x66\x67\x68\x6a\x6b\x6c\x3b";constexpr std::string_view l614="\x7a"
"\x78\x63\x76\x62\x6e\x6d\x2c\x2e\x2f";if(const auto lh=l705.find(
l378);lh!=std::string_view::npos){return{static_cast<int>(lh),112.0};
}if(const auto lh=l631.find(l378);lh!=std::string_view::npos){return{
static_cast<int>(lh),72.0};}if(const auto lh=l614.find(l378);lh!=std
::string_view::npos){return{static_cast<int>(lh),40.0};}return{};}
bool l588(char l75){switch(l75){case'-':l289=!l289;if(l289){l125(1.0);
l109("\x50\x65\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53"
"\x6f\x66\x74\x20\x70\x65\x64\x61\x6c\x20\x28\x75\x6e\x61\x20\x63\x6f"
"\x72\x64\x61\x29\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x70\x72\x65"
"\x73\x73\x65\x64\x2e");}else{l125(0.0);l109("\x50\x65\x64\x61\x6c"
"\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x6f\x66\x74\x20\x70\x65"
"\x64\x61\x6c\x20\x28\x75\x6e\x61\x20\x63\x6f\x72\x64\x61\x29\x20\x68"
"\x61\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e"
);}return true;case'=':l281=!l281;if(l281){l119(1.0);l109("\x50\x65"
"\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x48\x61\x72\x6d\x6f"
"\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65"
"\x6e\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l119(0.0);l109(""
"\x50\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x48\x61"
"\x72\x6d\x6f\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20"
"\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}return
true;case'[':l252=!l252;if(l252){l127(1.0);l109("\x50\x65\x64\x61\x6c"
"\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x6f\x73\x74\x65\x6e\x75\x74"
"\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20"
"\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l127(0.0);l109("\x50\x65"
"\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x6f\x73\x74"
"\x65\x6e\x75\x74\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62"
"\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}return true;
case']':l269=!l269;if(l269){sustainpedal_control(1.0);l109("\x50\x65"
"\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x75\x73\x74\x61"
"\x69\x6e\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e"
"\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{sustainpedal_control(
0.0);l109("\x50\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a"
"\x20\x53\x75\x73\x74\x61\x69\x6e\x20\x70\x65\x64\x61\x6c\x20\x68\x61"
"\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}
return true;return true;default:return false;}}void l279()noexcept{
l289=false;l281=false;l252=false;l269=false;l125(0.0);l119(0.0);l127(
0.0);sustainpedal_control(0.0);}static void l721(){std::cout<<"\x50"
"\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20\x73\x74\x61\x72\x74\x65"
"\x64\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e"
"\n"<<"\x20\x20\x51\x2d\x50\x20\x2f\x20\x41\x2d\x3b\x20\x2f\x20\x5a"
"\x2d\x2f\x20\x3a\x20\x76\x65\x6c\x6f\x63\x69\x74\x79\x20\x31\x31\x32"
"\x20\x2f\x20\x37\x32\x20\x2f\x20\x34\x30\n"<<"\x20\x20\x31\x2d\x39"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x3a\x20"
"\x73\x65\x6c\x65\x63\x74\x20\x70\x69\x74\x63\x68\x20\x62\x61\x6e\x6b"
"\n"<<"\x20\x20\x2d\x20\x3d\x20\x5b\x20\x5d\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x3a\x20\x73\x6f\x66\x74\x20\x2f\x20\x68\x61\x72"
"\x6d\x6f\x6e\x69\x63\x20\x2f\x20\x73\x6f\x73\x74\x65\x6e\x75\x74\x6f"
"\x20\x2f\x20\x73\x75\x73\x74\x61\x69\x6e\n";}inline void l109(std::
string_view lj){std::cout<<"\r\x1b\x5b\x32\x4b"<<lj<<std::flush;}
inline void l702(std::string_view lj,double lt){std::cout<<"\r\x1b"
"\x5b\x32\x4b"<<lj<<lt<<std::flush;}inline void l689(std::string_view
l733,double l797,std::string_view l732,double l799){std::cout<<"\r"
"\x1b\x5b\x32\x4b"<<l733<<l797<<l732<<l799<<std::flush;}std::atomic<
int>l547{350};int l424=60;std::array<l190,30>l306{};bool l289=false;
bool l281=false;bool l252=false;bool l269=false;};namespace{void l407
(std::stop_token stop_token){std::mutex mutex;std::
condition_variable_any l275;std::unique_lock lock(mutex);l275.wait(
lock,stop_token,[]{return false;});}}int l116(int l204,char*l18[],
const char*l13,const char*l29,std::stop_token l238){return l466(l204,
l18,l13,l29,l238);}void l560(const std::string&l44,std::stop_token
stop_token){std::cout<<"\x50\x6c\x61\x79\x69\x6e\x67\x20\x4d\x49\x44"
"\x49\x3a\x20"<<l44<<'\n';const l130::l56 l148=l130::l727(1.0,0.0,l44
,stop_token);if(l148==l130::l56::l214){std::cout<<"\x4d\x49\x44\x49"
"\x20\x70\x6c\x61\x79\x62\x61\x63\x6b\x20\x63\x6f\x6d\x70\x6c\x65\x74"
"\x65\x64\x2e\n";}}void l500(std::stop_token stop_token){lp l54;
MidiKeyboard l70(l54);l70.l68();l54.l68();std::cout<<"\x4d\x49\x44"
"\x49\x20\x70\x69\x61\x6e\x6f\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28"
"\x43\x74\x72\x6c\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e\n";l407(
stop_token);l54.l67();l70.l67();}void l437(std::stop_token stop_token
){l89 l70;l70.l792(stop_token);}void l561(std::string l44,std::
stop_token stop_token){std::filesystem::path lw(l44);lw.
replace_extension("\x2e\x77\x61\x76");const l285::l56 l148=l285::l663
(l44,lw,static_cast<std::uint32_t>(ln),5.0,stop_token);if(l148==l285
::l56::l214){std::cout<<"\x45\x78\x70\x6f\x72\x74\x65\x64\x20\x57\x41"
"\x56\x3a\x20"<<lw<<'\n';}}void l537(std::stop_token stop_token){lp
l54;MidiKeyboard l70(l54);l74 l549(l54);l70.l68();l549.l68();l54.l68(
);std::cout<<"\x4d\x49\x44\x49\x20\x72\x65\x63\x6f\x72\x64\x69\x6e"
"\x67\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c\x2d\x43"
"\x20\x73\x61\x76\x65\x73\x20\x61\x6e\x64\x20\x65\x78\x69\x74\x73\x29"
"\x2e\n";l407(stop_token);l54.l67();const std::filesystem::path lw=
l549.l67();l70.l67();std::cout<<"\x52\x65\x63\x6f\x72\x64\x65\x64\x20"
"\x4d\x49\x44\x49\x3a\x20"<<lw<<'\n';}void l505(std::stop_token
stop_token){std::cout<<"\x54\x65\x73\x74\x20\x73\x65\x72\x76\x69\x63"
"\x65\x20\x73\x74\x61\x72\x74\x65\x64\x2e\n\n";for(int lm=21;lm<=108;
++lm){l49(lm,110.0);}std::this_thread::sleep_for(std::chrono::
milliseconds(500));l454();}void l475(std::stop_token stop_token){std
::cout<<"\x49\x6e\x74\x65\x72\x6e\x61\x6c\x20\x74\x65\x73\x74\x20\x73"
"\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65\x78"
"\x69\x74\x73\x29\x2e\n";sustainpedal_control(0.6);l49(69,110);l407(
stop_token);l59();}
#include<numbers>
class Damper{float l124=0.0f;float l209=0.0f;public:inline void l34(
float&lo){constexpr float l597=0.020f;constexpr float l632=0.25f;
constexpr float l697=0.38f;constexpr float l452=1.0f-l597;constexpr
float l510=l697*l632;constexpr float l747=1.0f-l510;constexpr float
l637=l452*l747;constexpr float l789=l452*l510;constexpr float l163=
0.292893218813f;constexpr float l191=0.585786437627f;constexpr float
l335=0.292893218813f;constexpr float l400=0.171572875254f;const float
l38=l163*lo+l124;l124=l191*lo+l209;l209=l335*lo-l400*l38;lo=l637*lo+
l789*l38;}inline void lx()noexcept{l124=0.0f;l209=0.0f;}inline float
state_energy()const noexcept{return 0.5f* (l124*l124+l209*l209);}};
class l490{float l120=0.0;float l217=0.0;float l212=0.0;public:bool
l578=false;l490(double l542,double l430){if(std::abs(l542)<1.0e-12){
l578=true;l120=0.0f;return;}const double l224=std::tan(0.5*l430*l542)/
std::tan(0.5*l430);const double l423=(1.0-l224)/(1.0+l224);if(!std::
isfinite(l423)||std::abs(l423)>=1.0){throw std::runtime_error("\x66"
"\x72\x61\x63\x74\x69\x6f\x6e\x61\x6c\x5f\x66\x69\x6c\x74\x65\x72\x3a"
"\x20\x69\x6e\x76\x61\x6c\x69\x64\x20\x61\x6c\x6c\x70\x61\x73\x73\x20"
"\x63\x6f\x65\x66\x66\x69\x63\x69\x65\x6e\x74");}l120=static_cast<
float>(l423);}inline void l34(float&lo){if(l578){return;}const float
l38=l217+l120* (lo-l212);l217=lo;l212=l38;lo=l38;}inline void lx(){
l217=0.0;l212=0.0;}inline float state_energy()const noexcept{return
0.5f* (l217*l217+l212*l212);}};
#include<complex>
class l308{inline static constexpr int l23=21;inline static constexpr
int l80=108;inline static constexpr std::size_t l299=2;struct ld{
float l163;float l191;float l120;};struct lf{int lm;double l425;
double l813;double l600;std::array<ld,l299>l276;};public:lf l273;
private:std::array<float,l299>l36{};public:explicit l308(int lm)noexcept
:l273(l192[static_cast<std::size_t>(std::clamp(lm,l23,l80)-l23)]){}[[
nodiscard]]inline double l808()const noexcept{return l273.l600;}[[
nodiscard]]inline double l598(double l355,double l397)const{const
double l550=2.0*std::numbers::pi_v<double> *l397/l355;const std::
complex<double>l124=std::polar(1.0,-l550);std::complex<double>l453{
1.0,0.0};for(const auto&l85:l273.l276)l453*=(double(l85.l163)+double(
l85.l191) *l124)/(1.0+double(l85.l120) *l124);return-std::arg(l453)/
l550;}inline void l34(float&lo)noexcept{for(std::size_t lc=0;lc<l299;
++lc){const auto&l85=l273.l276[lc];const float l38=l85.l163*lo+l36[lc
];l36[lc]=l85.l191*lo-l85.l120*l38;lo=l38;}}inline void lx()noexcept{
l36.fill(0.0f);}[[nodiscard]]inline float state_energy()const noexcept
{float l571=0.0f;for(float lt:l36)l571+=lt*lt;return l571/static_cast
<float>(l299);}private:inline static constexpr std::array<lf,88>l192=
{{lf{21,27.5,0.90716426692764396,0.9071646747176394,{{ld{0.509180427f
,0.508661687f,0.0483981334f},ld{0.524460435f,0.52394706f,
0.0484074801f},}}},lf{22,29.13523509488062,0.88551598492877714,
0.88551653056390844,{{ld{0.515856087f,0.515351951f,0.0604129694f},ld{
0.530497193f,0.529971659f,0.0604688823f},}}},lf{23,30.867706328507751
,0.86378779457930455,0.86378849770897626,{{ld{0.522680938f,
0.522195697f,0.0727941841f},ld{0.536697924f,0.536072493f,
0.0727704167f},}}},lf{24,32.703195662574828,0.84231887734651623,
0.84231975921181745,{{ld{0.529564381f,0.529080033f,0.085329771f},ld{
0.542902589f,0.542405844f,0.0853083879f},}}},lf{25,34.64782887210901,
0.82080496635912426,0.82080605026171982,{{ld{0.536565423f,
0.536063969f,0.0981385782f},ld{0.549319744f,0.548812091f,
0.0981318802f},}}},lf{26,36.70809598967594,0.79922932279552239,
0.79923063493358293,{{ld{0.54370755f,0.543204606f,0.111300826f},ld{
0.55590862f,0.555393159f,0.111301772f},}}},lf{27,38.890872965260115,
0.77695290386536875,0.77695446585983385,{{ld{0.551802397f,
0.549461842f,0.124579713f},ld{0.562767565f,0.561819613f,0.124587208f}
,}}},lf{28,41.203444614108747,0.75690961413037727,0.75691146524411568
,{{ld{0.558250427f,0.557452202f,0.137990847f},ld{0.569241643f,
0.568750918f,0.137992606f},}}},lf{29,43.653528929125486,
0.73630926537057295,0.73631143434880675,{{ld{0.565373719f,
0.564894021f,0.151573971f},ld{0.576047182f,0.575548828f,0.151596025f}
,}}},lf{30,46.2493028389543,0.71603221279778084,0.71603473319048061,{
{ld{0.572652698f,0.572174609f,0.165191963f},ld{0.582844019f,
0.58235693f,0.165200919f},}}},lf{31,48.999429497718666,
0.69589490920829777,0.69589781890593205,{{ld{0.580047488f,
0.579477608f,0.178995475f},ld{0.589761376f,0.589222431f,0.178983793f}
,}}},lf{32,51.913087197493141,0.6762882890346601,0.67629162994047298,
{{ld{0.587343633f,0.586869538f,0.192821503f},ld{0.596657455f,
0.596169353f,0.192826778f},}}},lf{33,55,0.65677078335117789,
0.65677460032584234,{{ld{0.594782948f,0.594311178f,0.206880808f},ld{
0.603679717f,0.603197217f,0.206876919f},}}},lf{34,58.270470189761241,
0.63765584921914897,0.63766019039365374,{{ld{0.602217615f,
0.601748049f,0.220966235f},ld{0.610726357f,0.610236168f,0.220962524f}
,}}},lf{35,61.735412657015502,0.61891035299060482,0.61891527074279018
,{{ld{0.609672725f,0.609206378f,0.235127836f},ld{0.617786109f,
0.617287874f,0.235073969f},}}},lf{36,65.406391325149656,
0.46606000189624142,0.46606333375157766,{{ld{0.709973693f,
0.508443058f,0.233750343f},ld{0.706111848f,0.527553976f,0.233665884f}
,}}},lf{37,69.295657744218019,0.33795031888478599,0.3379514202006888,
{{ld{0.789180994f,0.390327096f,0.193514511f},ld{0.798136592f,
0.395159841f,0.193296418f},}}},lf{38,73.416191979351879,
0.32446968201521958,0.32447051448061071,{{ld{0.641469777f,
0.441583872f,0.095186308f},ld{0.992804527f,0.972382426f,0.965186954f}
,}}},lf{39,77.781745930520231,0.26545615584581045,0.26545572753212443
,{{ld{0.706958234f,0.365230888f,0.0835227966f},ld{0.996231437f,
0.986721754f,0.982953131f},}}},lf{40,82.406889228217494,
0.23121818140255349,0.23121689314889357,{{ld{0.745844185f,
0.317866057f,0.0743207335f},ld{0.996882737f,0.988090754f,0.98497349f}
,}}},lf{41,87.307057858250971,0.20170706983317438,0.20170534215666122
,{{ld{0.777087629f,0.288775146f,0.0759222656f},ld{0.997336626f,
0.987483203f,0.984819829f},}}},lf{42,92.4986056779086,
0.18298007617707771,0.18297774073523468,{{ld{0.798847556f,
0.261485577f,0.0697784498f},ld{0.996821523f,0.988175392f,0.984996915f
},}}},lf{43,97.998858995437331,0.16574132548992879,
0.16573837139231343,{{ld{0.816609859f,0.239585862f,0.0650762916f},ld{
0.999991238f,0.983839512f,0.98383075f},}}},lf{44,103.82617439498628,
0.15340556480886153,0.15340213410908995,{{ld{0.830932021f,
0.223347992f,0.0626475513f},ld{0.99898237f,0.986017644f,0.985000014f}
,}}},lf{45,110,0.14057175715261291,0.14056772174027904,{{ld{
0.84485811f,0.205899343f,0.0586304404f},ld{0.99999243f,0.985005498f,
0.984997928f},}}},lf{46,116.54094037952248,0.13194577695036819,
0.13194142342631751,{{ld{0.853956044f,0.199678212f,0.0610868856f},ld{
0.999991536f,0.98500824f,0.984999716f},}}},lf{47,123.47082531403103,
0.122932070583298,0.12292724873682467,{{ld{0.864486277f,0.188451856f,
0.0599704273f},ld{0.998938501f,0.985935986f,0.984874487f},}}},lf{48,
130.81278265029931,0.11726177523485393,0.11725675281697417,{{ld{
0.871660352f,0.185513124f,0.0638398975f},ld{0.996441066f,0.988557756f
,0.984998822f},}}},lf{49,138.59131548843604,0.11023193956390301,
0.11022650456498929,{{ld{0.970262229f,0.0248138309f,0.00100000005f},
ld{0.90311116f,0.219625294f,0.122736461f},}}},lf{50,
146.83238395870379,0.10448517554549977,0.10447932426321985,{{ld{
0.958856881f,0.0365479961f,0.00100000005f},ld{0.919616461f,
0.249251649f,0.168868139f},}}},lf{51,155.56349186104046,
0.098940300779251977,0.098934106275699582,{{ld{0.957032382f,
0.0386828519f,0.00100000005f},ld{0.927161217f,0.265097588f,
0.19225882f},}}},lf{52,164.81377845643496,0.093876942006254085,
0.093870367412676708,{{ld{0.956007719f,0.0400018543f,0.00100000005f},
ld{0.933470905f,0.282402754f,0.215873659f},}}},lf{53,
174.61411571650194,0.088216497057697338,0.088209468102949093,{{ld{
0.956085384f,0.0401989892f,0.00100000005f},ld{0.939637244f,
0.295529902f,0.235167161f},}}},lf{54,184.9972113558172,
0.081874532355640517,0.081866728552633966,{{ld{0.948426247f,
0.048101902f,0.00100000005f},ld{0.953352094f,0.394542992f,
0.347895116f},}}},lf{55,195.99771799087463,0.079403861189346711,
0.079395778395922129,{{ld{0.956778407f,0.0400209539f,0.00100000005f},
ld{0.948613226f,0.32784009f,0.276453346f},}}},lf{56,
207.65234878997256,0.074875933368343084,0.074867620877405297,{{ld{
0.948016644f,0.0731995255f,0.0252859648f},ld{0.962163746f,
0.395742744f,0.357906491f},}}},lf{57,220,0.068620238112223605,
0.068610706070286867,{{ld{0.955820799f,0.0414119139f,0.00100000005f},
ld{0.961351275f,0.414186478f,0.375537753f},}}},lf{58,
233.08188075904496,0.067523041778097467,0.067514426673541947,{{ld{
0.938335717f,0.122939162f,0.0650494993f},ld{0.980198801f,0.571277678f
,0.551476419f},}}},lf{59,246.94165062806206,0.06373878282173423,
0.063729639280263137,{{ld{0.94180274f,0.120574035f,0.0659451708f},ld{
0.981056929f,0.582652271f,0.563709199f},}}},lf{60,261.62556530059862,
0.056928784569556183,0.056916971741435812,{{ld{0.961704791f,
0.0361174941f,0.00100000005f},ld{0.969245374f,0.445581794f,
0.414827168f},}}},lf{61,277.18263097687208,0.051863156817875206,
0.05184992702763528,{{ld{0.962329268f,0.0356558673f,0.00100000005f},
ld{0.974796534f,0.495570719f,0.470367283f},}}},lf{62,
293.66476791740757,0.052558724607467883,0.052546743102426727,{{ld{
0.9568578f,0.0852884501f,0.0450923853f},ld{0.979442894f,0.502439857f,
0.481882781f},}}},lf{63,311.12698372208092,0.049443333938828764,
0.049430413027113804,{{ld{0.958348155f,0.0859873146f,0.0471233912f},
ld{0.981708527f,0.52803278f,0.509741306f},}}},lf{64,
329.62755691286992,0.05018487445041131,0.050172868054033259,{{ld{
0.952816129f,0.125764444f,0.0812796876f},ld{0.986026585f,0.609295309f
,0.595321953f},}}},lf{65,349.22823143300388,0.043864353550868297,
0.043849830886102087,{{ld{0.964000463f,0.0780902132f,0.0445734337f},
ld{0.982624471f,0.514371037f,0.496995509f},}}},lf{66,
369.9944227116344,0.041445699304470202,0.041429139620925384,{{ld{
0.971147656f,0.0412886031f,0.0147097185f},ld{0.97836256f,0.448060066f
,0.426422626f},}}},lf{67,391.99543598174927,0.038569751150702061,
0.038551444027183399,{{ld{0.973727226f,0.0304478798f,0.0063054515f},
ld{0.979394138f,0.439392835f,0.418787003f},}}},lf{68,
415.30469757994513,0.038647813187822573,0.038630939741655525,{{ld{
0.965877533f,0.0969027132f,0.0648946837f},ld{0.987163246f,
0.537800133f,0.524963379f},}}},lf{69,440,0.034213136651369561,
0.034192177687860578,{{ld{0.977191687f,0.0219166316f,0.00100000005f},
ld{0.981147885f,0.437750757f,0.418898672f},}}},lf{70,
466.16376151808993,0.031707762542134407,0.031684544575275947,{{ld{
0.977934003f,0.0212783217f,0.00100000005f},ld{0.983647823f,
0.447832286f,0.43148008f},}}},lf{71,493.88330125612413,
0.030318248694925849,0.03029388714829187,{{ld{0.979133189f,
0.0201799199f,0.00100000005f},ld{0.983928025f,0.459363192f,
0.443291217f},}}},lf{72,523.25113060119725,0.03068623959455101,
0.030665106224438483,{{ld{0.972732961f,0.0948506445f,0.069271937f},ld
{0.989834726f,0.542254806f,0.532089531f},}}},lf{73,554.36526195374415
,0.027104148923911456,0.027076669008337221,{{ld{0.981550395f,
0.0179468431f,0.00100000005f},ld{0.985468745f,0.444626749f,
0.430095524f},}}},lf{74,587.32953583481515,0.025603578913498359,
0.025574067243458867,{{ld{0.982260883f,0.0173193868f,0.00100000005f},
ld{0.986480057f,0.468735278f,0.455215335f},}}},lf{75,
622.25396744416184,0.024063781460123581,0.02403193743460982,{{ld{
0.982919574f,0.0167379603f,0.00100000005f},ld{0.987605214f,
0.499471009f,0.487076193f},}}},lf{76,659.25511382573984,
0.022281722725889672,0.022246635253493741,{{ld{0.983857989f,
0.0158759598f,0.00100000005f},ld{0.989221215f,0.463819087f,
0.453040302f},}}},lf{77,698.45646286600777,0.02065037867038455,
0.02061290564519579,{{ld{0.9846977f,0.0151007511f,0.00100000005f},ld{
0.990332127f,0.482459813f,0.47279191f},}}},lf{78,739.9888454232688,
0.020043458871884919,0.020004698848767131,{{ld{0.98559016f,
0.0142779136f,0.00100000005f},ld{0.989940584f,0.493299633f,
0.483240217f},}}},lf{79,783.99087196349853,0.018325303177284917,
0.018281455438323522,{{ld{0.985981226f,0.0139489817f,0.00100000005f},
ld{0.991916418f,0.506072938f,0.497989357f},}}},lf{80,
830.60939515989025,0.017645326943717989,0.017600578528719345,{{ld{
0.986990869f,0.0129980929f,0.00100000005f},ld{0.991523027f,
0.503933728f,0.495456755f},}}},lf{81,880,0.016392722264951056,
0.016343443718932864,{{ld{0.987505317f,0.0125398571f,0.00100000005f},
ld{0.992678523f,0.5085181f,0.501196623f},}}},lf{82,932.32752303617985
,0.0094970911571389555,0.0094607660781394938,{{ld{0.991669536f,
0.00827072188f,0.00100000005f},ld{0.996464968f,0.579506099f,
0.575971127f},}}},lf{83,987.76660251224826,0.014365968890251529,
0.014308583761182969,{{ld{0.988678813f,0.0114687765f,0.00100000005f},
ld{0.994058967f,0.518969297f,0.513028204f},}}},lf{84,
1046.5022612023945,0.013574570699782013,0.013514183071725415,{{ld{
0.989387631f,0.0108065158f,0.00100000005f},ld{0.994288981f,
0.508853912f,0.503142893f},}}},lf{85,1108.7305239074883,
0.01276677222496244,0.012702772695009284,{{ld{0.990014732f,
0.0102229388f,0.00100000005f},ld{0.994642735f,0.502920568f,
0.497563332f},}}},lf{86,1174.6590716696303,0.012941280600085283,
0.012880002647250057,{{ld{0.991223514f,0.00906440616f,0.00100000005f}
,ld{0.993042648f,0.424177974f,0.417220592f},}}},lf{87,
1244.5079348883237,0.012221098748060795,0.012160903879548815,{{ld{
0.989729166f,0.0512266159f,0.0416597314f},ld{0.995333135f,
0.543007791f,0.538340986f},}}},lf{88,1318.5102276514797,
0.0017922484159481496,0.001798657881747373,{{ld{0.996675551f,
0.442208976f,0.440436333f},ld{0.999548852f,0.985142946f,0.984691739f}
,}}},lf{89,1396.9129257320155,0.0047712965714784674,
0.0047233363066292609,{{ld{0.995141327f,0.00506559992f,0.00100000005f
},ld{0.998724103f,0.749601364f,0.748325467f},}}},lf{90,
1479.9776908465376,0.010812578606363645,0.010746295946221391,{{ld{
0.990956485f,0.0596992671f,0.0512560122f},ld{0.995483577f,
0.628141522f,0.6236251f},}}},lf{91,1567.9817439269971,
0.0098216176121961995,0.0097395917746298487,{{ld{0.993366241f,
0.00709735323f,0.00100000005f},ld{0.994622886f,0.432546169f,
0.427169055f},}}},lf{92,1661.2187903197805,0.00902255340091874,
0.0089306525999325211,{{ld{0.992716491f,0.0256401431f,0.018873984f},
ld{0.996320605f,0.527242482f,0.523563087f},}}},lf{93,1760,
0.0055945008864219641,0.005506444387682401,{{ld{0.994621933f,
0.0057295477f,0.00100000005f},ld{0.998482108f,0.669281781f,
0.667763829f},}}},lf{94,1864.6550460723597,0.0082852901255033654,
0.0081898274417011689,{{ld{0.9935866f,0.0297955833f,0.0238480493f},ld
{0.996219933f,0.50925529f,0.505475223f},}}},lf{95,1975.5332050244961,
0.0055404491964765375,0.0054367339517353563,{{ld{0.994855464f,
0.00554319937f,0.00100000005f},ld{0.998185754f,0.72871834f,
0.726904094f},}}},lf{96,2093.004522404789,0.060685177171099142,
0.060692289086111587,{{ld{0.97908479f,0.0214933921f,0.00100000005f},
ld{0.925710559f,0.922291398f,0.848001957f},}}},lf{97,
2217.4610478149766,0.060749840930865204,0.060774294471047235,{{ld{
0.980520725f,0.0200777538f,0.00100000005f},ld{0.923189104f,
0.920074701f,0.843263745f},}}},lf{98,2349.3181433392601,
0.016905258493571152,0.016756016971397454,{{ld{0.98256427f,
0.0179850217f,0.00100000005f},ld{0.99999243f,0.985007584f,
0.985000014f},}}},lf{99,2489.0158697766474,0.04549384227279344,
0.045464904972224253,{{ld{0.981879115f,0.018758826f,0.00100000005f},
ld{0.947710276f,0.935872495f,0.883582771f},}}},lf{100,
2637.0204553029598,0.014840154729723729,0.014673957114837209,{{ld{
0.984619975f,0.0159272701f,0.00100000005f},ld{0.99999243f,
0.985007584f,0.985000014f},}}},lf{101,2793.8258514640311,
0.01452019461730175,0.014337431487034582,{{ld{0.984981537f,
0.015616239f,0.00100000005f},ld{0.99999243f,0.985007584f,0.985000014f
},}}},lf{102,2959.9553816930752,0.010612211603816275,
0.010460440954997697,{{ld{0.988836408f,0.0116895735f,0.00100000005f},
ld{0.99999243f,0.985007584f,0.985000014f},}}},lf{103,
3135.9634878539946,0.010844919341281791,0.01067088188757064,{{ld{
0.988635302f,0.0119341016f,0.00100000005f},ld{0.99999243f,
0.985007584f,0.985000014f},}}},lf{104,3322.437580639561,
0.011659108656308941,0.011449516347192946,{{ld{0.987855554f,
0.012767286f,0.00100000005f},ld{0.99999243f,0.985007584f,0.985000014f
},}}},lf{105,3520,0.01138980654730897,0.011159682642585593,{{ld{
0.988144815f,0.0125085283f,0.00100000005f},ld{0.99999243f,
0.985007584f,0.985000014f},}}},lf{106,3729.3100921447194,
0.01253811026852846,0.012254592508744652,{{ld{0.987052023f,
0.0136855589f,0.00100000005f},ld{0.99999243f,0.985007584f,
0.985000014f},}}},lf{107,3951.0664100489921,0.015514637692152404,
0.0154273199751744,{{ld{0.981381297f,0.19368723f,0.175470486f},ld{
0.99999243f,0.985007584f,0.985000014f},}}},lf{108,4186.009044809578,
0.011231121314898345,0.01090946437169898,{{ld{0.988356352f,
0.012397076f,0.00100000005f},ld{0.99999243f,0.985007584f,0.985000014f
},}}},}};};
class l504{inline static constexpr std::size_t l196=4;inline static
constexpr std::size_t l826=237;struct la{float l120;float l400;};
struct lb{int lm;double l425;double l814;double l355;double l686;std
::size_t l433;std::array<la,l196>l276;};lb l164;std::array<float,l196
>l159{};std::array<float,l196>l232{};std::array<float,l196>l160{};std
::array<float,l196>l195{};public:explicit l504(double l397)noexcept:
l164(l739(l397)){}inline void l34(float&lo)noexcept{for(std::size_t lc
=0;lc<l164.l433;++lc){const auto&l85=l164.l276[lc];const float l38=
l232[lc]+l85.l120* (l159[lc]-l160[lc])+l85.l400* (lo-l195[lc]);l232[
lc]=l159[lc];l159[lc]=lo;l195[lc]=l160[lc];l160[lc]=l38;lo=l38;}}
inline void lx()noexcept{l159.fill(0.0f);l232.fill(0.0f);l160.fill(
0.0f);l195.fill(0.0f);}[[nodiscard]]inline double state_energy()const
noexcept{double l540=0.0;for(std::size_t lc=0;lc<l164.l433;++lc)l540
+=double(l159[lc]) *l159[lc]+double(l232[lc]) *l232[lc]+double(l160[
lc]) *l160[lc]+double(l195[lc]) *l195[lc];return l540;}[[nodiscard]]
inline double l800()const noexcept{return l164.l686;}[[nodiscard]]
inline double l766()const noexcept{return l164.l355;}private:[[
nodiscard]]inline static const lb&l739(double l52)noexcept{std::
size_t l534=0;double l471=std::abs(l192[0].l425-l52);for(std::size_t
lc=1;lc<l192.size();++lc){const double l575=std::abs(l192[lc].l425-
l52);if(l575<l471){l471=l575;l534=lc;}}return l192[l534];}inline
static constexpr std::array<lb,237>l192={{lb{21,27.3782,
5.4850291759431087e-05,44100,1541.7305100913354,1,{{la{-1.90728378f,
0.909946561f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{22,
29.0101,5.3600800116909449e-05,44100,1455.264909384322,1,{{la{-
1.90071738f,0.903761387f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{23,30.7393,5.2581980993204519e-05,44100,1374.6424138246502,1,{{
la{-1.8939029f,0.897369862f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{24,32.5716,5.17816694448717e-05,44100,1297.3661782899435,1,{{
la{-1.88674212f,0.890680254f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{25,34.5131,5.1190427994123997e-05,44100,1224.671388938563
,1,{{la{-1.87943876f,0.883891225f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{26,36.5703,5.0801325194743499e-05,44100,
1156.6907988106329,1,{{la{-1.8719455f,0.87696135f},la{0.0f,0.0f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{27,38.7502,5.0609819378936862e-05,
44100,1091.6784149089171,1,{{la{-1.86419332f,0.869826496f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{28,41.06,
5.0613704795535706e-05,44100,1029.9373047879021,1,{{la{-1.85628712f,
0.862589061f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{29,
43.5075,5.081302372431709e-05,44100,971.98672922011406,1,{{la{-
1.84819758f,0.855223536f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{30,46.1008,5.1210071831366423e-05,44100,917.11638324109629,1,{{
la{-1.84008586f,0.847881615f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{31,48.8248,5.180354900715938e-05,44100,865.80304148471339
,1,{{la{-1.83175099f,0.840378582f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{31,48.8488,5.1809506247468992e-05,44100,
865.36617035449808,1,{{la{-1.83174014f,0.840369821f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{32,51.7351,5.2610545216462892e-05,
44100,816.5033262965826,1,{{la{-1.82346487f,0.832967877f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{32,51.7605,
5.2618306901262375e-05,44100,816.10064392066192,1,{{la{-1.82339191f,
0.832902849f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{33,
54.8189,5.3636409104809483e-05,44100,770.97618189053412,1,{{la{-
1.81493628f,0.825384736f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{33,54.8458,5.3646069605784835e-05,44100,770.59380513931922,1,{{
la{-1.81488287f,0.825337946f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{34,58.084,5.4892370405986803e-05,44100,727.34574204677892
,1,{{la{-1.80638218f,0.817827463f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{34,58.1126,5.4904088538290463e-05,44100,
726.97922677321367,1,{{la{-1.80635965f,0.817808688f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{35,61.5437,5.6394372616627355e-05,
44100,685.65388992961891,1,{{la{-1.79784334f,0.810333908f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{35,61.5739,
5.6408210716939515e-05,44100,685.3045228564747,1,{{la{-1.79786325f,
0.810353398f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{36,
65.2094,5.8160810311740388e-05,44100,647.17713547280141,1,{{la{-
1.78948474f,0.803052485f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{36,65.2414,5.8176980625087649e-05,44100,646.85176856332339,1,{{
la{-1.78946221f,0.803034186f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{37,69.0934,6.0213806022023323e-05,44100,
610.39644925454104,1,{{la{-1.78124762f,0.795928895f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{37,69.1273,6.0232509095508895e-05,
44100,610.09456927639189,1,{{la{-1.78117132f,0.795863211f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{38,73.2088,
6.2579636542420321e-05,44100,575.69150124584189,1,{{la{-1.77267659f,
0.788554311f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{38,
73.2447,6.2601105219795922e-05,44100,575.39312308055673,1,{{la{-
1.77275443f,0.788625002f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{38,73.2807,6.2622647886681856e-05,44100,575.09871134776449,1,{{
la{-1.77278173f,0.788651168f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{39,77.5692,6.5289084468286726e-05,44100,
542.85526282450405,1,{{la{-1.76506317f,0.782082379f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{39,77.6074,6.5313716505639545e-05,
44100,542.58205968392144,1,{{la{-1.76502848f,0.782054305f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{39,77.6455,
6.5338299358686647e-05,44100,542.31181894593988,1,{{la{-1.76496768f,
0.782003224f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{40,
82.1895,6.8378564983695274e-05,44100,511.7028347978403,1,{{la{-
1.75697994f,0.775241733f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{40,82.2299,6.8406550346842109e-05,44100,511.43316165908902,1,{{
la{-1.7571075f,0.775355577f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{40,82.2702,6.8434483060773974e-05,44100,511.16892606758256,1,
{{la{-1.75717556f,0.775417507f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{41,87.0849,7.1890266102344605e-05,44100,
467.63522064770319,2,{{la{-1.71961462f,0.758091688f},la{-1.74344909f,
0.761718035f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{41,87.1277,
7.1922034626807979e-05,44100,467.38034294030035,2,{{la{-1.74358439f,
0.761837125f},la{-1.71972156f,0.758194506f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{41,87.1705,7.1953821532202286e-05,44100,
467.13208985764447,2,{{la{-1.74365556f,0.761900365f},la{-1.71976578f,
0.758244991f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{42,92.2719,
7.5873719965610862e-05,44100,440.54259396528943,2,{{la{-1.73672414f,
0.756019533f},la{-1.71059048f,0.7519328f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lb{42,92.3172,7.5909690237425587e-05,44100,440.32054014149577,2,{
{la{-1.73666394f,0.755968809f},la{-1.71050513f,0.751875341f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{42,92.3625,7.594568091914677e-05,44100,
440.10007447267259,2,{{la{-1.71040261f,0.751802862f},la{-1.73658466f,
0.755901456f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{43,97.7678,
8.0386576743260001e-05,44100,415.00789633402809,2,{{la{-1.72976196f,
0.750146508f},la{-1.70123148f,0.745624542f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{43,97.8158,8.0427313370761084e-05,44100,
414.79181753560545,2,{{la{-1.72978604f,0.750168979f},la{-1.70122385f,
0.745634556f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{43,97.8639,
8.0468157850107554e-05,44100,414.57884517223556,2,{{la{-1.72976816f,
0.750155151f},la{-1.70117569f,0.745610416f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{44,103.591,8.5496148564465589e-05,44100,
390.76785299239674,2,{{la{-1.72403705f,0.74538517f},la{-1.69254625f,
0.740105987f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{44,103.642,
8.5542394832813175e-05,44100,390.56648219051255,2,{{la{-1.69250095f,
0.740086138f},la{-1.72402477f,0.745376408f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{44,103.693,8.5588667164501184e-05,44100,
390.36759668382945,2,{{la{-1.6924243f,0.740039587f},la{-1.72397971f,
0.745339334f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{45,109.761,
9.1281010195021513e-05,44100,368.06996703882902,2,{{la{-1.71835983f,
0.740683317f},la{-1.68398094f,0.734822631f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{45,109.815,9.133333879731857e-05,44100,367.98798158442258
,2,{{la{-1.71672058f,0.739276588f},la{-1.68243909f,0.733532131f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{45,109.869,9.1385697045794626e-05,
44100,367.76400986847068,2,{{la{-1.71716464f,0.739658654f},la{-
1.68283749f,0.733879328f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{46,
116.295,9.7829378225853023e-05,44100,346.51591724918137,2,{{la{-
1.7135973f,0.736794472f},la{-1.6760515f,0.730251372f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lb{46,116.352,9.7888438760023548e-05,44100,
346.33941344991752,2,{{la{-1.71356142f,0.736765802f},la{-1.67597783f,
0.730211318f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{46,116.41,
9.7948570054725719e-05,44100,346.16153923271332,2,{{la{-1.71349943f,
0.73671484f},la{-1.67587984f,0.73015058f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lb{47,123.218,0.00010525133288627102,44100,326.15791582683482,2,{
{la{-1.70950055f,0.73349911f},la{-1.66855502f,0.726238549f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{47,123.279,0.0001053189775995625,44100,
326.00006034973956,2,{{la{-1.70898354f,0.733052075f},la{-1.66824913f,
0.72594434f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{47,123.339,
0.00010538555204720589,44100,325.93954192659555,2,{{la{-1.70741618f,
0.731720984f},la{-1.66655993f,0.724586189f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{48,130.553,0.00011367230448717747,44100,
306.92120419999026,2,{{la{-1.7061435f,0.730855882f},la{-1.66162205f,
0.722863913f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{48,130.617,
0.00011374835315072723,44100,306.75740024921924,2,{{la{-1.70623505f,
0.730936348f},la{-1.66166723f,0.722928584f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{48,130.682,0.00011382563612761529,44100,
306.59420970337823,2,{{la{-1.70627701f,0.730974615f},la{-1.66166234f,
0.722951889f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{49,138.325,
0.00012324025444501751,44100,288.72989675985536,2,{{la{-1.70381606f,
0.7291044f},la{-1.65560985f,0.720463634f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lb{49,138.393,0.00012332696434276942,44100,288.57756617350378,2,{
{la{-1.7038666f,0.729150057f},la{-1.65561128f,0.720494568f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{49,138.461,0.00012341372685106358,44100,
288.42759507611714,2,{{la{-1.65557218f,0.720491052f},la{-1.70387566f,
0.729160428f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{50,146.559,
0.00013412721454709035,44100,271.6302366314066,2,{{la{-1.70046377f,
0.726493001f},la{-1.64856255f,0.717350602f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{50,146.631,0.00013422590355159281,44100,
271.46127700433595,2,{{la{-1.70097804f,0.726932049f},la{-1.64903319f,
0.717769146f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{50,146.703,
0.00013432465389000837,44100,271.29872031551616,2,{{la{-1.70138562f,
0.727280855f},la{-1.64939523f,0.71810025f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{51,155.284,0.00014653930235788255,44100,255.34876719705056,2,
{{la{-1.70043147f,0.72671622f},la{-1.6447041f,0.717114031f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{51,155.36,0.00014665148916490561,44100,
255.19747770911891,2,{{la{-1.70081687f,0.72704643f},la{-1.6450429f,
0.717430353f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{51,155.436,
0.00014676374729231839,44100,255.05086523647654,2,{{la{-1.70111585f,
0.727303326f},la{-1.6452924f,0.717673898f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{52,164.528,0.00016071595204200263,44100,240.26494508485547,2,
{{la{-1.68809927f,0.716412425f},la{-1.62984002f,0.706115425f},la{0.0f
,0.0f},la{0.0f,0.0f}}}},lb{52,164.609,0.00016084498160109391,44100,
240.14081229761734,2,{{la{-1.68806255f,0.716384232f},la{-1.62975192f,
0.706074893f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{52,164.689,
0.00016097250147062826,44100,240.01891853813606,2,{{la{-1.68800771f,
0.716340601f},la{-1.62964988f,0.706020772f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{53,174.322,0.00017694240546449472,44100,
225.52405521060365,2,{{la{-1.70074975f,0.727513492f},la{-1.63737762f,
0.717504442f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{53,174.408,
0.00017709056309902615,44100,225.3776672269214,2,{{la{-1.70137f,
0.728043079f},la{-1.63795972f,0.718029499f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{53,174.493,0.0001772370966770329,44100,225.23866630124314
,2,{{la{-1.70187414f,0.728474557f},la{-1.63842142f,0.718456686f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{54,184.699,0.0001951909802984074,44100
,203.9999694512602,3,{{la{-1.68527603f,0.71306771f},la{-1.64684045f,
0.707494318f},la{-1.53819752f,0.681497335f},la{0.0f,0.0f}}}},lb{54,
184.79,0.00019535414519352071,44100,203.90586837843782,3,{{la{-
1.68091476f,0.709328413f},la{-1.64433408f,0.704797864f},la{-
1.53855598f,0.680709898f},la{0.0f,0.0f}}}},lb{54,184.881,
0.00019551736416016424,44100,203.73809291804122,3,{{la{-1.68201184f,
0.710252106f},la{-1.64541388f,0.705712736f},la{-1.53948498f,
0.681490779f},la{0.0f,0.0f}}}},lb{55,195.694,0.00021529375640180309,
44100,200.09579898333064,2,{{la{-1.64323115f,0.679390848f},la{-
1.57884014f,0.66754061f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{55,195.79,
0.00021547270321706786,44100,199.99988417289157,2,{{la{-1.64257419f,
0.67884165f},la{-1.57848203f,0.667183518f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{55,195.886,0.00021565170887217493,44100,199.99992166490415,2,
{{la{-1.63336277f,0.671094298f},la{-1.57512319f,0.662976563f},la{0.0f
,0.0f},la{0.0f,0.0f}}}},lb{56,207.344,0.00023743618266172512,44100,
187.3630276126153,2,{{la{-1.67523205f,0.706587732f},la{-1.60162377f,
0.694542706f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{56,207.445,
0.00023763187926342927,44100,187.24404108287766,2,{{la{-1.60212827f,
0.695002556f},la{-1.67578137f,0.70705086f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{56,207.547,0.00023782957807628698,44100,187.12795321240651,2,
{{la{-1.67624247f,0.707440495f},la{-1.60253906f,0.695388138f},la{0.0f
,0.0f},la{0.0f,0.0f}}}},lb{57,219.687,0.00026181988958400476,44100,
168.53956139982969,3,{{la{-1.65688801f,0.689693034f},la{-1.61376131f,
0.683596373f},la{-1.49376166f,0.657053471f},la{0.0f,0.0f}}}},lb{57,
219.794,0.00026203536410030967,44100,168.44816431987141,3,{{la{-
1.65694106f,0.689739704f},la{-1.61377382f,0.683634996f},la{-
1.49365652f,0.657052994f},la{0.0f,0.0f}}}},lb{57,219.902,
0.00026225292323324228,44100,168.35743312568883,3,{{la{-1.61374927f,
0.683642805f},la{-1.49351573f,0.657023668f},la{-1.65695775f,
0.689756215f},la{0.0f,0.0f}}}},lb{58,232.768,0.00028867597150438904,
44100,158.92431841039709,3,{{la{-1.59445465f,0.638638198f},la{-
1.55410087f,0.632223368f},la{-1.45113933f,0.613824725f},la{0.0f,0.0f}
}}},lb{58,232.882,0.00028891453989226564,44100,158.83001858622475,3,{
{la{-1.59466314f,0.638807058f},la{-1.55426526f,0.632377625f},la{-
1.45120609f,0.613937795f},la{0.0f,0.0f}}}},lb{58,232.997,
0.00028915527950406771,44100,165.99998973351111,2,{{la{-1.62173986f,
0.662646532f},la{-1.54332674f,0.647634983f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{59,246.628,0.00031824458244894441,44100,
156.11193406127686,2,{{la{-1.60553157f,0.649590433f},la{-1.52692068f,
0.634756386f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{59,246.749,
0.00031850768825534171,44100,156.03149962532618,2,{{la{-1.60544384f,
0.649523377f},la{-1.5267669f,0.634672523f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{59,246.871,0.00031877305486527742,44100,149.09717087616855,3,
{{la{-1.57424152f,0.622724235f},la{-1.53125119f,0.614853621f},la{-
1.42772758f,0.597330272f},la{0.0f,0.0f}}}},lb{60,261.314,
0.00035079667538140852,44100,138.5691297424554,3,{{la{-1.66556501f,
0.697691381f},la{-1.61332083f,0.691656649f},la{-1.461689f,
0.658281744f},la{0.0f,0.0f}}}},lb{60,261.442,0.0003510858324722655,
44100,138.49168762457796,3,{{la{-1.66567636f,0.697787702f},la{-
1.61338758f,0.691751063f},la{-1.46159673f,0.658326507f},la{0.0f,0.0f}
}}},lb{60,261.571,0.00035137734297986819,44100,138.4151738964589,3,{{
la{-1.66574478f,0.69784826f},la{-1.61340928f,0.691807687f},la{-
1.46146131f,0.658335209f},la{0.0f,0.0f}}}},lb{61,276.874,
0.00038662562228112331,44100,130.00001382085082,3,{{la{-1.59736741f,
0.673822582f},la{-1.4578017f,0.655451834f},la{-1.61741745f,
0.657155693f},la{0.0f,0.0f}}}},lb{61,277.01,0.00038694476329759826,
44100,124.09310649079725,4,{{la{-1.63671756f,0.672853768f},la{-
1.5985738f,0.668106735f},la{-1.50252497f,0.64973855f},la{-1.32154179f
,0.611172438f}}}},lb{61,277.146,0.00038726400718821016,44100,
129.9530814447281,3,{{la{-1.65810633f,0.69178021f},la{-1.60251451f,
0.6858567f},la{-1.44025612f,0.651507139f},la{0.0f,0.0f}}}},lb{62,
293.361,0.00042605830918566789,44100,129.35295333954386,2,{{la{-
1.57941461f,0.629579604f},la{-1.48713899f,0.612355947f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lb{62,293.505,0.00042640927717448165,44100,
129.26582510990727,2,{{la{-1.5799855f,0.63003397f},la{-1.48766851f,
0.612800539f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{62,293.649,
0.0004267603578166185,44100,129.18146586011756,2,{{la{-1.58047521f,
0.630424619f},la{-1.48811162f,0.613182008f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{63,310.829,0.00046944827536626285,44100,
116.00000438657698,3,{{la{-1.45243406f,0.530127525f},la{-1.42921555f,
0.534075439f},la{-1.34516895f,0.535162151f},la{0.0f,0.0f}}}},lb{63,
310.982,0.00046983553170952373,44100,121.99993500479322,2,{{la{-
1.53170836f,0.592331111f},la{-1.4398011f,0.573955417f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lb{63,311.134,0.00047022037995252336,44100,
121.99319178197217,2,{{la{-1.52481818f,0.586901307f},la{-1.43683302f,
0.570421636f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{64,329.337,
0.00051718777559689097,44100,122.20711562041862,1,{{la{-1.49957097f,
0.57270956f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{64,
329.499,0.00051761353584727128,44100,122.14478925812629,1,{{la{-
1.49944794f,0.572623312f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{64,329.661,0.00051803943206598496,44100,122.08262930576529,1,{{
la{-1.49931812f,0.572531819f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{65,348.948,0.00056970865048497213,44100,106.9192410536036
,2,{{la{-1.55440784f,0.610868633f},la{-1.44662583f,0.590984166f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{65,349.119,0.00057017523572402582,
44100,106.85493509045318,2,{{la{-1.55466354f,0.61107254f},la{-
1.44680882f,0.591177285f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{65,
349.291,0.00057064469874168445,44100,106.79162289241843,2,{{la{-
1.55487216f,0.61123997f},la{-1.44694018f,0.591332257f},la{0.0f,0.0f},
la{0.0f,0.0f}}}},lb{66,369.726,0.00062747715154224781,44100,
100.50569474405552,2,{{la{-1.52664959f,0.589478195f},la{-1.41796839f,
0.569149554f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{66,369.908,
0.00062799265582099805,44100,100.45112288604224,2,{{la{-1.52663231f,
0.589469731f},la{-1.41786146f,0.569120705f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{66,370.09,0.00062850832366120868,44100,100.39715358720066
,2,{{la{-1.52659214f,0.589443803f},la{-1.41773021f,0.569073379f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{67,391.742,0.00069101348515588257,
44100,84.042107012581454,4,{{la{-1.37467241f,0.473442465f},la{-
1.41170871f,0.515360773f},la{-1.33792949f,0.516312838f},la{-
1.16216171f,0.490651101f}}}},lb{67,391.934,0.00069157794011855437,
44100,94.000629782208435,2,{{la{-1.53065419f,0.592750967f},la{-
1.424667f,0.580176115f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{67,392.127,
0.0006921455140092512,44100,87.999996408722282,3,{{la{-1.56325436f,
0.616463542f},la{-1.49227667f,0.606119335f},la{-1.3159287f,
0.583127975f},la{0.0f,0.0f}}}},lb{68,415.068,0.00076087878767108698,
44100,95.999984059809321,1,{{la{-1.43051875f,0.523545504f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{68,415.272,
0.00076150118321163189,44100,95.999985547659207,1,{{la{-1.42509258f,
0.519396424f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{68,
415.476,0.00076212377448834864,44100,95.99997995382931,1,{{la{-
1.4207859f,0.516199052f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}}
,lb{69,439.784,0.00083769948617548108,44100,82.680025255656361,2,{{la
{-1.5217613f,0.587205887f},la{-1.38840675f,0.56369406f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lb{69,440,0.00083838330327871036,44100,
82.632238133499257,2,{{la{-1.52188933f,0.587310255f},la{-1.38842547f,
0.563779712f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{69,440.216,
0.00083906733450199965,44100,82.585183566179964,2,{{la{-1.52198219f,
0.587387621f},la{-1.38840663f,0.563836813f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{70,465.994,0.00092222624045597843,44100,
72.843882909627879,3,{{la{-1.41895306f,0.507745922f},la{-1.35627747f,
0.499199718f},la{-1.19910359f,0.479445487f},la{0.0f,0.0f}}}},lb{70,
466.223,0.0009229784278811439,44100,72.795308617155314,3,{{la{-
1.41943979f,0.508101642f},la{-1.35654366f,0.499460816f},la{-
1.19913054f,0.479602635f},la{0.0f,0.0f}}}},lb{70,466.452,
0.00092373085009990236,44100,77.999436553685769,2,{{la{-1.4712956f,
0.549401104f},la{-1.33555019f,0.525074363f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{71,493.767,0.0010151496048357825,44100,72.760638514413927
,2,{{la{-1.48457658f,0.559686422f},la{-1.34177971f,0.534435153f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{71,494.01,0.0010159776288841188,44100,
72.716769766165967,2,{{la{-1.48476493f,0.559833646f},la{-1.34186065f,
0.534566283f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{71,494.252,
0.0010168025017225687,44100,72.673856364473664,2,{{la{-1.48491418f,
0.559951723f},la{-1.34189999f,0.534666657f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{72,523.195,0.0011172856755547671,44100,75.066398476934694
,1,{{la{-1.36788511f,0.480399638f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{72,523.452,0.0011181940327211753,44100,
75.022645528941837,1,{{la{-1.36810052f,0.480552822f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{72,523.709,0.0011191026712183991,
44100,74.979414294388164,1,{{la{-1.36828232f,0.480683237f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{73,554.376,
0.0012295302954572266,44100,64.559449572422196,2,{{la{-1.36328685f,
0.471692443f},la{-1.24414563f,0.452196807f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{73,554.649,0.0012305310187171968,44100,64.523316603035099
,2,{{la{-1.36324668f,0.471669734f},la{-1.2440114f,0.452152878f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{73,554.921,0.0012315283841869743,44100
,64.487689042277765,2,{{la{-1.36318839f,0.471634626f},la{-1.24385619f
,0.452094942f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{74,587.416,
0.0013528730659360278,44100,60.399025589222802,2,{{la{-1.36316907f,
0.472321451f},la{-1.22963083f,0.448741466f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{74,587.705,0.0013539716118675233,44100,60.366490280458905
,2,{{la{-1.36304235f,0.472239703f},la{-1.22939694f,0.448633254f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{74,587.994,0.0013550704960375276,44100
,60.334198055545087,2,{{la{-1.36290383f,0.472149909f},la{-1.22914863f
,0.448515564f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{75,622.426,
0.0014883936924585063,44100,56.278005347913229,2,{{la{-1.3905021f,
0.492356181f},la{-1.23628032f,0.466606855f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{75,622.732,0.0014895997064160145,44100,56.248408298437333
,2,{{la{-1.3903302f,0.49224183f},la{-1.23597872f,0.466460019f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{75,623.037,0.0014908021470083413,44100
,56.21909016283729,2,{{la{-1.39014769f,0.492119968f},la{-1.23566687f,
0.466305375f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{76,659.521,
0.0016372643994772614,44100,53.647361274657662,2,{{la{-1.18815577f,
0.352928519f},la{-1.1768254f,0.39876619f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lb{76,659.845,0.0016385882132663129,44100,53.59509538480301,2,{{
la{-1.18917024f,0.35353145f},la{-1.17800963f,0.399410337f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{76,660.169,0.0016399124310108822,44100,
53.544757083904777,2,{{la{-1.19008827f,0.354077548f},la{-1.1790998f,
0.4000099f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{77,698.828,
0.0018007898642832853,44100,49.336102616369587,2,{{la{-1.36842954f,
0.477932394f},la{-1.19453776f,0.45108068f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{77,699.171,0.0018022425396217285,44100,49.289120820251838,2,{
{la{-1.36975622f,0.478861898f},la{-1.19566774f,0.451920271f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{77,699.515,0.0018036998933031727,44100,
49.243813709814127,2,{{la{-1.37097216f,0.479715824f},la{-1.19669259f,
0.452694923f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{78,740.477,
0.0019803801748292742,44100,46.385955556054128,2,{{la{-1.3047446f,
0.434676617f},la{-1.14055443f,0.404868633f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{78,740.841,0.0019819779269423863,44100,46.360979727365887
,2,{{la{-1.30455792f,0.43455863f},la{-1.14026606f,0.404729933f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{78,741.205,0.0019835761632297278,44100
,46.336178618789248,2,{{la{-1.30436265f,0.434435099f},la{-1.13996518f
,0.404583931f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{79,784.608,
0.0021775887830562946,44100,43.878387309525671,2,{{la{-1.23793519f,
0.391294152f},la{-1.08687913f,0.370977789f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{79,784.994,0.0021793445621545167,44100,43.810871665373767
,2,{{la{-1.24066925f,0.392985702f},la{-1.08938134f,0.372332752f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{79,785.379,0.0021810963210037405,44100
,43.747821982763718,2,{{la{-1.24313259f,0.394516289f},la{-1.09167647f
,0.373601317f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{80,831.37,
0.0023941198237539172,44100,40.521560503631257,2,{{la{-1.27978539f,
0.41938895f},la{-1.09849524f,0.387336731f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{80,831.778,0.0023960427315172735,44100,40.498605091523558,2,{
{la{-1.27966249f,0.419314116f},la{-1.0982877f,0.38725394f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{80,832.187,0.0023979709317304625,44100,
40.475825688329657,2,{{la{-1.2795248f,0.419229865f},la{-1.09806073f,
0.387159497f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{81,880.918,
0.002631825353009612,44100,38.000001040391886,2,{{la{-1.23303115f,
0.388890773f},la{-1.06836414f,0.36557433f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{81,881.351,0.0026339395379290798,44100,38.000003048429278,2,{
{la{-1.26055777f,0.408567339f},la{-1.04223216f,0.356890619f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{81,881.784,0.0026360543561619763,44100,
28.810582430470763,3,{{la{-1.67943501f,0.716469049f},la{-1.56717443f,
0.715458035f},la{-1.34266305f,0.737543881f},la{0.0f,0.0f}}}},lb{82,
933.564,0.0028934788772695488,44100,35.999398633048123,2,{{la{-
1.18066537f,0.357276201f},la{-1.00815642f,0.338214397f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lb{82,934.023,0.0028958005795701332,44100,
25.005968735839822,3,{{la{-1.69301069f,0.728176415f},la{-1.56494498f,
0.706582487f},la{-1.39924335f,0.731660843f},la{0.0f,0.0f}}}},lb{82,
934.481,0.0028981179144138163,44100,35.999398278276473,2,{{la{-
1.16977155f,0.350440681f},la{-1.00435591f,0.333384663f},la{0.0f,0.0f}
,la{0.0f,0.0f}}}},lb{83,989.356,0.0031807169710454416,44100,
33.752701631736031,2,{{la{-1.05635417f,0.278971046f},la{-1.00954318f,
0.31854564f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{83,989.842,
0.0031832632707381259,44100,33.702724011203713,2,{{la{-1.05832803f,
0.280014575f},la{-1.01163399f,0.319453597f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{83,990.328,0.0031858103268383163,44100,33.655269372935571
,2,{{la{-1.06012034f,0.280963808f},la{-1.01359892f,0.320326835f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{84,1048.48,0.003495987417455864,44100,
31.207532040951989,2,{{la{-1.07174265f,0.287158042f},la{-1.01241469f,
0.324895978f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{84,1049,
0.0034988090267594118,44100,31.178847348520645,2,{{la{-1.07229316f,
0.287453175f},la{-1.01320815f,0.325293094f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{84,1049.51,0.0035015771931539356,44100,31.151420723227311
,2,{{la{-1.07278764f,0.287718326f},la{-1.0139277f,0.32565701f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{85,1111.14,0.0038419949048638536,44100
,29.046443071307014,2,{{la{-1.08274305f,0.296697021f},la{-
0.978730202f,0.311012149f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{85,
1111.69,0.0038450794309375725,44100,29.019016136778021,2,{{la{-
1.08440006f,0.297693133f},la{-0.979093552f,0.311425745f},la{0.0f,0.0f
},la{0.0f,0.0f}}}},lb{85,1112.23,0.0038481075901828548,44100,
28.992943021549337,2,{{la{-1.08588767f,0.2985892f},la{-0.979406953f,
0.311797619f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{86,1177.55,
0.0042119619561208719,44100,31.111949802100611,1,{{la{-1.13222432f,
0.338807166f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{86,
1178.12,0.0042151127347369224,44100,31.090127131861273,1,{{la{-
1.13281262f,0.339163661f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{86,1178.7,0.0042183183075681406,44100,31.068234067358759,1,{{la{
-1.13337111f,0.339503378f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lb{87,1247.92,0.0045970393473528538,44100,29.147572444372653,1,{{
la{-1.12069178f,0.334013283f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{87,1248.53,0.0046003399631090302,44100,29.126057766197953
,1,{{la{-1.12137485f,0.334419966f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{87,1249.15,0.004603693977777587,44100,
29.104520805230379,1,{{la{-1.12202537f,0.334808677f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{88,1322.5,0.0049951145405383321,44100
,22.001678087881835,2,{{la{-1.34653533f,0.471067786f},la{-1.09960759f
,0.470321536f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{88,1323.15,
0.0049985326872045224,44100,22.001676889746108,2,{{la{-1.34472454f,
0.469817698f},la{-1.09743679f,0.468809187f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{88,1323.8,0.0050019499015450716,44100,22.001675841719194,
2,{{la{-1.34290862f,0.468565524f},la{-1.09526122f,0.467292696f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{89,1401.53,0.0054035704786987411,44100
,11.999244280970814,3,{{la{-1.67194998f,0.715583622f},la{-1.52167797f
,0.692718208f},la{-1.35547566f,0.766354263f},la{0.0f,0.0f}}}},lb{89,
1402.22,0.0054070707146882764,44100,16.000000602940069,3,{{la{-
1.28436768f,0.425174922f},la{-1.14711654f,0.384320587f},la{-
1.06536663f,0.517623067f},la{0.0f,0.0f}}}},lb{89,1402.91,
0.0054105697677488025,44100,12.000000949162381,4,{{la{-1.28115773f,
0.418808609f},la{-1.23580074f,0.420883447f},la{-1.09668899f,
0.4560467f},la{-0.935927272f,0.503410339f}}}},lb{90,1485.29,
0.0058195612069360638,44100,24.16759504491073,1,{{la{-1.01932752f,
0.277674139f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{90,
1486.02,0.0058231055893683322,44100,24.155592417324264,1,{{la{-
1.01894295f,0.277479619f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{90,1486.75,0.0058266485350518971,44100,24.143619638163401,1,{{la
{-1.0185554f,0.277283579f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lb{91,1574.06,0.0062398203557380589,44100,22.684424655220411,1,{{
la{-0.987202406f,0.260481417f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{91,1574.83,0.006243369216496159,44100,22.671343361306526,
1,{{la{-0.987166286f,0.260477662f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{91,1575.6,0.0062469163912102438,44100,
22.658357650952986,1,{{la{-0.987114847f,0.260465831f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{92,1668.12,0.0066607089561924035,
44100,21.492102171226474,1,{{la{-0.923197329f,0.230241418f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{92,1668.94,
0.0066642651179016771,44100,21.466901841111053,1,{{la{-0.925539911f,
0.231360078f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{92,
1669.76,0.0066678193086544425,44100,21.442509616138615,1,{{la{-
0.927740097f,0.232417539f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}
}},lb{93,1767.81,0.0070785229884286109,44100,13.714811665059679,2,{{
la{-1.64643347f,0.722500026f},la{-1.20364571f,0.722500026f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{93,1768.68,0.0070820399260056689,44100,
13.707084511199966,2,{{la{-1.64638722f,0.722500026f},la{-1.20303035f,
0.722500026f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{93,1769.55,
0.0070855546206974353,44100,13.699373010697011,2,{{la{-1.64634085f,
0.722500026f},la{-1.20241547f,0.722500026f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{94,1874.6,0.0074934700986100814,44100,18.673239995495855,
1,{{la{-0.917059243f,0.229527399f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{94,1875.52,0.0074968984776709983,44100,
18.662323841681104,1,{{la{-0.917016745f,0.229525939f},la{0.0f,0.0f},
la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{94,1876.45,0.007500361588523464,
44100,18.651374782565242,1,{{la{-0.916957498f,0.229516298f},la{0.0f,
0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{95,1987.84,
0.0078968439886875907,44100,12.026817452720875,2,{{la{-1.63294947f,
0.722500026f},la{-1.0655334f,0.722500026f},la{0.0f,0.0f},la{0.0f,0.0f
}}}},lb{95,1988.82,0.0079001722277890086,44100,12.020278126646875,2,{
{la{-1.63288331f,0.722500026f},la{-1.06497121f,0.722500026f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{95,1989.8,0.0079034976998747491,44100,
14.000000891882513,2,{{la{-1.4056859f,0.540458202f},la{-0.912294567f,
0.722661912f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{96,2107.92,
0.0082842751064562091,88200,33.318386347516658,1,{{la{-1.3204248f,
0.447994292f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{96,
2108.96,0.0082874532511910421,88200,33.296762799490836,1,{{la{-
1.32058275f,0.448110223f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}}}
},lb{96,2109.99,0.0082905978744504689,88200,33.275532810630736,1,{{la
{-1.32072508f,0.448215634f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,0.0f}
}}},lb{97,2235.25,0.0086513811057734348,88200,31.318346804009952,1,{{
la{-1.29215562f,0.429868311f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{97,2236.35,0.0086543623851430551,88200,31.29772136916305,
1,{{la{-1.29234743f,0.430005938f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f
,0.0f}}}},lb{97,2237.45,0.0086573404646191551,88200,
31.277282384839008,1,{{la{-1.29252338f,0.430133104f},la{0.0f,0.0f},la
{0.0f,0.0f},la{0.0f,0.0f}}}},lb{98,2370.28,0.0089938977410334853,
88200,22.000012741929531,2,{{la{-1.59982431f,0.657696545f},la{-
1.36219263f,0.629240453f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{98,
2371.44,0.0089966395612102908,88200,13.996157562636316,3,{{la{-
1.69268107f,0.723850667f},la{-1.63934112f,0.759000838f},la{-
1.44566512f,0.773839831f},la{0.0f,0.0f}}}},lb{98,2372.61,
0.0089994016309385607,88200,22.000003729908478,2,{{la{-1.5972755f,
0.655631363f},la{-1.35910559f,0.626883447f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{99,2513.46,0.0093076518307531313,88200,27.777379913662099
,1,{{la{-1.22344017f,0.38829419f},la{0.0f,0.0f},la{0.0f,0.0f},la{0.0f
,0.0f}}}},lb{99,2514.7,0.0093101571409708352,88200,27.752521509315972
,1,{{la{-1.22431958f,0.388849646f},la{0.0f,0.0f},la{0.0f,0.0f},la{
0.0f,0.0f}}}},lb{99,2515.93,0.0093126387443926931,88200,
27.72829710779693,1,{{la{-1.22514904f,0.389375001f},la{0.0f,0.0f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{100,2665.29,0.009588770619505492,88200
,18.905815836152264,2,{{la{-1.66974306f,0.722500026f},la{-1.38070571f
,0.722500026f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{100,2666.6,
0.0095909775822555152,88200,18.895092046144043,2,{{la{-1.66971457f,
0.722500026f},la{-1.38039565f,0.722500026f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{100,2667.91,0.0095931809174093739,88200,
18.884372716231621,2,{{la{-1.66968608f,0.722500026f},la{-1.3800863f,
0.722500026f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{101,2826.29,
0.009833681755363196,88200,18.000583123255574,2,{{la{-1.56310523f,
0.634928048f},la{-1.24395955f,0.598789275f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{101,2827.68,0.0098355725817048413,88200,
18.000586746003947,2,{{la{-1.56167495f,0.633785248f},la{-1.24230957f,
0.597527325f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{101,2829.07,
0.0098374597092516624,88200,18.000590786471179,2,{{la{-1.56024241f,
0.632641554f},la{-1.24065709f,0.596263409f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{102,2997.02,0.010039223629270608,88200,16.000000355382916
,3,{{la{-1.36554718f,0.486091226f},la{-1.01614058f,0.423193067f},la{-
0.0296824481f,0.000270999182f},la{0.0f,0.0f}}}},lb{102,2998.49,
0.010040768085133125,88200,10.525596218358849,3,{{la{-1.66923022f,
0.714031577f},la{-1.50187647f,0.690803409f},la{-1.23671746f,
0.721973896f},la{0.0f,0.0f}}}},lb{102,2999.97,0.010042319309200817,
88200,14.160639420037748,3,{{la{-1.64843595f,0.706556797f},la{-
1.32500219f,0.629247069f},la{-0.867283881f,0.724189162f},la{0.0f,0.0f
}}}},lb{103,3178.06,0.010202684190796315,88200,16.017567723774462,2,{
{la{-1.49830008f,0.589225113f},la{-1.11408174f,0.545398951f},la{0.0f,
0.0f},la{0.0f,0.0f}}}},lb{103,3179.62,0.010203867825481026,88200,
16.000000381490487,2,{{la{-1.49889171f,0.589689612f},la{-1.11514616f,
0.546209872f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{103,3181.18,
0.010205047779361105,88200,16.017571190214831,2,{{la{-1.49509454f,
0.586748004f},la{-1.11050975f,0.542654335f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{104,3370.04,0.010321893182107819,88200,16.000002032792771
,2,{{la{-1.27500296f,0.427498698f},la{-0.888396561f,0.363244414f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{104,3371.69,0.010322696893368198,88200
,16.000002677693999,2,{{la{-1.27262473f,0.425892979f},la{-
0.886437476f,0.361479849f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{104,
3373.35,0.010323501853965826,88200,8.6495971530366376,3,{{la{-
1.76842439f,0.807777464f},la{-1.62427771f,0.858462095f},la{-
1.04093957f,0.779230833f},la{0.0f,0.0f}}}},lb{105,3573.61,
0.010395243004170114,88200,6.0001273435570042,4,{{la{-1.39757311f,
0.506655812f},la{-1.33216977f,0.495056063f},la{-1.1535207f,
0.566112936f},la{-0.799214065f,0.607840836f}}}},lb{105,3575.37,
0.010395661433177543,88200,14.000126790649743,2,{{la{-1.47835219f,
0.581238091f},la{-1.01673913f,0.535792649f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{105,3577.12,0.010396074005476357,88200,10.00012784677288,
3,{{la{-1.42734838f,0.522639096f},la{-1.26879501f,0.537110388f},la{-
0.875723064f,0.56214875f},la{0.0f,0.0f}}}},lb{106,3789.48,
0.01042174655198037,88200,14.000001613681457,2,{{la{-1.24373782f,
0.413003951f},la{-0.786167085f,0.34910962f},la{0.0f,0.0f},la{0.0f,
0.0f}}}},lb{106,3791.34,0.010421769215013135,88200,14.000001675880899
,2,{{la{-1.24137294f,0.411434382f},la{-0.784190536f,0.347363144f},la{
0.0f,0.0f},la{0.0f,0.0f}}}},lb{106,3793.21,0.010421788656279837,88200
,14.000001588951225,2,{{la{-1.23898542f,0.409852296f},la{-
0.782206714f,0.34560445f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{107,
4018.39,0.010401042854577084,88200,8.0005738075314419,3,{{la{-
1.51870525f,0.599547863f},la{-1.28559804f,0.603122532f},la{-
0.789705694f,0.614614069f},la{0.0f,0.0f}}}},lb{107,4020.37,
0.010400669406307886,88200,8.0005748618613062,3,{{la{-1.51785016f,
0.598956406f},la{-1.28394163f,0.601855993f},la{-0.788839817f,
0.613911569f},la{0.0f,0.0f}}}},lb{107,4022.34,0.010400294740517305,
88200,6.0000023601015702,4,{{la{-1.36827624f,0.509053886f},la{-
1.40227294f,0.545726359f},la{-1.05850959f,0.688098848f},la{-
0.376962274f,0.730586529f}}}},lb{108,4261.13,0.010333412023005079,
88200,12.000000207904682,2,{{la{-1.28697085f,0.451614052f},la{-
0.725080848f,0.397727132f},la{0.0f,0.0f},la{0.0f,0.0f}}}},lb{108,
4263.23,0.010332647299167762,88200,12.00196466743404,2,{{la{-
1.28418291f,0.449678481f},la{-0.722703278f,0.395609587f},la{0.0f,0.0f
},la{0.0f,0.0f}}}},lb{108,4265.32,0.010331883366577739,88200,
12.001963952874597,2,{{la{-1.28201258f,0.448183239f},la{-0.720752478f
,0.393981755f},la{0.0f,0.0f},la{0.0f,0.0f}}}},}};};
namespace lu::l5{enum class l43{l389,l688,l802,l772,l714,l635};
constexpr int l23=21;constexpr int l80=108;constexpr int l184=l80-l23
+1;static_assert(l184==88);constexpr int l651=69;constexpr double l572
=440.0;using l58=std::array<double,l184>;using l267=std::array<double
,l184>;using l298=std::array<double,l184>;enum class l73{l0=1,l324=2,
l1=3};constexpr double l769=1.7;}class l45{l45(const l45&)=delete;l45
&operator=(const l45&)=delete;public:using l43=lu::l5::l43;using l58=
lu::l5::l58;using l73=lu::l5::l73;using l267=lu::l5::l267;using l298=
lu::l5::l298;constexpr l45()noexcept=default;double l679(int lk,l43
l100=l43::l389,l73 l722=l73::l324)const noexcept{if(lk<lu::l5::l23||
lk>lu::l5::l80){return 0.0;}const int lh=lk-lu::l5::l23;const l175&ly
=l263();const double l258=l735(ly,lh,l100);switch(l722){case l73::l0:
return l258-ly.l220[lh];case l73::l324:return l258;case l73::l1:
return l258+ly.l220[lh];}return l258;}const l58&l820()const noexcept{
return l263().l37;}double l447(int lk)const noexcept{if(lk<lu::l5::
l23||lk>lu::l5::l80){return 0.0;}const int lh=lk-lu::l5::l23;return
l263().l485[lh];}const l298&l817()const noexcept{return l263().l485;}
private:struct l175{l58 l37{};l58 l458{};l58 l529{};l58 l481{};l58
l435{};l58 l438{};l267 l220{};l298 l485{};};static l175 l736(){l175 ly
{};l676(ly.l37);ly.l458=ly.l37;ly.l529=ly.l37;ly.l481=ly.l37;ly.l435=
ly.l37;ly.l438=ly.l37;l606(ly.l37,ly.l220);return ly;}static const
l175&l263(){static const l175 ly=l736();return ly;}static double l336
(int lk){return lu::l5::l572*std::pow(2.0,static_cast<double>(lk-lu::
l5::l651)/12.0);}static double l667(double l656){constexpr double l785
=-1.3333333333333333;constexpr double l784=1.3333333333333333;
constexpr double l625=-6.5760753629956712;constexpr double l624=
1.9137732290077178;constexpr double l623=0.27614763151078042;
constexpr double l622=-0.28572738557770405;constexpr double l621=-
0.30709648310494486;const double lo=std::log2(l656/440.0);double l360
=l625+l624*lo+l623*lo*lo+l622*std::pow(std::max(lo-l785,0.0),2.0)+
l621*std::pow(std::max(lo-l784,0.0),2.0);l360=std::clamp(l360,std::
log(5e-6),std::log(2e-2));return std::exp(l360);}static double l246(
int lk){return l667(l336(lk));}static double l459(int l144){constexpr
double l770=3.5;constexpr double l761=60.0;constexpr double l69=25.0;
const double lo=(static_cast<double>(l144)-l761)/l69;return(l770*0.5) *
(1.0-std::erf(lo))+1.0;}static double l780(int l144,int l356,double
l657){const double l200=l459(l144);const double l213=l200*l200;const
double l392=l246(l144);const double l342=l246(l356);return 2.0*l657*
std::sqrt((1.0+l392*4.0*l213)/(1.0+l342*l213));}static double l607(
int l144,int l356,double l698){const double l200=l459(l144);const
double l213=l200*l200;const double l392=l246(l144);const double l342=
l246(l356);const double l681=std::sqrt((1.0+l392*4.0*l213)/(1.0+l342*
l213));return l698/(2.0*l681);}static double l791(int lk,const std::
array<int,8>&l17,const std::array<double,8>&l145){if(lk<=l17.front()){
return l145.front();}for(std::size_t lc=0;lc+1<l17.size();++lc){const
int l484=l17[lc];const int l514=l17[lc+1];if(lk<=l514){const double
l677=static_cast<double>(lk-l484)/static_cast<double>(l514-l484);
return l145[lc]+l677* (l145[lc+1]-l145[lc]);}}const std::size_t l241=
l17.size()-1;const std::size_t l450=l241-1;const double l746=(l145[
l241]-l145[l450])/static_cast<double>(l17[l241]-l17[l450]);return l145
[l241]+l746*static_cast<double>(lk-l17[l241]);}static void l676(l58&
l37){l58 l533{};for(int lm=lu::l5::l23;lm<=lu::l5::l80;++lm){const int
lh=lm-lu::l5::l23;l533[lh]=l336(lm);}constexpr std::array<int,8>l17={
21,33,45,57,69,81,93,105};std::array<double,l17.size()>l174{};std::
array<double,l17.size()>l462{};constexpr int l412=4;l174[l412]=lu::l5
::l572;for(int lc=l412+1;lc<static_cast<int>(l17.size());++lc){l174[
lc]=l780(l17[lc-1],l17[lc],l174[lc-1]);}for(int lc=l412-1;lc>=0;--lc){
l174[lc]=l607(l17[lc],l17[lc+1],l174[lc+1]);}for(std::size_t lc=0;lc<
l17.size();++lc){l462[lc]=1200.0*std::log2(l174[lc]/l336(l17[lc]));}
for(int lm=lu::l5::l23;lm<=lu::l5::l80;++lm){const int lh=lm-lu::l5::
l23;const double l636=l791(lm,l17,l462);l37[lh]=l533[lh] *std::pow(
2.0,l636/1200.0);}}static double l735(const l175&ly,int lh,l43 l100)noexcept
{switch(l100){case l43::l389:return ly.l37[lh];case l43::l688:return
ly.l458[lh];case l43::l802:return ly.l529[lh];case l43::l772:return ly
.l481[lh];case l43::l714:return ly.l435[lh];case l43::l635:return ly.
l438[lh];}return ly.l37[lh];}static int l619(int lk)noexcept{if(lk<=
28){return 1;}if(lk<=33){return 2;}return 3;}static double l693(
double l52,int l105){if(l105<=1){return 0.0;}const double l712=std::
pow(2.0,lu::l5::l769/1200.0);const double l783=l52*l712;const double
l675=l783-l52;return l675*0.5;}static void l606(const l58&l37,l267&
l220){for(int lm=lu::l5::l23;lm<=lu::l5::l80;++lm){const int lh=lm-lu
::l5::l23;const int l105=l619(lm);const double l52=l37[lh];l220[lh]=
l693(l52,l105);}}};
namespace lu::l358::l339{struct le{double l752,l751,l211,l236;double
l743,l227,l742,l198;double l703,l758;};inline constexpr std::array<le
,88>l596{{le{1.90124055258e+15,1375613.66073,0.119408639313,
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
0.755604694289,0.0053364,-3.19222563179e-05},}};}class l170{l170(
const l170&)=delete;l170&operator=(const l170&)=delete;public:using lb
=lu::l358::l339::le;using l231=std::array<lb,88>;constexpr l170()noexcept
=default;const lb*l367(int lk)const noexcept{if(lk<21||lk>108)return
nullptr;return&l234()[static_cast<std::size_t>(lk-21)];}const l231&
l749()const noexcept{return l234();}private:static constexpr const
l231&l234()noexcept{return lu::l358::l339::l596;}};
namespace lu::l456::l494{struct lg{double l765;};inline constexpr std
::array<lg,88>l716{{lg{12.3988206919},lg{12.8891815684},lg{
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
1.41467684717},lg{1.38608153033},}};}class l179{l179(const l179&)=
delete;l179&operator=(const l179&)=delete;public:using lb=lu::l456::
l494::lg;using l231=std::array<lb,88>;constexpr l179()noexcept=
default;const lb*l367(int lk)const noexcept{constexpr int l23=21;
constexpr int l80=108;if(lk<l23||lk>l80){return nullptr;}const std::
size_t lh=static_cast<std::size_t>(lk-l23);return&l234()[lh];}double
l447(int lk)const noexcept{const lb*l543=l367(lk);if(l543==nullptr){
return 0.0;}return l543->l765;}const l231&l749()const noexcept{return
l234();}private:static constexpr const l231&l234()noexcept{return lu
::l456::l494::l716;}};class l78{l78(const l78&)=delete;l78&operator=(
const l78&)=delete;public:static constexpr l45 l664{};static constexpr
l170 l48{};static constexpr l179 l771{};constexpr l78()noexcept=
default;};class l229{public:struct l470{int size=0;int l518=0;double
l8=0.0;double l317=0.0;int l131=0;int l168=0;float l181=1.0f;float
l240=0.0f;l470(int size,double l8):size(size),l8(l8){if(size<2)throw
std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c"
"\x3a\x20\x73\x69\x7a\x65\x20\x69\x73\x20\x74\x6f\x6f\x20\x73\x6d\x61"
"\x6c\x6c\x2c\x20\x73\x69\x7a\x65\x3a\x20"+std::to_string(size));if(
l8>1.0||l8<=0.0)throw std::runtime_error("\x73\x74\x72\x69\x6e\x67"
"\x5f\x6d\x6f\x64\x65\x6c\x3a\x20\x70\x6f\x73\x69\x74\x69\x6f\x6e\x20"
"\x69\x73\x6e\x27\x74\x20\x67\x6f\x6f\x64\x2c\x20\x70\x6f\x73\x69\x74"
"\x69\x6f\x6e\x3a\x20"+std::to_string(l8));l518=size-1;l317=l518*l8;
l131=std::ceil(l317);l168=l131-1;l181=l317-l168;l240=1-l181;}};public
:const l78*l330=nullptr;int l31=69;double l52=440.0;double l61=
44100.0;double l261=44100.0;l308 l219;l504 l161;double l695=0.0;
double l815=0.0;double l169=0.0;int l71=0;double l480=0.0;int l90=0;
std::vector<float>l0;std::vector<float>l1;int l42=0;int l40=0;float*
l153=nullptr;float*l271=nullptr;double l764=1.0/9.4;l470 l15;double
l242=0.0;Damper l351;bool l193=false;l490 l262;bool l112=false;double
l558=0.0;public:l229(double ln,int lk,l45::l43 l100,l45::l73 l737,
const l78*l46):l330(l46),l31(lk),l52(l46->l664.l679(l31,l100,l737)),
l61(ln),l261(l31>=96?2.0*l61:l61),l219(l31),l161(l52),l695(l219.l598(
l261,l52)),l169(l161.l800()/2.0),l71(static_cast<int>(std::floor(l169
))+1),l480(2.0* (l169-std::floor(l169))),l90(l71-1),l15(l71,l764),
l262(l480,2.0*std::numbers::pi_v<double> *l52/l261){if(l71<4)throw std
::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c\x3a"
"\x20\x64\x65\x6c\x61\x79\x5f\x69\x6e\x74\x20\x69\x73\x20\x74\x6f\x6f"
"\x20\x73\x6d\x61\x6c\x6c\x3a\x20"+std::to_string(l71));if(l15.l131+1
>l90||l15.l131-1<0){throw std::runtime_error("\x73\x74\x72\x69\x6e"
"\x67\x5f\x6d\x6f\x64\x65\x6c\x3a\x20\x6e\x65\x78\x74\x5f\x69\x6e\x64"
"\x65\x78\x20\x64\x6f\x65\x73\x6e\x27\x74\x20\x65\x78\x69\x73\x74\x3a"
"\x20"+std::to_string(l71));}if(std::abs(l161.l766()-l261)>0.5){throw
std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c"
"\x3a\x20\x64\x69\x73\x70\x65\x72\x73\x69\x6f\x6e\x20\x70\x72\x65\x73"
"\x65\x74\x20\x73\x61\x6d\x70\x6c\x65\x20\x72\x61\x74\x65\x20\x6d\x69"
"\x73\x6d\x61\x74\x63\x68");}l0.resize(l71,0.0f);l1.resize(l71,0.0f);
l42=0;l40=0;l153=&l0[l42];l271=&l1[l27(l90,l40)];l242=l46->l771.l447(
l31);l558=1.0/(2.0*l242);}inline void l672(){if(l42==l90)l42=0;else
l42++;if(l40==0)l40=l90;else l40--;l1[l27(0,l40)]=-l0[l27(0,l42)];l0[
l27(l90,l42)]=-l1[l27(l90,l40)];l153=&l1[l27(0,l40)];l271=&l0[l27(l90
,l42)];}inline int l27(int l362,int l374){return l362+l374<=l90?l362+
l374:l362+l374-l71;}inline void l415(double l87){const bool l566=l87>
0.0;if(l566){l723(l87*l558);}l672();filter();l366();if(l566){l112=
true;}}inline void l723(double l288){l0[l27(l15.l131,l42)]+=l288*l15.
l181;l0[l27(l15.l168,l42)]+=l288*l15.l240;l1[l27(l15.l131,l40)]+=l288
 *l15.l181;l1[l27(l15.l168,l40)]+=l288*l15.l240;}inline double l420(){
return-2.0*l242*static_cast<double>( *l271);}inline double l642(){
return l15.l181*l0[l27(l15.l131,l42)]+l15.l240*l0[l27(l15.l168,l42)]+
l15.l181*l1[l27(l15.l131,l40)]+l15.l240*l1[l27(l15.l168,l40)];}inline
void lx(){l42=0;l40=0;l112=false;l193=false;l406=0;l315=0;std::fill(
l0.begin(),l0.end(),0.0f);std::fill(l1.begin(),l1.end(),0.0f);l153=&
l0[l42];l271=&l1[l27(l90,l40)];l219.lx();l262.lx();l351.lx();l161.lx(
);}private:inline void filter(){l262.l34( *l153);l219.l34( *l153);
l161.l34( *l153);if(l193){l351.l34( *l153);}}int l406=0;int l315=0;
inline void l366(){const int l605=l31>=96?128:64;constexpr int l777=8
;if(++l315<l605){return;}l315=0;constexpr float l538=1.0e-5f;
constexpr float l687=l538*l538;if(l704()<l687){if(++l406>=l777){l112=
false;lx();}}else{l406=0;l112=true;}}inline float l704(){float l343=
0.0f;for(int lh=0;lh<l71;++lh){l343+=l0[static_cast<std::size_t>(lh)]
 *l0[static_cast<std::size_t>(lh)]+l1[static_cast<std::size_t>(lh)] *
l1[static_cast<std::size_t>(lh)];}l343/=static_cast<float>(2*l71);
float state_energy=std::max(l219.state_energy(),l262.state_energy());
state_energy=std::max(state_energy,static_cast<float>(l161.
state_energy()));if(l193){state_energy=std::max(state_energy,l351.
state_energy());}return std::max(l343,state_energy);}};
class l477{public:int l31=69;const lu::l358::l339::le*l48=nullptr;
double l139=0.0;double l65=0.0;double l79=0.0;double l103=0.0;double
l172=0.0;double l188=0.0;bool l228=false;double l457=0.0;public:l477(
double ln,int lk,const l78*l46):l139((1.0/2.0)/ln),l31(lk),l48(l46->
l48.l367(l31)){l457=l139/l48->l703;}inline double l524(double l165){
if(!l228)return 0.0;if(l65<0.0||l79<0.0){lx();return 0.0;}double l87=
l725(l165);if(!std::isfinite(l87)||l87<=0.0){lx();return 0.0;}if(l103
<0||l188<0.0){l188+=l103*l139;if(l188>0.0){l188=0.0;}}if(l188<l48->
l758){lx();return 0.0;}l65=l65+(l172-l165) *l139;l79=l79+(l103-l172) *
l139;constexpr double l442=1.0e-12;if(!std::isfinite(l65)||!std::
isfinite(l79)||l65<-l442||l79<-l442){lx();return 0.0;}l65=std::max(
0.0,l65);l79=std::max(0.0,l79);if(l87>0.0)l103-=l87*l457;return l87;}
inline void l377(double l778){if(l228)return;lx();l103=l778;l228=true
;}inline void lx(){l65=0.0;l79=0.0;l103=0.0;l172=0.0;l188=0.0;l228=
false;}private:inline double l553(double lo,double l488){return lo>=0
?std::pow(lo,l488):-std::pow(-lo,l488);}inline double l532(double l473
,double l779){return std::pow(l473>0.0?l473:0.0,l779);}inline double
l725(double l165){double l128=l165-l65/l139;double l123=l103+l79/l139
;if(!(l128<=l123)){return 0.0;}const double l497=l48->l752*l532(l65,
l48->l743);const double l496=l48->l751*l532(l79,l48->l742);const
double l211=l48->l211;const double l236=l48->l236;const double l227=
l48->l227;const double l198=l48->l198;const auto l463=[&](double l451
){const double l297=l497+l211*l553(l451-l165,l227);const double l396=
l496+l236*l553(l103-l451,l198);return l297-l396;};const double l525=
l463(l128);const double l489=l463(l123);if(!std::isfinite(l525)||!std
::isfinite(l489)||l525>0.0||l489<0.0){return 0.0;}double l150=std::
clamp(l172,l128,l123);constexpr int l680=8;constexpr double l609=
1.0e-10;constexpr double l669=1.0e-10;constexpr double l763=1.0e-14;
double l515=0.0;for(int lc=0;lc<l680;++lc){const double l545=l150-
l165;const double l544=l103-l150;const double l398=std::abs(l545);
const double l338=std::abs(l544);const double l467=std::pow(l398,l227
);const double l468=std::pow(l338,l198);const double l731=std::
copysign(l467,l545);const double l682=std::copysign(l468,l544);const
double l297=l497+l211*l731;const double l396=l496+l236*l682;const
double l287=l297-l396;if(!std::isfinite(l287)){return 0.0;}l515=l297;
if(l287>0.0){l123=l150;}else{l128=l150;}if(std::abs(l287)<=l609){
break;}if((l123-l128)<=l669){break;}double l203=0.0;if(l398>0.0){l203
+=l211*l227* (l467/l398);}if(l338>0.0){l203+=l236*l198* (l468/l338);}
double l257;if(std::isfinite(l203)&&l203>l763){const double l265=l150
-l287/l203;if(std::isfinite(l265)&&l265>l128&&l265<l123){l257=l265;}
else{l257=(l128+l123) *0.5;}}else{l257=(l128+l123) *0.5;}l150=l257;}
l172=l150;return l515;}};
class BridgeModel{public:BridgeModel(){}void l34()const{}};
class l108{static constexpr bool l617=true;l108(const l108&)=delete;
l108&operator=(const l108&)=delete;static constexpr std::size_t l184=
88;static constexpr std::size_t l122=8;static constexpr std::size_t
l464=721;static constexpr std::array<std::size_t,l122>l517{37,87,181,
271,359,492,687,721};struct l650{std::array<float,l464>data{};std::
size_t length=1;std::size_t lh=0;inline void l760(std::size_t l594)noexcept
{length=std::clamp<std::size_t>(l594,1,l464);lh=0;data.fill(0.0f);}
inline float read()const noexcept{return data[lh];}inline void write(
float lo)noexcept{data[lh]=lo;++lh;if(lh>=length){lh=0;}}inline void
reset()noexcept{data.fill(0.0f);lh=0;}};struct l308{float l272=1.0f;
float l419=0.0f;float l51=0.0f;inline float l34(float lo)noexcept{
const float l38=l272* (1.0f-l419) *lo+l419*l51;l51=l38;return l38;}
inline void reset()noexcept{l51=0.0f;}};struct l413{float l69=0.0f;
float l310=1.0f;float l304=1.0f;float l292=0.0f;inline float l34(
float lo)noexcept{l292=l69*l292+(1.0f-l69) *lo;const float l260=l292;
const float l384=lo-l260;return l310*l260+l304*l384;}inline void reset
()noexcept{l292=0.0f;}};std::array<l650,l122>l274{};std::array<l308,
l122>l239{};l413 l221{};l413 l243{};l413 l223{};double l61=44100.0;
public:mutable std::array<float,l184>l121{};explicit l108(double ln=
44100.0):l61(ln){l684();l696();l660();}inline float l216()noexcept{if
(!l617){float l513=0.0;for(int lc=0;lc<l121.size();lc++){l513+=l121[
lc];}return l513;}float l522=0.0f;float l486=0.0f;float l555=0.0f;for
(std::size_t lc=0;lc<20;++lc){l522+=l121[lc];}for(std::size_t lc=20;
lc<55;++lc){l486+=l121[lc];}for(std::size_t lc=55;lc<88;++lc){l555+=
l121[lc];}const float l440=l221.l34(l522)+l243.l34(l486)+l223.l34(
l555);std::array<float,l122>l66{};float l530=0.0f;for(std::size_t lc=
0;lc<l122;++lc){l66[lc]=l239[lc].l34(l274[lc].read());l530+=l66[lc];}
constexpr float l224=0.3535533905932738f;const float l591=l224* (l66[
0]-l66[1]+l66[2]-l66[3]+l66[4]-l66[5]+l66[6]-l66[7]);const float l662
=0.25f*l530;const float l206=l224*l440;for(std::size_t lc=0;lc<l122;
++lc){const std::size_t l585=(lc+1)&7;const float l701=l66[l585]-l662
;l274[lc].write(l701+l206);}constexpr float l640=0.18f;return l591+
l640*l440;}inline void lx()noexcept{l121.fill(0.0f);for(auto&l169:
l274){l169.reset();}for(auto&l674:l239){l674.reset();}l221.reset();
l243.reset();l223.reset();}private:inline void l684()noexcept{for(std
::size_t lc=0;lc<l122;++lc){l274[lc].l760(l517[lc]);}}inline void l696
()noexcept{constexpr double l798=0.34;constexpr float l661=0.075f;for
(std::size_t lc=0;lc<l122;++lc){const double l641=static_cast<double>
(l517[lc])/l61;const double l272=std::pow(10.0,-3.0*l641/l798);l239[
lc].l272=static_cast<float>(l272);l239[lc].l419=l661;l239[lc].l51=
0.0f;}}inline void l660()noexcept{constexpr double l627=1800.0;const
float l69=static_cast<float>(std::exp(-2.0*3.14159265358979323846*
l627/l61));l221.l69=l69;l221.l310=1.00f;l221.l304=0.82f;l243.l69=l69;
l243.l310=1.00f;l243.l304=1.00f;l223.l69=l69;l223.l310=0.95f;l223.
l304=1.28f;}};class l235{public:double l629=0.0;int l31;int l57=3;
const BridgeModel*l371;const l108*l142;l477 l99;std::array<l229,3>l19
;std::array<double,3>l305={0.0,0.0,0.0};bool l207=false;bool l104=
false;bool l226=false;const l78*l330=nullptr;double l250=0.0;double
l474=0.0;double l581=0.0;std::array<double,3>l222={0.0,0.0,0.0};std::
array<float,3>l418{};l235(int lk,double ln,int l105,l45::l43 l100,
const l108*l666,const BridgeModel*l699,const l78*l46):l31(lk),l57(
l105),l629(ln),l99(ln,lk,l46),l19{l229(ln,lk,l100,l45::l73::l0,l46),
l229(ln,lk,l100,l45::l73::l324,l46),l229(ln,lk,l100,l45::l73::l1,l46),
},l142(l666),l371(l699),l330(l46){for(int lc=0;lc<l57;++lc){l250+=l19
[lc].l242;}if(l250>0.0){l474=1.0/l250;l581=1.0/(l250*2.0);for(int lc=
0;lc<l57;++lc){l222[lc]=l19[lc].l242*l474;}}}inline void l700(){l647(
);double l369=0.0;for(int lc=0;lc<l57;++lc){l305[lc]=l19[lc].l642();
l369+=l222[lc] *l305[lc];}double l354=l99.l524(l369);double l512=l99.
l524(l369+l354*l581);double l87=(l354+l512)/2.0;for(int lc=0;lc<l57;
lc++){if(l19[lc].l31<96){l19[lc].l415(l87*l222[lc]);}else{l19[lc].
l415(l354*l222[lc]);l418[lc]=l19[lc].l420();l19[lc].l415(l512*l222[lc
]);}}l709();l366();if(!l104){l142->l121[l31-21]=0.0f;l418.fill(0.0f);
}}inline void l377(double l586){for(int lc=0;lc<l57;lc++){l19[lc].
l112=true;l305[lc]=0.0;}l99.l377(l586);}inline void l709(){float l148
=0.0;for(int lc=0;lc<l57;lc++){if(l19[lc].l31<96){l148+=l19[lc].l420(
);}else{l148+=(l19[lc].l420()+l418[lc])/2.0;}}l142->l121[l31-21]=l148
;}inline void lx(){for(int lc=0;lc<l57;lc++){l19[lc].lx();}l99.lx();
l207=false;l104=false;l305.fill(0.0);l226=false;}private:inline void
l366(){l104=l99.l228;for(int lc=0;lc<l57;++lc){l104=l104||l19[lc].
l112;}}inline void l647(){const bool l595=!l207&&!l226;for(int lh=0;
lh<l57;++lh){l19[lh].l193=l595;}}};class l88{l88(const l88&)=delete;
l88&operator=(const l88&)=delete;public:static constexpr l78 l46{};
BridgeModel l371;l108 l142;std::array<l235* ,88>l10;l88(double ln){
for(std::size_t lh=0;lh<l10.size();++lh){const int lk=static_cast<int
>(lh)+21;int l105=1;if(lk>=34){l105=3;}else if(lk>=29){l105=2;}l10[lh
]=new l235(lk,ln,l105,l45::l43::l389,&l142,&l371,&l46);}}inline void
l365(){for(int lc=0;lc<l10.size();lc++){if(l10[lc]->l104)l10[lc]->
l700();}}inline float l216(){return 0.01*l142.l216();}inline void
sustainpedal_control(bool l112){if(l112){for(int lc=0;lc<l10.size();
lc++){l10[lc]->l226=true;}}else{for(int lc=0;lc<l10.size();lc++){l10[
lc]->l226=false;}}}inline void lx(){for(int lc=0;lc<l10.size();lc++){
l10[lc]->lx();}l142.lx();}~l88()noexcept{for(int lc=0;lc<l10.size();
lc++){delete l10[lc];}}};
#include<AudioToolbox/AudioToolbox.h>
class l96{public:class l132{public:explicit l132(l96&l431)noexcept:
l573(&l431),l141(l431.l713()){}l132(const l132&)=delete;l132&operator
=(const l132&)=delete;~l132(){if(l141){l573->l691();}}explicit
operator bool()const noexcept{return l141;}private:l96*l573;bool l141
;};l96()noexcept=default;l96(const l96&)=delete;l96&operator=(const
l96&)=delete;void open()noexcept{l36.store(0,std::
memory_order_release);}void l559()noexcept{std::uint32_t l51=l36.
fetch_or(l180,std::memory_order_acq_rel)|l180;while((l51&l321)!=0){
l36.wait(l51,std::memory_order_acquire);l51=l36.load(std::
memory_order_acquire);}}bool l811()const noexcept{return(l36.load(std
::memory_order_acquire)&l180)==0;}private:static constexpr std::
uint32_t l180=0x80000000U;static constexpr std::uint32_t l321=~l180;
bool l713()noexcept{std::uint32_t l51=l36.load(std::
memory_order_acquire);while((l51&l180)==0){if((l51&l321)==l321){
return false;}if(l36.compare_exchange_weak(l51,l51+1,std::
memory_order_acq_rel,std::memory_order_acquire)){return true;}}return
false;}void l691()noexcept{l36.fetch_sub(1,std::memory_order_release);
l36.notify_all();}std::atomic<std::uint32_t>l36{l180};};class l28{l28
(l28&&)=delete;l28&operator=(l28&&)=delete;AudioUnit l50=nullptr;l96
l254;bool l93=false;public:l28(const l28&)=delete;l28&operator=(const
l28&)=delete;explicit l28(double ln){try{AudioComponentDescription
l177{};l177.componentType=kAudioUnitType_Output;l177.componentSubType
=kAudioUnitSubType_DefaultOutput;l177.componentManufacturer=
kAudioUnitManufacturer_Apple;l177.componentFlags=0;l177.
componentFlagsMask=0;AudioComponent l570=AudioComponentFindNext(
nullptr,&l177);if(l570==nullptr){throw std::runtime_error("\x46\x61"
"\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e\x64\x20\x61\x75\x64\x69"
"\x6f\x20\x63\x6f\x6d\x70\x6f\x6e\x65\x6e\x74\x2e");}OSStatus l668=
AudioComponentInstanceNew(l570,&l50);if(l668!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}
AudioStreamBasicDescription l25{};l25.mSampleRate=ln;l25.mFormatID=
kAudioFormatLinearPCM;l25.mFormatFlags=
kAudioFormatFlagsNativeFloatPacked;l25.mBitsPerChannel=32;l25.
mChannelsPerFrame=1;l25.mBytesPerFrame=sizeof(Float32) *l25.
mChannelsPerFrame;l25.mFramesPerPacket=1;l25.mBytesPerPacket=l25.
mBytesPerFrame*l25.mFramesPerPacket;l25.mReserved=0;OSStatus l753=
AudioUnitSetProperty(l50,kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Input,0,&l25,sizeof(l25));if(l753!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74"
"\x20\x73\x74\x72\x65\x61\x6d\x20\x66\x6f\x72\x6d\x61\x74\x2e");}
AURenderCallbackStruct l158{};l158.inputProc=&l28::l724;l158.
inputProcRefCon=this;OSStatus l741=AudioUnitSetProperty(l50,
kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&l158,
sizeof(l158));if(l741!=noErr){throw std::runtime_error("\x46\x61\x69"
"\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74\x20\x72\x65\x6e\x64\x65\x72"
"\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x2e");}}catch(...){
AudioComponentInstanceDispose(l50);l50=nullptr;throw;}}static OSStatus
l724(void*l715,AudioUnitRenderActionFlags*l84,const AudioTimeStamp*
l819,UInt32 l825,UInt32 l587,AudioBufferList*l328)noexcept{auto*l155=
static_cast<l28* >(l715);if(l155==nullptr||l328==nullptr){return
kAudio_ParamError;}l96::l132 l612(l155->l254);if(!l612){l245(l84,l328
);return noErr;}return l155->l519(l84,l587,l328);}OSStatus l519(
AudioUnitRenderActionFlags*l84,UInt32 l253,AudioBufferList*data)noexcept
;void l68(){if(l93)return;OSStatus l4=AudioUnitInitialize(l50);if(l4
!=noErr){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x20\x61\x75\x64\x69"
"\x6f\x20\x75\x6e\x69\x74\x2e");}l254.open();l4=AudioOutputUnitStart(
l50);if(l4!=noErr){l254.l559();AudioUnitUninitialize(l50);throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x74\x61"
"\x72\x74\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}l93=true
;}void l67()noexcept{if(!l93)return;l254.l559();AudioOutputUnitStop(
l50);AudioUnitUninitialize(l50);l93=false;}static void l245(
AudioUnitRenderActionFlags*l84,AudioBufferList*data)noexcept;~l28()noexcept
{l67();if(l50!=nullptr){AudioComponentInstanceDispose(l50);l50=
nullptr;}}};
#include<mach/mach_time.h>
class l333{private:uint64_t l441=0;double l61=44100.0;double l118=0.0
;public:explicit l333(double ln):l61(ln){}inline void l690()noexcept{
l441=clock_gettime_nsec_np(CLOCK_UPTIME_RAW);}inline void l620(int
l584)noexcept{const uint64_t end=clock_gettime_nsec_np(
CLOCK_UPTIME_RAW);const uint64_t l334=end-l441;const double l720=
1000000000.0*static_cast<double>(l584)/l61;const double l658=
static_cast<double>(l334)/l720;l118=0.9*l118+0.1*l658;}[[nodiscard]]
inline double l805()const noexcept{return l118;}[[nodiscard]]inline
double l822()const noexcept{return l118*100.0;}[[nodiscard]]inline
double l807()const noexcept{if(l118<=0.0){return 0.0;}return 1.0/l118
;}[[nodiscard]]inline bool l821()const noexcept{return l118<=1.0;}[[
nodiscard]]double l626(std::unique_ptr<l88>&l91,int l671=512,int l499
=8192,int l202=5)const{constexpr int l509=31;l202=std::clamp(l202,1,
l509);std::array<double,l509>l300{};volatile float l311=0.0f;for(int
l320=0;l320<l202;++l320){for(int l53=0;l53<l671;++l53){l91->l365();
l311=l311+l91->l216();}const uint64_t l68=clock_gettime_nsec_np(
CLOCK_UPTIME_RAW);for(int l53=0;l53<l499;++l53){l91->l365();l311=l311
+l91->l216();}const uint64_t end=clock_gettime_nsec_np(
CLOCK_UPTIME_RAW);const double l334=static_cast<double>(end-l68);
const double l730=1000000000.0*static_cast<double>(l499)/l61;l300[
l320]=l334/l730;}std::sort(l300.begin(),l300.begin()+l202);return l300
[static_cast<std::size_t>(l202/2)];}inline void reset()noexcept{l118=
0.0;}void l303(std::unique_ptr<l88>&l91,std::string l795)const{const
double l312=l626(l91);const double l531=1000000.0/l61;const double
l652=l531*l312;std::cout<<std::fixed<<std::setprecision(2)<<"\x20\x20"
"\x20\x20\x46\x72\x61\x6d\x65\x3a\x20"<<l652<<"\x20\x75\x73\x20\x2f"
"\x20"<<l531<<"\x20\x75\x73\n"<<"\x20\x20\x20\x20\x4f\x63\x63\x75\x70"
"\x61\x6e\x63\x79\x3a\x20"<<l312*100.0<<"\x25\n"<<"\x20\x20\x20\x20"
"\x53\x70\x65\x65\x64\x3a\x20"<<(1.0/l312)<<"\x78\x20\x72\x65\x61\x6c"
"\x74\x69\x6d\x65\n"<<"\x20\x20\x20\x20"+l795+"\x20\x72\x65\x61\x6c"
"\x74\x69\x6d\x65\x3a\x20"<<(l312<=1.0?"\x50\x41\x53\x53":"\x46\x41"
"\x49\x4c")<<"\n\n";}};
#include<type_traits>
enum class l6:std::uint8_t{l49,l98,l187,l562,l469,l536,l448,l59,};
struct l92{l6 l14=l6::l59;int lq=0;double lt=0.0;};template<std::
size_t l117>class l162{static_assert(l117>=2&&(l117&(l117-1))==0);
static_assert(std::is_trivially_copyable_v<l92>);public:l162()noexcept
{for(std::size_t lh=0;lh<l117;++lh){l332[lh].l107.store(lh,std::
memory_order_relaxed);}}l162(const l162&)=delete;l162&operator=(const
l162&)=delete;bool l738(l92 l3)noexcept{std::size_t l8=l318.load(std
::memory_order_relaxed);for(;;){l364&l24=l332[l8&(l117-1)];const std
::size_t l107=l24.l107.load(std::memory_order_acquire);const auto l296
=static_cast<std::intptr_t>(l107)-static_cast<std::intptr_t>(l8);if(
l296==0){if(l318.compare_exchange_weak(l8,l8+1,std::
memory_order_relaxed)){l24.l3=l3;l24.l107.store(l8+1,std::
memory_order_release);return true;}}else if(l296<0){return false;}
else{l8=l318.load(std::memory_order_relaxed);}}}bool l375(l92&l3)noexcept
{l364&l24=l332[l293&(l117-1)];const std::size_t l107=l24.l107.load(
std::memory_order_acquire);const auto l296=static_cast<std::intptr_t>
(l107)-static_cast<std::intptr_t>(l293+1);if(l296!=0){return false;}
l3=l24.l3;l24.l107.store(l293+l117,std::memory_order_release);++l293;
return true;}void clear()noexcept{l92 l3;while(l375(l3)){}}private:
struct l364{std::atomic<std::size_t>l107{0};l92 l3{};};alignas(64)std
::array<l364,l117>l332{};alignas(64)std::atomic<std::size_t>l318{0};
alignas(64)std::size_t l293=0;};class l788{public:bool l590(l92 l3)noexcept
{if(l286.l738(l3)){return true;}l337.store(true,std::
memory_order_release);return false;}template<class l608>void l654(
l608&&l564){l92 l3;if(l337.exchange(false,std::memory_order_acq_rel)){
while(l286.l375(l3)){}l564(l92{l6::l59,0,0.0});}while(l286.l375(l3)){
l564(l3);}}void clear()noexcept{l286.clear();l337.store(false,std::
memory_order_release);}private:l162<4096>l286;std::atomic<bool>l337{
false};};std::unique_ptr<l88>lz;std::unique_ptr<l28>l155;std::
unique_ptr<l333>l82;namespace{constexpr int l189=21;constexpr int l383
=108;l788 l140;std::atomic<bool>l394{false};std::atomic<std::uint32_t
>l301{0};bool l63(int l249)noexcept{return l249>=l189&&l249<=l383;}
void l565()noexcept{l301.fetch_add(1,std::memory_order_release);l301.
notify_all();}void l114(l6 l14,int l249,double lt)noexcept{
static_cast<void>(l140.l590({l14,l249,lt}));}void l233(const l92&l3){
if(!lz){throw std::logic_error("\x50\x69\x61\x6e\x6f\x20\x65\x6e\x67"
"\x69\x6e\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x69\x74\x69\x61"
"\x6c\x69\x7a\x65\x64\x2e");}switch(l3.l14){case l6::l49:{if(!l63(l3.
lq))break;l235*l72=lz->l10[l3.lq-l189];l72->l207=true;for(int lc=0;lc
<l72->l57;++lc){l72->l19[lc].l193=false;}l72->l104=true;const double
l790=std::pow(2,(l3.lt-52.0)/25.0);l72->l377(l790);break;}case l6::
l98:{if(l63(l3.lq)){lz->l10[l3.lq-l189]->l207=false;}l235*l72=lz->l10
[l3.lq-l189];l72->l207=false;if(!l72->l226){for(int lc=0;lc<l72->l57;
++lc){l72->l19[lc].l193=true;}}l72->l104=true;break;}case l6::l448:lz
->sustainpedal_control(l3.lt>0.01);break;case l6::l59:lz->lx();break;
case l6::l187:case l6::l562:case l6::l469:case l6::l536:break;}}}void
l577(double ln){lz=std::make_unique<l88>(ln);}void l535()noexcept{lz.
reset();}void l493(double ln){l82=std::make_unique<l333>(ln);}void
l502()noexcept{l82.reset();}void l436(double ln){l155=std::
make_unique<l28>(ln);l155->l68();}void l372()noexcept{l155.reset();}
void l373(float*out,int l347,double l645){if(out==nullptr||l347<0){
throw std::invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20\x61\x75"
"\x64\x69\x6f\x20\x6f\x75\x74\x70\x75\x74\x20\x62\x75\x66\x66\x65\x72"
"\x2e");}if(!lz||!l82){throw std::logic_error("\x50\x69\x61\x6e\x6f"
"\x20\x65\x6e\x67\x69\x6e\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e"
"\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x2e");}l140.l654([](const l92&
l3){l233(l3);});l82->l690();for(int lc=0;lc<l347;++lc){lz->l365();
float lo=lz->l216();if(!std::isfinite(lo)){std::cerr<<"\x4f\x55\x54"
"\x50\x55\x54\x20\x41\x42\x4e\x4f\x52\x4d\x41\x4c\n"<<"\x73\x61\x6d"
"\x70\x6c\x65\x20\x3d\x20"<<lo<<'\n'<<"\x66\x72\x61\x6d\x65\x20\x3d"
"\x20"<<lc<<'\n';for(int lc=0;lc<lz->l10.size();lc++){if(lz->l10[lc]
->l104){std::cout<<"\x68\x61\x6d\x6d\x65\x72\x5f\x2e\x77\x5f\x61\x5f"
"\x31\x5f"<<lz->l10[lc]->l99.l65<<"\n";std::cout<<"\x68\x61\x6d\x6d"
"\x65\x72\x5f\x2e\x77\x5f\x62\x5f\x31\x5f"<<lz->l10[lc]->l99.l79<<""
"\n";std::cout<<"\x68\x61\x6d\x6d\x65\x72\x5f\x2e\x6d\x69\x64\x64\x6c"
"\x65\x5f\x76\x5f"<<lz->l10[lc]->l99.l172<<"\n";}}std::cout<<"\n";lo=
0.0f;}out[lc]=lo*l645;}l82->l620(l347);}void l49(int lk,double l94)noexcept
{if(l63(lk)){l114(l6::l49,lk,l94);}}void l98(int lk,double l94)noexcept
{if(l63(lk)){l114(l6::l98,lk,l94);}}void l187(int lk,double l516)noexcept
{if(l63(lk)){l114(l6::l187,lk,l516);}}void l125(double l11)noexcept{
l114(l6::l562,0,l11);}void l119(double l11)noexcept{l114(l6::l469,0,
l11);}void l127(double l11)noexcept{l114(l6::l536,0,l11);}void
sustainpedal_control(double l11)noexcept{l114(l6::l448,0,l11);}void
l59()noexcept{l114(l6::l59,0,0.0);}void l576()noexcept{l140.clear();
if(lz){lz->lx();}}void l386()noexcept{l394.store(true,std::
memory_order_release);l565();}bool l197()noexcept{return l394.load(
std::memory_order_acquire);}void l521()noexcept{l394.store(false,std
::memory_order_release);}bool l520(std::stop_token stop_token)noexcept
{std::stop_callback l551(stop_token,[]{l565();});while(!stop_token.
stop_requested()&&!l197()){const std::uint32_t l610=l301.load(std::
memory_order_acquire);if(stop_token.stop_requested()||l197()){break;}
l301.wait(l610,std::memory_order_acquire);}return l197();}void l454(){
if(!lz||!l82){throw std::logic_error("\x50\x69\x61\x6e\x6f\x20\x65"
"\x6e\x67\x69\x6e\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x69\x74"
"\x69\x61\x6c\x69\x7a\x65\x64\x2e");}l140.clear();lz->lx();for(int lm
=79;lm<=l383;++lm){l233({l6::l49,lm,110.0});}l82->l303(lz,"\x48\x69"
"\x67\x68\x2d\x72\x61\x6e\x67\x65");l140.clear();lz->lx();for(int lm=
48;lm<=78;++lm){l233({l6::l49,lm,110.0});}l82->l303(lz,"\x4d\x69\x64"
"\x2d\x72\x61\x6e\x67\x65");l140.clear();lz->lx();for(int lm=l189;lm
<=47;++lm){l233({l6::l49,lm,110.0});}l82->l303(lz,"\x42\x61\x73\x73"
"\x2d\x72\x61\x6e\x67\x65");l140.clear();lz->lx();for(int lm=l189;lm
<=l383;++lm){l233({l6::l49,lm,110.0});}l82->l303(lz,"\x38\x38\x2d\x6b"
"\x65\x79\x73");lz->lx();}
#include<cstring>
OSStatus l28::l519(AudioUnitRenderActionFlags*l84,UInt32 l253,
AudioBufferList*data)noexcept{if(data==nullptr||data->mNumberBuffers
!=1||data->mBuffers[0].mData==nullptr||data->mBuffers[0].
mNumberChannels!=1||data->mBuffers[0].mDataByteSize<l253*sizeof(
Float32)||l253>static_cast<UInt32>(std::numeric_limits<int>::max())){
l386();l245(l84,data);return noErr;}AudioBuffer&buffer=data->mBuffers
[0];auto*l692=static_cast<Float32* >(buffer.mData);try{l373(l692,
static_cast<int>(l253),0.90);return noErr;}catch(...){l386();l245(l84
,data);return noErr;}}void l28::l245(AudioUnitRenderActionFlags*l84,
AudioBufferList*data)noexcept{if(data!=nullptr){for(UInt32 lc=0;lc<
data->mNumberBuffers;++lc){AudioBuffer&buffer=data->mBuffers[lc];if(
buffer.mData!=nullptr){std::memset(buffer.mData,0,buffer.
mDataByteSize);}}}if(l84!=nullptr){ *l84|=
kAudioUnitRenderAction_OutputIsSilence;}}
#include<csignal>
#include<cstdlib>
#include<exception>
#include<getopt.h>
#include<pthread.h>
#include<signal.h>
namespace{void l83(const char*l13,const char*l29);void l185();void
l579(bool l166);void l327();class l152{public:explicit l152(bool l166
){l579(l166);}l152(const l152&)=delete;l152&operator=(const l152&)=
delete;~l152(){l327();}};}int l466(int l204,char*l18[],const char*l13
,const char*l29,std::stop_token l238){sigset_t l135;sigemptyset(&l135
);sigaddset(&l135,SIGINT);sigaddset(&l135,SIGTERM);sigaddset(&l135,
SIGUSR1);const int l593=pthread_sigmask(SIG_BLOCK,&l135,nullptr);if(
l593!=0){std::cerr<<"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x62\x6c"
"\x6f\x63\x6b\x20\x70\x72\x6f\x63\x65\x73\x73\x20\x73\x69\x67\x6e\x61"
"\x6c\x73\x2e\n";return EXIT_FAILURE;}static const option l633[]={{""
"\x68\x65\x6c\x70",no_argument,nullptr,'h'},{"\x76\x65\x72\x73\x69"
"\x6f\x6e",no_argument,nullptr,'v'},{"\x6d\x69\x64\x69",
required_argument,nullptr,'m'},{"\x70\x69\x61\x6e\x6f",no_argument,
nullptr,'p'},{"\x6b\x65\x79\x62\x6f\x61\x72\x64",no_argument,nullptr,
'k'},{"\x65\x78\x70\x6f\x72\x74",required_argument,nullptr,'e'},{""
"\x72\x65\x63\x6f\x72\x64",no_argument,nullptr,'r'},{"\x74\x65\x73"
"\x74",no_argument,nullptr,'t'},{"\x69\x6e\x74\x65\x72\x6e\x61\x6c"
"\x2d\x74\x65\x73\x74",no_argument,nullptr,'i'},{nullptr,0,nullptr,0}
};int l495;struct l757{bool l185=false;bool l13=false;std::optional<
std::string>lm=std::nullopt;bool l91=false;bool l70=false;std::
optional<std::string>l208=std::nullopt;bool l325=false;bool l247=
false;bool l314=false;};l757 lv;while((l495=getopt_long(l204,l18,""
"\x68\x76\x6d\x3a\x70\x6b\x65\x3a\x72\x74\x69",l633,nullptr))!=-1){
switch(l495){case'h':lv.l185=true;break;case'v':lv.l13=true;break;
case'm':lv.lm=optarg;break;case'p':lv.l91=true;break;case'k':lv.l70=
true;break;case'e':lv.l208=optarg;break;case'r':lv.l325=true;break;
case't':lv.l247=true;break;case'i':lv.l314=true;break;default:return
EXIT_FAILURE;}}if(!(lv.l185||lv.l13||lv.lm||lv.l91||lv.l70||lv.l208||
lv.l325||lv.l247||lv.l314)){l83(l13,l29);return EXIT_FAILURE;}if(lv.
l185){l185();return EXIT_SUCCESS;}if(lv.l13){l83(l13,l29);return
EXIT_SUCCESS;}std::stop_source l86;const std::stop_token stop_token=
l86.get_token();std::stop_callback l806(l238,[&l86]{l86.request_stop(
);});const bool l166=!lv.l208.has_value()&&!lv.l247;l152 l809(l166);
std::jthread l511([l135,&l86](std::stop_token l611){const pthread_t
l482=pthread_self();std::stop_callback l816(l611,[l482]{static_cast<
void>(pthread_kill(l482,SIGUSR1));});int l548=0;if(sigwait(&l135,&
l548)==0&&l548!=SIGUSR1){l86.request_stop();}});std::jthread l768([&
l86]{const bool l618=l520(l86.get_token());if(l618&&!l86.
stop_requested()){l86.request_stop();}});std::exception_ptr l426;try{
if(lv.lm){l83(l13,l29);l560( *lv.lm,stop_token);}else if(lv.l91){l83(
l13,l29);l500(stop_token);}else if(lv.l70){l83(l13,l29);l437(
stop_token);}else if(lv.l208){l83(l13,l29);l561( *lv.l208,stop_token);
}else if(lv.l325){l83(l13,l29);l537(stop_token);}else if(lv.l247){l83
(l13,l29);l505(stop_token);}else if(lv.l314){l83(l13,l29);l475(
stop_token);}}catch(...){l426=std::current_exception();}l86.
request_stop();l511.request_stop();l511.join();l768.join();l372();
const bool l745=l197();if(l426){try{std::rethrow_exception(l426);}
catch(const std::exception&l648){std::cerr<<"\x62\x62\x70\x6c\x3a\x20"
<<l648.what()<<'\n';}catch(...){std::cerr<<"\x62\x62\x70\x6c\x3a\x20"
"\x75\x6e\x6b\x6e\x6f\x77\x6e\x20\x73\x65\x72\x76\x69\x63\x65\x20\x66"
"\x61\x69\x6c\x75\x72\x65\x2e\n";}return EXIT_FAILURE;}if(l745){std::
cerr<<"\x54\x68\x65\x20\x61\x75\x64\x69\x6f\x20\x72\x65\x6e\x64\x65"
"\x72\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x20\x66\x61\x69\x6c\x65\x64"
"\x2e\n";return EXIT_FAILURE;}return EXIT_SUCCESS;}namespace{void l83
(const char*l13,const char*l29){std::cout<<l29;}void l185(){std::cout
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
"\x72\x77\x72\x69\x74\x74\x65\x6e\x2e\n\n\x20\x20\x20\x20";}void l579
(bool l166){l521();try{l577(ln);l493(ln);if(l166){l436(ln);}}catch(
...){l327();throw;}}void l327(){l372();l576();l502();l535();}}extern "C" {void midi_service_start(const char*l44);void midi_service_stop
(void);void piano_service_start(void);void piano_service_stop(void);
void keyboard_service_start(void);void keyboard_service_stop(void);
void export_service_start(const char*l44);void export_service_stop(
void);void record_service_start(void);void record_service_stop(void);
void test_service_start(void);void test_service_stop(void);void
internal_test_service_start(void);void internal_test_service_stop(
void);void close_all_service(void);}
namespace{static constexpr const char*l138="\x4c\x31\x2d\x43\x6c\x61"
"\x76\x69\x65\x72";static constexpr const char*l143="\n\x20\x20\x20"
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
"\n\n\x20\x20\x20\x20" "\x20\x20\x20\x20";std::jthread l428;std::
jthread l349;std::jthread l393;std::jthread l421;std::jthread l353;
std::jthread l340;std::jthread l427;void l39(std::jthread&l370){if(!
l370.joinable()){return;}l370.request_stop();l370.join();}}void
midi_service_start(const char*l44){if(l44==nullptr){return;}l39(l428);
std::string path(l44);l428=std::jthread([path=std::move(path)](std::
stop_token stop_token)mutable{char l47[]="\x62\x62\x70\x6c";char
option[]="\x2d\x6d";char*l18[]={l47,option,path.data()};l116(3,l18,
l138,l143,stop_token);});}void midi_service_stop(void){l39(l428);}
void piano_service_start(void){l39(l349);l349=std::jthread([](std::
stop_token stop_token){char l47[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x70";char*l18[]={l47,option};l116(2,l18,l138,l143,stop_token);}
);}void piano_service_stop(void){l39(l349);}void
keyboard_service_start(void){l39(l393);l393=std::jthread([](std::
stop_token stop_token){char l47[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x6b";char*l18[]={l47,option};l116(2,l18,l138,l143,stop_token);}
);}void keyboard_service_stop(void){l39(l393);}void
export_service_start(const char*l44){if(l44==nullptr){return;}l39(
l421);std::string path(l44);l421=std::jthread([path=std::move(path)](
std::stop_token stop_token)mutable{char l47[]="\x62\x62\x70\x6c";char
option[]="\x2d\x65";char*l18[]={l47,option,path.data()};l116(3,l18,
l138,l143,stop_token);});}void export_service_stop(void){l39(l421);}
void record_service_start(void){l39(l353);l353=std::jthread([](std::
stop_token stop_token){char l47[]="\x62\x62\x70\x6c";char option[]=""
"\x2d\x72";char*l18[]={l47,option};l116(2,l18,l138,l143,stop_token);}
);}void record_service_stop(void){l39(l353);}void test_service_start(
void){l39(l340);l340=std::jthread([](std::stop_token stop_token){char
l47[]="\x62\x62\x70\x6c";char option[]="\x2d\x74";char*l18[]={l47,
option};l116(2,l18,l138,l143,stop_token);});}void test_service_stop(
void){l39(l340);}void internal_test_service_start(void){l39(l427);
l427=std::jthread([](std::stop_token stop_token){char l47[]="\x62\x62"
"\x70\x6c";char option[]="\x2d\x69";char*l18[]={l47,option};l116(2,
l18,l138,l143,stop_token);});}void internal_test_service_stop(void){
l39(l427);}void close_all_service(void){midi_service_stop();
piano_service_stop();keyboard_service_stop();export_service_stop();
record_service_stop();test_service_stop();internal_test_service_stop(
);}
