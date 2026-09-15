/*
   'input.cpp' Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Sun Sep 13 10:33:08 2026
*/
#include<csignal>
#include<cstdlib>
#include<getopt.h>
#include<iostream>
#include<stdexcept>
#include<string>
#include<utility>
#include<mutex>
#include<pthread.h>
#include<signal.h>
#include<stop_token>
#include<system_error>
#include<thread>
#include<unistd.h>
#include<exception>
#include<optional>
int l392(int l136,char*l127[],const char*l28,const char*l50);
#include<AudioToolbox/AudioToolbox.h>
#include<atomic>
#include<cstdint>
class l73{public:class l134{public:explicit l134(l73&l398)noexcept:
l391(&l398),l131(l398.l627()){}l134(const l134&)=delete;l134&operator
=(const l134&)=delete;~l134(){if(l131){l391->l616();}}explicit
operator bool()const noexcept{return l131;}private:l73*l391;bool l131
;};l73()noexcept=default;l73(const l73&)=delete;l73&operator=(const
l73&)=delete;void open()noexcept{l78.store(0,std::l96);}void l372()noexcept
{std::uint32_t l87=l78.l721(l167,std::l408)|l167;while((l87&l283)!=0){
l78.wait(l87,std::l72);l87=l78.load(std::l72);}}bool l700()const
noexcept{return(l78.load(std::l72)&l167)==0;}private:static constexpr
std::uint32_t l167=0x80000000U;static constexpr std::uint32_t l283=~
l167;bool l627()noexcept{std::uint32_t l87=l78.load(std::l72);while((
l87&l167)==0){if((l87&l283)==l283){return false;}if(l78.l543(l87,l87+
1,std::l408,std::l72)){return true;}}return false;}void l616()noexcept
{l78.l708(1,std::l96);l78.notify_all();}std::atomic<std::uint32_t>l78
{l167};};class l24{l24(l24&&)=delete;l24&operator=(l24&&)=delete;
AudioUnit l36=nullptr;l73 l216;bool l61=false;public:l24(const l24&)=
delete;l24&operator=(const l24&)=delete;explicit l24(double ll){try{
AudioComponentDescription l147{};l147.componentType=
kAudioUnitType_Output;l147.componentSubType=
kAudioUnitSubType_DefaultOutput;l147.componentManufacturer=
kAudioUnitManufacturer_Apple;l147.componentFlags=0;l147.
componentFlagsMask=0;AudioComponent l389=AudioComponentFindNext(
nullptr,&l147);if(l389==nullptr){throw std::runtime_error("\x46\x61"
"\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e\x64\x20\x61\x75\x64\x69"
"\x6f\x20\x63\x6f\x6d\x70\x6f\x6e\x65\x6e\x74\x2e");}OSStatus l593=
AudioComponentInstanceNew(l389,&l36);if(l593!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}
AudioStreamBasicDescription l20{};l20.mSampleRate=ll;l20.mFormatID=
kAudioFormatLinearPCM;l20.mFormatFlags=
kAudioFormatFlagsNativeFloatPacked;l20.mBitsPerChannel=32;l20.
mChannelsPerFrame=1;l20.mBytesPerFrame=sizeof(Float32) *l20.
mChannelsPerFrame;l20.mFramesPerPacket=1;l20.mBytesPerPacket=l20.
mBytesPerFrame*l20.mFramesPerPacket;l20.mReserved=0;OSStatus l668=
AudioUnitSetProperty(l36,kAudioUnitProperty_StreamFormat,
kAudioUnitScope_Input,0,&l20,sizeof(l20));if(l668!=noErr){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74"
"\x20\x73\x74\x72\x65\x61\x6d\x20\x66\x6f\x72\x6d\x61\x74\x2e");}
AURenderCallbackStruct l162{};l162.inputProc=&l24::l637;l162.
inputProcRefCon=this;OSStatus l662=AudioUnitSetProperty(l36,
kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&l162,
sizeof(l162));if(l662!=noErr){throw std::runtime_error("\x46\x61\x69"
"\x6c\x65\x64\x20\x74\x6f\x20\x73\x65\x74\x20\x72\x65\x6e\x64\x65\x72"
"\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x2e");}}catch(...){
AudioComponentInstanceDispose(l36);l36=nullptr;throw;}}static OSStatus
l637(void*l640,AudioUnitRenderActionFlags*l58,const AudioTimeStamp*
l715,UInt32 l691,UInt32 l531,AudioBufferList*l344)noexcept{auto*l123=
static_cast<l24* >(l640);if(l123==nullptr||l344==nullptr){return l701
;}l73::l134 l551(l123->l216);if(!l551){l229(l58,l344);return noErr;}
return l123->l460(l58,l531,l344);}OSStatus l460(
AudioUnitRenderActionFlags*l58,UInt32 l219,AudioBufferList*data)noexcept
;void l68(){if(l61)return;OSStatus l1=AudioUnitInitialize(l36);if(l1
!=noErr){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x20\x61\x75\x64\x69"
"\x6f\x20\x75\x6e\x69\x74\x2e");}l216.open();l1=AudioOutputUnitStart(
l36);if(l1!=noErr){l216.l372();AudioUnitUninitialize(l36);throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x73\x74\x61"
"\x72\x74\x20\x61\x75\x64\x69\x6f\x20\x75\x6e\x69\x74\x2e");}l61=true
;}void l55()noexcept{if(!l61)return;l216.l372();AudioOutputUnitStop(
l36);AudioUnitUninitialize(l36);l61=false;}static void l229(
AudioUnitRenderActionFlags*l58,AudioBufferList*data)noexcept;~l24()noexcept
{l55();if(l36!=nullptr){AudioComponentInstanceDispose(l36);l36=
nullptr;}}};
static constexpr double ll=44100.0;int l405(int l136,char*l127[],
const char*l28,const char*l50);void l490(const std::string&l158,std::
stop_token stop_token);void l447(std::stop_token stop_token);void l411
(std::stop_token stop_token);void l380(std::string l695,std::
stop_token stop_token);void l467(std::stop_token stop_token);void l422
(std::stop_token stop_token);void l464(std::stop_token stop_token);
void l388(double ll);void l456()noexcept;void l446(double ll);void
l453()noexcept;void l402(double ll);void l294()noexcept;void l329(
float*out,int l424,double l717);double l666()noexcept;void l85(int lh
,double l65)noexcept;void l76(int lh,double l65)noexcept;void l166(
int lh,double l465)noexcept;void l99(double l7)noexcept;void l100(
double l7)noexcept;void l93(double l7)noexcept;void l53(double l7)noexcept
;void l44()noexcept;void l431()noexcept;bool l469(std::stop_token
stop_token)noexcept;bool l173()noexcept;void l270()noexcept;void l471
()noexcept;namespace{void l271(const char*l28,const char*l50);void
l149();void l378(bool l146);void l288();class l116{public:explicit
l116(bool l146){l378(l146);}l116(const l116&)=delete;l116&operator=(
const l116&)=delete;~l116(){l288();}};}int l392(int l136,char*l127[],
const char*l28,const char*l50){sigset_t l137;sigemptyset(&l137);
sigaddset(&l137,SIGINT);sigaddset(&l137,SIGTERM);sigaddset(&l137,l494
);const int l535=pthread_sigmask(SIG_BLOCK,&l137,nullptr);if(l535!=0){
std::cerr<<"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x62\x6c\x6f\x63"
"\x6b\x20\x70\x72\x6f\x63\x65\x73\x73\x20\x73\x69\x67\x6e\x61\x6c\x73"
"\x2e\n";return EXIT_FAILURE;}static const option l569[]={{"\x68\x65"
"\x6c\x70",no_argument,nullptr,'h'},{"\x76\x65\x72\x73\x69\x6f\x6e",
no_argument,nullptr,'v'},{"\x6d\x69\x64\x69",required_argument,
nullptr,'m'},{"\x70\x69\x61\x6e\x6f",no_argument,nullptr,'p'},{"\x6b"
"\x65\x79\x62\x6f\x61\x72\x64",no_argument,nullptr,'k'},{"\x65\x78"
"\x70\x6f\x72\x74",required_argument,nullptr,'e'},{"\x72\x65\x63\x6f"
"\x72\x64",no_argument,nullptr,'r'},{"\x74\x65\x73\x74",no_argument,
nullptr,'t'},{"\x69\x6e\x74\x65\x72\x6e\x61\x6c\x2d\x74\x65\x73\x74",
no_argument,nullptr,'i'},{nullptr,0,nullptr,0}};int l451;struct l683{
bool l149=false;bool l28=false;std::optional<std::string>l0=std::
nullopt;bool l264=false;bool l51=false;std::optional<std::string>l183
=std::nullopt;bool l287=false;bool l280=false;bool l275=false;};l683
ls;while((l451=getopt_long(l136,l127,"\x68\x76\x6d\x3a\x70\x6b\x65"
"\x3a\x72\x74\x69",l569,nullptr))!=-1){switch(l451){case'h':ls.l149=
true;break;case'v':ls.l28=true;break;case'm':ls.l0=optarg;break;case
'p':ls.l264=true;break;case'k':ls.l51=true;break;case'e':ls.l183=
optarg;break;case'r':ls.l287=true;break;case't':ls.l280=true;break;
case'i':ls.l275=true;break;default:return EXIT_FAILURE;}}if(!(ls.l149
||ls.l28||ls.l0||ls.l264||ls.l51||ls.l183||ls.l287||ls.l280||ls.l275)){
l271(l28,l50);return EXIT_FAILURE;}if(ls.l149){l149();return
EXIT_SUCCESS;}if(ls.l28){l271(l28,l50);return EXIT_SUCCESS;}std::l709
l101;const std::stop_token stop_token=l101.l597();const bool l146=!ls
.l183.has_value();l116 l698(l146);std::l607 l462([l137,&l101](std::
stop_token l537){const l705 l420=l711();std::stop_callback l712(l537,
[l420]{static_cast<void>(l714(l420,l494));});int l492=0;if(sigwait(&
l137,&l492)==0&&l492!=l494){l101.l293();}});std::l607 l664([&l101]{
const bool l553=l469(l101.l597());if(l553&&!l101.stop_requested()){
l101.l293();}});std::l719 l353;try{if(ls.l0){l490( *ls.l0,stop_token);
}else if(ls.l264){l447(stop_token);}else if(ls.l51){l411(stop_token);
}else if(ls.l183){l380( *ls.l183,stop_token);}else if(ls.l287){l467(
stop_token);}else if(ls.l280){l422(stop_token);}else if(ls.l275){l464
(stop_token);}}catch(...){l353=std::l693();}l101.l293();l462.l293();
l462.l509();l664.l509();l294();const bool l660=l173();if(l353){try{
std::l706(l353);}catch(const std::exception&l326){std::cerr<<"\x62"
"\x62\x70\x6c\x3a\x20"<<l326.what()<<'\n';}catch(...){std::cerr<<""
"\x62\x62\x70\x6c\x3a\x20\x75\x6e\x6b\x6e\x6f\x77\x6e\x20\x73\x65\x72"
"\x76\x69\x63\x65\x20\x66\x61\x69\x6c\x75\x72\x65\x2e\n";}return
EXIT_FAILURE;}if(l660){std::cerr<<"\x54\x68\x65\x20\x61\x75\x64\x69"
"\x6f\x20\x72\x65\x6e\x64\x65\x72\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b"
"\x20\x66\x61\x69\x6c\x65\x64\x2e\n";return EXIT_FAILURE;}return
EXIT_SUCCESS;}namespace{void l271(const char*l28,const char*l50){std
::cout<<l50<<"\n\n";std::cout<<l28<<"\n";}void l149(){std::cout<<""
"\n\x20\x20\x20\x20\x62\x62\x70\x6c\x20\x2d\x20\x62\x42\x70\x69\x61"
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
"\x72\x77\x72\x69\x74\x74\x65\x6e\x2e\n\x20\x20\x20\x20\n\x20\x20\x20"
"\x20";}void l378(bool l146){l471();try{l388(ll);l446(ll);if(l146){
l402(ll);}}catch(...){l288();throw;}}void l288(){l294();l431();l453();
l456();}}
#include<cstring>
#include<limits>
OSStatus l24::l460(AudioUnitRenderActionFlags*l58,UInt32 l219,
AudioBufferList*data)noexcept{if(data==nullptr||data->l506!=1||data->
mBuffers[0].mData==nullptr||data->mBuffers[0].l707!=1||data->mBuffers
[0].l576<l219*sizeof(Float32)||l219>static_cast<UInt32>(std::
numeric_limits<int>::max())){l270();l229(l58,data);return noErr;}
AudioBuffer&l90=data->mBuffers[0];auto*l599=static_cast<Float32* >(
l90.mData);try{l329(l599,static_cast<int>(l219),0.95);return noErr;}
catch(...){l270();l229(l58,data);return noErr;}}void l24::l229(
AudioUnitRenderActionFlags*l58,AudioBufferList*data)noexcept{if(data
!=nullptr){for(UInt32 lb=0;lb<data->l506;++lb){AudioBuffer&l90=data->
mBuffers[lb];if(l90.mData!=nullptr){std::l696(l90.mData,0,l90.l576);}
}}if(l58!=nullptr){ *l58|=l718;}}
#include<algorithm>
#include<array>
#include<cmath>
namespace lt::ly{enum class l34{l311,l581,l507,l688,l612,l570};
constexpr int l29=21;constexpr int l98=108;constexpr int l203=l98-l29
+1;static_assert(l203==88,"\x74\x75\x6e\x69\x6e\x67\x20\x70\x72\x65"
"\x73\x65\x74\x20\x74\x61\x62\x6c\x65\x20\x6d\x75\x73\x74\x20\x63\x6f"
"\x76\x65\x72\x20\x38\x38\x20\x6b\x65\x79\x73");constexpr int l580=69
;constexpr double l377=440.0;using l38=std::array<double,l203>;using
l228=std::array<double,l203>;using l199=std::array<double,l203>;
constexpr double l566=2.31;enum class l47{lw=1,l286=2,lv=3};constexpr
double l669=1.7;}class l35{l35(const l35&)=delete;l35&operator=(const
l35&)=delete;public:using l34=lt::ly::l34;using l38=lt::ly::l38;using
l47=lt::ly::l47;using l228=lt::ly::l228;using l199=lt::ly::l199;
constexpr l35()noexcept=default;double l604(int lh,l34 l75=l34::l311,
l47 l633=l47::l286)const noexcept{if(lh<lt::ly::l29||lh>lt::ly::l98){
return 0.0;}const int le=lh-lt::ly::l29;const l160&lu=l246();const
double l226=l644(lu,le,l75);switch(l633){case l47::lw:return l226-lu.
l191[le];case l47::l286:return l226;case l47::lv:return l226+lu.l191[
le];}return l226;}const l38&l713()const noexcept{return l246().l31;}
double l699(int lh)const noexcept{if(lh<lt::ly::l29||lh>lt::ly::l98){
return 0.0;}const int le=lh-lt::ly::l29;return l246().l192[le];}const
l199&l710()const noexcept{return l246().l192;}private:struct l160{l38
l31{};l38 l479{};l38 l476{};l38 l473{};l38 l400{};l38 l384{};l228 l191
{};l199 l192{};};static l160 l655(){l160 lu{};l571(lu.l31);lu.l479=lu
.l31;lu.l476=lu.l31;lu.l473=lu.l31;lu.l400=lu.l31;lu.l384=lu.l31;l540
(lu.l31,lu.l191);l606(lu.l192);return lu;}static const l160&l246(){
static const l160 lu=l655();return lu;}static double l301(int lh){
return lt::ly::l377*std::pow(2.0,static_cast<double>(lh-lt::ly::l580)/
12.0);}static double l594(double l552){constexpr double l511=-
1.3333333333333333;constexpr double l510=1.3333333333333333;constexpr
double l559=-6.5760753629956712;constexpr double l557=
1.9137732290077178;constexpr double l556=0.27614763151078042;
constexpr double l555=-0.28572738557770405;constexpr double l554=-
0.30709648310494486;const double lk=std::log2(l552/440.0);double l299
=l559+l557*lk+l556*lk*lk+l555*std::pow(std::max(lk-l511,0.0),2.0)+
l554*std::pow(std::max(lk-l510,0.0),2.0);l299=std::clamp(l299,std::
log(5e-6),std::log(2e-2));return std::exp(l299);}static double l208(
int lh){return l594(l301(lh));}static double l416(int l139){constexpr
double l501=3.5;constexpr double l686=60.0;constexpr double l665=25.0
;const double lk=(static_cast<double>(l139)-l686)/l665;return(l501*
0.5) * (1.0-std::erf(lk))+1.0;}static double l514(int l139,int l321,
double l675){const double l177=l416(l139);const double l179=l177*l177
;const double l369=l208(l139);const double l309=l208(l321);return 2.0
 *l675*std::sqrt((1.0+l369*4.0*l179)/(1.0+l309*l179));}static double
l546(int l139,int l321,double l600){const double l177=l416(l139);
const double l179=l177*l177;const double l369=l208(l139);const double
l309=l208(l321);const double l605=std::sqrt((1.0+l369*4.0*l179)/(1.0+
l309*l179));return l600/(2.0*l605);}static double l517(int lh,const
std::array<int,8>&l12,const std::array<double,8>&l128){if(lh<=l12.
front()){return l128.front();}for(std::size_t lb=0;lb+1<l12.size();++
lb){const int l435=l12[lb];const int l455=l12[lb+1];if(lh<=l455){
const double l589=static_cast<double>(lh-l435)/static_cast<double>(
l455-l435);return l128[lb]+l589* (l128[lb+1]-l128[lb]);}}const std::
size_t l188=l12.size()-1;const std::size_t l414=l188-1;const double
l671=(l128[l188]-l128[l414])/static_cast<double>(l12[l188]-l12[l414]);
return l128[l188]+l671*static_cast<double>(lh-l12[l188]);}static void
l571(l38&l31){l38 l478{};for(int l0=lt::ly::l29;l0<=lt::ly::l98;++l0){
const int le=l0-lt::ly::l29;l478[le]=l301(l0);}constexpr std::array<
int,8>l12={21,33,45,57,69,81,93,105};std::array<double,l12.size()>
l150{};std::array<double,l12.size()>l423{};constexpr int l268=4;l150[
l268]=lt::ly::l377;for(int lb=l268+1;lb<static_cast<int>(l12.size());
++lb){l150[lb]=l514(l12[lb-1],l12[lb],l150[lb-1]);}for(int lb=l268-1;
lb>=0;--lb){l150[lb]=l546(l12[lb],l12[lb+1],l150[lb+1]);}for(std::
size_t lb=0;lb<l12.size();++lb){l423[lb]=1200.0*std::log2(l150[lb]/
l301(l12[lb]));}for(int l0=lt::ly::l29;l0<=lt::ly::l98;++l0){const int
le=l0-lt::ly::l29;const double l573=l517(l0,l12,l423);l31[le]=l478[le
] *std::pow(2.0,l573/1200.0);}}static double l644(const l160&lu,int le
,l34 l75)noexcept{switch(l75){case l34::l311:return lu.l31[le];case
l34::l581:return lu.l479[le];case l34::l507:return lu.l476[le];case
l34::l688:return lu.l473[le];case l34::l612:return lu.l400[le];case
l34::l570:return lu.l384[le];}return lu.l31[le];}static int l548(int
lh)noexcept{if(lh<=28){return 1;}if(lh<=33){return 2;}return 3;}
static double l621(double l126,int l77){if(l77<=1){return 0.0;}const
double l634=std::pow(2.0,lt::ly::l669/1200.0);const double l502=l126*
l634;const double l598=l502-l126;return l598*0.5;}static void l540(
const l38&l31,l228&l191){for(int l0=lt::ly::l29;l0<=lt::ly::l98;++l0){
const int le=l0-lt::ly::l29;const int l77=l548(l0);const double l126=
l31[le];l191[le]=l621(l126,l77);}}static void l606(l199&l192){l192.
fill(lt::ly::l566);}};
namespace lt::l319::l367{struct ld{double l254,l365,l202,l305;double
l250,l204,l316,l356;double l625,l687;};inline constexpr std::array<ld
,88>l526{{ld{6.12081538397e+12,7268117.35239,0.173047550188,
84.5865652624,3.38793275297,2.46996503446,1.70189411996,1.39072232225
,0.0110001,-0.000240832882693},ld{2.05088568496e+13,2039736.84416,
1.01552603213,88.3384147364,3.51182437587,2.40432865557,1.55693252736
,1.5019573828,0.0109264,-0.00018028613841},ld{1.70094749778e+16,
21507176.7739,1.74610687156,75.6096122833,4.31941733801,2.28770707675
,1.83759093501,1.42834399857,0.0108529,-0.000163800701705},ld{
1.43066472528e+18,269201381.657,0.44817226607,55.5525611047,
4.89861580358,1.4855834808,2.01692459807,1.24354040394,0.0107796,-
0.000213657113702},ld{1.28047700014e+16,9958335.98381,0.587093274444,
74.1579959467,4.28376738725,1.20404692556,1.65976525549,1.57424506987
,0.0107065,-0.000186292327257},ld{3.32790595344e+16,83677125.7068,
0.1864900088,61.7541413228,4.40007248295,1.20475883841,1.90449259098,
1.41819302732,0.0106336,-0.000134891306302},ld{3.05903227125e+16,
211852543.065,0.0467939187322,69.199896095,4.38243676889,
1.24619014953,2.05177120188,1.44927089406,0.0105609,-
0.000208910462526},ld{3.04343261984e+14,16475294834.3,0.0613979825323
,65.1614795684,3.82922860312,1.4790957827,2.58173152874,1.32189541943
,0.0104884,-0.000175317084769},ld{3.93806865967e+14,41028014460.9,
0.382855849117,76.1472058154,3.8316174556,1.55237740074,2.72555419449
,1.39804563065,0.0104161,-0.000171318877713},ld{5.91379458356e+13,
4867145600.73,0.211964190447,74.60252053,3.59281107716,1.91819022189,
2.50085086362,1.39277325047,0.010344,-0.000147904446437},ld{
1.75912745691e+15,59774063.9782,0.077483832071,74.4874948296,
4.01927469017,1.76581867744,1.9252691081,1.39169248333,0.0102721,-
0.000217273777138},ld{1.63734661394e+16,98674109.6427,0.200361291071,
71.8114384336,4.2815642894,1.99500419498,1.92997505294,1.40670434132,
0.0102004,-0.000154190357793},ld{5.94916213618e+14,234050607.738,
0.0173613736074,71.8583331437,3.87379692279,1.72886116894,
2.07952324174,1.42872370063,0.0101289,-0.000152232643808},ld{
5.47166894428e+15,1227205414.64,0.0422979634125,69.2404172645,
4.16172584334,1.51888066202,2.42534408947,1.31816746731,0.0100576,-
0.000134955265835},ld{2.99308168484e+17,9978375029.65,0.348537008845,
73.508323946,4.63313341362,1.04557661809,2.524521658,1.40454740355,
0.0099865,-0.000129499528745},ld{1.80789786066e+16,2939909865.95,
0.132646171183,61.5460575085,4.28129183158,0.822177378051,
2.36751717769,1.31846574807,0.0099156,-0.00011467998292},ld{
1.11161451847e+17,7948661983.11,0.0611758474368,67.046651141,
4.48286761069,0.960186364174,2.42864301657,1.47701212091,0.0098449,-
0.00015593603103},ld{4.3873299959e+16,14066672533.3,0.602356114745,
77.2698922774,4.33937569638,1.10145575138,2.48473433962,1.46390632598
,0.0097744,-0.000142140399416},ld{6.64036522952e+15,10885944735.2,
0.482796352246,118.409198106,4.1169797795,1.92069137648,2.60414154691
,1.82924643484,0.0097041,-0.000106101910165},ld{7.52015082932e+15,
1461948234.27,0.329085795901,81.9724928385,4.14838456475,
2.07168784066,2.31923999311,1.52963689192,0.009634,-0.000139245873089
},ld{3.63842757684e+17,9320916907.38,0.218811062133,55.2565646144,
4.61479726558,2.26048438152,2.46487321948,1.28640617819,0.0095641,-
0.000143197277015},ld{3.58403288348e+16,8565007519.99,0.927386311232,
102.433280027,4.28916483916,1.9945573967,2.51191038189,1.7792231948,
0.0094944,-0.0001224398261},ld{8.23076317641e+16,649778394.407,
2.28641234541,83.7367590475,4.42359679766,2.19690143581,2.32083997998
,1.43902473264,0.0094249,-0.000122482726022},ld{3.3979801765e+16,
818055823.337,0.8490773077,91.795290029,4.28817491186,2.0979330401,
2.23498494235,1.67794392947,0.0093556,-0.00011048420585},ld{
6.85316129755e+16,24537212.608,0.0990729420225,89.570297158,
4.36089984709,2.19478368016,1.87339784481,1.70747583916,0.0092865,-
9.86116088299e-05},ld{1.14836898136e+15,1106156.11785,0.0206232866243
,104.996871955,3.86466527428,1.95457331721,1.50260124357,
1.69572915827,0.0092176,-0.000150059205021},ld{5.9386297485e+15,
615012.218485,0.00395170464125,109.687897491,4.08416674014,
1.70329086095,1.42638327433,1.75936700139,0.0091489,-
0.000126863498162},ld{8.40867323391e+16,16886379.6639,0.0154874576752
,75.1823009166,4.37897395449,1.29985707264,1.81187361598,
1.53113952189,0.0090804,-8.25289782926e-05},ld{1.75075561354e+17,
695572536.641,0.0295452819837,68.2299094554,4.46578984111,
1.54091858355,2.24949569842,1.44845846783,0.0090121,-
9.06305407311e-05},ld{1.95553212497e+16,501889284.916,0.0268602111684
,77.7457099064,4.20698732544,1.84179000891,2.24347524952,
1.51827359521,0.008944,-0.000116152020079},ld{6.80773669038e+14,
830099507.144,0.162766497881,152.21452683,3.78442648176,1.31053598299
,2.30096661871,1.92111512139,0.0088761,-9.61739005427e-05},ld{
1.86249577319e+16,6878735053.43,0.0310467483149,77.0641796563,
4.19422447696,1.73461964107,2.52028759044,1.47872785955,0.0088084,-
0.000101569192447},ld{3.08480440022e+16,1434321835.9,0.0133523040884,
84.0466555179,4.23548215362,2.19637389171,2.28998871888,1.60544471869
,0.0087409,-9.2426268564e-05},ld{4.03736424957e+16,2608087797.63,
0.00462598413432,79.4944705655,4.24872957473,1.89452471514,
2.33777731055,1.66095134083,0.0086736,-9.252302921e-05},ld{
1.58658823971e+17,1537489440.88,0.0468354619328,93.9515075811,
4.42983583775,1.16967583018,2.31486853522,1.69043809226,0.0086065,-
9.23118369882e-05},ld{1.84240793618e+16,2159031010.05,0.0111939988424
,86.3877695931,4.15564296003,1.28349079737,2.35904250499,
1.64937717906,0.0085396,-0.00010664590568},ld{3.17279131622e+17,
284003292.833,0.0265099644602,57.7681478094,4.51290967527,
1.15477107304,2.05994848584,1.31804440641,0.0084729,-
0.000116107345643},ld{3.01517714711e+18,2108863.39846,
0.00307640867698,67.1211212683,4.76784670986,1.17244891847,
1.55611459377,1.48555840795,0.0084064,-8.72959524805e-05},ld{
7.40364748179e+17,1366651.53153,0.0135924407086,101.04703182,
4.57979593098,0.863547487894,1.47063604581,1.81417028644,0.0083401,-
9.08197687832e-05},ld{2.69668710128e+17,6320896.86965,0.0153778449451
,91.8014437053,4.45649895447,1.18555507615,1.72364702433,
1.68364822372,0.008274,-0.000100024312038},ld{7.01022741856e+16,
1495587.71863,0.0187125258374,77.6207155999,4.30684825465,
0.880924261839,1.5523973248,1.54502162314,0.0082081,-
5.94637994639e-05},ld{3.79992684818e+17,54382408.4461,0.131274471917,
111.112906718,4.48263033949,0.93591460207,1.86966028203,1.81767717585
,0.0081424,-7.58998294437e-05},ld{8.87112318882e+17,305565116.86,
0.0767186834196,63.5116681676,4.61057339598,0.970558875582,
2.16273983112,1.35689462368,0.0080769,-5.61894732513e-05},ld{
3.85890821265e+17,357365011.642,0.0361168622469,83.7804616143,
4.49485645864,1.24692341398,2.26275070249,1.57643956966,0.0080116,-
7.25966743016e-05},ld{9.23149354767e+17,931989055.868,0.386288934707,
89.2485834017,4.5847289672,1.6960394939,2.3645157475,1.63462513089,
0.0079465,-6.4849644817e-05},ld{2.29849323929e+17,65002137.8398,
0.0779058794289,75.688103673,4.42215902784,1.68132291708,
2.13943670296,1.52560293439,0.0078816,-7.05457166508e-05},ld{
1.58001184157e+19,78506929.8641,0.0469724246026,60.2836006408,
4.93635670466,1.66168238879,1.95738624106,1.38694441485,0.0078169,-
7.13536785106e-05},ld{5.52945049014e+19,512003117.553,0.0122250676744
,62.8529001507,5.06533567608,1.98084421652,2.26332921528,
1.44539645386,0.0077524,-5.47052854692e-05},ld{2.66411818534e+20,
400898669.294,0.00495141283471,51.5446398547,5.24726355653,
1.89297802452,2.0817456657,1.28229569878,0.0076881,-6.62128909781e-05
},ld{2.0470731423e+18,3885808.26328,0.00341396395117,67.022264337,
4.65826508365,2.22742559166,1.63940639635,1.53679517104,0.007624,-
6.28707126703e-05},ld{3.34948590137e+18,12952701.5597,0.0097176651895
,63.4140424471,4.73515719862,1.85074888271,1.78488722286,
1.40682362896,0.0075601,-6.39758013627e-05},ld{6.99713810569e+18,
89772122.2919,0.125955286852,63.9844916791,4.79977526118,
1.50599270882,2.07440713967,1.45909911609,0.0074964,-
4.89610360494e-05},ld{2.03291992018e+20,165614438.325,0.2377033212,
65.0174809964,5.21540875517,1.52683837021,2.019935665,1.37547327547,
0.0074329,-7.99541636494e-05},ld{5.30882559405e+18,10699726.875,
0.103146188937,84.0632908434,4.74640783061,2.17085850623,
1.80337361018,1.72149854665,0.0073696,-4.26791095164e-05},ld{
8.6103548347e+19,49199367.119,0.150880288497,77.8692064912,
5.06802677928,2.20383444454,2.02828444257,1.64897132571,0.0073065,-
5.20397478966e-05},ld{6.09898835753e+18,140779974.598,0.0201824689513
,81.9324344629,4.77288305291,1.93851085594,2.1907378878,1.58712308643
,0.0072436,-5.15129894092e-05},ld{2.87954360485e+20,11809206407,
0.0136201524187,79.780519705,5.21177115036,1.57758020434,
2.64144720702,1.67035325939,0.0071809,-6.00313913622e-05},ld{
3.2107004926e+19,484619209.124,0.0219179598819,69.1525817477,
4.94907749709,1.70512358731,2.34566083126,1.52072666309,0.0071184,-
4.45263687638e-05},ld{6.68451838058e+18,431658885.444,
0.00286483642128,73.5452789079,4.75309880571,1.89821085522,
2.20768849561,1.60597264682,0.0070561,-6.34014896232e-05},ld{
2.75958393866e+21,1239715862.9,0.00350949528286,69.7554760276,
5.4750500309,2.09530766973,2.42229351194,1.56574199757,0.006994,-
4.32489177768e-05},ld{7.518286448e+20,610900905.43,0.0156682731818,
66.1106585821,5.30004577637,2.06663798191,2.4814627321,1.5494106736,
0.0069321,-4.0318126869e-05},ld{7.62179737645e+18,852725158.576,
0.0216623219946,78.8231914797,4.76360601146,2.12292249894,
2.48761900103,1.60263733894,0.0068704,-5.45801703171e-05},ld{
2.12794580884e+18,27623951.7227,0.0305077782504,78.2913899486,
4.5947127491,1.541871116,1.97014555643,1.61340296991,0.0068089,-
5.92707444379e-05},ld{1.16871965036e+20,51539825.8653,0.0810568058881
,80.789083344,5.07655295168,1.35504019548,1.98314208378,1.64017842455
,0.0067476,-5.39880996761e-05},ld{3.24137404567e+20,5072063.73559,
0.568468257758,61.57318377,5.18910506974,1.48539430692,1.72983151503,
1.45804461915,0.0066865,-5.78325673e-05},ld{4.00751304778e+22,
146924550.932,0.0443016654732,57.5921579617,5.74595753395,
1.25195520644,2.15930930931,1.4821114199,0.0066256,-3.52198499655e-05
},ld{9.46768625127e+20,1329432732.41,0.035283372649,57.1182786246,
5.31012438602,1.58670898436,2.42082539278,1.39122194855,0.0065649,-
4.93889624351e-05},ld{4.54970804487e+19,332918846.218,0.0259927209181
,93.8738585264,4.91629767066,1.26622815242,2.24120238765,1.8378597154
,0.0065044,-4.8388438852e-05},ld{8.42939926719e+17,47775187.8263,
0.0212259118121,73.8820979692,4.49623532243,1.90688037593,
2.13590236617,1.57428882706,0.0064441,-4.89430745838e-05},ld{
3.71241760075e+19,3344156.26191,0.0601518293813,80.4405827537,
4.91046527265,1.57502390008,1.7726230429,1.65666970099,0.006384,-
3.95109830121e-05},ld{8.38522332429e+19,89963.4880359,0.115503487942,
72.2407673436,4.99936010094,1.24346029204,1.39562707666,1.60284429297
,0.0063241,-4.20363373062e-05},ld{8.1599422871e+20,2994947.65869,
0.00597002740455,53.5205451065,5.29016624252,1.83041950294,
1.75059841621,1.34529935649,0.0062644,-5.05389212316e-05},ld{
1.64528268705e+21,47423786.1228,0.0341444737295,59.6540445007,
5.35378023914,1.78423223013,1.91036077599,1.43410727222,0.0062049,-
4.46456914795e-05},ld{4.55748742769e+20,76260437.7861,0.113895241839,
69.3773930113,5.19087473149,1.75362498278,2.03097593981,1.55844130702
,0.0061456,-4.25945959224e-05},ld{5.42368592292e+21,406429.801268,
0.325306171305,78.967283677,5.43977089541,1.64976720327,1.51653963048
,1.77028353264,0.0060865,-3.66512463134e-05},ld{2.69009076545e+21,
1348183.87672,0.0629505455017,70.9960153032,5.37811721796,
1.90756606314,1.6203009308,1.62802544856,0.0060276,-4.30352919142e-05
},ld{3.80159652288e+20,5830266.06929,0.0110490651358,81.7787329807,
5.12350303094,1.6938407938,1.78426337346,1.82868902896,0.0059689,-
3.45655368927e-05},ld{5.45140735911e+19,247318.681515,0.0203426337268
,63.7844932619,4.92433852586,1.79391889099,1.44966098995,
1.57035165751,0.0059104,-4.76855458406e-05},ld{1.15906857342e+20,
7136604.90082,0.0275828672486,99.2710201841,4.99431774514,
1.99546195276,1.83179261041,1.84885339967,0.0058521,-
5.77172394249e-05},ld{4.69015845548e+20,63244978.2904,0.0633674431757
,69.5119732568,5.15068922668,2.06318440708,2.11336602306,1.6648003566
,0.005794,-3.75561240567e-05},ld{7.22285784332e+20,174231642.895,
0.00493062277846,83.1349974469,5.19944125144,1.97588801451,
2.22014410961,1.73027686866,0.0057361,-3.97668413356e-05},ld{
5.60286217725e+20,116511694.151,0.00842675675405,77.3752303013,
5.16264458347,1.27033964,2.23849777832,1.68480765862,0.0056784,-
3.89291593564e-05},ld{1.58998921391e+19,10198182060.7,0.0794004699382
,94.0330025781,4.7494907978,1.28292650364,2.6595467747,1.84365668086,
0.0056209,-3.28062357347e-05},ld{1.95479056967e+20,277870749.672,
0.0357174964528,91.0184821,5.0294422286,1.51648833519,2.25808271842,
1.81251568537,0.0055636,-3.65732605707e-05},ld{5.29008842315e+20,
33989225.639,0.00545791120395,86.7454329897,5.14072781649,
1.32959930804,2.0896802887,1.71433288236,0.0055065,-6.28285973846e-05
},ld{2.55630605508e+22,57085259.2244,0.0309389687422,63.0303939658,
5.58842075436,1.76256088718,2.16874780095,1.60941254252,0.0054496,-
3.93350768256e-05},ld{5.40371453112e+21,80671319.5347,0.11835275404,
54.7815359372,5.41288357537,1.82050754398,2.1023839908,1.5076551051,
0.0053929,-5.68561651874e-05},ld{9.23752411116e+21,6308262.87978,
0.0677048221337,50.6273356288,5.47204979767,2.13194835042,
1.85308941552,1.40797452509,0.0053364,-4.21693002932e-05},}};}class
l157{l157(const l157&)=delete;l157&operator=(const l157&)=delete;
public:using l439=lt::l319::l367::ld;using l495=std::array<l439,88>;
constexpr l157()noexcept=default;const l439*l645(int lh)const noexcept
{constexpr int l29=21;constexpr int l98=108;if(lh<l29||lh>l98){return
nullptr;}const std::size_t le=static_cast<std::size_t>(lh-l29);return
&l486()[le];}const l495&l720()const noexcept{return l486();}private:
static constexpr const l495&l486()noexcept{return lt::l319::l367::
l526;}};class l54{l54(const l54&)=delete;l54&operator=(const l54&)=
delete;public:static constexpr l35 l591{};static constexpr l157 lp{};
constexpr l54()noexcept=default;};
#include<condition_variable>
#include<filesystem>
#include<fstream>
#include<vector>
#include<chrono>
#include<cstddef>
#include<unordered_set>
class l3:public std::runtime_error{public:using std::runtime_error::
runtime_error;};class l92{public:struct l4{std::uint64_t l23=0;double
l141=0.0;std::uint8_t l1=0;std::uint8_t l26=0;std::uint8_t l206=0;std
::string l215;int l518()const noexcept{return static_cast<int>(l1&
0x0F);}std::uint8_t l196()const noexcept{return l1&0xF0;}bool l349()const
noexcept{return l196()==0x90&&l206!=0;}bool l314()const noexcept{
return l196()==0x80||(l196()==0x90&&l206==0);}bool l397()const
noexcept{return l196()==0xA0;}bool l523()const noexcept{return l196()==
0xB0;}bool l417()const noexcept{if(!l523()){return false;}return l26
==64||l26==66||l26==67||l26==68;}double l461()const noexcept{return
std::clamp(static_cast<double>(l206)/127.0,0.0,1.0);}};enum class l45
{l190,l253};l92()=delete;static std::vector<l4>l379(const std::string
&l143){auto ln=l639(l679(l689(l143)));std::stable_sort(ln.begin(),ln.
end(),[](const l4&lw,const l4&lv){return lw.l141<lv.l141;});return ln
;}static l45 l635(double l430,double l539,const std::string&l143,std
::stop_token stop_token={}){if(!(l430>0.0)){throw std::
invalid_argument("\x70\x6c\x61\x79\x62\x61\x63\x6b\x5f\x72\x61\x74"
"\x65\x20\x6d\x75\x73\x74\x20\x62\x65\x20\x67\x72\x65\x61\x74\x65\x72"
"\x20\x74\x68\x61\x6e\x20\x7a\x65\x72\x6f\x2e");}const double l406=
std::max(0.0,l539);std::vector<l4>ln=l379(l143);const auto l498=std::
lower_bound(ln.begin(),ln.end(),l406,[](const l4&lf,double time){
return lf.l141<time;});std::unordered_set<int>l217;std::mutex l547;
std::condition_variable l265;std::stop_callback l491(stop_token,[&
l265]{l265.notify_all();});const auto l622=std::chrono::steady_clock
::now();auto l230=[&l217]{for(int lo:l217){l76(lo,0.0);}l224();};try{
for(auto lf=l498;lf!=ln.end();++lf){if(stop_token.stop_requested()){
l230();return l45::l253;}const double l677=std::max(0.0,lf->l141-l406
)/l430;const auto l536=l622+std::chrono::duration_cast<std::chrono::
steady_clock::duration>(std::chrono::duration<double>(l677));{std::
unique_lock lock(l547);l265.wait_until(lock,l536,[&stop_token]{return
stop_token.stop_requested();});}if(stop_token.stop_requested()){l230(
);return l45::l253;}l454( *lf);if(lf->l349()&&l41(lf->l26)){l217.
insert(static_cast<int>(lf->l26));}else if(lf->l314()){l217.erase(
static_cast<int>(lf->l26));}}}catch(...){l230();throw;}l230();return
l45::l190;}static void l454(const l4&lf){const int lo=static_cast<int
>(lf.l26);const double l159=static_cast<double>(lf.l206);if(lf.l349()){
if(l41(lf.l26)){l85(lo,l159);}}else if(lf.l314()){if(l41(lf.l26)){l76
(lo,l159);}}else if(lf.l397()){if(l41(lf.l26)){l166(lo,lf.l461());}}
else if(lf.l417()){l629(lf.l26,lf.l461());}}static void l224(){l99(
0.0);l100(0.0);l93(0.0);l53(0.0);}private:struct l132{std::uint64_t
l23=0;double l385=500000.0;std::size_t l433=0;};class l394{public:
explicit l394(std::vector<std::uint8_t>l21):l110(std::move(l21)){}std
::size_t l6()const noexcept{return l39;}std::size_t size()const
noexcept{return l110.size();}void l412(std::size_t l6){if(l6>l110.
size()){throw l3("\x4d\x49\x44\x49\x20\x63\x68\x75\x6e\x6b\x20\x65"
"\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c\x65\x20\x73\x69\x7a\x65\x2e"
);}l39=l6;}std::uint8_t l46(std::size_t l10=std::numeric_limits<std::
size_t>::max()){if(l39>=l110.size()||l39>=l10){throw l3("\x55\x6e\x65"
"\x78\x70\x65\x63\x74\x65\x64\x20\x65\x6e\x64\x20\x6f\x66\x20\x4d\x49"
"\x44\x49\x20\x64\x61\x74\x61\x2e");}return l110[l39++];}std::
uint16_t l273(std::size_t l10=std::numeric_limits<std::size_t>::max()){
const std::uint16_t l673=l46(l10);const std::uint16_t l626=l46(l10);
return static_cast<std::uint16_t>((l673<<8)|l626);}std::uint32_t l474
(std::size_t l10=std::numeric_limits<std::size_t>::max()){const std::
uint32_t l138=l46(l10);const std::uint32_t l140=l46(l10);const std::
uint32_t l133=l46(l10);const std::uint32_t l565=l46(l10);return(l138
<<24)|(l140<<16)|(l133<<8)|l565;}std::uint32_t l259(std::size_t l10){
std::uint32_t lr=0;for(int count=0;count<4;++count){const std::
uint8_t l27=l46(l10);lr=(lr<<7)|static_cast<std::uint32_t>(l27&0x7F);
if((l27&0x80)==0){return lr;}}throw l3("\x49\x6e\x76\x61\x6c\x69\x64"
"\x20\x4d\x49\x44\x49\x20\x76\x61\x72\x69\x61\x62\x6c\x65\x2d\x6c\x65"
"\x6e\x67\x74\x68\x20\x76\x61\x6c\x75\x65\x2e");}std::string l458(std
::size_t count,std::size_t l10=std::numeric_limits<std::size_t>::max(
)){l284(count,l10);const auto begin=l110.begin()+static_cast<std::
ptrdiff_t>(l39);l39+=count;return std::string(begin,begin+static_cast
<std::ptrdiff_t>(count));}std::vector<std::uint8_t>l534(std::size_t
count,std::size_t l10){l284(count,l10);const auto begin=l110.begin()+
static_cast<std::ptrdiff_t>(l39);l39+=count;return{begin,begin+
static_cast<std::ptrdiff_t>(count)};}void l681(std::size_t count,std
::size_t l10=std::numeric_limits<std::size_t>::max()){l284(count,l10);
l39+=count;}private:void l284(std::size_t count,std::size_t l10)const
{const std::size_t l436=std::min(l10,l110.size());if(l39>l436||count>
l436-l39){throw l3("\x55\x6e\x65\x78\x70\x65\x63\x74\x65\x64\x20\x65"
"\x6e\x64\x20\x6f\x66\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x2e");}
}std::vector<std::uint8_t>l110;std::size_t l39=0;};static bool l41(
std::uint8_t lo)noexcept{return lo>=21&&lo<=108;}static void l629(std
::uint8_t l114,double l7){switch(l114){case 64:l53(l7);break;case 66:
l93(l7);break;case 67:l99(l7);break;case 68:l100(l7);break;default:
break;}}static std::vector<std::uint8_t>l689(const std::string&path){
std::ifstream l197(path,std::ios::binary|std::ios::ate);if(!l197){
throw l3("\x43\x61\x6e\x6e\x6f\x74\x20\x6f\x70\x65\x6e\x20\x4d\x49"
"\x44\x49\x20\x66\x69\x6c\x65\x3a\x20"+path);}const std::streampos end
=l197.tellg();if(end<=0){throw l3("\x4d\x49\x44\x49\x20\x66\x69\x6c"
"\x65\x20\x69\x73\x20\x65\x6d\x70\x74\x79\x3a\x20"+path);}if(
static_cast<std::uintmax_t>(end)>std::numeric_limits<std::size_t>::
max()){throw l3("\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x20\x69\x73\x20"
"\x74\x6f\x6f\x20\x6c\x61\x72\x67\x65\x2e");}std::vector<std::uint8_t
>l21(static_cast<std::size_t>(end));l197.seekg(0,std::ios::beg);l197.
read(reinterpret_cast<char* >(l21.data()),static_cast<std::streamsize
>(l21.size()));if(!l197){throw l3("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x72\x65\x61\x64\x20\x4d\x49\x44\x49\x20\x66\x69\x6c\x65\x3a"
"\x20"+path);}return l21;}static std::vector<l4>l679(std::vector<std
::uint8_t>l21){l394 lx(std::move(l21));if(lx.l458(4)!="\x4d\x54\x68"
"\x64"){throw l3("\x4d\x69\x73\x73\x69\x6e\x67\x20\x4d\x49\x44\x49"
"\x20\x68\x65\x61\x64\x65\x72\x20\x63\x68\x75\x6e\x6b\x2e");}const std
::uint32_t l395=lx.l474();if(l395<6){throw l3("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x4d\x49\x44\x49\x20\x68\x65\x61\x64\x65\x72\x20\x6c\x65"
"\x6e\x67\x74\x68\x2e");}const std::size_t l155=lx.l6()+l395;if(l155<
lx.l6()||l155>lx.size()){throw l3("\x4d\x49\x44\x49\x20\x68\x65\x61"
"\x64\x65\x72\x20\x65\x78\x63\x65\x65\x64\x73\x20\x66\x69\x6c\x65\x20"
"\x73\x69\x7a\x65\x2e");}const std::uint16_t l20=lx.l273(l155);const
std::uint16_t l277=lx.l273(l155);const std::uint16_t l370=lx.l273(
l155);lx.l412(l155);if(l20>1){throw l3("\x4f\x6e\x6c\x79\x20\x4d\x49"
"\x44\x49\x20\x66\x6f\x72\x6d\x61\x74\x20\x30\x20\x61\x6e\x64\x20\x31"
"\x20\x61\x72\x65\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}if(
l277==0||(l20==0&&l277!=1)){throw l3("\x49\x6e\x76\x61\x6c\x69\x64"
"\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x63\x6f\x75\x6e\x74"
"\x2e");}if((l370&0x8000)!=0||l370==0){throw l3("\x53\x4d\x50\x54\x45"
"\x20\x6f\x72\x20\x7a\x65\x72\x6f\x20\x4d\x49\x44\x49\x20\x74\x69\x6d"
"\x65\x20\x64\x69\x76\x69\x73\x69\x6f\x6e\x20\x69\x73\x20\x75\x6e\x73"
"\x75\x70\x70\x6f\x72\x74\x65\x64\x2e");}std::vector<l4>ln;std::
vector<l132>l154{{0,500000.0,0}};std::size_t l520=1;for(std::uint16_t
l9=0;l9<l277;++l9){if(lx.l458(4)!="\x4d\x54\x72\x6b"){throw l3("\x4d"
"\x69\x73\x73\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b"
"\x20\x63\x68\x75\x6e\x6b\x2e");}const std::size_t l415=lx.l474();
const std::size_t l484=lx.l6();if(l415>lx.size()-l484){throw l3("\x4d"
"\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x65\x78\x63\x65\x65\x64\x73"
"\x20\x66\x69\x6c\x65\x20\x73\x69\x7a\x65\x2e");}const std::size_t l42
=l484+l415;std::uint64_t l212=0;std::uint8_t l40=0;std::string l215;
while(lx.l6()<l42){const std::uint32_t l442=lx.l259(l42);if(l442>std
::numeric_limits<std::uint64_t>::max()-l212){throw l3("\x4d\x49\x44"
"\x49\x20\x61\x62\x73\x6f\x6c\x75\x74\x65\x20\x74\x69\x63\x6b\x20\x6f"
"\x76\x65\x72\x66\x6c\x6f\x77\x2e");}l212+=l442;const std::uint8_t
l267=lx.l46(l42);std::uint8_t l1=l267;std::uint8_t l26=0;bool l438=
false;if(l267<0x80){if(l40==0){throw l3("\x4d\x49\x44\x49\x20\x72\x75"
"\x6e\x6e\x69\x6e\x67\x20\x73\x74\x61\x74\x75\x73\x20\x68\x61\x73\x20"
"\x6e\x6f\x20\x73\x74\x61\x74\x75\x73\x20\x62\x79\x74\x65\x2e");}l1=
l40;l26=l267;l438=true;}if(l1==0xFF){l40=0;const std::uint8_t l11=lx.
l46(l42);const std::size_t length=lx.l259(l42);const auto data=lx.
l534(length,l42);if(l11==0x03){l215.assign(data.begin(),data.end());}
else if(l11==0x51&&data.size()==3){const double l164=static_cast<
double>(data[0]) *65536.0+static_cast<double>(data[1]) *256.0+
static_cast<double>(data[2]);if(l164>0.0){l154.push_back({l212,l164,
l520++});}}continue;}if(l1==0xF0||l1==0xF7){l40=0;lx.l681(lx.l259(l42
),l42);continue;}if(l1>=0xF0){throw l3("\x55\x6e\x73\x75\x70\x70\x6f"
"\x72\x74\x65\x64\x20\x73\x79\x73\x74\x65\x6d\x20\x6d\x65\x73\x73\x61"
"\x67\x65\x20\x69\x6e\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x2e"
);}l40=l1;const std::uint8_t l11=l1&0xF0;const std::uint8_t l434=l438
?l26:lx.l46(l42);if((l434&0x80)!=0){throw l3("\x49\x6e\x76\x61\x6c"
"\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62\x79\x74\x65"
"\x2e");}switch(l11){case 0x80:case 0x90:case 0xA0:case 0xB0:{const
std::uint8_t l404=lx.l46(l42);if((l404&0x80)!=0){throw l3("\x49\x6e"
"\x76\x61\x6c\x69\x64\x20\x4d\x49\x44\x49\x20\x64\x61\x74\x61\x20\x62"
"\x79\x74\x65\x2e");}ln.push_back({l212,0.0,l1,l434,l404,l215});break
;}case 0xE0:static_cast<void>(lx.l46(l42));break;case 0xC0:case 0xD0:
break;default:throw l3("\x49\x6e\x76\x61\x6c\x69\x64\x20\x4d\x49\x44"
"\x49\x20\x63\x68\x61\x6e\x6e\x65\x6c\x20\x6d\x65\x73\x73\x61\x67\x65"
"\x2e");}}lx.l412(l42);}l500(ln,l154,static_cast<double>(l370));
return ln;}static void l500(std::vector<l4>&ln,std::vector<l132>&l154
,double l376){std::stable_sort(l154.begin(),l154.end(),[](const l132&
lw,const l132&lv){if(lw.l23!=lv.l23){return lw.l23<lv.l23;}return lw.
l433<lv.l433;});std::vector<l132>l105;for(const l132&l164:l154){if(!
l105.empty()&&l105.back().l23==l164.l23){l105.back()=l164;}else{l105.
push_back(l164);}}std::stable_sort(ln.begin(),ln.end(),[](const l4&lw
,const l4&lv){return lw.l23<lv.l23;});std::size_t l260=0;std::
uint64_t l151=0;double l205=0.0;double l340=l105.front().l385;for(l4&
lf:ln){while(l260+1<l105.size()&&l105[l260+1].l23<=lf.l23){const l132
&l355=l105[++l260];l205+=static_cast<double>(l355.l23-l151) *l340/(
1000000.0*l376);l151=l355.l23;l340=l355.l385;}lf.l141=l205+
static_cast<double>(lf.l23-l151) *l340/(1000000.0*l376);}}static std
::vector<l4>l639(const std::vector<l4>&ln){std::vector<l4>l241;for(
const l4&lf:ln){if(lf.l349()||lf.l314()||lf.l397()||lf.l417()){l241.
push_back(lf);}}std::vector<l4>l347;for(const l4&lf:l241){std::string
l84=lf.l215;std::transform(l84.begin(),l84.end(),l84.begin(),[](
unsigned char l48){return static_cast<char>(std::tolower(l48));});if(
l84.find("\x70\x69\x61\x6e\x6f")!=std::string::npos||l84.find("\x6b"
"\x65\x79\x62\x6f\x61\x72\x64")!=std::string::npos||l84.find("\x67"
"\x72\x61\x6e\x64")!=std::string::npos){l347.push_back(lf);}}if(!l347
.empty()){return l347;}std::vector<l4>l285;for(const l4&lf:l241){if(
lf.l518()==0){l285.push_back(lf);}}return l285.empty()?l241:l285;}};
class l237{public:enum class l45{l190,l253};l237()=delete;static l45
l590(const std::string&l143,std::filesystem::path lq={},std::uint32_t
ll=44100,double l263=5.0,std::stop_token stop_token={}){if(l143.empty
()){throw std::invalid_argument("\x4e\x6f\x20\x4d\x49\x44\x49\x20\x66"
"\x69\x6c\x65\x20\x70\x61\x74\x68\x20\x70\x72\x6f\x76\x69\x64\x65\x64"
"\x2e");}if(ll==0||ll>std::numeric_limits<std::uint32_t>::max()/
sizeof(float)||!std::isfinite(l263)||l263<0.0){throw std::
invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20\x57\x41\x56\x20"
"\x72\x65\x6e\x64\x65\x72\x69\x6e\x67\x20\x63\x6f\x6e\x66\x69\x67\x75"
"\x72\x61\x74\x69\x6f\x6e\x2e");}std::vector<l92::l4>ln=l92::l379(
l143);if(lq.empty()){lq=std::filesystem::path(l143);lq.
replace_extension("\x2e\x77\x61\x76");}l663(lq);const double l667=ln.
empty()?0.0:ln.back().l141;const long double l207=(static_cast<long
double>(l667)+l263) *ll;constexpr std::uint64_t l528=std::
numeric_limits<std::uint32_t>::max()-36ULL;if(!std::isfinite(l207)||
l207<0.0L||l207>l528/sizeof(float)){throw std::runtime_error("\x57"
"\x41\x56\x20\x65\x78\x63\x65\x65\x64\x73\x20\x74\x68\x65\x20\x52\x49"
"\x46\x46\x20\x33\x32\x2d\x62\x69\x74\x20\x73\x69\x7a\x65\x20\x6c\x69"
"\x6d\x69\x74\x3b\x20\x52\x46\x36\x34\x20\x69\x73\x20\x6e\x6f\x74\x20"
"\x69\x6d\x70\x6c\x65\x6d\x65\x6e\x74\x65\x64\x2e");}const std::
uint64_t l300=static_cast<std::uint64_t>(std::ceil(l207));const std::
uint64_t l235=l300*sizeof(float);std::filesystem::path l30=lq;l30+=""
"\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72\x74";if(std::filesystem::exists
(l30)){throw std::runtime_error("\x54\x65\x6d\x70\x6f\x72\x61\x72\x79"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x61\x6c\x72\x65\x61"
"\x64\x79\x20\x65\x78\x69\x73\x74\x73\x3a\x20"+l30.string());}l44();
try{std::ofstream li(l30,std::ios::binary);if(!li){throw std::
runtime_error("\x43\x61\x6e\x6e\x6f\x74\x20\x63\x72\x65\x61\x74\x65"
"\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lq.string());}
l610(li,ll,static_cast<std::uint32_t>(l235));constexpr std::size_t
l488=512;std::array<float,l488>l90{};std::uint64_t l153=0;std::size_t
l161=0;while(l153<l300){if(stop_token.stop_requested()){li.close();
l44();std::error_code l172;std::filesystem::remove(l30,l172);return
l45::l253;}while(l161<ln.size()&&l450(ln[l161],ll)<=l153){l92::l454(
ln[l161]);++l161;}std::uint64_t l193=std::min<std::uint64_t>(l300,
l153+l488);if(l161<ln.size()){l193=std::min(l193,l450(ln[l161],ll));}
if(l193==l153){continue;}const auto l428=static_cast<std::size_t>(
l193-l153);l329(l90.data(),static_cast<int>(l428),1.0);li.write(
reinterpret_cast<const char* >(l90.data()),static_cast<std::
streamsize>(l428*sizeof(float)));if(!li){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69\x6c\x65\x20\x77\x72\x69\x74"
"\x69\x6e\x67\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lq
.string());}l153=l193;}l44();li.close();if(!li){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x66\x69\x6e"
"\x61\x6c\x69\x7a\x65\x20\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x3a"
"\x20"+lq.string());}std::filesystem::rename(l30,lq);return l45::l190
;}catch(...){l44();std::error_code l172;std::filesystem::remove(l30,
l172);throw;}}private:static std::uint64_t l450(const l92::l4&lf,std
::uint32_t ll)noexcept{return static_cast<std::uint64_t>(std::llround
(lf.l141*ll));}static void l663(const std::filesystem::path&lq){if(
std::filesystem::exists(lq)){throw std::runtime_error("\x52\x65\x66"
"\x75\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f\x76\x65\x72\x77\x72\x69\x74"
"\x65\x20\x65\x78\x69\x73\x74\x69\x6e\x67\x20\x57\x41\x56\x20\x66\x69"
"\x6c\x65\x3a\x20"+lq.string());}const std::filesystem::path l262=lq.
parent_path();if(!l262.empty()&&!std::filesystem::exists(l262)){throw
std::runtime_error("\x57\x41\x56\x20\x6f\x75\x74\x70\x75\x74\x20\x64"
"\x69\x72\x65\x63\x74\x6f\x72\x79\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74"
"\x20\x65\x78\x69\x73\x74\x3a\x20"+l262.string());}}static void l238(
std::ostream&li,std::uint16_t lr){li.put(static_cast<char>(lr&0xFF));
li.put(static_cast<char>((lr>>8)&0xFF));}static void l195(std::
ostream&li,std::uint32_t lr){li.put(static_cast<char>(lr&0xFF));li.
put(static_cast<char>((lr>>8)&0xFF));li.put(static_cast<char>((lr>>16
)&0xFF));li.put(static_cast<char>((lr>>24)&0xFF));}static void l610(
std::ostream&li,std::uint32_t ll,std::uint32_t l235){constexpr std::
uint16_t l549=3;constexpr std::uint16_t l515=1;constexpr std::
uint16_t l558=32;constexpr std::uint16_t l463=sizeof(float);constexpr
std::uint32_t l387=16;const std::uint32_t l609=ll*l463;const std::
uint32_t l568=4+(8+l387)+(8+l235);li.write("\x52\x49\x46\x46",4);l195
(li,l568);li.write("\x57\x41\x56\x45\x66\x6d\x74\x20",8);l195(li,l387
);l238(li,l549);l238(li,l515);l195(li,ll);l195(li,l609);l238(li,l463);
l238(li,l558);li.write("\x64\x61\x74\x61",4);l195(li,l235);}};
#include<CoreMIDI/CoreMIDI.h>
#include<functional>
#include<memory>
#include<span>
class lj{public:using l74=std::span<const std::uint8_t>;using l368=
std::function<void(l74)>;private:struct l129{explicit l129(l368
new_handler):l89(std::move(new_handler)){}void l542(l74 lg)noexcept{{
std::lock_guard lock(mutex);if(!l332){return;}++l357;}try{l89(lg);}
catch(...){}{std::lock_guard lock(mutex);--l357;}l233.notify_all();}
void l614()noexcept{std::unique_lock lock(mutex);l332=false;l233.wait
(lock,[this]{return l357==0;});l89={};}bool l508()const noexcept{std
::lock_guard lock(mutex);return l332;}mutable std::mutex mutex;std::
condition_variable l233;l368 l89;std::size_t l357=0;bool l332=true;};
public:class l25{public:l25()noexcept=default;l25(const l25&)=delete;
l25&operator=(const l25&)=delete;l25(l25&&l240)noexcept:l82(std::
exchange(l240.l82,{})){}l25&operator=(l25&&l240)noexcept{if(this!=&
l240){reset();l82=std::exchange(l240.l82,{});}return*this;}~l25(){
reset();}void reset()noexcept{if(l82){l82->l614();l82.reset();}}
explicit operator bool()const noexcept{return static_cast<bool>(l82);
}private:friend class lj;explicit l25(std::shared_ptr<l129>l18):l82(
std::move(l18)){}std::shared_ptr<l129>l82;};static constexpr std::
uint8_t l308=64;static constexpr std::uint8_t l261=66;static constexpr
std::uint8_t l290=67;static constexpr std::uint8_t l279=68;lj()=
default;lj(const lj&)=delete;lj&operator=(const lj&)=delete;lj(lj&&)=
delete;lj&operator=(lj&&)=delete;~lj(){l55();}l25 l374(l368 l89){if(!
l89){throw std::invalid_argument("\x49\x6e\x76\x61\x6c\x69\x64\x20"
"\x4d\x49\x44\x49\x20\x6d\x65\x73\x73\x61\x67\x65\x20\x68\x61\x6e\x64"
"\x6c\x65\x72\x2e");}auto l18=std::make_shared<l129>(std::move(l89));
std::lock_guard lock(l71);l497();l122.push_back(l18);return l25(std::
move(l18));}void l68(){std::lock_guard l221(l117);{std::lock_guard
l223(l71);if(l61){return;}}const ItemCount l304=
MIDIGetNumberOfSources();if(l304==0){throw std::runtime_error("\x4e"
"\x6f\x20\x4d\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69"
"\x63\x65\x20\x66\x6f\x75\x6e\x64\x2e");}MIDIClientRef l63=0;
MIDIPortRef l56=0;std::vector<std::unique_ptr<l242>>l200;OSStatus l1=
MIDIClientCreate(CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49"
"\x6e\x70\x75\x74\x20\x48\x75\x62"),nullptr,nullptr,&l63);if(l1!=
noErr||l63==0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20"
"\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d\x49\x44"
"\x49\x20\x63\x6c\x69\x65\x6e\x74\x2e");}l1=MIDIInputPortCreate(l63,
CFSTR("\x62\x62\x70\x6c\x20\x4d\x49\x44\x49\x20\x49\x6e\x70\x75\x74"
"\x20\x50\x6f\x72\x74"),&lj::l656,nullptr,&l56);if(l1!=noErr||l56==0){
MIDIClientDispose(l63);throw std::runtime_error("\x46\x61\x69\x6c\x65"
"\x64\x20\x74\x6f\x20\x63\x72\x65\x61\x74\x65\x20\x43\x6f\x72\x65\x4d"
"\x49\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x70\x6f\x72\x74\x2e");}l200
.reserve(static_cast<std::size_t>(l304));for(ItemCount le=0;le<l304;
++le){const MIDIEndpointRef l118=MIDIGetSource(le);if(l118==0){
continue;}auto l32=std::make_unique<l242>();l32->l121=this;l32->l118=
l118;l1=MIDIPortConnectSource(l56,l118,l32.get());if(l1==noErr){l200.
push_back(std::move(l32));}}if(l200.empty()){MIDIPortDispose(l56);
MIDIClientDispose(l63);throw std::runtime_error("\x4e\x6f\x20\x4d\x49"
"\x44\x49\x20\x69\x6e\x70\x75\x74\x20\x64\x65\x76\x69\x63\x65\x20\x63"
"\x6f\x75\x6c\x64\x20\x62\x65\x20\x6f\x70\x65\x6e\x65\x64\x2e");}{std
::lock_guard l223(l71);l348=l63;l302=l56;l361=std::move(l200);l274=
true;l61=true;}}void l55()noexcept{std::lock_guard l221(l117);
MIDIClientRef l63=0;MIDIPortRef l56=0;{std::lock_guard l223(l71);if(!
l61){return;}l274=false;l61=false;l63=l348;l56=l302;}for(const auto&
l32:l361){if(l56!=0&&l32&&l32->l118!=0){MIDIPortDisconnectSource(l56,
l32->l118);}}if(l56!=0){MIDIPortDispose(l56);}if(l63!=0){
MIDIClientDispose(l63);}{std::unique_lock l223(l71);l441.wait(l223,[
this]{return l334==0;});l348=0;l302=0;l361.clear();}}bool l578()const
noexcept{std::lock_guard lock(l71);return l61;}private:struct l181{
std::array<std::uint8_t,3>l21{};std::size_t size=0;};struct l445{std
::mutex mutex;std::uint8_t l40=0;std::uint8_t l115=0;std::array<std::
uint8_t,2>data{};std::size_t l148=0;std::size_t l145=0;bool l328=
false;};struct l242{lj*l121=nullptr;MIDIEndpointRef l118=0;l445 lm;};
class l337{public:explicit l337(lj&l121)noexcept:l421(&l121),l131(
l121.l603()){}~l337(){if(l131){l421->l602();}}explicit operator bool(
)const noexcept{return l131;}private:lj*l421;bool l131;};static void
l656(const MIDIPacketList*l306,void* ,void*l680)noexcept{auto*l32=
static_cast<l242* >(l680);if(l306==nullptr||l32==nullptr||l32->l121==
nullptr){return;}l337 l162( *l32->l121);if(!l162){return;}const
MIDIPacket*packet=&l306->packet[0];for(UInt32 le=0;le<l306->
numPackets;++le){l32->l121->l585(l32->lm,packet->data,packet->length);
packet=MIDIPacketNext(packet);}}bool l603()noexcept{std::lock_guard
lock(l71);if(!l274){return false;}++l334;return true;}void l602()noexcept
{{std::lock_guard lock(l71);--l334;}l441.notify_all();}static std::
size_t l409(std::uint8_t l1)noexcept{switch(l1&0xF0){case 0x80:case
0x90:case 0xA0:case 0xB0:case 0xE0:return 2;case 0xC0:case 0xD0:
return 1;default:break;}switch(l1){case 0xF1:case 0xF3:return 1;case
0xF2:return 2;case 0xF6:return 0;default:return 0;}}void l585(l445&lm
,const Byte*data,UInt16 length)noexcept{std::vector<l181>l186;l186.
reserve(static_cast<std::size_t>(length)/2+1);{std::lock_guard l690(
lm.mutex);for(UInt16 le=0;le<length;++le){const std::uint8_t l27=
static_cast<std::uint8_t>(data[le]);if(l27>=0xF8){l181 lg;lg.l21[0]=
l27;lg.size=1;l186.push_back(lg);continue;}if(lm.l328){if(l27==0xF7){
lm.l328=false;}continue;}if((l27&0x80)!=0){lm.l145=0;lm.l115=0;if(l27
==0xF0){lm.l328=true;lm.l40=0;continue;}if(l27==0xF7){lm.l40=0;
continue;}lm.l115=l27;lm.l148=l409(l27);if(l27<0xF0){lm.l40=l27;}else
{lm.l40=0;}if(lm.l148==0){l181 lg;lg.l21[0]=l27;lg.size=1;l186.
push_back(lg);lm.l115=0;}continue;}if(lm.l115==0){if(lm.l40==0){
continue;}lm.l115=lm.l40;lm.l148=l409(lm.l115);lm.l145=0;}if(lm.l145<
lm.data.size()){lm.data[lm.l145++]=l27;}if(lm.l145==lm.l148){l181 lg;
lg.l21[0]=lm.l115;for(std::size_t l124=0;l124<lm.l148;++l124){lg.l21[
l124+1]=lm.data[l124];}lg.size=lm.l148+1;l186.push_back(lg);lm.l115=0
;lm.l145=0;}}}for(const l181&lg:l186){l541(l74(lg.l21.data(),lg.size));
}}void l541(l74 lg)noexcept{std::vector<std::shared_ptr<l129>>l276;{
std::lock_guard lock(l71);l276.reserve(l122.size());for(const auto&
l331:l122){if(auto l18=l331.lock()){l276.push_back(std::move(l18));}}
}for(const auto&l89:l276){l89->l542(lg);}}void l497(){l122.erase(std
::remove_if(l122.begin(),l122.end(),[](const std::weak_ptr<l129>&l331
){const auto l18=l331.lock();return!l18||!l18->l508();}),l122.end());
}mutable std::mutex l117;mutable std::mutex l71;std::
condition_variable l441;bool l61=false;bool l274=false;std::size_t
l334=0;MIDIClientRef l348=0;MIDIPortRef l302=0;std::vector<std::
unique_ptr<l242>>l361;std::vector<std::weak_ptr<l129>>l122;};class l59
{public:explicit l59(lj&l37)noexcept:l176(l37){}l59(const l59&)=
delete;l59(l59&&)=delete;l59&operator=(l59&&)=delete;~l59(){l55();}
void l68(){if(l19){return;}l19.emplace(l176.l374([](lj::l74 lg){l522(
lg);}));}void l55()noexcept{if(!l19){return;}l19->reset();l19.reset();
l503();}bool l578()const noexcept{return l19.has_value();}private:
static bool l41(std::uint8_t lo)noexcept{return lo>=21&&lo<=108;}
static void l522(lj::l74 lg){if(lg.empty()){return;}const std::
uint8_t l11=lg[0]&0xF0;if(l11==0x80||l11==0x90||l11==0xA0){if(lg.size
()<3||!l41(lg[1])){return;}const int lo=static_cast<int>(lg[1]);const
double l159=static_cast<double>(lg[2]);if(l11==0x80||(l11==0x90&&lg[2
]==0)){l76(lo,l159);}else if(l11==0x90){l85(lo,l159);}else{l166(lo,
std::clamp(l159/127.0,0.0,1.0));}return;}if(l11!=0xB0||lg.size()<3){
return;}const double l7=std::clamp(static_cast<double>(lg[2])/127.0,
0.0,1.0);switch(lg[1]){case lj::l308:l53(l7);break;case lj::l261:l93(
l7);break;case lj::l290:l99(l7);break;case lj::l279:l100(l7);break;
default:break;}}static void l503()noexcept{l99(0.0);l100(0.0);l93(0.0
);l53(0.0);l44();}lj&l176;std::optional<lj::l25>l19;};
#include<sys/event.h>
#include<sys/time.h>
#include<termios.h>
#include<cctype>
#include<cerrno>
#include<string_view>
class l62{public:l62()=default;l62(const l62&)=delete;l62&operator=(
const l62&)=delete;l62(l62&&)=delete;l62&operator=(l62&&)=delete;void
l702(std::chrono::milliseconds duration)noexcept{const auto l505=std
::clamp(duration.count(),20LL,2000LL);l493.store(static_cast<int>(
l505));}void l512(std::stop_token stop_token={}){l346 l694;l327 l174;
l545(l174.get());std::stop_callback l491(stop_token,[l620=l174.get()]
{struct kevent lf{};EV_SET(&lf,l342,EVFILT_USER,0,NOTE_TRIGGER,0,
nullptr);static_cast<void>(kevent(l620,&lf,1,nullptr,0,nullptr));});
l636();try{while(!stop_token.stop_requested()){l630();const timespec
l685=l564();struct kevent lf{};const int count=kevent(l174.get(),
nullptr,0,&lf,1,&l685);if(count<0){if(errno==EINTR){continue;}throw
std::runtime_error("\x6b\x71\x75\x65\x75\x65\x20\x77\x61\x69\x74\x20"
"\x66\x61\x69\x6c\x65\x64\x2e");}if(count==0){continue;}if(lf.filter
==EVFILT_USER&&lf.ident==l342){break;}if(lf.filter==EVFILT_READ&&lf.
ident==STDIN_FILENO&&!l643()){break;}}}catch(...){l410();l224();throw
;}l410();l224();}private:using l57=std::chrono::steady_clock;static
constexpr uintptr_t l342=1;static constexpr int l608=24;static
constexpr int l651=108;static constexpr int l513=10;class l346{public
:l346(){if(!isatty(STDIN_FILENO)){throw std::runtime_error("\x50\x43"
"\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20\x72\x65\x71\x75\x69\x72\x65"
"\x73\x20\x61\x6e\x20\x69\x6e\x74\x65\x72\x61\x63\x74\x69\x76\x65\x20"
"\x74\x65\x72\x6d\x69\x6e\x61\x6c\x2e");}if(tcgetattr(STDIN_FILENO,&
l362)!=0){throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74"
"\x6f\x20\x72\x65\x61\x64\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d"
"\x6f\x64\x65\x2e");}termios l247=l362;l247.c_lflag&=static_cast<
tcflag_t>(~(ICANON|ECHO));l247.c_cc[VMIN]=1;l247.c_cc[VTIME]=0;if(
tcsetattr(STDIN_FILENO,TCSANOW,&l247)!=0){throw std::runtime_error(""
"\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x65\x6e\x74\x65\x72\x20\x72"
"\x61\x77\x20\x74\x65\x72\x6d\x69\x6e\x61\x6c\x20\x6d\x6f\x64\x65\x2e"
);}l496=true;}~l346(){if(l496){static_cast<void>(tcsetattr(
STDIN_FILENO,TCSANOW,&l362));}}private:termios l362{};bool l496=false
;};class l327{public:l327():l252(kqueue()){if(l252<0){throw std::
runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x63\x72\x65"
"\x61\x74\x65\x20\x6b\x71\x75\x65\x75\x65\x2e");}}~l327(){close(l252);
}int get()const noexcept{return l252;}private:int l252;};struct l168{
int lo=0;l57::time_point l352{};bool l108=false;};struct l487{int l124
=-1;double l65=0.0;};static void l545(int l174){std::array<struct
kevent,2>l257{};EV_SET(&l257[0],STDIN_FILENO,EVFILT_READ,EV_ADD|
EV_ENABLE,0,0,nullptr);EV_SET(&l257[1],l342,EVFILT_USER,EV_ADD|
EV_CLEAR,0,0,nullptr);if(kevent(l174,l257.data(),static_cast<int>(
l257.size()),nullptr,0,nullptr)!=0){throw std::runtime_error("\x46"
"\x61\x69\x6c\x65\x64\x20\x74\x6f\x20\x72\x65\x67\x69\x73\x74\x65\x72"
"\x20\x6b\x71\x75\x65\x75\x65\x20\x65\x76\x65\x6e\x74\x73\x2e");}}
bool l643(){std::array<char,64>l307{};const ssize_t count=read(
STDIN_FILENO,l307.data(),l307.size());if(count==0){return false;}if(
count<0){return errno==EINTR||errno==EAGAIN;}for(ssize_t le=0;le<
count;++le){l574(l307[static_cast<std::size_t>(le)]);}return true;}
void l574(char l48){if(l48>='1'&&l48<='9'){l291=l608+(static_cast<int
>(l48-'1') *l513);l618("\x50\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64"
"\x20\x62\x61\x73\x65\x20\x4d\x49\x44\x49\x20\x6e\x6f\x74\x65\x3a\x20"
,l291);return;}if(l530(l48)){return;}const l487 l325=l519(l48);const
int lo=l291+l325.l124;if(l325.l124<0||lo<21||lo>l651){return;}l646(lo
,l325.l65);}void l646(int lo,double l65){l168*l125=nullptr;for(l168&
l14:l232){if(l14.l108&&l14.lo==lo){l125=&l14;break;}if(!l14.l108&&
l125==nullptr){l125=&l14;}}if(l125==nullptr){return;}l85(lo,l65);l586
("\x4e\x6f\x74\x65\x4f\x6e\x3a\x20\x4d\x49\x44\x49\x20",lo,"\x2c\x20"
"\x76\x65\x6c\x6f\x63\x69\x74\x79\x20",l65);l125->lo=lo;l125->l352=
l57::now()+std::chrono::milliseconds(l493.load());l125->l108=true;}
void l630(){const auto now=l57::now();for(l168&l14:l232){if(l14.l108
&&l14.l352<=now){l76(l14.lo,0.0);l14.l108=false;}}}void l410()noexcept
{for(l168&l14:l232){if(l14.l108){l76(l14.lo,0.0);l14.l108=false;}}}
timespec l564()const noexcept{auto l251=l57::time_point::max();for(
const l168&l14:l232){if(l14.l108){l251=std::min(l251,l14.l352);}}if(
l251==l57::time_point::max()){return timespec{3600,0};}const auto l650
=std::max(l57::duration::zero(),l251-l57::now());const auto
nanoseconds=std::chrono::duration_cast<std::chrono::nanoseconds>(l650
).count();return timespec{static_cast<time_t>(nanoseconds/
1000000000LL),static_cast<long>(nanoseconds%1000000000LL)};}static
l487 l519(char l48)noexcept{const char l350=static_cast<char>(std::
tolower(static_cast<unsigned char>(l48)));constexpr std::string_view
l619="\x71\x77\x65\x72\x74\x79\x75\x69\x6f\x70";constexpr std::
string_view l638="\x61\x73\x64\x66\x67\x68\x6a\x6b\x6c\x3b";constexpr
std::string_view l544="\x7a\x78\x63\x76\x62\x6e\x6d\x2c\x2e\x2f";if(
const auto le=l619.find(l350);le!=std::string_view::npos){return{
static_cast<int>(le),112.0};}if(const auto le=l638.find(l350);le!=std
::string_view::npos){return{static_cast<int>(le),72.0};}if(const auto
le=l544.find(l350);le!=std::string_view::npos){return{static_cast<int
>(le),40.0};}return{};}bool l530(char l48){switch(l48){case'-':l225=!
l225;if(l225){l99(1.0);l86("\x50\x65\x64\x61\x6c\x50\x72\x65\x73\x73"
"\x65\x64\x3a\x20\x53\x6f\x66\x74\x20\x70\x65\x64\x61\x6c\x20\x28\x75"
"\x6e\x61\x20\x63\x6f\x72\x64\x61\x29\x20\x68\x61\x73\x20\x62\x65\x65"
"\x6e\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l99(0.0);l86("\x50"
"\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x6f\x66"
"\x74\x20\x70\x65\x64\x61\x6c\x20\x28\x75\x6e\x61\x20\x63\x6f\x72\x64"
"\x61\x29\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61"
"\x73\x65\x64\x2e");}return true;case'=':l248=!l248;if(l248){l100(1.0
);l86("\x50\x65\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x48"
"\x61\x72\x6d\x6f\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73"
"\x20\x62\x65\x65\x6e\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{
l100(0.0);l86("\x50\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64"
"\x3a\x20\x48\x61\x72\x6d\x6f\x6e\x69\x63\x20\x70\x65\x64\x61\x6c\x20"
"\x68\x61\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64"
"\x2e");}return true;case'[':l213=!l213;if(l213){l93(1.0);l86("\x50"
"\x65\x64\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x6f\x73\x74"
"\x65\x6e\x75\x74\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62"
"\x65\x65\x6e\x20\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l93(0.0);
l86("\x50\x65\x64\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53"
"\x6f\x73\x74\x65\x6e\x75\x74\x6f\x20\x70\x65\x64\x61\x6c\x20\x68\x61"
"\x73\x20\x62\x65\x65\x6e\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}
return true;case']':l256=!l256;if(l256){l53(1.0);l86("\x50\x65\x64"
"\x61\x6c\x50\x72\x65\x73\x73\x65\x64\x3a\x20\x53\x75\x73\x74\x61\x69"
"\x6e\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e\x20"
"\x70\x72\x65\x73\x73\x65\x64\x2e");}else{l53(0.0);l86("\x50\x65\x64"
"\x61\x6c\x52\x65\x6c\x65\x61\x73\x65\x64\x3a\x20\x53\x75\x73\x74\x61"
"\x69\x6e\x20\x70\x65\x64\x61\x6c\x20\x68\x61\x73\x20\x62\x65\x65\x6e"
"\x20\x72\x65\x6c\x65\x61\x73\x65\x64\x2e");}return true;return true;
default:return false;}}void l224()noexcept{l225=false;l248=false;l213
=false;l256=false;l99(0.0);l100(0.0);l93(0.0);l53(0.0);}static void
l636(){std::cout<<"\x50\x43\x20\x6b\x65\x79\x62\x6f\x61\x72\x64\x20"
"\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65"
"\x78\x69\x74\x73\x29\x2e\n"<<"\x20\x20\x51\x2d\x50\x20\x2f\x20\x41"
"\x2d\x3b\x20\x2f\x20\x5a\x2d\x2f\x20\x3a\x20\x76\x65\x6c\x6f\x63\x69"
"\x74\x79\x20\x31\x31\x32\x20\x2f\x20\x37\x32\x20\x2f\x20\x34\x30\n"
<<"\x20\x20\x31\x2d\x39\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x3a\x20\x73\x65\x6c\x65\x63\x74\x20\x70\x69\x74\x63"
"\x68\x20\x62\x61\x6e\x6b\n"<<"\x20\x20\x2d\x20\x3d\x20\x5b\x20\x5d"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x3a\x20\x73\x6f\x66\x74"
"\x20\x2f\x20\x68\x61\x72\x6d\x6f\x6e\x69\x63\x20\x2f\x20\x73\x6f\x73"
"\x74\x65\x6e\x75\x74\x6f\x20\x2f\x20\x73\x75\x73\x74\x61\x69\x6e\n";
}inline void l86(std::string_view lg){std::cout<<"\r\x1b\x5b\x32\x4b"
<<lg<<std::flush;}inline void l618(std::string_view lg,double lr){std
::cout<<"\r\x1b\x5b\x32\x4b"<<lg<<lr<<std::flush;}inline void l586(
std::string_view l653,double l525,std::string_view l652,double l524){
std::cout<<"\r\x1b\x5b\x32\x4b"<<l653<<l525<<l652<<l524<<std::flush;}
std::atomic<int>l493{650};int l291=60;std::array<l168,30>l232{};bool
l225=false;bool l248=false;bool l213=false;bool l256=false;};
#include<iomanip>
#include<sstream>
class l49{public:struct l70{std::uint64_t l182=0;std::array<std::
uint8_t,3>lg{};std::uint8_t l266=0;};explicit l49(lj&l37)noexcept:
l176(l37){}l49(const l49&)=delete;l49&operator=(const l49&)=delete;
l49(l49&&)=delete;l49&operator=(l49&&)=delete;~l49(){l631();}void l68
(std::filesystem::path lq={}){std::lock_guard l221(l117);if(l19){
return;}{std::lock_guard l315(l175);l152.clear();l152.reserve(4096);
l209=std::move(lq);l386=l57::now();}l19.emplace(l176.l374([this](lj::
l74 lg){l560(lg);}));}std::filesystem::path l55(){std::lock_guard l221
(l117);const bool l561=l19.has_value();if(l19){l19->reset();l19.reset
();}std::vector<l70>l220;std::filesystem::path l278;{std::lock_guard
l315(l175);l220=l152;l278=l209;}if(!l561&&l220.empty()){return l470;}
const std::filesystem::path l345=l278.empty()?std::filesystem::
current_path()/l592(l220):l278;l587(l345,l220);{std::lock_guard l315(
l175);l152.clear();l209.clear();l470=l345;}return l345;}void l631()noexcept
{std::lock_guard l221(l117);if(l19){l19->reset();l19.reset();}std::
lock_guard l315(l175);l152.clear();l209.clear();}bool l704()const
noexcept{std::lock_guard lock(l117);return l19.has_value();}private:
using l57=std::chrono::steady_clock;static constexpr std::uint16_t
l396=480;static constexpr std::uint32_t l596=500000;void l560(lj::l74
lg){if(!l674(lg)){return;}l70 lf;lf.l182=static_cast<std::uint64_t>(
std::chrono::duration_cast<std::chrono::microseconds>(l57::now()-l386
).count());lf.l266=static_cast<std::uint8_t>(lg.size());std::copy(lg.
begin(),lg.end(),lf.lg.begin());std::lock_guard lock(l175);l152.
push_back(std::move(lf));}static bool l674(lj::l74 lg)noexcept{if(lg.
empty()||lg[0]>=0xF0){return false;}switch(lg[0]&0xF0){case 0x80:case
0x90:case 0xA0:case 0xB0:case 0xE0:return lg.size()==3;case 0xC0:case
0xD0:return lg.size()==2;default:return false;}}static std::string
l592(const std::vector<l70>&ln){const std::time_t now=std::time(
nullptr);std::tm l437{};localtime_r(&now,&l437);std::size_t l459=0;
std::size_t l472=0;for(const l70&lf:ln){if(lf.l266<3){continue;}const
std::uint8_t l11=lf.lg[0]&0xF0;if(l11==0x90&&lf.lg[2]!=0){++l459;}
else if(l11==0xB0&&l584(lf.lg[1])){++l472;}}const std::uint64_t l205=
ln.empty()?0:ln.back().l182/1000000ULL;std::ostringstream l84;l84<<
std::put_time(&l437,"\x25\x59\x2d\x25\x6d\x2d\x25\x64\x5f\x25\x48\x2d"
"\x25\x4d\x2d\x25\x53")<<"\x5f"<<l459<<"\x2d\x6e\x6f\x74\x65\x73\x5f"
<<l472<<"\x2d\x70\x65\x64\x61\x6c\x73\x5f"<<l205<<"\x2d\x73\x65\x63"
"\x6f\x6e\x64\x73\x2e\x6d\x69\x64";return l84.str();}static bool l584
(std::uint8_t l114)noexcept{return l114==lj::l308||l114==lj::l261||
l114==lj::l290||l114==lj::l279;}static void l483(std::vector<std::
uint8_t>&li,std::uint32_t lr){std::uint8_t l21[5]{};int le=4;l21[le]=
static_cast<std::uint8_t>(lr&0x7F);while((lr>>=7)!=0&&le>0){l21[--le]
=static_cast<std::uint8_t>((lr&0x7F)|0x80);}for(;le<5;++le){li.
push_back(l21[le]);}}static std::uint32_t l563(std::uint64_t
microseconds)noexcept{const long double l393=static_cast<long double>
(microseconds) *l396/l596;return l393>=std::numeric_limits<std::
uint32_t>::max()?std::numeric_limits<std::uint32_t>::max():
static_cast<std::uint32_t>(l393);}static void l641(std::vector<std::
uint8_t>&l9){const std::uint8_t l657[]={lj::l308,lj::l261,lj::l290,lj
::l279};for(std::uint8_t l114:l657){l483(l9,0);l9.insert(l9.end(),{
0xB0,l114,0});}}static void l323(std::ostream&li,std::uint16_t lr){li
.put(static_cast<char>((lr>>8)&0xFF));li.put(static_cast<char>(lr&
0xFF));}static void l373(std::ostream&li,std::uint32_t lr){li.put(
static_cast<char>((lr>>24)&0xFF));li.put(static_cast<char>((lr>>16)&
0xFF));li.put(static_cast<char>((lr>>8)&0xFF));li.put(static_cast<
char>(lr&0xFF));}static void l587(const std::filesystem::path&lq,std
::vector<l70>ln){std::stable_sort(ln.begin(),ln.end(),[](const l70&lw
,const l70&lv){return lw.l182<lv.l182;});std::vector<std::uint8_t>l9;
l9.reserve(ln.size() *4+32);l9.insert(l9.end(),{0x00,0xFF,0x51,0x03,
0x07,0xA1,0x20});std::uint32_t l151=0;for(const l70&lf:ln){const std
::uint32_t l23=l563(lf.l182);l483(l9,l23-l151);l151=l23;l9.insert(l9.
end(),lf.lg.begin(),lf.lg.begin()+lf.l266);}l641(l9);l9.insert(l9.end
(),{0x00,0xFF,0x2F,0x00});if(l9.size()>std::numeric_limits<std::
uint32_t>::max()){throw std::runtime_error("\x52\x65\x63\x6f\x72\x64"
"\x65\x64\x20\x4d\x49\x44\x49\x20\x74\x72\x61\x63\x6b\x20\x69\x73\x20"
"\x74\x6f\x6f\x20\x6c\x61\x72\x67\x65\x2e");}if(!lq.parent_path().
empty()&&!std::filesystem::exists(lq.parent_path())){throw std::
runtime_error("\x4d\x49\x44\x49\x20\x6f\x75\x74\x70\x75\x74\x20\x64"
"\x69\x72\x65\x63\x74\x6f\x72\x79\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74"
"\x20\x65\x78\x69\x73\x74\x3a\x20"+lq.parent_path().string());}if(std
::filesystem::exists(lq)){throw std::runtime_error("\x52\x65\x66\x75"
"\x73\x69\x6e\x67\x20\x74\x6f\x20\x6f\x76\x65\x72\x77\x72\x69\x74\x65"
"\x20\x65\x78\x69\x73\x74\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x66\x69"
"\x6c\x65\x3a\x20"+lq.string());}std::filesystem::path l30=lq;l30+=""
"\x2e\x62\x62\x70\x6c\x2d\x70\x61\x72\x74";if(std::filesystem::exists
(l30)){throw std::runtime_error("\x54\x65\x6d\x70\x6f\x72\x61\x72\x79"
"\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70\x75\x74\x20\x61\x6c\x72\x65"
"\x61\x64\x79\x20\x65\x78\x69\x73\x74\x73\x3a\x20"+l30.string());}try
{std::ofstream li(l30,std::ios::binary);if(!li){throw std::
runtime_error("\x43\x61\x6e\x6e\x6f\x74\x20\x63\x72\x65\x61\x74\x65"
"\x20\x4d\x49\x44\x49\x20\x6f\x75\x74\x70\x75\x74\x3a\x20"+lq.string(
));}li.write("\x4d\x54\x68\x64",4);l373(li,6);l323(li,0);l323(li,1);
l323(li,l396);li.write("\x4d\x54\x72\x6b",4);l373(li,static_cast<std
::uint32_t>(l9.size()));li.write(reinterpret_cast<const char* >(l9.
data()),static_cast<std::streamsize>(l9.size()));li.close();if(!li){
throw std::runtime_error("\x46\x61\x69\x6c\x65\x64\x20\x77\x68\x69"
"\x6c\x65\x20\x77\x72\x69\x74\x69\x6e\x67\x20\x4d\x49\x44\x49\x20\x6f"
"\x75\x74\x70\x75\x74\x3a\x20"+lq.string());}std::filesystem::rename(
l30,lq);}catch(...){std::error_code l172;std::filesystem::remove(l30,
l172);throw;}}lj&l176;mutable std::mutex l117;mutable std::mutex l175
;std::optional<lj::l25>l19;std::vector<l70>l152;std::filesystem::path
l209;std::filesystem::path l470;l57::time_point l386{};};namespace{
void l244(std::stop_token stop_token){std::mutex mutex;std::
condition_variable_any l233;std::unique_lock lock(mutex);l233.wait(
lock,stop_token,[]{return false;});}}int l405(int l136,char*l127[],
const char*l28,const char*l50){return l392(l136,l127,l28,l50);}void
l490(const std::string&l158,std::stop_token stop_token){std::cout<<""
"\x50\x6c\x61\x79\x69\x6e\x67\x20\x4d\x49\x44\x49\x3a\x20"<<l158<<
'\n';const l92::l45 l165=l92::l635(1.0,0.0,l158,stop_token);if(l165==
l92::l45::l190){std::cout<<"\x4d\x49\x44\x49\x20\x70\x6c\x61\x79\x62"
"\x61\x63\x6b\x20\x63\x6f\x6d\x70\x6c\x65\x74\x65\x64\x2e\n";}}void
l447(std::stop_token stop_token){lj l37;l59 l51(l37);l51.l68();l37.
l68();std::cout<<"\x4d\x49\x44\x49\x20\x70\x69\x61\x6e\x6f\x20\x73"
"\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65\x78"
"\x69\x74\x73\x29\x2e\n";l244(stop_token);l37.l55();l51.l55();}void
l411(std::stop_token stop_token){l62 l51;l51.l512(stop_token);}void
l380(std::string l158,std::stop_token stop_token){std::filesystem::
path lq(l158);lq.replace_extension("\x2e\x77\x61\x76");const l237::
l45 l165=l237::l590(l158,lq,static_cast<std::uint32_t>(ll),5.0,
stop_token);if(l165==l237::l45::l190){std::cout<<"\x45\x78\x70\x6f"
"\x72\x74\x65\x64\x20\x57\x41\x56\x3a\x20"<<lq<<'\n';}}void l467(std
::stop_token stop_token){lj l37;l59 l51(l37);l49 l480(l37);l51.l68();
l480.l68();l37.l68();std::cout<<"\x4d\x49\x44\x49\x20\x72\x65\x63\x6f"
"\x72\x64\x69\x6e\x67\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74"
"\x72\x6c\x2d\x43\x20\x73\x61\x76\x65\x73\x20\x61\x6e\x64\x20\x65\x78"
"\x69\x74\x73\x29\x2e\n";l244(stop_token);l37.l55();const std::
filesystem::path lq=l480.l55();l51.l55();std::cout<<"\x52\x65\x63\x6f"
"\x72\x64\x65\x64\x20\x4d\x49\x44\x49\x3a\x20"<<lq<<'\n';}void l422(
std::stop_token stop_token){std::cout<<"\x54\x65\x73\x74\x20\x73\x65"
"\x72\x76\x69\x63\x65\x20\x73\x74\x61\x72\x74\x65\x64\x20\x28\x43\x74"
"\x72\x6c\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e\n";l244(stop_token);
}void l464(std::stop_token stop_token){std::cout<<"\x49\x6e\x74\x65"
"\x72\x6e\x61\x6c\x20\x74\x65\x73\x74\x20\x73\x74\x61\x72\x74\x65\x64"
"\x20\x28\x43\x74\x72\x6c\x2d\x43\x20\x65\x78\x69\x74\x73\x29\x2e\n";
l53(0.6);l85(69,128.0);l244(stop_token);l44();}
class l440{float l81=0.0;float l94=0.0;float l91=0.0;public:l440(
double l477){l81=(1-l477)/(1+l477);}inline void l189(float&lk){float
l52=static_cast<float>(l81*lk)+static_cast<float>(l94)-static_cast<
float>(l81*l91);l94=lk;l91=l52;lk=l52;}inline void l13(){l94=0.0;l91=
0.0;}};
class l577{double l330=0.0;double l335=0.0;public:inline void l189(
float&lk){constexpr float l538=0.020;constexpr float l567=0.25;
constexpr float l617=0.38;const float l550=1.0-l538;const float l452=
l617*l567;const float l670=1.0-l452;constexpr float l138=
0.292893218813;constexpr float l140=0.585786437627;constexpr float
l133=0.292893218813;constexpr float l81=0.000000000000;constexpr float
l178=0.171572875254;const float l52=l138*lk+l330;l330=l140*lk-l81*l52
+l335;l335=l133*lk-l178*l52;lk=l550* (l670*lk+l452*l52);}inline void
l13()noexcept{l330=0.0;l335=0.0;}};
class l429{public:int l142=69;const lt::l319::l367::ld*lp=nullptr;
static constexpr double l339=1e-3;static constexpr double l312=1e-3;
double l187=0.0;double l80=0.0;double l106=0.0;double l83=0.0;double
l170=0.0;double l163=0.0;bool l171=false;public:l429(double ll,int lh
,const l54*l43):l187(1.0/ll),l142(lh),lp(l43->lp.l645(l142)){}inline
double l649(double l135){if(!l171)return 0.0;double l113=l642(l135);
if(l83<0||l163<0.0){l163+=l83*l187;if(l163>0.0){l163=0.0;}}if(l163<lp
->l687){l13();return 0.0;}l80=l80+(l170-l135) *l187;l106=l106+(l83-
l170) *l187;if(l113>0.0)l83-=(l113/lp->l625) *l187;return l113;}
inline void l359(double l504){if(l171)return;l13();l83=l504;l171=true
;}inline void l13(){l80=0.0;l106=0.0;l83=0.0;l170=0.0;l163=0.0;l171=
false;}private:inline double l67(double lk,double l169){return lk>=0?
std::pow(lk,l169):-std::pow(-lk,l169);}inline double l236(double lk,
double l169){if(lk>l339)lk=l339;return l67(lk,l169);}inline double
l269(double lk,double l169){if(lk>l312)lk=l312;return l67(lk,l169);}
inline double l642(double l135){double l249=20.0;double l231=-20.0;
double l33=(l249+l231)/2.0;for(int lb=0;lb<20;lb++){double l684=lp->
l254*l236(l80,lp->l250)+lp->l202*l67(l33-l135,lp->l204);double l682=
lp->l365*l269(l106,lp->l316)+lp->l305*l67(l83-l33,lp->l356);if(l684-
l682>0){l249=l33;}else{l231=l33;}l33=(l249+l231)/2.0;}l170=l33;if(
l180(std::abs(l33),20.0f))std::cout<<"\xe4\xba\x8c\xe5\x88\x86\xe6"
"\xb3\x95\xe6\x97\xa0\xe8\xa7\xa3\n";if(l180(l33,l249)&&l180(l80,l339
)){std::cout<<"\xe4\xb8\x8a\xe5\xb1\x82\xe5\xb7\xb2\xe7\xbb\x8f\xe5"
"\x8e\x8b\xe5\x88\xb0\xe6\x9c\x80\xe5\xa4\xa7\xe5\x8e\x8b\xe7\xbc\xa9"
"\xe9\x87\x8f\n";return lp->l365*l269(l106,lp->l316)+lp->l305*l67(l83
-l33,lp->l356)-(lp->l254*l236(l80,lp->l250)+lp->l202*l67(l33-l135,lp
->l204));}else if(l180(l33,l231)&&l180(l106,l312)){std::cout<<"\xe4"
"\xb8\x8b\xe5\xb1\x82\xe5\xb7\xb2\xe7\xbb\x8f\xe5\x8e\x8b\xe5\x88\xb0"
"\xe6\x9c\x80\xe5\xa4\xa7\xe5\x8e\x8b\xe7\xbc\xa9\xe9\x87\x8f\n";
return lp->l254*l236(l80,lp->l250)+lp->l202*l67(l33-l135,lp->l204)-(
lp->l365*l269(l106,lp->l316)+lp->l305*l67(l83-l33,lp->l356));}else{
return lp->l254*l236(l80,lp->l250)+lp->l202*l67(l33-l135,lp->l204);}}
inline bool l180(float lk,float l52){return std::abs(lk-l52)<1e-5;}};
class l403{public:inline static constexpr int l214=21;inline static
constexpr int l489=108;inline static constexpr int l185=4;struct la{
double l138=0.0;double l140=0.0;double l133=0.0;double l81=0.0;double
l178=0.0;};struct lc{int l0;double l692;double l723;double l703;
double l697;int l272;std::array<la,l185>l468;};lc l234;std::array<
float,l185>l94{};std::array<float,l185>l320{};std::array<float,l185>
l91{};std::array<float,l185>l322{};l403(int lh){const int l310=std::
clamp(lh,l214,l489);l234=l407[static_cast<std::size_t>(l310-l214)];}
inline constexpr const lc&l457(int l0){const int l310=std::clamp(l0,
l214,l489);return l407[static_cast<std::size_t>(l310-l214)];}inline
constexpr const lc&l457(int l0,int){return l457(l0);}inline void l189
(float&lk){const std::size_t l272=std::min<std::size_t>(static_cast<
std::size_t>(l234.l272),l234.l468.size());for(std::size_t lb=0;lb<
l272;++lb){const auto&l194=l234.l468[lb];const float l138=static_cast
<float>(l194.l138);const float l140=static_cast<float>(l194.l140);
const float l133=static_cast<float>(l194.l133);const float l81=
static_cast<float>(l194.l81);const float l178=static_cast<float>(l194
.l178);const float l52=l138*lk+l140*l94[lb]+l133*l320[lb]-l81*l91[lb]
-l178*l322[lb];l320[lb]=l94[lb];l94[lb]=lk;l322[lb]=l91[lb];l91[lb]=
l52;lk=l52;}}inline void l13(){l94.fill(0.0f);l320.fill(0.0f);l91.
fill(0.0f);l322.fill(0.0f);}inline static constexpr std::array<lc,88>
l407={{lc{21,27.500752747255206,5.4746004184965221e-05,
0.99990087000431871,0.99860572014366777,1,{{la{0.85524455404366084,-
1.4574578798127882,0.62093426352466863,-1.724548608074796,
0.74352366709610784},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},
la{1.0,0.0,0.0,0.0,0.0}}}},lc{22,29.136014703845198,
5.3517288776830332e-05,0.93504707674792753,0.93382238178436761,1,{{la
{0.85770692127969217,-1.4492543424992075,0.6122012660443078,-
1.7107044502407514,0.73163401370474712},la{1.0,0.0,0.0,0.0,0.0},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{23,
30.868516860440025,5.2517188366529486e-05,0.8748355316821087,
0.87367587198799346,1,{{la{0.86015165920358638,-1.4405113590468699,
0.6031179586496469,-1.6963531920572441,0.71941001272139082},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{24,32.704041582192367,5.1733816278101171e-05,0.81893240468839823,
0.81783313890174492,1,{{la{0.86255861962049163,-1.4311436248975087,
0.59363806185289203,-1.6814442562985892,0.70682005812131798},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{25,34.648715116965342,5.1157963985472533e-05,0.76703196787656458,
0.76598942660794456,1,{{la{0.86489226299101574,-1.4209914435949118,
0.58366658549069195,-1.6658763290910472,0.69379223006780388},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{26,36.709028050002956,5.0782920493655748e-05,0.71885670071086694,
0.71786845472387029,1,{{la{0.86709517948818959,-1.4097797517925814,
0.57303306574583635,-1.6494652698304613,0.68019004078622103},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{27,38.891856976326189,5.0604341825923305e-05,0.67263918255781652,
0.67171770326604963,1,{{la{0.86865499988017369,-1.3944742249331028,
0.55965137638110207,-1.6299180842727934,0.6641642178199012},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{28,41.204487463536495,5.0620167059091969e-05,0.62767739938064815,
0.62684163152552208,1,{{la{0.86918822246093452,-1.3724500534556132,
0.5417802649727671,-1.6051190239861004,0.64410756724123719},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{29,43.654638382094525,5.0830578631877072e-05,0.58558296399633225,
0.58483914228126765,1,{{la{0.86882497360235134,-1.3443365505360123,
0.52002908562762529,-1.5754684420802127,0.62053078775799864},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{30,46.250487684802032,5.1238005854742099e-05,0.54632626223306779,
0.54568395456538099,1,{{la{0.86695901898078509,-1.3063904046800647,
0.49214299986223864,-1.5380103434085513,0.59137427646044349},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{31,49.000699722172065,5.1847171715901196e-05,0.51032302998256895,
0.50979464579434741,1,{{la{0.86243105251553032,-1.2516725210665922,
0.45415172484367605,-1.4872506623678001,0.55298361001592466},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{32,51.914454185646264,5.2665184247641983e-05,0.47935375409376169,
0.47895194184990197,1,{{la{0.85274887554407564,-1.1663392460452988,
0.39881597005805564,-1.4122970195833964,0.49865020571942409},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{33,55.001476776234526,5.3701674930605477e-05,0.45983655566041376,
0.4595679834782862,1,{{la{0.83143852538203611,-1.0194311940941201,
0.31248531340016855,-1.2893541627758245,0.41561227976613324},la{1.0,
0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{34,58.272071702138966,5.4968987913623004e-05,0.47813665915292708,
0.47799297926856432,1,{{la{0.77524336849611453,-0.72725174851842356,
0.17055930024710272,-1.05772487477488,0.27969799495378661},la{1.0,0.0
,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{
35,61.737156115312786,5.648242525940063e-05,0.73105899462071122,
0.73101442764413593,1,{{la{0.52713822478512429,-
2.2638141294984311e-17,2.4305297433952198e-34,-0.53538039280963146,
0.071658686177489633},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}
,la{1.0,0.0,0.0,0.0,0.0}}}},lc{36,65.408296603730406,
5.8260555043704514e-05,0.88320510112563344,0.88322034188138276,1,{{la
{0.42039073342913613,-8.4861120607071809e-18,5.6650318337393614e-35,-
0.76630629035406705,0.19419710231333145},la{1.0,0.0,0.0,0.0,0.0},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{37,
69.29774786345007,6.0325590997685519e-05,0.96286525218389529,
0.96295161932147366,1,{{la{0.36928514752976388,-
1.0413780964603002e-17,9.7994226851961131e-35,-0.88535291592647636,
0.26156419740035541},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},
la{1.0,0.0,0.0,0.0,0.0}}}},lc{38,73.418493682383158,
6.2703854560252503e-05,1.0076043040363933,1.0077738348986749,1,{{la{
0.33807533049917304,-2.5843098569112109e-19,6.5567651397817594e-38,-
0.9628619911193993,0.30771029566440372},la{1.0,0.0,0.0,0.0,0.0},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{39,
77.784290376099392,6.5426332784645604e-05,1.0321750116315993,
1.0324435905345239,1,{{la{0.3159774413283522,-4.1518020798592777e-23,
1.799610106258456e-45,-1.0210775685682199,0.34393683326906593},la{1.0
,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{40,82.409712825063934,6.8529348623202173e-05,1.0427609028546918,
1.0431477965243854,1,{{la{0.29900215671173597,-6.245442704809064e-19,
4.280953456170851e-37,-1.0683605249365391,0.37456227620912297},la{1.0
,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},
lc{41,87.310203272500118,7.2055363822390889e-05,1.0425937299198396,
1.0431202231042724,1,{{la{0.28544344513483461,-2.2758168323175716e-20
,5.9295487195682841e-40,-1.1081794803326106,0.40131677562620671},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}
}},lc{42,92.502123052696234,7.6053939151254118e-05,1.0341237094936899
,1.0348114642827715,1,{{la{0.27448141843685259,-
1.4772202804621997e-21,2.5896681074631917e-42,-1.1420216369780192,
0.42483002144534987},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},
la{1.0,0.0,0.0,0.0,0.0}}}},lc{43,98.002807431147573,
8.0582882152181823e-05,1.0197964868636062,1.0206670158470608,1,{{la{
0.26556817790895781,-5.1631392110548553e-23,3.2618368541082098e-45,-
1.1708125142448689,0.44543599721615779},la{1.0,0.0,0.0,0.0,0.0},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{44,
103.83062375058964,8.5709619281934343e-05,1.2001283873396378,
1.2002372133204768,3,{{la{0.90683565959872947,-0.7921685244027199,
0.78007241946289874,-0.76504465300753921,0.65978420766644752},la{
0.40097431278483964,0.52482150620294377,0.20662999721244549,
0.12753328328581018,0.0048925329144187844},la{0.86838415192402452,-
0.34505419980556035,0.61305279839910587,-0.31920360565322414,
0.45558635617079413},la{1.0,0.0,0.0,0.0,0.0}}}},lc{45,
110.00503309091708,9.1512837500875427e-05,1.3930405979823997,
1.3933456773450001,3,{{la{0.74692683079775435,-0.47989610666528099,
0.60014189307431931,-0.18110890979495672,0.063843572899267523},la{
0.51341335424685397,0.28330812556023444,0.20327852019291148,
1.1291358221782439e-43,1.6578031534344206e-86},la{0.60118050511203036
,-6.2841561442582118e-45,4.054120865232601e-89,-0.91079748138448136,
0.51197798649651172},la{1.0,0.0,0.0,0.0,0.0}}}},lc{46,
116.54665566642632,9.8084450442480643e-05,1.3737953504401821,
1.3739418297286574,3,{{la{0.84204817504418916,-0.56613526389717839,
0.74883459933751961,-0.53194440161667522,0.55669191210120539},la{
0.79365545127737025,-0.18502510560430913,0.59922027113962872,-
0.16349793791643263,0.37134855472912237},la{0.44873354797492604,
0.46433332226147728,0.21040734625028279,0.11743494651634738,
0.0060392699703388664},la{1.0,0.0,0.0,0.0,0.0}}}},lc{47,
123.47734020105011,0.00010553195672191441,1.2852311063607338,
1.2854189766240314,3,{{la{0.78237637959802242,-0.20785022086082602,
0.63383476900800062,-0.18515517227898787,0.39351610002418491},la{
0.59869981100932923,0.30347583931011274,0.36337802920662815,
0.18484353997793687,0.080710139548133358},la{0.81977971022898033,-
0.4777116899396327,0.73471529861911655,-0.44436245047016404,
0.52114576937862833},la{1.0,0.0,0.0,0.0,0.0}}}},lc{48,
130.82023754164183,0.00011398127331188704,1.8563710233626813,
1.856590252823465,4,{{la{0.7809653538889253,-0.16958373396678675,
0.66524755881595921,-0.15447915017945416,0.43110832891755202},la{
0.6821128293019989,0.21051683376480962,0.50452914241835689,
0.17075043866918482,0.22640836681598073},la{0.40005003355938473,
0.49605584318128126,0.21944501377562878,0.11114382443576572,
0.0044070660805291019},la{0.81083360203183263,-0.37879150666754663,
0.7375055850559975,-0.3546491238331258,0.52419680425340898}}}},lc{49,
138.59987879137987,0.00012358014597107486,1.8419372657560449,
1.8421757710764595,4,{{la{0.67815963024209347,0.23265426230064981,
0.49779134015444565,0.1880520410793437,0.22055319161784523},la{
0.81023669773285056,-0.36048647542119172,0.73552530832367613,-
0.33769291998177892,0.52296845061711383},la{0.39852247386749357,
0.50540129428607627,0.22043717942586633,0.11945342635801463,
0.004907521221421669},la{0.77996586084158726,-0.14720412976368771,
0.66046503295988479,-0.13406651242892254,0.42729327646670701}}}},lc{
50,146.84225827060348,0.00013450226239783287,1.8259943578943805,
1.8262540233505724,4,{{la{0.3971740508222103,0.51541910990856021,
0.2218637454048536,0.12894203681663166,0.0055148693189926712},la{
0.77950544941917121,-0.12522192759127132,0.65539685978630569,-
0.11404033302765662,0.42372071464186239},la{0.8096241663668301,-
0.34314513945564618,0.73398785217688733,-0.32164143235182946,
0.52210831143990089},la{0.67455755137782403,0.25479311252198161,
0.49083018272399914,0.20527519322440732,0.2149056533993976}}}},lc{51,
155.57492164164427,0.00014695222329557325,1.7925038026421427,
1.7927866245143291,4,{{la{0.81216454840059971,-0.33434640898098977,
0.73389120027200094,-0.31400206901925859,0.5257114087108693},la{
0.78184667879953962,-0.10816666372720689,0.65117524825924578,-
0.098654116443599463,0.42350937977517794},la{0.67418385641557443,
0.27485649939756951,0.48470087919722538,0.22150706623984862,
0.21223416877052068},la{0.39814596165299798,0.52899270293779987,
0.22467237876124496,0.14508247671737887,0.0067285666346639126}}}},lc{
52,164.82705956849367,0.00016117156339635311,1.8318520270629184,
1.8321607486002394,4,{{la{0.77158682133214906,-0.073829549458318891,
0.64614971695405565,-0.067028258385040521,0.41093524721292629},la{
0.39076640575834254,0.52336851989223643,0.22305495704437939,
0.13167284038894816,0.0055170423060103349},la{0.80464783654193039,-
0.29504974108237353,0.72898418404078735,-0.27647948910171721,
0.51506176860206143},la{0.661925353478789,0.29453555147798777,
0.47873361359970568,0.23443563961366914,0.20075887894281369}}}},lc{53
,174.62960732278373,0.00017744606063331591,1.7777087021441078,
1.7780454298823132,4,{{la{0.66552332264769232,0.31269994256519362,
0.47341569255596339,0.25009667701274041,0.20154228075610886},la{
0.77761475967765514,-0.062918118435130052,0.64246865931546704,-
0.057308075563664025,0.4144733761216563},la{0.81044463750491424,-
0.29543900360909492,0.72983465355777344,-0.27768077595418567,
0.5225210634077786},la{0.3946577235278238,0.5417611887263335,
0.22788010551120377,0.15676843659313303,0.0075305811722281221}}}},lc{
54,185.01531484113596,0.00019572588119413348,1.8027929828392102,
1.8031608550239093,4,{{la{0.76992651654348632,-0.033240211859066664,
0.63786261221782892,-0.030175557669934835,0.40472447457218347},la{
0.38899950685405255,0.5393464017350208,0.22711686175424747,
0.14874326251791362,0.0067195078254070149},la{0.80538759194076182,-
0.26318681685482853,0.72621609689911026,-0.24708310766026198,
0.51549997964530558},la{0.65556120482435254,0.33043338384663162,
0.4675649986490571,0.26153675631115497,0.19202283100888626}}}},lc{55,
196.0188708926301,0.00021586009702127855,1.8220418203826252,
1.8224436983790775,4,{{la{0.76317866522055766,-0.0048671068125598687,
0.6327531615941383,-0.0044045034080393442,0.39546922341017554},la{
0.64648187684162373,0.34810473556535698,0.46129246193741896,
0.27277068577121577,0.18310838857318404},la{0.80099796851662364,-
0.23374235382430994,0.72271993043339056,-0.21921923739195007,
0.50919478251765427},la{0.38418003854879795,0.53904900056260285,
0.22697972671807487,0.14398702269117472,0.0062217431383009602}}}},lc{
56,207.67706146198978,0.00023803383575226843,1.7662672396236248,
1.7667075039997193,4,{{la{0.80413901496088391,-0.2327740157227085,
0.72204689541035305,-0.21858470380490486,0.51199659845343348},la{
0.77004394525262798,0.008372571400022016,0.62588039521627525,
0.0075960359289465066,0.39670087593997894},la{0.38942331393042634,
0.56168869716737202,0.23457026565637559,0.17664747916941342,
0.0090347975847603641},la{0.65088483988566415,0.37004690111106014,
0.45466009143912534,0.29170227260772918,0.18388955982812061}}}},lc{57
,220.02886764999613,0.00026245039957700279,1.7652270331806645,
1.7657089514032218,4,{{la{0.80273270743186387,-0.21180731399037436,
0.72033124395852921,-0.19896050252595668,0.51021713992597539},la{
0.76711069527806519,0.030361724609454206,0.62198874017926842,
0.027518051116711011,0.39194310895007695},la{0.38743463399745015,
0.5653504198731395,0.23612278221303939,0.17966842257094376,
0.0092394135126852885},la{0.64594194268653071,0.38511854677851515,
0.44986413304384226,0.30210751923153562,0.17881710327735231}}}},lc{58
,233.11559746243384,0.00028933301843624387,1.7517608113022625,
1.7522886874582739,4,{{la{0.64364457281810739,0.39920230140250573,
0.44589417582727081,0.31267451672344276,0.17606653332444119},la{
0.76634015810174716,0.048852371377210831,0.61861553087462817,
0.044282528699174512,0.38952553165441145},la{0.80322807473756053,-
0.19621778194822642,0.71934760646654561,-0.18450166262510986,
0.5108595618809898},la{0.38725438061115197,0.57172555073064457,
0.23875002918435118,0.18775839603035138,0.0099715644957964825}}}},lc{
59,246.98102564028997,0.00031892676924959808,1.722511782808311,
1.7230903762131455,4,{{la{0.64480035512729794,0.41241963130847409,
0.44307157213631421,0.32398464355795298,0.17630691501413318},la{
0.7683299239991378,0.063014730068522651,0.61598298852704536,
0.057211189879150921,0.39011645271555506},la{0.80610838604814983,-
0.18750898730915164,0.71922438831447588,-0.17663092369359887,
0.51445471074707305},la{0.38947679206223429,0.58158596072273561,
0.24277936138576664,0.2023909960165805,0.011451118154156028}}}},lc{60
,261.67154204234828,0.00035150067659325503,1.8241763177887147,
1.8248077445283379,4,{{la{0.6186833486229727,0.42698841402721499,
0.43231160052857981,0.32399102838849331,0.15399233479027424},la{
0.37384227470394205,0.56131522453380656,0.23565782061620855,
0.16335399192462111,0.0074613279293359669},la{0.74756064900937746,
0.10861969717761091,0.60779062072676571,0.097544302806611111,
0.36642666410714286},la{0.79024035787028468,-0.12458712483128905,
0.71057313957165946,-0.11673333069006227,0.49295970330071742}}}},lc{
61,277.23630912698644,0.00038735001165072271,1.9239666419353099,
1.9272074401705011,4,{{la{0.30638489906467958,-6.2410885793764811e-50
,3.9135850467646334e-99,-1.0027602891330949,0.30953922642078202},la{
0.813528759254464,-5.1609109647338134e-44,3.1447127462667622e-87,-
0.79776195144128503,0.61129071069574903},la{0.46665974171823499,
0.24818176211925769,0.28515849616250755,3.9521386117670613e-41,
3.3745142041420906e-81},la{0.73660617247472315,0.019733016865706797,
0.64167062105954809,0.01770060661787028,0.38030920378210792}}}},lc{62
,293.72742911860752,0.00042679880778770526,2.1610749485494587,
2.1621826650352611,4,{{la{0.83131736405068957,-3.7875069123247481e-40
,1.8146165458798272e-79,-0.73172604922186246,0.56304341327255203},la{
0.53008642903194325,0.031319063247417782,0.45934858109223836,
0.0073486092971235635,0.013405464074475772},la{0.30951568459609674,
0.48681082686030291,0.20367348854360029,2.2630662354532913e-41,
1.3623579700966239e-82},la{0.44051681594186654,0.27545726106119706,
0.28402592299693646,3.5118967405930994e-42,2.0337403137915133e-83}}}}
,lc{63,311.20012148602342,0.00047020261276590553,2.1864389583355623,
2.1875844144357983,4,{{la{0.84109147180108479,-1.2638455670783654e-43
,2.0361479961067161e-86,-0.72952358886369373,0.57061506066477852},la{
0.43597115903510014,0.27700939877601088,0.28701944218888903,
1.1259589930344634e-41,2.0674033459831961e-82},la{0.31408781579836786
,0.48425140829239877,0.2016607759092332,2.4448182893011489e-42,
1.614447437706064e-84},la{0.50082566867945677,0.061612027606464857,
0.43756230371407839,3.8318574634296431e-41,8.4764532065839248e-80}}}}
,lc{64,329.71291140560572,0.00051795149942170777,1.928707234431728,
1.9318848535460593,4,{{la{0.9184500139232844,-1.4364847448825133e-38,
2.8207126572462874e-76,-0.70427393734915655,0.62272395127244096},la{
0.40414440545329416,-1.1999415693086445e-44,1.2652937739721639e-88,-
0.85621521047358851,0.26035961592688267},la{0.42848841816496142,
0.37507913857684028,0.19643244325819831,6.626232035549544e-41,
2.6268771690193748e-81},la{0.51366319594019116,0.054415264705176575,
0.43192153935463229,6.9495603623771096e-38,3.6187325133546361e-73}}}}
,lc{65,349.32782993150971,0.00057047335860718198,1.9574757876331579,
1.9610653047728726,4,{{la{0.93492897919899287,-1.1737139364047039e-41
,1.9143964330198564e-82,-0.69792505280601824,0.63285403200501111},la{
0.39440794489233139,-3.8213897357063722e-46,1.294808741220689e-91,-
0.87073584260537518,0.26514378749770656},la{0.42641470764316003,
0.37073659196930131,0.20284870038753872,1.8993918476267672e-39,
2.2704013583229737e-78},la{0.49752472066342557,0.081594557195582534,
0.42088072214099181,2.2578240874489454e-38,1.6033617641545745e-74}}}}
,lc{66,370.11062664921354,0.0006282375003364934,1.8713294379805907,
1.8742449590281991,4,{{la{0.94074818118247328,-3.1252951619650871e-36
,1.4202169507347681e-71,-0.66602383561227252,0.60677201679474579},la{
0.43636762268974572,-2.7140000760277919e-38,6.0869915492715461e-76,-
0.78695807512045957,0.22332569781020528},la{0.42933090073792363,
0.38277834607251765,0.18789075318955875,9.3919716962083904e-38,
4.8564215993479688e-75},la{0.51209429376731896,0.062359547996954409,
0.4255461582357265,8.0885737580518696e-33,3.6663556735469277e-63}}}},
lc{67,392.13099564747938,0.00069175859141567208,1.8943576785377716,
1.8972845361672501,4,{{la{0.94918355985457403,-1.5733975301045952e-33
,3.6482568241664642e-66,-0.65983690370455428,0.60902046355912831},la{
0.43547780713347622,-1.0673611105924069e-37,9.2712036158079239e-75,-
0.78029536976322722,0.21577317689670344},la{0.4270582191395908,
0.38275759273255144,0.19018418812785789,3.482226736999403e-33,
6.7224557799373304e-66},la{0.49910172322318413,0.084785221749654971,
0.41611305502716084,3.2185735491948606e-30,2.9928638857699541e-58}}}}
,lc{68,415.46281570820787,0.00076160096037399168,1.9186597273597237,
1.9213211202859977,4,{{la{0.95189438729196829,-2.172452146674629e-40,
6.9864560788571804e-80,-0.65776579309825267,0.60966018039022096},la{
0.43784204929611248,-2.5461216329864874e-38,5.1182046692424643e-76,-
0.76385414325985113,0.2016961925559636},la{0.42441754339242582,
0.38576952306137663,0.18981293354619755,4.7703916290517981e-39,
1.231888408596557e-77},la{0.48805977923344812,0.10528572259563578,
0.40665449817091592,2.7196926822242915e-41,1.3243394034058489e-80}}}}
,lc{69,440.1844056841573,0.00083838330327871036,1.5397239014500914,
1.5395318579045452,3,{{la{0.81939940903379604,-4.5762166083920735e-35
,2.5730981408007958e-69,-0.75596693989281605,0.57536634892661209},la{
0.30570587941298066,-1.3961402878224216e-33,1.7592418700864173e-66,-
0.93604110597138024,0.24174698538436087},la{0.6604705739463792,
0.055757662924567984,0.51191369650108021,0.041434341357714635,
0.18670759201431267},la{1.0,0.0,0.0,0.0,0.0}}}},lc{70,
466.37879611185355,0.0009227838270206535,1.6995081965154242,
1.6969866675750764,4,{{la{0.8249804917317014,-2.2673988993536562e-37,
6.526338653734352e-74,-0.72405471675082156,0.54903520848252296},la{
0.39776862794779327,-5.4867992298432199e-37,1.892132424460408e-73,-
0.73862392536009136,0.13639255330788463},la{0.49164273728618058,
0.24741015703006231,0.26094710568375723,5.194592830570893e-35,
5.6554880090514096e-69},la{0.826511306080154,0.087171448697988527,
0.69181167086686679,0.083304802786387938,0.52218962285862114}}}},lc{
71,494.13401819204773,0.0010155458699268624,1.7377858496045961,
1.7355437636857822,4,{{la{0.47279885804412658,0.28055453730088736,
0.24664660465498592,5.5487265359987527e-34,4.5614572578290812e-67},la
{0.84375082038595683,-7.0546120143213622e-33,6.473668360180192e-65,-
0.71086409270833173,0.55461491309428856},la{0.38536122668380524,-
1.5803702636915289e-36,1.6922230084182991e-72,-0.76907269362081299,
0.15443392030461825},la{0.80153296098219295,0.13360930904459054,
0.66747303942014036,0.1260955134297152,0.47651979601720862}}}},lc{72,
523.54341136371761,0.0011174840431107992,1.9194205856350413,
1.9076708193982626,4,{{la{0.59541948147801482,-6.8468488121673229e-22
,4.7824285444992898e-43,-0.93046702919755953,0.52588651067557435},la{
1.2363554901045501,-3.4565256171392165e-17,3.6955439696543937e-33,-
0.41163629718215611,0.64799178728670626},la{0.25526993255653652,-
7.3936170994525123e-21,5.3537509548177583e-41,-0.98951946192201123,
0.24478939447854778},la{0.49956879703451884,0.11493791817568985,
0.38549328478979145,9.7261734360766485e-18,1.3790165794807108e-33}}}}
,lc{73,554.70595080091482,0.0012294909398345667,1.8697967381891152,
1.8578719536461605,4,{{la{0.59513734312680155,-2.1280396439599575e-32
,4.6292634706729986e-64,-0.92174934489241933,0.51688668801922089},la{
1.2367525707986902,-1.1905203734575172e-28,4.4151628589297397e-56,-
0.40959565042679441,0.64634822122548452},la{0.26176580336535693,-
9.9902098661850298e-30,9.5319165195870356e-59,-0.97674312747693881,
0.23850893084229571},la{0.50534870072958837,0.12021318322076259,
0.37443811604964905,3.7448047730570376e-26,1.836222791281893e-50}}}},
lc{74,587.726596275931,0.0013525444643578469,2.2721560597064556,
2.2497487616297134,4,{{la{0.50601613121066114,-5.0814939198974783e-10
,2.5798864858244047e-19,-1.020633771210389,0.52664990191290073},la{
1.1068805259825993,-1.4082336322663443e-08,3.813122249958832e-16,-
0.55959999103340186,0.66648050293366534},la{0.23768898785669135,-
0.026056077545083309,0.00071408996843597117,-1.0783831952136855,
0.29073019549372953},la{0.3664121531866158,0.44448230485458357,
0.18910555513069316,1.3171892452300609e-08,6.0850136487015347e-17}}}}
,lc{75,622.71666395867419,0.0014877158363130931,1.9557644989831819,
1.9549046039842655,4,{{la{0.35044198973667962,-1.6925713823151483e-31
,2.2543704191277419e-62,-0.84774736572881604,0.19818935546549565},la{
0.8193850073257346,-7.0955038103824902e-33,6.3992301691956794e-65,-
0.71896489066268876,0.53834989798842336},la{0.25381016155296293,
0.49993687445431367,0.24625296399272339,1.490034648235062e-31,
5.5520435653692295e-63},la{0.9017875701537541,0.086060648824081556,
0.74716127667026799,0.084557647486080259,0.6504518481620235}}}},lc{76
,659.79422286235888,0.001636178331607003,1.8006808520174964,
1.7984917669906975,4,{{la{0.79841502089760352,-3.8082930807583384e-33
,1.8762097841767287e-65,-0.68199583466876212,0.48041085556636565},la{
0.42108806392356657,-1.649542529276959e-38,1.6285636265493627e-76,-
0.70372346012087539,0.12481152404444198},la{0.25381016126079203,
0.49993687502995621,0.24625296370925193,7.8391280113376565e-38,
1.5367231774271624e-75},la{0.93525308297040399,-0.095204853133242021,
0.78656451688925388,-0.094243561385755792,0.7208563081121715}}}},lc{
77,699.08451780140376,0.0017992168262377724,1.7218728744503611,
1.7225267427300839,4,{{la{0.77686805544331128,-2.8567294007943108e-13
,9.5631444036885878e-26,-0.78705475303714645,0.56392280848017207},la{
0.25381017126500244,0.49993685532002735,0.24625297341558694,
6.1687330449681065e-13,9.5159499635919918e-26},la{0.29444606844752802
,-9.0857632931115183e-14,7.8262306108853067e-27,-0.96609375417025278,
0.26053982261768993},la{1.5012895643365627,0.70564118219731831,
0.36188255142517772,0.82542268528384766,0.74339061267521112}}}},lc{78
,740.72042090083153,0.0019782382152723548,1.3477050117397311,
1.3524241661697456,4,{{la{1.1506304430989278,-2.4201458091989297e-08,
3.1551828093314013e-15,-0.25619046162759324,0.40682088053847421},la{
0.61560515447419406,-2.6045378634018488e-21,3.9608100410198681e-42,-
0.46067569231610001,0.07628084679029401},la{0.25381016011855723,
0.49993687728041569,0.24625296260102714,1.3489078789152533e-24,
4.5501394153049702e-49},la{0.80142262908969064,-0.17176132369933242,
0.60326056006614537,-0.15357260618450891,0.38649447164101258}}}},lc{
79,784.84291388976635,0.0021747827855870525,2.0998745036794801,
2.100302844605678,4,{{la{0.42482988146917677,0.51205048427415256,
0.15429452462164095,0.089186339668248765,0.0019885506967213793},la{
0.34281625677786376,0.544651993583401,0.20372787878461812,
0.089206671946951671,0.0019894571989313679},la{0.46660628882181049,
0.92047022701319992,0.45395093014944154,0.80702528402599616,
0.034002161958455276},la{0.78713499271026621,1.552972233893025,
0.76598130602517678,1.5248197303225797,0.58126880230588773}}}},lc{80,
831.6016036274757,0.0023905366278806632,1.9762871820984709,
1.9767624931441057,3,{{la{0.51777577070670378,0.47969153347023874,
0.11110213160009218,0.10577248134934411,0.0027969544276904802},la{
0.27753781578083575,0.55455005172486382,0.27701248476972784,
0.10627666964761093,0.0028236826278163888},la{0.44239191488536783,
0.88422303301880378,0.44183125250524974,0.7323537834166951,
0.036092416992726258},la{1.0,0.0,0.0,0.0,0.0}}}},lc{81,
881.15527355179665,0.0026273451809679005,1.8708605165672132,
1.8707476880446992,4,{{la{0.25380826832008513,0.49994060454906014,
0.24625112713085451,3.9433671261629507e-22,3.8884954418099932e-44},la
{0.28585248879988273,0.56305993105882479,0.27734123094586616,
0.12250109538231924,0.0037525554222545832},la{0.88952208092469398,-
0.0064729040827279415,0.71042475087685919,-0.0063060548915932204,
0.59977998261041843},la{1.0383830520835158,-0.76997097176181872,
0.54447841516365059,-0.80481443341564596,0.6177049289009936}}}},lc{82
,933.67247401013549,0.0028872280095014581,2.0388747381894436,
2.0394596395426214,4,{{la{0.45112344629003676,0.5061998888374255,
0.14200011635724805,0.09697253343732698,0.0023509180473833898},la{
0.35501186156001641,0.54840482786114375,0.19593154486578732,
0.096996170065073967,0.002352064221873975},la{0.78016445339767337,
1.5351675201087673,0.75520593911588041,1.50459008308955,
0.56594782953277112},la{0.46687075127423489,0.91856289695870263,
0.45181550655131658,0.80077660608903134,0.036472548695222949}}}},lc{
83,989.33215473644816,0.0031723949251077372,1.9024172889302722,
1.9018663137522731,4,{{la{1.042764534267739,-0.72987722420299184,
0.53778445420681609,-0.76437573681212478,0.61504750108368789},la{
0.89575029717809751,0.043047618447192237,0.72690166510496435,
0.042127074117200104,0.62357250661305408},la{0.25380826832008513,
0.49994060454906014,0.24625112713085451,4.1383520760944167e-20,
4.2825460654108689e-40},la{0.27134525539412535,0.53448420700060773,
0.26326595041476519,0.067941128779785656,0.0011542840297125866}}}},lc
{84,1048.3243430781151,0.0034852635705114195,1.0594020733073863,
1.0640945818572931,4,{{la{1.1548371675316658,0.4211469536699663,
0.37428563430560308,0.45211853184439016,0.49815122366284514},la{
1.0213940519196587,-1.2788329610590397e-14,3.4867114476539521e-28,-
0.47969800997874573,0.50109206189839162},la{0.73677023909081396,-
0.20688757298020105,0.44629981725134699,-0.03138442002002742,
0.0075669033819872784},la{0.25380826832008513,0.49994060454906014,
0.24625112713085451,2.4790979307549648e-49,1.5368608115867825e-98}}}}
,lc{85,1110.8508719595052,0.003828478596631016,1.7633217485142672,
1.7666973013945726,3,{{la{0.90902533052221868,0.092900928235506497,
0.73675295099176741,0.091335298494345629,0.64734391125514712},la{
0.30775396841189562,0.58667199832076145,0.27966250791524611,
0.16710567490406752,0.0069827997438355769},la{0.29949232731417752,
0.58657250091071078,0.28887859854703063,0.1678583648881832,
0.0070850618837356083},la{1.0,0.0,0.0,0.0,0.0}}}},lc{86,
1177.120911749653,0.0041959746291991085,1.5620620380571175,
1.5656155363348243,3,{{la{0.94054569790510689,-0.10277527232420591,
0.7743826264466821,-0.1017515354379267,0.71390458746550955},la{
0.28262852139953343,0.55670949881386578,0.27421325717486228,
0.11049806832518054,0.0030532090630807827},la{0.41676926970298822,
0.69652535441578844,0.29108869683938249,0.37012646845165059,
0.034256852506508673},la{1.0,0.0,0.0,0.0,0.0}}}},lc{87,
1247.3537111270525,0.0045785644772919076,1.7881362855494654,
1.7922276964864026,3,{{la{0.29523742121146646,0.58154596705187656,
0.28644672699489498,0.15706151357995357,0.006168601678284525},la{
0.91873394310890844,0.074190594296544773,0.75427279598088659,
0.073169272247545686,0.67402806113879399},la{0.29522801220751543,
0.58152743360080095,0.28643759814403658,0.15702714188034395,
0.0061659020720090914},la{1.0,0.0,0.0,0.0,0.0}}}},lc{88,
1321.7853695473279,0.0049741132176691462,1.4570313419875911,
1.4574051141483038,4,{{la{0.54655201047155422,0.94821039270077379,
0.4113628771155094,0.76122402588188964,0.1449012544059475},la{
0.25380826832008513,0.49994060454906014,0.24625112713085451,
1.0625107529960579e-28,2.8230192453726091e-57},la{1.5086526169519763,
-0.13011226328119824,0.087575417328505448,-0.3167129979145975,
0.78282876891388076},la{0.61528735962619863,-0.15753932707784274,
0.52302546628020363,-0.036521147810284663,0.017294646638844304}}}},lc
{89,1400.6656635601635,0.005380118591033785,1.7232347092229401,
1.6761248629556442,4,{{la{0.31088149764518741,0.55285446276690409,
0.24585210712067804,0.10673905799997774,0.0028490095327915973},la{
0.25380826832008513,0.49994060454906014,0.24625112713085451,
3.6945350675641147e-25,3.4132394551794613e-50},la{1.2169611280560235,
-1.3155844829053791,0.42158010321794248,-0.9376705192505973,
0.26062726761918409},la{0.92443345841255853,-0.016021151914802377,
0.76319501035771264,-0.015814291481467419,0.68742160833693611}}}},lc{
90,1484.2587908408173,0.0057937250598484348,1.8635311143413418,
1.864741068645668,4,{{la{0.28898613490878466,0.56923245231077835,
0.28038157274187553,0.13410313757971767,0.0044970223817208379},la{
0.26227798306981692,0.51662388417728311,0.25446865612391306,
0.033096609402523014,0.00027391396849001006},la{0.91988495865767594,
0.15809079774294621,0.76931581072487565,0.15622400174795037,
0.69106756537754765},la{1.025122469813875,-0.70330274875462273,
0.78057927736337707,-0.70760100157737027,0.80999999999999983}}}},lc{
91,1572.8441576234102,0.0062117471027249554,1.3555610236070983,
1.3584356419803381,4,{{la{0.25380826832008513,0.49994060454906014,
0.24625112713085451,5.4740011789809913e-37,7.4930209036184441e-74},la
{1.1183496665094224,-0.36633220003782246,0.67987628925599508,-
0.37810624427240508,0.80999999999999983},la{0.61653415633500641,
0.9533109937137656,0.36860331100660143,0.78453621837102638,
0.15391224268434758},la{0.85216827437198883,-0.21677222085909906,
0.7113094672586282,-0.20722332767972199,0.5539288484512398}}}},lc{92,
1666.7172140392568,0.0066307018611877029,0.95484745081519651,
0.97950411431483786,4,{{la{0.96618582614800952,0.017132177042211715,
0.81271285808385785,0.017063700968161229,0.77896716030591795},la{
0.29676136198063663,0.58454775999770392,0.28792529242766313,
0.1626213564408836,0.0066130579651200814},la{0.81633534376788941,-
0.71180490547500497,0.22558976428799121,-1.1534779349948177,
0.48359813757569337},la{1.1142704501543739,-1.3979887086031719,
0.61036774479161726,-1.0770069167046121,0.40365640304743128}}}},lc{93
,1766.1903423903611,0.0070468509662259069,1.6144129817487587,
1.6157309760247456,3,{{la{0.49012173397677766,0.4836209634333235,
0.11930160408126131,0.0909751806561509,0.0020691208352116375},la{
0.27927195487909234,0.54646010395910904,0.267318860132222,
0.0909815102250775,0.0020694087453457547},la{0.77422347724640284,
1.5151467697642336,0.74128136171086312,1.4817533564124923,
0.54889825230900702},la{1.0,0.0,0.0,0.0,0.0}}}},lc{94,
1871.5938039022826,0.0074562510678259147,0.93153301129185218,
0.93687801239804414,4,{{la{0.86253688118292982,0.58875218071464552,
0.18171418488561894,0.513687185084023,0.11931606169917129},la{
1.0680078046612123,-0.47891320420628869,0.73413503069152941,-
0.48677036885354702,0.80999999999999983},la{0.32460320106342794,
0.63938941647697334,0.3149381407517643,0.26179267837547182,
0.017138079916693888},la{0.90006575917513887,-0.28561930823660592,
0.73058809530549607,-0.27802094169656094,0.62305548794058985}}}},lc{
95,1983.2767499858901,0.0078548122818324299,1.3531285214151354,
1.3561637820199477,4,{{la{0.2538103198914628,0.49993656249184282,
0.24625311761669444,3.7949832281436875e-27,3.6014794102877818e-54},la
{0.86417911458991736,-0.15823822000844837,0.70109207335370816,-
0.15199882520439426,0.55903179313957141},la{1.1055695342172753,-
0.37516062346599738,0.69417204882411387,-0.38541904042460834,
0.80999999999999983},la{0.61375684004062159,0.91307435369717205,
0.33967463302516182,0.73237825700137738,0.13412756976157814}}}},lc{96
,2101.6083044778529,0.0082383634672643612,0.95922296616437186,
0.9624810790288264,3,{{la{0.87774704778613055,0.036008373347515235,
0.72736975256339975,0.035082677780675191,0.60604249591637005},la{
0.27494387626496108,0.54157261552888936,0.26675742234917182,
0.081608515793732114,0.0016653983492901487},la{1.0861814908752172,-
0.14143436108338325,0.72145898421512045,-0.14379388593489342,
0.80999999994184779},la{1.0,0.0,0.0,0.0,0.0}}}},lc{97,
2226.9787237091823,0.0086027229663233999,0.83320520600641479,
0.84213551044950641,3,{{la{0.49618812163070014,0.97662900604284186,
0.48070939169098875,0.79533845926313507,0.15818806010139586},la{
0.63998330812284121,0.51596750634927968,0.17399760667171685,
0.29383462387268083,0.036113797271156919},la{0.97868243661863064,-
0.39694875911573718,0.8158938979184176,-0.39564772755706806,
0.79327530297837889},la{1.0,0.0,0.0,0.0,0.0}}}},lc{98,
2359.8006415554264,0.0089437731913762364,0.96406104922997105,
0.96214417636791194,3,{{la{1.0756380023036713,-0.55911528618972139,
0.67046199593680011,-0.57454625594264797,0.76153096799339792},la{
0.26054124769467046,0.51319532008337732,0.25278363204452531,
0.026346615803167186,0.00017358401940618056},la{0.91509602865260609,-
0.35542937883653408,0.72511641044696118,-0.34674746407795892,
0.63153052434099211},la{1.0,0.0,0.0,0.0,0.0}}}},lc{99,
2500.5104068388341,0.0092575372400099439,0.93452441634829031,
0.94255255564975182,4,{{la{1.7836150729543079,0.58365394307027718,
0.18602964239432737,0.84983500147580648,0.70346365694310597},la{
0.25380826832008513,0.49994060454906014,0.24625112713085451,
1.518024780082326e-27,5.7624197855157179e-55},la{0.63314101891170904,
0.54467982052821562,0.49252731185996973,0.45384523319349795,
0.21650291810639677},la{0.94858811797194065,-0.076255640675083031,
0.77084496945183867,-0.075604600602846975,0.71878204735154327}}}},lc{
100,2649.5695205729303,0.0095402555713281437,1.3208448314275882,
1.3233601457495745,3,{{la{0.58790480875923412,0.46427885233311966,
0.091662310595201621,0.13901469998498789,0.0048312717025675777},la{
0.4102238946319316,0.57181771828029415,0.16182480914102723,
0.13903382164011469,0.004832600413138116},la{0.66047232379721788,
1.3204492175952152,0.65997698670532867,1.2501683206254399,
0.39073020747232157},la{1.0,0.0,0.0,0.0,0.0}}}},lc{101,
2807.4661805581104,0.0097884606924807434,0.780997028088941,
0.7818006468955534,2,{{la{0.53764749987844307,0.4482941074409118,
0.093447661714620753,0.077873209831799864,0.0015160592021755226},la{
0.63020549212040189,1.1907117200845989,0.56243337211606714,
1.0876208214878096,0.29572976283325847},la{1.0,0.0,0.0,0.0,0.0},la{
1.0,0.0,0.0,0.0,0.0}}}},lc{102,2974.7169407523006,
0.0099990477900469728,0.88568272621865451,0.88696519757810144,2,{{la{
0.70227203397691096,0.3816463261084872,0.051850960508153542,
0.13144957308174918,0.0043197475118023866},la{0.41662029058306932,
0.81490880625657569,0.39849016928685899,0.59522060402151855,
0.034798662104985327},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}
}}},lc{103,3151.868492657848,0.010169339299493733,
0.013049888848034315,1.0638102674823051,4,{{la{1.0481933866475628,-
0.26052299729462697,0.13169754432035999,-0.57607826475289015,
0.67497850026949591},la{1.0535211319724285,-1.2199299283000902,
0.56562538411597296,-1.0060533706585468,0.40526995844685815},la{
0.97352656424783546,-1.3598989079792561,0.61436828321769188,-
1.4906378114523859,0.71863375093865722},la{0.75420030932802473,-
0.34720107001453193,0.57450266923376714,-0.25829173525246335,
0.23979364379972326}}}},lc{104,3339.4995756943131,
0.010297141537553957,0.5820332730330029,1.0426688248702074,4,{{la{
1.3124841978445625,0.92000030814510136,0.30916446770252004,
1.2482887460117607,0.74702962073536527},la{0.87311508588532194,-
2.2568895874266374e-19,6.6382695344037845e-38,-0.72500634557725152,
0.59812143146257346},la{0.19536474372391743,-1.9443807737922464e-21,
5.7271831347466718e-42,-1.3214019391755742,0.51676668289949168},la{
1.882030572966811,0.20187647046787921,0.061422510390911199,
0.48316180660687896,0.6621677472187224}}}},lc{105,3538.2230231819567,
0.010380791723823489,0.87958049303705133,0.89713977789604782,3,{{la{
0.3044859901114223,0.58102897950646348,0.27778668612299473,
0.15711681544493009,0.0061848402959505068},la{1.022044562474534,
0.88189179157978081,0.72504034265623352,0.88415123186896594,
0.7448254648415823},la{0.9513518582940701,0.039476334494305002,
0.74851306509766613,0.039145012054931837,0.70019624583110929},la{1.0,
0.0,0.0,0.0,0.0}}}},lc{106,3748.6879501631079,0.010419193982802631,
1.0208742179955408,1.0416316341761414,4,{{la{0.46799608004059606,
0.67300618784408073,0.24209501428856339,0.35208795868307252,
0.031009323490167547},la{2.1492705522776028,0.3894311721250816,
0.017779664770177877,1.1960348284815601,0.36044656069130232},la{
0.93781958989057479,0.17984431198101331,0.74117703988981798,
0.17799235458496865,0.68084858717643737},la{0.25403385613335472,
0.50037619435000669,0.24646903625109828,0.00087889356731545194,
1.931671444699642e-07}}}},lc{107,3971.5820888637736,
0.01041184323671299,0.48413942707064433,0.48434992256746501,1,{{la{
0.51099400171690135,0.79105557770063339,0.28064403544536887,
0.57910721031405876,0.0035864045488448125},la{1.0,0.0,0.0,0.0,0.0},la
{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}}},lc{108,
4207.6342771754062,0.010358836260864999,0.51393716091099384,
0.51449160907648139,1,{{la{0.49020045081507407,0.70540352556504449,
0.21536527339235753,0.41055969008239385,0.00040955969008239394},la{
1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0},la{1.0,0.0,0.0,0.0,0.0}}
}}}};};
class l198{public:struct l364{int size=0;int l466=0;double l6=0.0;
double l371=0.0;;int l104=0;int l112=0;float l107=1.0f;float l120=
0.0f;l364(int size,double l6):size(size),l6(l6){if(size<2)throw std::
runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64\x65\x6c\x3a"
"\x20\x73\x69\x7a\x65\x20\x69\x73\x20\x74\x6f\x6f\x20\x73\x6d\x61\x6c"
"\x6c\x2c\x20\x73\x69\x7a\x65\x3a\x20"+std::to_string(size));if(l6>
1.0||l6<=0.0)throw std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f"
"\x6d\x6f\x64\x65\x6c\x3a\x20\x70\x6f\x73\x69\x74\x69\x6f\x6e\x20\x69"
"\x73\x6e\x27\x74\x20\x67\x6f\x6f\x64\x2c\x20\x70\x6f\x73\x69\x74\x69"
"\x6f\x6e\x3a\x20"+std::to_string(l6));l466=size-1;l371=l466*l6;l104=
std::ceil(l371);l112=l104-1;l107=l371-l112;l120=1-l107;}};public:
const l54*l289=nullptr;int l142=69;double l126=440.0;double l255=
44100.0;double l295=0.0;int l64=0;double l475=0.0;int l60=0;std::
vector<float>lw;std::vector<float>lv;int l22=0;int l16=0;float*l156=
nullptr;float*l324=nullptr;double l678=1.0/9.0;double l647=7.0/9.0;
l364 l17;l364 l69;double l419=0.0;l577 l448;bool l297=false;l440 l336
;l403 l296;bool l97=false;public:l198(double l255,int lh,l35::l34 l75
,l35::l47 l648,const l54*l43):l289(l43),l142(lh),l126(l43->l591.l604(
l142,l75,l648)),l295(l255/(2*l126)),l64(l295),l475(l295-l64),l60(l64-
1),l255(l255),l17(l64,l678),l69(l64,l647),l336(l475),l296(lh){if(l64<
5)throw std::runtime_error("\x73\x74\x72\x69\x6e\x67\x5f\x6d\x6f\x64"
"\x65\x6c\x3a\x20\x64\x65\x6c\x61\x79\x5f\x69\x6e\x74\x20\x69\x73\x20"
"\x74\x6f\x6f\x20\x73\x6d\x61\x6c\x6c\x3a\x20"+std::to_string(l64));
lw.resize(l64,0.0);lv.resize(l64,0.0);l22=0;l16=0;l156=&lw[l22];l324=
&lv[l5(l60,l16)];l419=2.31;}inline void l601(){if(l22==l60)l22=0;else
l22++;if(l16==0)l16=l60;else l16--;lv[l5(0,l16)]=-lw[l5(0,l22)];lw[l5
(l60,l22)]=-lv[l5(l60,l16)];l156=&lv[l5(0,l16)];l324=&lw[l5(l60,l22)]
;}inline int l5(int l313,int l351){return l313+l351<=l60?l313+l351:
l313+l351-l64;}inline void l516(double l113){const bool l382=l113>0.0
;if(l382){l632(l113/(2*l419));}l601();filter();l343();if(l382){l97=
true;}}inline void l632(double l258){lw[l5(l17.l104,l22)]+=l258*l17.
l107;lw[l5(l17.l112,l22)]+=l258*l17.l120;lv[l5(l17.l104,l16)]+=l258*
l17.l107;lv[l5(l17.l112,l16)]+=l258*l17.l120;}inline double l575(){
return l17.l107*lw[l5(l17.l104,l22)]+l17.l120*lw[l5(l17.l112,l22)]+
l17.l107*lv[l5(l17.l104,l16)]+l17.l120*lv[l5(l17.l112,l16)];}inline
float l184(){return l69.l107*lw[l5(l69.l104,l22)]+l69.l120*lw[l5(l69.
l112,l22)]+l69.l107*lv[l5(l69.l104,l16)]+l69.l120*lv[l5(l69.l112,l16)]
;}inline void l13(){l22=0;l16=0;l97=false;l156=&lw[l22];l324=&lv[l5(
l60,l16)];std::fill(lw.begin(),lw.end(),0.0);std::fill(lv.begin(),lv.
end(),0.0);l296.l13();l336.l13();l448.l13();l297=false;}private:
inline void filter(){l336.l189( *l156);l296.l189( *l156);if(l297){
l448.l189( *l156);}}int l426=0;int l399=0;inline void l343(){
constexpr int l658=64;constexpr int l661=8;if(++l399<l658){return;}
l399=0;constexpr float l485=1.0e-6f;constexpr float l613=l485*l485;if
(l628()<l613){if(++l426>=l661){l97=false;l13();}}else{l426=0;l97=true
;}}inline float l628(){constexpr int l298=8;float l383=0.0f;for(int lb
=1;lb<=l298;++lb){const int le=(l60*lb)/(l298+1);const float l449=lw[
l5(le,l22)];const float l432=lv[l5(le,l16)];l383+=l449*l449+l432*l432
;}return l383/static_cast<float>(l298);}};class l222{public:double
l562=0.0;int l142;int l88=3;l429 l95;std::array<l198,3>l109;std::
array<double,3>l227={0.0,0.0,0.0};bool l243=false;bool l130=false;
bool l211=false;const l54*l289=nullptr;l222(int lh,double ll,int l77,
l35::l34 l75,const l54*l43):l142(lh),l88(l77),l562(ll),l95(ll,lh,l43),
l109{l198(ll,lh,l75,l35::l47::lw,l43),l198(ll,lh,l75,l35::l47::l286,
l43),l198(ll,lh,l75,l35::l47::lv,l43),},l289(l43){}void l624(){l582();
for(int lb=0;lb<l88;lb++){l227[lb]=l109[lb].l575();}double l113=l95.
l649(l227[0]);for(int lb=0;lb<l88;lb++){l109[lb].l516(l113/double(l88
));}l343();}void l359(double l529){for(int lb=0;lb<l88;lb++){l227[lb]
=l109[lb].l97=true;}l95.l359(l529);}float l184(){float l165=0.0;for(
auto&string:l109){l165+=string.l184();}return l165;}void l13(){for(
int lb=0;lb<l88;lb++){l109[lb].l13();}l95.l13();l243=false;l130=false
;l227.fill(0.0);l211=false;}private:inline void l343(){l130=l95.l171;
for(int lb=0;lb<l88;++lb){l130=l130||l109[lb].l97;}}void l582(){const
bool l521=!l243&&!l211;for(int le=0;le<l88;++le){l109[le].l297=l521;}
}};
class l103{l103(const l103&)=delete;l103&operator=(const l103&)=
delete;public:static constexpr l54 l43{};std::array<l222* ,88>l2;l103
(double ll){for(std::size_t le=0;le<l2.size();++le){const int lh=
static_cast<int>(le)+21;int l77=1;if(lh>=34){l77=3;}else if(lh>=29){
l77=2;}l2[le]=new l222(lh,ll,l77,l35::l34::l311,&l43);}}inline void
l611(){for(int lb=0;lb<l2.size();lb++){if(l2[lb]->l130)l2[lb]->l624();
}}inline float l184(){float l390=0.0;for(int lb=0;lb<l2.size();lb++){
l390+=l2[lb]->l184();}return l390* (1.0f/500.0f);}inline void l53(
bool l97){if(l97){for(int lb=0;lb<l2.size();lb++){l2[lb]->l211=true;}
}else{for(int lb=0;lb<l2.size();lb++){l2[lb]->l211=false;}}}inline
void l13(){for(int lb=0;lb<l2.size();lb++){l2[lb]->l13();}}~l103()noexcept
{for(int lb=0;lb<l2.size();lb++){delete l2[lb];}}};
class l292{uint64_t l401=0;uint64_t l482=0;uint64_t l444=0;double l443
=0.0;double l375=44100.0;double l282=0.0;public:explicit l292(double
ll):l375(ll){}inline void l615()noexcept{l401=clock_gettime_nsec_np(
CLOCK_UPTIME_RAW);}inline void l572(int l424)noexcept{l482=
clock_gettime_nsec_np(CLOCK_UPTIME_RAW);l444=l482-l401;l443=
1000000000.0*double(l424)/double(l375);const double l588=l444/l443;
l282=0.9*l282+0.1*l588;}inline double l532()const noexcept{return l282
;}};
#include<type_traits>
enum class l8:std::uint8_t{l85,l76,l166,l381,l418,l481,l413,l44,};
struct l66{l8 l11=l8::l44;int lo=0;double lr=0.0;};template<std::
size_t l111>class l144{static_assert(l111>=2&&(l111&(l111-1))==0);
static_assert(std::l722<l66>);public:l144()noexcept{for(std::size_t le
=0;le<l111;++le){l303[le].l79.store(le,std::l281);}}l144(const l144&)=
delete;l144&operator=(const l144&)=delete;bool l654(l66 lz)noexcept{
std::size_t l6=l360.load(std::l281);for(;;){l341&l18=l303[l6&(l111-1)]
;const std::size_t l79=l18.l79.load(std::l72);const auto l201=
static_cast<std::l318>(l79)-static_cast<std::l318>(l6);if(l201==0){if
(l360.l543(l6,l6+1,std::l281)){l18.lz=lz;l18.l79.store(l6+1,std::l96);
return true;}}else if(l201<0){return false;}else{l6=l360.load(std::
l281);}}}bool l354(l66&lz)noexcept{l341&l18=l303[l239&(l111-1)];const
std::size_t l79=l18.l79.load(std::l72);const auto l201=static_cast<
std::l318>(l79)-static_cast<std::l318>(l239+1);if(l201!=0){return
false;}lz=l18.lz;l18.l79.store(l239+l111,std::l96);++l239;return true
;}void clear()noexcept{l66 lz;while(l354(lz)){}}private:struct l341{
std::atomic<std::size_t>l79{0};l66 lz{};};alignas(64)std::array<l341,
l111>l303{};alignas(64)std::atomic<std::size_t>l360{0};alignas(64)std
::size_t l239=0;};class l499{public:bool l527(l66 lz)noexcept{if(l245
.l654(lz)){return true;}l366.store(true,std::l96);return false;}
template<class l623>void l583(l623&&l425){l66 lz;if(l366.exchange(
false,std::l408)){while(l245.l354(lz)){}l425(l66{l8::l44,0,0.0});}
while(l245.l354(lz)){l425(lz);}}void clear()noexcept{l245.clear();
l366.store(false,std::l96);}private:l144<4096>l245;std::atomic<bool>
l366{false};};std::unique_ptr<l103>l15;std::unique_ptr<l24>l123;std::
unique_ptr<l292>l119;namespace{constexpr int l358=21;constexpr int
l672=108;l499 l338;std::atomic<bool>l363{false};std::atomic<std::
uint32_t>l218{0};bool l41(int l210)noexcept{return l210>=l358&&l210<=
l672;}void l427()noexcept{l218.l716(1,std::l96);l218.notify_all();}
void l102(l8 l11,int l210,double lr)noexcept{static_cast<void>(l338.
l527({l11,l210,lr}));}void l595(const l66&lz){if(!l15){throw std::
l659("\x50\x69\x61\x6e\x6f\x20\x65\x6e\x67\x69\x6e\x65\x20\x69\x73"
"\x20\x6e\x6f\x74\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65\x64\x2e"
);}switch(lz.l11){case l8::l85:{if(!l41(lz.lo))break;l222*l333=l15->
l2[lz.lo-l358];l333->l243=true;l333->l130=true;const float l676=lz.lr
 * (7.0f/128.0f);l333->l359(l676);break;}case l8::l76:if(l41(lz.lo)){
l15->l2[lz.lo-l358]->l243=false;}break;case l8::l413:l15->l53(lz.lr>
0.01);break;case l8::l44:l15->l13();break;case l8::l166:case l8::l381
:case l8::l418:case l8::l481:break;}}}void l388(double ll){l15=std::
make_unique<l103>(ll);}void l456()noexcept{l15.reset();}void l446(
double ll){l119=std::make_unique<l292>(ll);}void l453()noexcept{l119.
reset();}void l402(double ll){l123=std::make_unique<l24>(ll);l123->
l68();}void l294()noexcept{l123.reset();}void l329(float*out,int l317
,double l579){if(out==nullptr||l317<0){throw std::invalid_argument(""
"\x49\x6e\x76\x61\x6c\x69\x64\x20\x61\x75\x64\x69\x6f\x20\x6f\x75\x74"
"\x70\x75\x74\x20\x62\x75\x66\x66\x65\x72\x2e");}if(!l15||!l119){
throw std::l659("\x50\x69\x61\x6e\x6f\x20\x65\x6e\x67\x69\x6e\x65\x20"
"\x69\x73\x20\x6e\x6f\x74\x20\x69\x6e\x69\x74\x69\x61\x6c\x69\x7a\x65"
"\x64\x2e");}l338.l583([](const l66&lz){l595(lz);});l119->l615();for(
int lb=0;lb<l317;++lb){l15->l611();float lk=l15->l184();if(!std::
isfinite(lk)){std::cerr<<"\x4f\x55\x54\x50\x55\x54\x20\x41\x42\x4e"
"\x4f\x52\x4d\x41\x4c\n"<<"\x73\x61\x6d\x70\x6c\x65\x20\x3d\x20"<<lk
<<'\n'<<"\x66\x72\x61\x6d\x65\x20\x3d\x20"<<lb<<'\n';for(int lb=0;lb<
l15->l2.size();lb++){if(l15->l2[lb]->l130){std::cout<<"\x68\x61\x6d"
"\x6d\x65\x72\x5f\x2e\x77\x5f\x61\x5f\x31\x5f"<<l15->l2[lb]->l95.l80
<<"\n";std::cout<<"\x68\x61\x6d\x6d\x65\x72\x5f\x2e\x77\x5f\x62\x5f"
"\x31\x5f"<<l15->l2[lb]->l95.l106<<"\n";std::cout<<"\x68\x61\x6d\x6d"
"\x65\x72\x5f\x2e\x6d\x69\x64\x64\x6c\x65\x5f\x76\x5f"<<l15->l2[lb]->
l95.l170<<"\n";}}std::cout<<"\n";lk=0.0f;}out[lb]=lk*l579;}l119->l572
(l317);}double l666()noexcept{return l119?l119->l532():0.0;}void l85(
int lh,double l65)noexcept{if(l41(lh)){l102(l8::l85,lh,l65);}}void l76
(int lh,double l65)noexcept{if(l41(lh)){l102(l8::l76,lh,l65);}}void
l166(int lh,double l465)noexcept{if(l41(lh)){l102(l8::l166,lh,l465);}
}void l99(double l7)noexcept{l102(l8::l381,0,l7);}void l100(double l7
)noexcept{l102(l8::l418,0,l7);}void l93(double l7)noexcept{l102(l8::
l481,0,l7);}void l53(double l7)noexcept{l102(l8::l413,0,l7);}void l44
()noexcept{l102(l8::l44,0,0.0);}void l431()noexcept{l338.clear();if(
l15){l15->l13();}}void l270()noexcept{l363.store(true,std::l96);l427(
);}bool l173()noexcept{return l363.load(std::l72);}void l471()noexcept
{l363.store(false,std::l96);}bool l469(std::stop_token stop_token)noexcept
{std::stop_callback l491(stop_token,[]{l427();});while(!stop_token.
stop_requested()&&!l173()){const std::uint32_t l533=l218.load(std::
l72);if(stop_token.stop_requested()||l173()){break;}l218.wait(l533,
std::l72);}return l173();}static constexpr const char*l28="\x4c\x30"
"\x2d\x43\x6f\x64\x61";static constexpr const char*l50="\n\x20\x20"
"\x20\x20\x20\x20\x20\x20\xe2\x94\x8c\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94" "\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2" "\x94\x80\xe2\x94\x80"
"\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80" "\xe2\x94\x90\xe2\x94\x80"
"\xe2\x94\x80\xe2\x95\xad\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x95\xae\n\x20\x20\x20\x20" "\x20\x20\x20\x20\xe2\x94\x82"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20" "\x20\x20\x20\xe2\x94\x82\x3d\x3d"
"\xe2\x94\x82\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94"
"\x82\n\x20\x20\x20\x20\x20" "\x20\x20\x20\xe2\x94\x82\x20\x20\x62"
"\x42\x70\x69\x61\x6e\x6f\x20\x30\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20" "\x20\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82"
"\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20"
"\x20\x20\x20\x20\x20" "\x20\x20\xe2\x94\x82\x20\x20\x4c\x30\x2d\x43"
"\x6f\x64\x61\x2f\x32\x36\x30\x39\x30\x33\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20" "\x20\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb2"
"\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n\x20\x20\x20\x20"
"\x20\x20\x20" "\x20\xe2\x94\x82\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20" "\xe2\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb1\xe2\x95\xb2"
"\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20\x20\x20\x20\x20\x20\x20"
"\x20" "\xe2\x94\x82\x20\x20\x50\x68\x79\x73\x69\x63\x61\x6c\x20\x4d"
"\x6f\x64\x65\x6c\x69\x6e\x67\x20\x50\x69\x61\x6e\x6f\x20\x20\x20\xe2" ""
"\x94\x82\x3d\x3d\xe2\x94\x82\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2"
"\x95\xb1\xe2\x94\x82\n\x20\x20\x20\x20\x20\x20\x20\x20\xe2" "\x94"
"\x82\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94" "\x82\x3d"
"\x3d\xe2\x94\x82\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2"
"\x94\x82\n\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94" "\x82\x20\x20"
"\x44\x65\x76\x65\x6c\x6f\x70\x65\x64\x20\x62\x79\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82" "\x3d\x3d\xe2\x94"
"\x82\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n"
"\x20\x20\x20\x20\x20\x20\x20\x20\xe2\x94\x82" "\x20\x20\x20\x20\x5a"
"\x69\x79\x61\x6e\x67\x20\x54\x61\x6e\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\xe2\x94\x82\x3d" "\x3d\xe2\x94\x82\xe2\x95"
"\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20\x20\x20"
"\x20\x20\x20\x20\x20\xe2\x94\x82\x20" "\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\x20\x20\x20\xe2\x94\x82\x3d\x3d" "\xe2\x94\x82\xe2\x95\xb2\xe2\x95"
"\xb1\xe2\x95\xb2\xe2\x95\xb1\xe2\x94\x82\n\x20\x20\x20\x20\x20\x20"
"\x20\x20\xe2\x94\x82\x20\x20" "\x62\x42\x53\x6f\x6e\x69\x63\x4c\x61"
"\x62\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
"\xe2\x94\x82\x3d\x3d\xe2" "\x94\x82\xe2\x95\xb1\xe2\x95\xb2\xe2\x95"
"\xb1\xe2\x95\xb2\xe2\x94\x82\n\x20\x20\x20\x20\x20\x20\x20\x20\xe2"
"\x94\x94\xe2\x94\x80" "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2"
"\x94\x80\xe2\x94" "\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2"
"\x94\x80\xe2" "\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x94"
"\x80\xe2\x94\x80\xe2\x94\x98\xe2\x94\x80\xe2\x94\x80\xe2\x95\xb0\xe2"
"\x94\x80" "\xe2\x94\x80\xe2\x94\x80\xe2\x94\x80\xe2\x95\xaf\n\x20"
"\x20\x20\x20\n\x20\x20\x20\x20\x20\x20\x20\x20\x57\x69\x74\x68\x20"
"\x53" "\x70\x65\x63\x69\x61\x6c\x20\x54\x68\x61\x6e\x6b\x73\x20\x74"
"\x6f\n\x20\x20\x20\x20\x20\x20\x20\x20\x5a\x68\x75\x6f\x72\x61\x6e" ""
"\x20\x43\x68\x65\x6e\n\x20\x20\x20\x20\x20\x20\x20\x20\x66\x6f\x72"
"\x20\x74\x68\x65\x20\x63\x6f\x6e\x76\x65\x72\x73\x61\x74\x69" "\x6f"
"\x6e\x73\x20\x61\x6e\x64\x20\x69\x6e\x73\x70\x69\x72\x61\x74\x69\x6f"
"\x6e\n\n\x20\x20\x20\x20";
int main(int l136,char*l127[]){try{return l405(l136,l127,l28,l50);}
catch(const std::exception&l326){std::cerr<<"\x62\x62\x70\x6c\x3a\x20"
<<l326.what()<<'\n';return EXIT_FAILURE;}catch(...){std::cerr<<"\x62"
"\x62\x70\x6c\x3a\x20\x75\x6e\x6b\x6e\x6f\x77\x6e\x20\x66\x61\x74\x61"
"\x6c\x20\x65\x72\x72\x6f\x72\x2e\n";return EXIT_FAILURE;}}
