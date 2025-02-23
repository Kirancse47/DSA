#include<iostream>
#include<string>
using namespace std;

class GameSetting{
    static GameSetting* _instance;
    int _brightness;
    int _width;
    int _height;
    GameSetting(): _width(786), _height(1300), _brightness(75) {}
public:
    static GameSetting* getInstance(){
        if(_instance==NULL){
            _instance= new GameSetting();
        }
        return _instance;
    }
    void setWidth(int width){ _width=width; }
    void setHeight(int height){ _height=height; }
    void setBrightness(int brightness){ _brightness=brightness; }
    
    int getWidth(){ return _width; }
    int getHeight(){ return _height; }
    int getBrightness(){ return _brightness; }

    void displaySetting(){
        cout<<"Brightness : "<<_brightness<<endl;
        cout<<"Width : "<<_width<<endl;
        cout<<"Height : "<<_height<<endl<<endl;
    }
};

GameSetting * GameSetting::_instance=NULL;

int main(){
    GameSetting *setting=GameSetting::getInstance();
    setting->displaySetting();
    setting->setBrightness(100);
    setting->displaySetting();
    
    GameSetting *setting2=GameSetting::getInstance();
    setting2->displaySetting();
    setting2->setHeight(1400);

    setting->displaySetting();
    
    return 0;
}