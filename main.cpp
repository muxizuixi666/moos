#include <iostream>


#include "moos_task.h"
#include "moos_thread.h"
#include "moos_looper.h"
#include "moos_signal.h"
#include "moos_object.h"
#include "moos_graphic_button.h"

#include <stdlib.h>


DEFINE_NAMESPACE_MOOS_BEGIN



struct Add : public MoosObject
{

    int add(const int t1_, const int t2_)
    {
        std::cout << t1_ << "+" << t2_ << "=" << t1_ + t2_ << std::endl;
        return t1_ + t2_;
    }
};



class InputEventThread : public MoosThread
{
public:
    InputEventThread(MoosLooper* looper_, Add* add_)
        : m_looper(looper_)
    {
//        s.connect(add_, &Add::add, CONNECT_SYNC);
        MOOS_CONNECT(s, add_, &Add::add);
        _t = add_;

    }

    bool threadRun()
    {
        std::cout << "Input Event Thread id: " << std::this_thread::get_id() << std::endl;

        Add a__;

        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 7 * 1000));
        if (rand() % 2 == 0) {
            m_looper->enqueue(new MoosCommonTask([](){ std::cout << "Common Task: " << std::this_thread::get_id() << "\n"; }));
        }
        else {
            MoosTaskBase* _task = new MoosDelayTask([](){ std::cout << "Delay Task: " << std::this_thread::get_id() << "\n"; });
            _task->setTtl(rand() % 5 * 1000);
            m_looper->enqueue(_task);
        }

//        s.emit(rand(), rand());
        MOOS_EMIT(s, rand() % 100, rand() % 100);

//        s.disconnect(_t, &Add::add);
//        MOOS_DISCONNECT(s, _t, &Add::add);


        return true;
    }


private:
    MoosLooper* m_looper;
//    Signal<int, int> s;
    MOOS_SIGNAL(int, int) s;

    Add* _t;

};



auto add(const int t1_, const int t2_) -> decltype(t1_ + t2_)
{
    std::cout << t1_ << "+" << t2_ << "=" << t1_ + t2_ << std::endl;
    return t1_ + t2_;
}


void addfun(const std::thread::id& id_)
{
    auto* _looper = MoosLooper::getLooper(id_);
    _looper->enqueue(new MoosCommonTask([](){ std::cout << "add fun task id:" << std::this_thread::get_id() << std::endl; }));
}


DEFINE_NAMESPACE_MOOS_END





int main(int argc, char *argv[])
{

    UN_USE(argc);
    UN_USE(argv);





    Moos::MoosGraphicButton button;




    Moos::MoosCommonTask cTask_(Moos::add, 1, 2);

    cTask_.run();



    Moos::Add _add;



    Moos::MoosCommonTask c1Task_(std::bind(&Moos::Add::add, &_add, 1, 2));

    c1Task_.run();


    Moos::MoosLooper* _looper = Moos::MoosLooper::currentLooper();

    Moos::InputEventThread iTh(_looper, &_add);


    iTh.start();

    std::this_thread::sleep_for(std::chrono::seconds(1));



    _looper->enqueue(new Moos::MoosCommonTask(Moos::addfun, iTh.getId()));


    _looper->exec_once();

    button.show();

    _looper->exec();



    return 0;
}
