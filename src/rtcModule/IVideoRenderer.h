#ifndef IVIDEORENDERER_H
#define IVIDEORENDERER_H
namespace rtcModule
{
class IVideoRenderer
{
public:
    virtual unsigned char* getImageBuffer(int bufSize, int width, int height, void** userData) = 0;
    virtual void frameComplete(void* userData) = 0;
    virtual void onStreamAttach() {}
    virtual void onStreamDetach() {}
    virtual void clearViewport() {}
    virtual ~IVideoRenderer() {}
};
}
#endif // IVIDEORENDERER_H
