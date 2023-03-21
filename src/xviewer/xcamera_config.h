#pragma once
#include <vector>
#include <mutex>
struct XCameraData
{
    char name[1024] = { 0 };
    char url[4096] = { 0 };         //�����������
    char sub_url[4096] = { 0 };     //�����������
    char save_path[4096] = { 0 };   //��Ƶ¼�ƴ��Ŀ¼
};

class XCameraConfig
{
public:

    //Ψһ����ʵ�� ����ģʽ
    static XCameraConfig* Instance()
    {
        static XCameraConfig xc;
        return &xc;
    }

    void Push(const XCameraData& data);

    // ��ȡ����ͷ
    XCameraData GetCam(int index);

    //�޸������
    bool SetCam(int index, const XCameraData& data);

    bool DelCam(int index);

    int GetCamCount();

    bool Save(const char* path);

    /// ��ȡ���� ����cams_
    bool Load(const char* path);
private:
    XCameraConfig() {}//����˽�� 
    std::vector<XCameraData> cams_;
    std::mutex mux_;
};

