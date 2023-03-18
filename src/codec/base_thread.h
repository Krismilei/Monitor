#pragma once

#ifdef CODEC_EXPORTS
#define CODEC_API __declspec(dllexport)
#else
#define CODEC_API __declspec(dllimport)
#endif

#include<thread>
#include<mutex>
#include<list>
enum LogLevel
{
	LOG_TYPE_DEBUG,
	LOG_TYPE_INFO,
	LOG_TYPE_ERROR,
	LOG_TYPE_FATAL
};
#define LOG_MIN_LEVEL LOG_TYPE_DEBUG
#define LOG(s,level) \
	if(level>=LOG_MIN_LEVEL) \
	std::cout<<level<<":"<<__FILE__<<":"<<__LINE__<<":\n"\
	<<s<<std::endl;
#define LOGDEBUG(s) LOG(s,LOG_TYPE_DEBUG)
#define LOGINFO(s) LOG(s,LOG_TYPE_INFO)
#define LOGERROR(s) LOG(s,LOG_TYPE_ERROR)
#define LOGFATAL(s) LOG(s,LOG_TYPE_FATAL)
struct AVPacket;
struct AVCodecParameters;
struct AVRational;
struct AVCodec;
struct AVCodecContext;
struct AVFrame;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct AVFormatContext;
class ParaTb;

CODEC_API void MSleep(unsigned int ms);
CODEC_API long long NowMs();

CODEC_API void FreeFrame(AVFrame** frame);

class CODEC_API BaseThread
{
public:
	virtual void Start();

	virtual void Stop();
	//传递到下一个节点
	virtual void Next(AVPacket* pkt);
	//执行任务 需要重载
	virtual void Do(AVPacket* pkt) {}

	void set_next(BaseThread* xt);

protected:
	virtual void Main() = 0;
	bool is_exit_ = false;
	int index_ = 0;

private:
	std::thread th_;
	std::mutex m_;
	BaseThread* next_ = nullptr;
};

class CODEC_API ParaTb
{
public:
	AVCodecParameters* para = nullptr;
	AVRational* time_base = nullptr;

	static ParaTb* Create() { return new ParaTb(); }
	~ParaTb();
private:
	//私有是禁止创建栈中对象
	ParaTb();
};

class CODEC_API CAVPacketList
{
public:
	AVPacket* Pop();
	void Push(AVPacket* pkt);
private:
	std::list<AVPacket*> pkts_;
	int max_packets_ = 200;
	std::mutex mux_;
};