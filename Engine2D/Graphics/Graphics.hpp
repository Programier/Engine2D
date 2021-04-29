
extern "C"
{
    namespace Graphics
    {
        namespace _Event
        {
            extern bool (*EventisInited)();
            extern int (*getX)();
            extern int (*getY)();
            extern bool (*pressed)(int);
        }
        void initGraphics(bool (*)(), int (*)(), int (*)(), bool (*)(int));
    }
}