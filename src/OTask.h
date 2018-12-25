#ifndef _TASK_H_
#define _TASK_H_
// 任务虚基类，继承该类的子类必须实现纯虚函数ORun()
class OTask
{
public:
	OTask(int id) { _TaskId = id; }

	~OTask() { }

public:
	virtual void ORun(int i) = 0;

	int OTaskId() { return _TaskId; }
private:
	int _TaskId;
};
#endif  /*_TASK_H_*/
